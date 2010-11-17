/*
 * linux/drivers/input/touchscreen/stkxxx.c
 *
 * Copyright (C) 2007-2008 Avionic Design Development GmbH
 * Copyright (C) 2008-2009 Avionic Design GmbH
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Written by Thierry Reding <thierry.reding@xxxxxxxxxxxxxxxxx>
 */

#include <linux/delay.h>
#include <linux/i2c.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/workqueue.h>
#include <linux/i2c/stkxxx.h>


#define        DRIVER_NAME     "stkxxx"
#define        DRIVER_VERSION  "1"

/* basic commands */
#define CANDO			1
#define SINTEK 			0
#define SINTEK_NEW		2

//#define TS_DELAY_WORK
#define MULTI_TOUCH

/* periodic polling delay and period */
#define        TS_POLL_DELAY   (1 * 1000000)
#define        TS_POLL_PERIOD  (5 * 1000000)

/**
 * struct ts_event - touchscreen event structure
 * @pendown:   state of the pen
 * @x:         X-coordinate of the event
 * @y:         Y-coordinate of the event
 * @z:         pressure of the event
 */
struct ts_event {
       short x;
       short y;
       short xz;
       short yz;
       short xw;
       short yw;
};

/**
 * struct stkxxx - touchscreen controller context
 * @client:    I2C client
 * @input:     touchscreen input device
 * @lock:      lock for resource protection
 * @timer:     timer for periodical polling
 * @work:      workqueue structure
 * @pendown:   current pen state
 * @event:     current touchscreen event
 * @pdata:     platform-specific information
 */
struct stkxxx {
       struct i2c_client *client;
       struct input_dev *input;
       spinlock_t lock;
       struct hrtimer timer;
#ifdef TS_DELAY_WORK
       struct delayed_work work;
#else
       struct work_struct work;
       struct workqueue_struct *workqueue;
#endif
       struct ts_event event[5];
       unsigned pendown:1;
       int touching_num;
       int xmax;
       int ymax;
       int vendor;
       struct stkxxx_platform_data *pdata;

       struct delayed_work cal_work;
};

/**
 * stkxxx_get_pendown_state() - obtain the current pen state
 * @ts:                touchscreen controller context
 */
static int stkxxx_get_pendown_state(struct stkxxx *ts)
{
       int state = 0;

       if (ts && ts->pdata && ts->pdata->get_irq_level)
               state = !ts->pdata->get_irq_level();

       return state;
}

static int stkxxx_register_input(struct stkxxx *ts)
{
	int ret;
	struct input_dev *dev;

	dev = input_allocate_device();
	if (dev == NULL)
		return -1;

	dev->name = "sintek capacitive touchscreen";
	//dev->phys = ts->phys;
	dev->id.bustype = BUS_I2C;

	dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
	dev->keybit[BIT_WORD(BTN_TOUCH)] = BIT_MASK(BTN_TOUCH);
	input_set_abs_params(dev, ABS_X, 0, ts->xmax, 0, 0);
	input_set_abs_params(dev, ABS_Y, 0, ts->ymax, 0, 0);
	input_set_abs_params(dev, ABS_PRESSURE, 0, 200, 0, 0);

#if 0//def MULTI_TOUCH
	set_bit(ABS_MT_TOUCH_MAJOR, dev->absbit);
	set_bit(ABS_MT_WIDTH_MAJOR, dev->absbit);
	set_bit(ABS_MT_POSITION_X, dev->absbit);
	set_bit(ABS_MT_POSITION_Y, dev->absbit);
	set_bit(ABS_TOOL_WIDTH, dev->absbit);

	input_set_abs_params(dev, ABS_MT_POSITION_X, 0, ts->xmax, 0, 0);
	input_set_abs_params(dev, ABS_MT_POSITION_Y, 0, ts->ymax, 0, 0);
	input_set_abs_params(dev, ABS_MT_TOUCH_MAJOR, 0, 255, 0, 0);
	input_set_abs_params(dev, ABS_MT_WIDTH_MAJOR, 0, 255, 0, 0);
#endif

	ret = input_register_device(dev);
	if (ret < 0) {
		input_free_device(dev);
		return -1;
	}
	
	ts->input = dev;
	return 0;
}

static int stkxxx_read_block(struct i2c_client *client, u8 addr, u8 len, u8 *data)
{
	u8 msgbuf0[1] = { addr };
	u16 slave = client->addr;
	u16 flags = client->flags;
	struct i2c_msg msg[2] = { { slave, flags, 1, msgbuf0 },
				  { slave, flags | I2C_M_RD, len, data }
	};

	return i2c_transfer(client->adapter, msg, ARRAY_SIZE(msg));
}

static int stkxxx_write_block(struct i2c_client *client, u8 addr, u8 len, u8 *data)
{
	u8 msgbuf0[1] = { addr };
	u16 slave = client->addr;
	u16 flags = client->flags;
	struct i2c_msg msg[2] = { { slave, flags, 1, msgbuf0 },
				  { slave, flags, len, data }
	};

	return i2c_transfer(client->adapter, msg, ARRAY_SIZE(msg));
}


static void stkxxx_reset(struct stkxxx *ts)
{
	unsigned char data[6] = {0, 0, 0, 0 ,0, 0};
	int ret;
	
	ret = stkxxx_read_block(ts->client, 26, 6, data);
	printk(" (26)=%d, (27)=%d,  (28)=%d, (29)=%d, (30)=%d,(31)=%d , kk=%d\n",
			data[0], data[1], data[2], data[3],data[4], data[5], ret);
	if (ret < 0) {
		dev_err(&ts->client->dev, "Read pannel info failed: %d\n", ret);
		return ret;
	}

	if (data[4] ==CANDO)
	{
		ts->xmax = data[1]<<8|data[0];
		ts->ymax = data[3]<<8|data[2];
		printk("CANDO: xmax=%d, ymax=%d\n", ts->xmax, ts->ymax);
		ts->vendor = CANDO;
	}
	else if (data[4] ==SINTEK||data[4] ==SINTEK_NEW)
	{
		ts->xmax = 1024;
		ts->ymax = 600;	
		printk("SINTEK: xmax=%d, ymax=%d\n", ts->xmax, ts->ymax);
		ts->vendor = SINTEK;
	}
	
	data[0] = 0x03;
	stkxxx_write_block(ts->client, 55, 1, data);
}

#define STK_INFO_ADDR	0
#define STK_INFO_LEN		20

#define FIRST_POINT_ADDR	2
#define X_OFFSET			0
#define Y_OFFSET			2
#define XW_OFFSET		8
#define YW_OFFSET		9
#define XZ_OFFSET		12
#define YZ_OFFSET		13

static int stkxxx_read_sensor(struct stkxxx *ts)
{
	int ret,i;
	u8 data[STK_INFO_LEN];
	struct ts_event *event;
	
	/* To ensure data coherency, read the sensor with a single transaction. */
	ret = stkxxx_read_block(ts->client, STK_INFO_ADDR, STK_INFO_LEN, data);
	if (ret < 0) {
		dev_err(&ts->client->dev, "Read block failed: %d\n", ret);
		return ret;
	}
	ts->touching_num = data[0];
	event = &ts->event[0];
	int ba = 2;
	for (i=0; i<ts->touching_num; i++) {
		event->x = (data[ba+X_OFFSET+1] << 8) | data[ba+X_OFFSET];
		event->y = (data[ba+Y_OFFSET+1] << 8) | data[ba+Y_OFFSET];
		event->xw = data[ba+XW_OFFSET];
		event->yw = data[ba+YW_OFFSET];
		event->xz = data[ba+XZ_OFFSET];
		event->yz = data[ba+YZ_OFFSET];
		ba += 4;
		event++;
	}
	
	return 0;
}

/**
 * stkxxx_work() - work queue handler (initiated by the interrupt handler)
 * @work:      work queue to handle
 */
static void stkxxx_work(struct work_struct *work)
{
#ifdef TS_DELAY_WORK
	struct stkxxx *ts = container_of(to_delayed_work(work), struct stkxxx, work);
#else
	struct stkxxx *ts = container_of(work, struct stkxxx, work);
#endif
	struct ts_event *event;
	int i;

//	printk(KERN_INFO "stkxxx work runing\n");
	if (stkxxx_get_pendown_state(ts)) {	
		if (stkxxx_read_sensor(ts) < 0) {
			printk(KERN_INFO "work read i2c failed\n");
			goto restart;
		}
		event = &ts->event[0];
		input_report_abs(ts->input, ABS_X, event->x);
		input_report_abs(ts->input, ABS_Y, event->y);	
		input_report_abs(ts->input, ABS_PRESSURE, event->xz + event->yz);
		if (!ts->pendown) {
			ts->pendown = 1;
			input_report_key(ts->input, BTN_TOUCH,  1);
	             	printk(KERN_INFO "DOWN\n");
		}
		
		for (i=0; i<ts->touching_num; i++) {
//			printk(KERN_INFO "point%d x=%d y=%d pressue=%d\n",
//					i, event->x, event->y, event->xz + event->yz);
#ifdef MULTI_TOUCH
			input_report_abs(ts->input, ABS_MT_POSITION_X, event->x);
			input_report_abs(ts->input, ABS_MT_POSITION_Y, event->y);
			input_report_abs(ts->input, ABS_MT_PRESSURE, event->xz + event->yz);
			input_mt_sync(ts->input);
#endif
			event++;
		}
		input_sync(ts->input);
restart:		
#ifdef TS_DELAY_WORK
		schedule_delayed_work(&ts->work, msecs_to_jiffies(TS_POLL_PERIOD));
#else
		hrtimer_start(&ts->timer, ktime_set(0, TS_POLL_PERIOD), HRTIMER_MODE_REL);
#endif
	}
	
	else {
		/* enable IRQ after the pen was lifted */
		if (ts->pendown) {
			ts->pendown = 0;
			input_report_key(ts->input, BTN_TOUCH, 0);
			input_report_abs(ts->input, ABS_PRESSURE, 0);
			input_sync(ts->input);
	             	printk(KERN_INFO "UP\n");
		}
		enable_irq(ts->client->irq);
	}
}

static void stkxxx_cal_work(struct work_struct *work)
{
	unsigned char data[2] = {0, 0};
	struct stkxxx *ts = container_of(to_delayed_work(work), struct stkxxx, cal_work);
	
	data[0] = 0x03;
	stkxxx_write_block(ts->client, 55, 1, data);
	printk(KERN_INFO "\n ***********re-calibration************\n\n");
}

#ifndef TS_DELAY_WORK
/**
 * stkxxx_timer() - timer callback function
 * @timer:     timer that caused this function call
 */
static enum hrtimer_restart stkxxx_timer(struct hrtimer *timer)
{
	struct stkxxx *ts = container_of(timer, struct stkxxx, timer);
	unsigned long flags = 0;
	
	spin_lock_irqsave(&ts->lock, flags);
//	printk(KERN_INFO "enter timer\n");
	queue_work(ts->workqueue, &ts->work);	
	spin_unlock_irqrestore(&ts->lock, flags);
	return HRTIMER_NORESTART;
}
#endif

/**
 * stkxxx_interrupt() - interrupt handler for touch events
 * @irq:       interrupt to handle
 * @dev_id:    device-specific information
 */
static irqreturn_t stkxxx_interrupt(int irq, void *dev_id)
{
	struct i2c_client *client = (struct i2c_client *)dev_id;
	struct stkxxx *ts = i2c_get_clientdata(client);
	unsigned long flags;
	
	spin_lock_irqsave(&ts->lock, flags);
//	printk(KERN_INFO "enter penirq\n");
	/* if the pen is down, disable IRQ and start timer chain */
	if (stkxxx_get_pendown_state(ts)) {
		disable_irq_nosync(client->irq);
#ifdef TS_DELAY_WORK
		schedule_delayed_work(&ts->work,	msecs_to_jiffies(TS_POLL_DELAY));
#else
		hrtimer_start(&ts->timer, ktime_set(0, TS_POLL_DELAY), HRTIMER_MODE_REL);
#endif
	}
	spin_unlock_irqrestore(&ts->lock, flags);
	return IRQ_HANDLED;
}

/**
 * stkxxx_probe() - initialize the I2C client
 * @client:    client to initialize
 * @id:                I2C device ID
 */
static int stkxxx_probe(struct i2c_client *client,
               const struct i2c_device_id *id)
{
       struct stkxxx *ts;
       int err = 0;
   
       ts = kzalloc(sizeof(struct stkxxx), GFP_KERNEL);
       if (!ts) {
               err = -ENOMEM;
               goto fail;
       }

	ts->client = client;
	stkxxx_reset(ts);
	
	if (stkxxx_register_input(ts) < 0) {
		dev_err(&client->dev, "register input fail!\n");
		goto fail;
	}

       /* setup platform-specific hooks */
       ts->pdata = client->dev.platform_data;
       if (!ts->pdata || !ts->pdata->init_irq || !ts->pdata->get_irq_level) {
               dev_err(&client->dev, "no platform-specific callbacks "
                               "provided\n");
               err = -ENXIO;
               goto fail;
       }

       if (ts->pdata->init_irq) {
               err = ts->pdata->init_irq();
               if (err < 0) {
                       dev_err(&client->dev, "failed to initialize IRQ#%d: "
                                       "%d\n", client->irq, err);
                       goto fail;
               }
       }

       spin_lock_init(&ts->lock);
#ifdef TS_DELAY_WORK
	INIT_DELAYED_WORK(&ts->work, stkxxx_work);
#else
       hrtimer_init(&ts->timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
       ts->timer.function = stkxxx_timer;
	INIT_WORK(&ts->work, stkxxx_work);
	ts->workqueue = create_singlethread_workqueue("stkxxx");
	if (ts->workqueue == NULL) {
		dev_err(&client->dev, "can't create work queue\n");
		err = -ENOMEM;
		goto fail;
	}
	printk("work create: %x\n", ts->workqueue);
#endif
      
       ts->pendown = 0;
       ts->touching_num = 0;
       
       err = request_irq(client->irq, stkxxx_interrupt, IRQF_TRIGGER_FALLING,
                       client->dev.driver->name, client);
       if (err) {
               dev_err(&client->dev, "failed to request IRQ#%d: %d\n",
                               client->irq, err);
                goto fail_irq;
       }

       i2c_set_clientdata(client, ts);
	INIT_DELAYED_WORK(&ts->cal_work, stkxxx_cal_work);
	schedule_delayed_work(&ts->cal_work, 20*HZ);
       err = 0;
       goto out;

fail_irq:
       free_irq(client->irq, client);

fail:
       if (ts) {
               input_free_device(ts->input);
               kfree(ts);
       }

       i2c_set_clientdata(client, NULL);
out:
       printk("stkxxx touch screen driver ok\n");
       return err;
}

/**
 * stkxxx_remove() - cleanup the I2C client
 * @client:    client to clean up
 */
static int stkxxx_remove(struct i2c_client *client)
{
       struct stkxxx *priv = i2c_get_clientdata(client);

       free_irq(client->irq, client);
       i2c_set_clientdata(client, NULL);
       input_unregister_device(priv->input);
       kfree(priv);

       return 0;
}

static const struct i2c_device_id stkxxx_ids[] = {
       { DRIVER_NAME, 0 },
       { }
};

MODULE_DEVICE_TABLE(i2c, stkxxx_ids);
/* SINTEK I2C Capacitive Touch Screen driver */
static struct i2c_driver stkxxx_driver = {
       .driver = {
               .name = DRIVER_NAME,
               .owner = THIS_MODULE,
       },
       .probe = stkxxx_probe,
       .remove = __devexit_p(stkxxx_remove),
       .id_table = stkxxx_ids,
};

/**
 * stkxxx_init() - module initialization
 */
static int __init stkxxx_init(void)
{
       return i2c_add_driver(&stkxxx_driver);
}

/**
 * stkxxx_exit() - module cleanup
 */
static void __exit stkxxx_exit(void)
{
       i2c_del_driver(&stkxxx_driver);
}

module_init(stkxxx_init);
module_exit(stkxxx_exit);

MODULE_AUTHOR("");
MODULE_DESCRIPTION("SINTEK I2C Capacitive Touch Screen driver");
MODULE_LICENSE("GPL v2");
MODULE_VERSION(DRIVER_VERSION);


