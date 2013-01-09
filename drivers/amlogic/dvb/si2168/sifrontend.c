/*****************************************************************
**
**  Copyright (C) 2009 Amlogic,Inc.
**  All rights reserved
**        Filename : avlfrontend.c
**
**  comment:
**        Driver for Si2168 demodulator
**  author :
**	    Shijie.Rong@amlogic
**  version :
**	    v1.0	 12/3/30
*****************************************************************/

/*
    Driver for Si2168 demodulator
*/

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/delay.h>
#include <linux/jiffies.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#ifdef ARC_700
#include <asm/arch/am_regs.h>
#else
#include <mach/am_regs.h>
#endif
#include <linux/i2c.h>
#include <linux/gpio.h>
#include "sifrontend.h"
#include <linux/kthread.h>



#if 1
#define pr_dbg	printk
//#define pr_dbg(fmt, args...) printk( KERN_DEBUG"DVB: " fmt, ## args)
#else
#define pr_dbg(fmt, args...)
#endif

#define pr_error(fmt, args...) printk( KERN_ERR"DVB: " fmt, ## args)

MODULE_PARM_DESC(frontend0_reset, "\n\t\t Reset GPIO of frontend0");
static int frontend0_reset = -1;
module_param(frontend0_reset, int, S_IRUGO);

MODULE_PARM_DESC(frontend0_i2c, "\n\t\t IIc adapter id of frontend0");
static int frontend0_i2c = -1;
module_param(frontend0_i2c, int, S_IRUGO);

MODULE_PARM_DESC(frontend0_tuner_addr, "\n\t\t Tuner IIC address of frontend0");
static int frontend0_tuner_addr = -1;
module_param(frontend0_tuner_addr, int, S_IRUGO);

MODULE_PARM_DESC(frontend0_demod_addr, "\n\t\t Demod IIC address of frontend0");
static int frontend0_demod_addr = -1;
module_param(frontend0_demod_addr, int, S_IRUGO);

static struct aml_fe si2168_fe[FE_DEV_COUNT];

MODULE_PARM_DESC(frontend_reset, "\n\t\t Reset GPIO of frontend");
static int frontend_reset = -1;
module_param(frontend_reset, int, S_IRUGO);

MODULE_PARM_DESC(frontend_i2c, "\n\t\t IIc adapter id of frontend");
static int frontend_i2c = -1;
module_param(frontend_i2c, int, S_IRUGO);

MODULE_PARM_DESC(frontend_tuner_addr, "\n\t\t Tuner IIC address of frontend");
static int frontend_tuner_addr = -1;
module_param(frontend_tuner_addr, int, S_IRUGO);

MODULE_PARM_DESC(frontend_demod_addr, "\n\t\t Demod IIC address of frontend");
static int frontend_demod_addr = -1;
module_param(frontend_demod_addr, int, S_IRUGO);

/* define how many front-ends will be used */
#ifndef   FRONT_END_COUNT
  #define FRONT_END_COUNT  1
#endif /* FRONT_END_COUNT */

SILABS_FE_Context   FrontEnd_Table[FRONT_END_COUNT];
int   fe1;
SILABS_FE_Context    front_end;
CUSTOM_Status_Struct  FE_Status;
CUSTOM_Status_Struct custom_status;
#ifndef   TUNER_ADDRESS_SAT
 #define  TUNER_ADDRESS_SAT 0
#endif /* TUNER_ADDRESS_SAT */

#ifndef   TUNER_ADDRESS_TER
 #define  TUNER_ADDRESS_TER 0
#endif /* TUNER_ADDRESS_TER */




static int SI2168_Reset(void)
{
	gpio_direction_output(frontend_reset, 0);
	msleep(200);
	gpio_direction_output(frontend_reset, 1);
	return 0;
}


static int SI2168_I2c_Gate_Ctrl(struct dvb_frontend *fe, int enable)
{

	return 0;
}


static int SI2168_Init(struct dvb_frontend *fe)
{
	pr_dbg("frontend_reset is %d\n",frontend_reset);
	//reset
	int standard;
	standard=SILABS_DVB_T2;
	SI2168_Reset();
	msleep(100);
	//Initialize LSI
	printf("Initializing LSI .. ");
	SiLabs_API_SW_Init(&front_end, DEMOD_ADDRESS+(fe1*2), TUNER_ADDRESS_TER+(fe1*2), TUNER_ADDRESS_SAT+(fe1*2));
	front_end.Si2168_FE->demod->i2c=DEMOD_ADDRESS;
	SiLabs_API_switch_to_standard		(&front_end, standard, 1);
	//front_end  = (FrontEnd_Table[fe]);
	printk("chip id is %d, i2c is %x\n",front_end.chip,front_end.Si2168_FE->demod->i2c);
	if(MDrv_Tuner_Init()==1)
		pr_dbg("TUNER OK\n");
	else
		pr_dbg("TUNER NG\n");
	msleep(200);
	return 0;
}

static int SI2168_Sleep(struct dvb_frontend *fe)
{
	struct si2168_state *state = fe->demodulator_priv;
//	msleep();
	return 0;
}

static int SI2168_Read_Status(struct dvb_frontend *fe, fe_status_t * status)
{
	unsigned char s=0;
	s=SiLabs_API_Demod_status(&front_end, &custom_status);

	if(s==1)
	{
		*status = FE_HAS_LOCK|FE_HAS_SIGNAL|FE_HAS_CARRIER|FE_HAS_VITERBI|FE_HAS_SYNC;
	}
	else
	{
		*status = FE_TIMEDOUT;
	}
	
	return  0;
}

static int SI2168_Read_Ber(struct dvb_frontend *fe, u32 * ber)
{
	*ber=SiLabs_API_Demod_status(&front_end, &custom_status);
	return 0;
}

static int SI2168_Read_Signal_Strength(struct dvb_frontend *fe, u16 *strength)
{
	*strength=SiLabs_API_Demod_status(&front_end, &custom_status);
	return 0;
}

static int SI2168_Read_Snr(struct dvb_frontend *fe, u16 * snr)
{
	*snr=SiLabs_API_Demod_status(&front_end, &custom_status);
	return 0;
}

static int SI2168_Read_Ucblocks(struct dvb_frontend *fe, u32 * ucblocks)
{
	ucblocks=NULL;
	return 0;
}

static int SI2168_Set_Frontend(struct dvb_frontend *fe, struct dvb_frontend_parameters *p)
{
	
	pr_dbg("Si2168_set_frontend\n");
	int                  lock;
    int                  freq;
    int                  bandwidth_Hz;
    unsigned int         symbol_rate_bps;
    CUSTOM_Constel_Enum  constellation;
    CUSTOM_Stream_Enum   stream;
    int                  standard;
    int                  polarization;
    int                  band;
    int                  plp_id;
	struct si2168_state *state = fe->demodulator_priv;
	freq=p->frequency;
	MDrv_Tuner_SetTuner(freq,8);
	printk("tuner set ok\n");
	standard=SILABS_DVB_T2;
	bandwidth_Hz=8000000;
	stream=0;
	symbol_rate_bps=0;
	constellation=0;
	polarization=0;
	band=0;
	plp_id=0;
	 /* Call SiLabs_API_switch_to_standard, in case the standard is different or the init has not been done yet */
	// if (SiLabs_API_switch_to_standard		(&front_end, standard, 1) ==0) return 0;

  	printk("now to lock carrier\n");
	SiLabs_API_lock_to_carrier (&front_end, standard, freq, bandwidth_Hz, stream, symbol_rate_bps, constellation, polarization, band, plp_id);
	state->freq=p->frequency;
	state->mode=p->u.vsb.modulation ; //these data will be writed to eeprom
	msleep(1000);

	pr_dbg("si2168=>frequency=%d\r\n",p->frequency);
	return  0;
}

static int SI2168_Get_Frontend(struct dvb_frontend *fe, struct dvb_frontend_parameters *p)
{//these content will be writed into eeprom .

	struct si2168_state *state = fe->demodulator_priv;
	
	p->frequency=state->freq;
	p->u.qam.modulation=state->mode;
	
	return 0;
}

static void SI2168_Release(struct dvb_frontend *fe)
{
	struct si2168_state *state = fe->demodulator_priv;
	kfree(state);
}

static ssize_t si_frontend_show_short_circuit(struct class* class, struct class_attribute* attr, char* buf)
{
	
	return ;
}

static struct class_attribute mn_frontend_class_attrs[] = {
	__ATTR(short_circuit,  S_IRUGO | S_IWUSR, si_frontend_show_short_circuit, NULL),
	__ATTR_NULL
};

static struct class mn_frontend_class = {
	.name = "mn_frontend",
	.class_attrs = mn_frontend_class_attrs,
};

static struct dvb_frontend_ops si2168_ops;

struct dvb_frontend *si2168_attach(const struct si2168_fe_config *config)
{
	struct si2168_state *state = NULL;

	/* allocate memory for the internal state */
	
	state = kmalloc(sizeof(struct si2168_state), GFP_KERNEL);
	if (state == NULL)
		return NULL;

	/* setup the state */
	state->config = *config;
	
	/* create dvb_frontend */
	memcpy(&state->fe.ops, &si2168_ops, sizeof(struct dvb_frontend_ops));
	state->fe.demodulator_priv = state;
	
	return &state->fe;
}

static struct dvb_frontend_ops si2168_ops = {


		.info = {
		 .name = "AMLOGIC DVBT2",
		.type = FE_ATSC,
		.frequency_min = 51000000,
		.frequency_max = 858000000,
		.frequency_stepsize = 0,
		.frequency_tolerance = 0,
		.caps =
			FE_CAN_FEC_1_2 | FE_CAN_FEC_2_3 | FE_CAN_FEC_3_4 |
			FE_CAN_FEC_5_6 | FE_CAN_FEC_7_8 | FE_CAN_FEC_AUTO |
			FE_CAN_QPSK | FE_CAN_QAM_16 |
			FE_CAN_QAM_64 | FE_CAN_QAM_AUTO |
			FE_CAN_TRANSMISSION_MODE_AUTO |
			FE_CAN_GUARD_INTERVAL_AUTO |
			FE_CAN_HIERARCHY_AUTO |
			FE_CAN_RECOVER |
			FE_CAN_MUTE_TS
	},

	.release = SI2168_Release,

	.init = SI2168_Init,
	.sleep = SI2168_Sleep,
	.i2c_gate_ctrl = SI2168_I2c_Gate_Ctrl,

	.set_frontend = SI2168_Set_Frontend,
	.get_frontend = SI2168_Get_Frontend,	
	.read_status = SI2168_Read_Status,
	.read_ber = SI2168_Read_Ber,
	.read_signal_strength =SI2168_Read_Signal_Strength,
	.read_snr = SI2168_Read_Snr,
	.read_ucblocks = SI2168_Read_Ucblocks,

};

static void si2168_fe_release(struct aml_dvb *advb, struct aml_fe *fe)
{
	if(fe && fe->fe) {
		pr_dbg("release si2168 frontend %d\n", fe->id);
		dvb_unregister_frontend(fe->fe);
		dvb_frontend_detach(fe->fe);
		if(fe->cfg){
			kfree(fe->cfg);
			fe->cfg = NULL;
		}
		fe->id = -1;
	}
}

static int si2168_fe_init(struct aml_dvb *advb, struct platform_device *pdev, struct aml_fe *fe, int id)
{
	struct dvb_frontend_ops *ops;
	int ret;
	struct resource *res;
	struct si2168_fe_config *cfg;
	char buf[32];
	
	pr_dbg("init si2168 frontend %d\n", id);


	cfg = kzalloc(sizeof(struct si2168_fe_config), GFP_KERNEL);
	if (!cfg)
		return -ENOMEM;
	
	cfg->reset_pin = frontend_reset;
	if(cfg->reset_pin==-1) {
		snprintf(buf, sizeof(buf), "frontend%d_reset_pin", id);
		res = platform_get_resource_byname(pdev, IORESOURCE_MEM, buf);
		if (!res) {
			pr_error("cannot get resource \"%s\"\n", buf);
			ret = -EINVAL;
			goto err_resource;
		}
		cfg->reset_pin = res->start;		
	}
	
	cfg->i2c_id = frontend_i2c;
	if(cfg->i2c_id==-1) {
		snprintf(buf, sizeof(buf), "frontend%d_i2c", id);
		res = platform_get_resource_byname(pdev, IORESOURCE_MEM, buf);
		if (!res) {
			pr_error("cannot get resource \"%s\"\n", buf);
			ret = -EINVAL;
			goto err_resource;
		}
		cfg->i2c_id = res->start;
	}
	
	cfg->tuner_addr = frontend_tuner_addr;
	
	if(cfg->tuner_addr==-1) {
		snprintf(buf, sizeof(buf), "frontend%d_tuner_addr", id);
		res = platform_get_resource_byname(pdev, IORESOURCE_MEM, buf);
		if (!res) {
			pr_error("cannot get resource \"%s\"\n", buf);
			ret = -EINVAL;
			goto err_resource;
		}
		cfg->tuner_addr = res->start>>1;
	}
	
	cfg->demod_addr = frontend_demod_addr;
	if(cfg->demod_addr==-1) {
		snprintf(buf, sizeof(buf), "frontend%d_demod_addr", id);
		res = platform_get_resource_byname(pdev, IORESOURCE_MEM, buf);
		if (!res) {
			pr_error("cannot get resource \"%s\"\n", buf);
			ret = -EINVAL;
			goto err_resource;
		}
		cfg->demod_addr = res->start>>1;
	}
	frontend_reset = cfg->reset_pin;
	frontend_i2c = cfg->i2c_id;
	frontend_tuner_addr = cfg->tuner_addr;
	frontend_demod_addr = cfg->demod_addr;	
	fe->fe = si2168_attach(cfg);
	if (!fe->fe) {
		ret = -ENOMEM;
		goto err_resource;
	}

	if ((ret=dvb_register_frontend(&advb->dvb_adapter, fe->fe))) {
		pr_error("frontend registration failed!");
		ops = &fe->fe->ops;
		if (ops->release != NULL)
			ops->release(fe->fe);
		fe->fe = NULL;
		goto err_resource;
	}
	
	fe->id = id;
	fe->cfg = cfg;
	
	return 0;

err_resource:
	kfree(cfg);
	return ret;
}

int si2168_get_fe_config(struct si2168_fe_config *cfg)
{
	struct i2c_adapter *i2c_handle;
	cfg->i2c_id = frontend_i2c;
	cfg->demod_addr = frontend_demod_addr;
	cfg->tuner_addr = frontend_tuner_addr;
	cfg->reset_pin = frontend_reset;
//	printk("\n frontend_i2c is %d,,frontend_demod_addr is %x,frontend_tuner_addr is %x,frontend_reset is %d",frontend_i2c,frontend_demod_addr,frontend_tuner_addr,frontend_reset);
	i2c_handle = i2c_get_adapter(cfg->i2c_id);
	if (!i2c_handle) {
		printk("cannot get i2c adaptor\n");
		return 0;
	}
	cfg->i2c_adapter =i2c_handle;
	return 1;
	
//	printk("\n frontend0_i2c is %d, frontend_i2c is %d,",frontend0_i2c,frontend_i2c);
	
}

static int si2168_fe_probe(struct platform_device *pdev)
{
	int ret = 0;
	struct aml_dvb *dvb = aml_get_dvb_device();
	
	if(si2168_fe_init(dvb, pdev, &si2168_fe[0], 0)<0)
		return -ENXIO;

	platform_set_drvdata(pdev, &si2168_fe[0]);

	if((ret = class_register(&mn_frontend_class))<0) {
		pr_error("register class error\n");

		struct aml_fe *drv_data = platform_get_drvdata(pdev);
		
		platform_set_drvdata(pdev, NULL);
	
		si2168_fe_release(dvb, drv_data);

		return ret;
	}
	
	return ret;
}

static int si2168_fe_remove(struct platform_device *pdev)
{
	struct aml_fe *drv_data = platform_get_drvdata(pdev);
	struct aml_dvb *dvb = aml_get_dvb_device();

	class_unregister(&mn_frontend_class);

	platform_set_drvdata(pdev, NULL);
	
	si2168_fe_release(dvb, drv_data);
	
	return 0;
}

static int si2168_fe_resume(struct platform_device *pdev)
{
	pr_dbg("si2168_fe_resume \n");
	//reset
	SI2168_Reset();
	msleep(100);
	//Initialize LSI
	printf("Initializing LSI .. ");
	SiLabs_API_SW_Init(&(FrontEnd_Table[fe1]), DEMOD_ADDRESS+(fe1*2), TUNER_ADDRESS_TER+(fe1*2), TUNER_ADDRESS_SAT+(fe1*2));
	return 0;

}

static int si2168_fe_suspend(struct platform_device *pdev, pm_message_t state)
{
	return 0;
}


static struct platform_driver aml_fe_driver = {
	.probe		= si2168_fe_probe,
	.remove		= si2168_fe_remove,
	.resume		= si2168_fe_resume,
	.suspend	= si2168_fe_suspend,
	.driver		= {
		.name	= "si2168",
		.owner	= THIS_MODULE,
	}
};

static int __init sifrontend_init(void)
{
	return platform_driver_register(&aml_fe_driver);
}


static void __exit sifrontend_exit(void)
{
	platform_driver_unregister(&aml_fe_driver);
}

module_init(sifrontend_init);
module_exit(sifrontend_exit);


MODULE_DESCRIPTION("si2168 DVB-T2 Demodulator driver");
MODULE_AUTHOR("RSJ");
MODULE_LICENSE("GPL");


