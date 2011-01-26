#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/platform_device.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/ioctl.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/dma-mapping.h>
#include <linux/soundcard.h>
#include <linux/timer.h>
#include <linux/debugfs.h>
#include <linux/major.h>

#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/initval.h>
#include <sound/control.h>
#include <sound/soc.h>
#include <sound/pcm_params.h>

#include <linux/dma-mapping.h>

#include <mach/am_regs.h>
#include <mach/pinmux.h>

#include <linux/amports/amaudio.h>

#include "aml_pcm.h"
#include "aml_audio_hw.h"


#define AOUT_EVENT_PREPARE  0x1
extern int aout_notifier_call_chain(unsigned long val, void *v);
static unsigned addr_audioin = 0;
static unsigned addr_audioout = 0;


/*--------------------------------------------------------------------------*\
 * Hardware definition
\*--------------------------------------------------------------------------*/
/* TODO: These values were taken from the AML platform driver, check
 *	 them against real values for AML
 */
static const struct snd_pcm_hardware aml_pcm_hardware = {
	.info			= SNDRV_PCM_INFO_INTERLEAVED|
							SNDRV_PCM_INFO_BLOCK_TRANSFER|
							SNDRV_PCM_INFO_MMAP |
				  		SNDRV_PCM_INFO_MMAP_VALID |
				  		SNDRV_PCM_INFO_PAUSE,
				  		
	.formats		= SNDRV_PCM_FMTBIT_S16_LE,
	.period_bytes_min	= 64,
	.period_bytes_max	= 8*1024,
	.periods_min		= 2,
	.periods_max		= 1024,
	.buffer_bytes_max	= 32 * 1024,
	
	.rate_min = 32000,
  .rate_max = 48000,
  .channels_min = 2,
  .channels_max = 2,
  .fifo_size = 0,  
};

static const struct snd_pcm_hardware aml_pcm_capture = {
	.info			= SNDRV_PCM_INFO_INTERLEAVED|
							SNDRV_PCM_INFO_BLOCK_TRANSFER|
							SNDRV_PCM_INFO_MMAP |
				  		SNDRV_PCM_INFO_MMAP_VALID |
				  		SNDRV_PCM_INFO_PAUSE,
				  		
	.formats		= SNDRV_PCM_FMTBIT_S16_LE,
	.period_bytes_min	= 64,
	.period_bytes_max	= 8*1024,
	.periods_min		= 2,
	.periods_max		= 1024,
	.buffer_bytes_max	= 32 * 1024,

	.rate_min = 8000,
  .rate_max = 48000,
  .channels_min = 2,
  .channels_max = 2,
  .fifo_size = 0,  
};

/*--------------------------------------------------------------------------*\
 * Data types
\*--------------------------------------------------------------------------*/
struct aml_runtime_data {
	struct aml_pcm_dma_params *params;
	dma_addr_t dma_buffer;		/* physical address of dma buffer */
	dma_addr_t dma_buffer_end;	/* first address beyond DMA buffer */

	struct snd_pcm *pcm;
	audio_stream_t s;	
	struct timer_list timer;	// timeer for playback and capture
};


/*--------------------------------------------------------------------------*\
 * Helper functions
\*--------------------------------------------------------------------------*/
static int aml_pcm_preallocate_dma_buffer(struct snd_pcm *pcm,
	int stream)
{
	struct snd_pcm_substream *substream = pcm->streams[stream].substream;
	struct snd_dma_buffer *buf = &substream->dma_buffer;
	
	size_t size = 0;
	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK){
		size = aml_pcm_hardware.buffer_bytes_max;
		buf->dev.type = SNDRV_DMA_TYPE_DEV;
		buf->dev.dev = pcm->card->dev;
		buf->private_data = NULL;
		buf->area = dma_alloc_coherent(pcm->card->dev, size,
					  &buf->addr, GFP_KERNEL);
		printk("aml-pcm %d:"
		"preallocate_dma_buffer: area=%p, addr=%p, size=%d\n", stream,
		(void *) buf->area,
		(void *) buf->addr,
		size);
	}else{
		size = aml_pcm_capture.buffer_bytes_max;
		buf->dev.type = SNDRV_DMA_TYPE_DEV;
		buf->dev.dev = pcm->card->dev;
		buf->private_data = NULL;
		buf->area = dma_alloc_coherent(pcm->card->dev, size*2,
					  &buf->addr, GFP_KERNEL);
		printk("aml-pcm %d:"
		"preallocate_dma_buffer: area=%p, addr=%p, size=%d\n", stream,
		(void *) buf->area,
		(void *) buf->addr,
		size);
	}

	if (!buf->area)
		return -ENOMEM;

	buf->bytes = size;
	return 0;
}
/*--------------------------------------------------------------------------*\
 * ISR
\*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*\
 * PCM operations
\*--------------------------------------------------------------------------*/
static int aml_pcm_hw_params(struct snd_pcm_substream *substream,
	struct snd_pcm_hw_params *params)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct aml_runtime_data *prtd = runtime->private_data;
	struct snd_soc_pcm_runtime *rtd = snd_pcm_substream_chip(substream);
	audio_stream_t *s = &prtd->s;
	
	/* this may get called several times by oss emulation
	 * with different params */

	snd_pcm_set_runtime_buffer(substream, &substream->dma_buffer);
	runtime->dma_bytes = params_buffer_bytes(params);

	s->I2S_addr = runtime->dma_addr;
	
	return 0;
}

static int aml_pcm_hw_free(struct snd_pcm_substream *substream)
{
	struct aml_runtime_data *prtd = substream->runtime->private_data;
	struct aml_pcm_dma_params *params = prtd->params;
	if (params != NULL) {
					
	}

	return 0;
}

static int aml_pcm_prepare(struct snd_pcm_substream *substream)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct aml_runtime_data *prtd = runtime->private_data;
	audio_stream_t *s = &prtd->s;
	
	if(prtd == 0)
		return 0;
	
	switch(runtime->rate){
		case 192000:
			s->sample_rate	=	AUDIO_CLK_FREQ_192;
			break;
		case 176400:
			s->sample_rate	=	AUDIO_CLK_FREQ_1764;
			break;
		case 96000:	
			s->sample_rate	=	AUDIO_CLK_FREQ_96;
			break;
		case 88200:	
			s->sample_rate	=	AUDIO_CLK_FREQ_882;
			break;
		case 48000:	
			s->sample_rate	=	AUDIO_CLK_FREQ_48;
			break;
		case 44100:	
			s->sample_rate	=	AUDIO_CLK_FREQ_441;
			break;
		case 32000:	
			s->sample_rate	=	AUDIO_CLK_FREQ_32;
			break;
		case 8000:
			s->sample_rate	=	AUDIO_CLK_FREQ_8;
			break;
		case 11025:
			s->sample_rate	=	AUDIO_CLK_FREQ_11;
			break;	
		case 16000:
			s->sample_rate	=	AUDIO_CLK_FREQ_16;
			break;		
		case 22050:	
			s->sample_rate	=	AUDIO_CLK_FREQ_22;
			break;	
		case 12000:
			s->sample_rate	=	AUDIO_CLK_FREQ_12;
			break;
		case 24000:
			s->sample_rate	=	AUDIO_CLK_FREQ_22;
			break;
		default:
			s->sample_rate	=	AUDIO_CLK_FREQ_441;
			break;	
	};
	audio_set_clk(s->sample_rate, AUDIO_CLK_256FS);
	audio_util_set_dac_format(AUDIO_ALGOUT_DAC_FORMAT_DSP);
#ifdef CONFIG_SND_AML_M1	
	audio_dac_set(s->sample_rate);
#endif	
	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK){
			//printk("aml_pcm_prepare SNDRV_PCM_STREAM_PLAYBACK: dma_addr=%x, dma_bytes=%x\n", runtime->dma_addr, runtime->dma_bytes);
			audio_set_aiubuf(runtime->dma_addr, runtime->dma_bytes);
			memset((void*)runtime->dma_area,0,runtime->dma_bytes);
          addr_audioout = runtime->dma_area;
	}
	else{
			printk("aml_pcm_prepare SNDRV_PCM_STREAM_CAPTURE: dma_addr=%x, dma_bytes=%x\n", runtime->dma_addr, runtime->dma_bytes);
			audio_in_i2s_set_buf(runtime->dma_addr, runtime->dma_bytes*2);
			memset((void*)runtime->dma_area,0,runtime->dma_bytes*2);
          addr_audioin = runtime->dma_area;
			int * ppp = (int*)(runtime->dma_area+runtime->dma_bytes*2-8);
			ppp[0] = 0x78787878;
			ppp[1] = 0x78787878;
	}

    aout_notifier_call_chain(AOUT_EVENT_PREPARE, substream);

	printk("Audio Parameters:\n");
	printk("\tsample rate: %d\n", runtime->rate);
	printk("\tchannel: %d\n", runtime->channels);
	printk("\tsample bits: %d\n", runtime->sample_bits);
	printk("\tperiod size: %d\n", runtime->period_size);
	printk("\tperiods: %d\n", runtime->periods);
	
	
	return 0;
}

static int aml_pcm_trigger(struct snd_pcm_substream *substream,
	int cmd)
{
	struct snd_pcm_runtime *rtd = substream->runtime;
	struct aml_runtime_data *prtd = rtd->private_data;
	audio_stream_t *s = &prtd->s;
	int ret = 0;
	
	spin_lock(&s->lock);
	switch (cmd) {
	case SNDRV_PCM_TRIGGER_START:
		
		del_timer_sync(&prtd->timer);
		
		prtd->timer.expires = jiffies + 1;
    del_timer(&prtd->timer);
    add_timer(&prtd->timer);
        
		// TODO
		if(substream->stream == SNDRV_PCM_STREAM_PLAYBACK){
			//printk("aml_pcm_trigger: SNDRV_PCM_TRIGGER_START\n");
			audio_enable_ouput(1);
		}else{
			printk("aml_pcm_trigger: SNDRV_PCM_TRIGGER_CAPTURE\n");
			audio_in_i2s_enable(1);
		}
		
		s->active = 1;
		
		break;		/* SNDRV_PCM_TRIGGER_START */
	case SNDRV_PCM_TRIGGER_SUSPEND:
	case SNDRV_PCM_TRIGGER_PAUSE_PUSH:
	case SNDRV_PCM_TRIGGER_STOP:
		// TODO
		//printk("aml_pcm_trigger: SNDRV_PCM_TRIGGER_STOP\n");
		s->active = 0;
		if(substream->stream == SNDRV_PCM_STREAM_PLAYBACK){
				audio_enable_ouput(0);
		}else{
				audio_in_i2s_enable(0);
		}
		break;

	case SNDRV_PCM_TRIGGER_RESUME:
	case SNDRV_PCM_TRIGGER_PAUSE_RELEASE:
		// TODO
		printk("aml_pcm_trigger: SNDRV_PCM_TRIGGER_RESUME\n");
		s->active = 1;
		if(substream->stream == SNDRV_PCM_STREAM_PLAYBACK){
				audio_enable_ouput(1);
		}else{
				audio_in_i2s_enable(1);
		}
		
		break;

	default:
		ret = -EINVAL;
	}
	spin_unlock(&s->lock);
	return ret;
}

static snd_pcm_uframes_t aml_pcm_pointer(
	struct snd_pcm_substream *substream)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct aml_runtime_data *prtd = runtime->private_data;
	audio_stream_t *s = &prtd->s;
	
	unsigned int addr, ptr;
	
	if(substream->stream == SNDRV_PCM_STREAM_PLAYBACK){
			ptr = read_i2s_rd_ptr();
	    addr = ptr - s->I2S_addr;
	    return bytes_to_frames(runtime, addr);
	}else{
			ptr = audio_in_i2s_wr_ptr();
			addr = ptr - s->I2S_addr;			
			return bytes_to_frames(runtime, addr)/2;
	}
	
	return 0;
}

static void aml_pcm_timer_callback(unsigned long data)
{
    struct snd_pcm_substream *substream = (struct snd_pcm_substream *)data;
    struct snd_pcm_runtime *runtime = substream->runtime;
    struct aml_runtime_data *prtd = runtime->private_data;
		audio_stream_t *s = &prtd->s;

    unsigned int last_ptr, size;

		if(substream->stream == SNDRV_PCM_STREAM_PLAYBACK){
				if(s->active == 1){
						spin_lock(&s->lock);
						last_ptr = read_i2s_rd_ptr();
						if (last_ptr < s->last_ptr) {
				        size = runtime->dma_bytes + last_ptr - (s->last_ptr);
				    } else {
				        size = last_ptr - (s->last_ptr);
				    }
    				s->last_ptr = last_ptr;
    				s->size += bytes_to_frames(substream->runtime, size);
    				if (s->size >= runtime->period_size) {
				        s->size %= runtime->period_size;
				        spin_unlock(&s->lock);
				        snd_pcm_period_elapsed(substream);
				        spin_lock(&s->lock);
				    }
				    mod_timer(&prtd->timer, jiffies + 1);
   					spin_unlock(&s->lock);
				}else{
						 mod_timer(&prtd->timer, jiffies + 1);
				}
		}else{
				if(s->active == 1){
						spin_lock(&s->lock);
						last_ptr = (audio_in_i2s_wr_ptr() - s->I2S_addr) / 2;
						if (last_ptr < s->last_ptr) {
				        size = runtime->dma_bytes + last_ptr - (s->last_ptr);
				    } else {
				        size = last_ptr - (s->last_ptr);
				    }
    				s->last_ptr = last_ptr;
    				s->size += bytes_to_frames(substream->runtime, size);
    				if (s->size >= runtime->period_size) {
				        s->size %= runtime->period_size;
				        spin_unlock(&s->lock);
				        snd_pcm_period_elapsed(substream);
				        spin_lock(&s->lock);
				    }
				    mod_timer(&prtd->timer, jiffies + 1);
   					spin_unlock(&s->lock);
				}else{
						 mod_timer(&prtd->timer, jiffies + 1);
				}
		}    
}

static struct tasklet_struct audioin_tasklet;

static void audioin_int_tasklet(ulong data)
{
 
    printk("ADIN FIFO0 OVERFLOW\n");
  if(READ_MPEG_REG(AUDIN_FIFO_INT)&1){
   // printk("ADIN FIFO0 OVERFLOW\n");
  }
}

static irqreturn_t audioin_int_handler(int irq, void *dev_id)
{
    tasklet_schedule(&audioin_tasklet);

    printk("ADIN FIFO0 OVERFLOW---\n");
    
    return IRQ_HANDLED;
}

static int aml_pcm_open(struct snd_pcm_substream *substream)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct aml_runtime_data *prtd;
	int ret = 0;

	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK){
		snd_soc_set_runtime_hwparams(substream, &aml_pcm_hardware);
		//printk("pinmux audio out\n");
		//set_audio_pinmux(AUDIO_OUT_JTAG);
	}else{
		snd_soc_set_runtime_hwparams(substream, &aml_pcm_capture);
		//printk("pinmux audio in\n");
		//set_audio_pinmux(AUDIO_IN_JTAG);
        ret = request_irq(INT_AUDIO_IN, audioin_int_handler, IRQF_SHARED, "audioin", "audioin-id");
        if(ret){
          printk("request audio in INT failed\n");
        }
        tasklet_init(&audioin_tasklet, audioin_int_tasklet,0);
	}
	
	/* ensure that buffer size is a multiple of period size */
	ret = snd_pcm_hw_constraint_integer(runtime,
						SNDRV_PCM_HW_PARAM_PERIODS);
	if (ret < 0)
	{
		printk("set period error\n");
		goto out;
	}

	prtd = kzalloc(sizeof(struct aml_runtime_data), GFP_KERNEL);
	if (prtd == NULL) {
		printk("alloc aml_runtime_data error\n");
		ret = -ENOMEM;
		goto out;
	}
	
	prtd->pcm = substream->pcm;
	prtd->timer.function = &aml_pcm_timer_callback;
	prtd->timer.data = (unsigned long)substream;
	init_timer(&prtd->timer);
	
	runtime->private_data = prtd;
	
	spin_lock_init(&prtd->s.lock);
 out:
	return ret;
}

static int aml_pcm_close(struct snd_pcm_substream *substream)
{
	struct aml_runtime_data *prtd = substream->runtime->private_data;
	if(substream->stream == SNDRV_PCM_STREAM_CAPTURE){
      free_irq(INT_AUDIO_IN, "audioin-id");
    }
	del_timer_sync(&prtd->timer);
	
	kfree(prtd);
	
	return 0;
}

static int aml_pcm_copy_playback(struct snd_pcm_runtime *runtime, int channel,
		    snd_pcm_uframes_t pos,
		    void __user *buf, snd_pcm_uframes_t count)
{
		unsigned short *tfrom, *to, *left, *right;
    int res = 0;
    int n;
    int i = 0, j = 0;
    char *hwbuf = runtime->dma_area + frames_to_bytes(runtime, pos);
    
    tfrom = (unsigned short *)buf;
    to = (unsigned short *)hwbuf;
    n = frames_to_bytes(runtime, count);
//    printk("count=%d, n=%d\n", count, n);
    if(access_ok(VERIFY_READ, buf, frames_to_bytes(runtime, count))){
		    left = to;
		    right = to + 16;
		    if (pos % 16) {
		        printk("audio data unligned\n");
		    }
		    for (j = 0; j < n; j += 64) {
		        for (i = 0; i < 16; i++) {
	              *left++ = (*tfrom++) ;
	              *right++ = (*tfrom++);
		         }
		        left += 16;
		        right += 16;
		    }
		}else{
			res = -EFAULT;
		}
		
		return res;
}
		    

static int aml_pcm_copy_capture(struct snd_pcm_runtime *runtime, int channel,
		    snd_pcm_uframes_t pos,
		    void __user *buf, snd_pcm_uframes_t count)
{
		unsigned int *tfrom, *left, *right;
		unsigned short *to;
		int res = 0;
		int n;
    int i = 0, j = 0;
    char *hwbuf = runtime->dma_area + frames_to_bytes(runtime, pos)*2;
    char *magic = runtime->dma_area + runtime->dma_bytes*2 - 8;
    
    to = (unsigned short *)buf;
    tfrom = (unsigned int *)hwbuf;	// 32bit buffer
    n = frames_to_bytes(runtime, count);
//printk("hwbuf = %x, count=%x, n = %x, pos=%x\n", hwbuf, count, n, pos);
    unsigned int t1, t2;

		if(access_ok(VERIFY_WRITE, buf, frames_to_bytes(runtime, count))){
				left = tfrom;
		    right = tfrom + 8;
		    if (pos % 8) {
		        printk("audio data unligned\n");
		    }
		    if((n*2)%64){
		    		printk("audio data unaligned 64 bytes\n");
		    }
		    for (j = 0; j < n*2 ; j += 64) {
		        for (i = 0; i < 8; i++) {
		        	t1 = (*left++);
		        	t2 = (*right++);
		        	//printk("%08x,%08x,", t1, t2);
	              *to++ = (unsigned short)((t1>>8)&0xffff);
	              *to++ = (unsigned short)((t2>>8)&0xffff);
		         }
		         //printk("\n");
		        left += 8;
		        right += 8;
		    }
/*            
			if((hwbuf + n*2) >= magic && 
		  	    (magic[0]==0x78 && magic[1]==0x78 && magic[2]==0x78 && magic[3]==0x78 &&
			    magic[4]==0x78 && magic[5]==0x78 && magic[6]==0x78 && magic[7]==0x78)){
                printk("error:%x-%x-%x-%x-%x-%x-%x-%x: %x-%x-%x-%x-%x-%x-%x-%x\n",magic[-8],magic[-7],magic[-6], magic[-5], magic[-4], magic[-3], magic[-2], magic[-1], magic[0],magic[1],magic[2],magic[3],magic[4],magic[5],magic[6],magic[7]);				
		    }	
	    	if(hwbuf + n*2 >= magic){
					magic[0] = 0x78; magic[1] = 0x78; magic[2] = 0x78; magic[3] = 0x78;
					magic[4] = 0x78; magic[5] = 0x78; magic[6] = 0x78; magic[7] = 0x78;

					magic[-1] = 0x78; magic[-2] = 0x78; magic[-3] = 0x78; magic[-4] = 0x78;
					magic[-5] = 0x78; magic[-6] = 0x78; magic[-7] = 0x78; magic[-8] = 0x78;

	    	}
*/            
		}
	
		return res;
}

static int aml_pcm_copy(struct snd_pcm_substream *substream, int channel,
		    snd_pcm_uframes_t pos,
		    void __user *buf, snd_pcm_uframes_t count)
{
		unsigned short *tfrom, *to, *left, *right;
    int res = 0;
    int n;
    int i = 0, j = 0;
    
   // register unsigned  int vol =(audio_mixer_control.output_volume*(1<<VOLUME_SHIFT))/VOLUME_SCALE;
    struct snd_pcm_runtime *runtime = substream->runtime;
    char *hwbuf = runtime->dma_area + frames_to_bytes(runtime, pos);
 		
 		if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK){
 			return aml_pcm_copy_playback(runtime, channel,pos, buf, count);
 		}else{
 			return aml_pcm_copy_capture(runtime, channel,pos, buf, count);
 		}
} 		

int aml_pcm_silence(struct snd_pcm_substream *substream, int channel, 
		       snd_pcm_uframes_t pos, snd_pcm_uframes_t count)
{
		char* ppos;
		int n;
		struct snd_pcm_runtime *runtime = substream->runtime;
		
		n = frames_to_bytes(runtime, count);
		ppos = runtime->dma_area + frames_to_bytes(runtime, pos);
		memset(ppos, 0, n);
		
		return 0;
}
		       		    
static struct snd_pcm_ops aml_pcm_ops = {
	.open		= aml_pcm_open,
	.close		= aml_pcm_close,
	.ioctl		= snd_pcm_lib_ioctl,
	.hw_params	= aml_pcm_hw_params,
	.hw_free	= aml_pcm_hw_free,
	.prepare	= aml_pcm_prepare,
	.trigger	= aml_pcm_trigger,
	.pointer	= aml_pcm_pointer,
	.copy 		= aml_pcm_copy,
	.silence	=	aml_pcm_silence,
};


/*--------------------------------------------------------------------------*\
 * ASoC platform driver
\*--------------------------------------------------------------------------*/
static u64 aml_pcm_dmamask = 0xffffffff;

static int aml_pcm_new(struct snd_card *card,
	struct snd_soc_dai *dai, struct snd_pcm *pcm)
{
	int ret = 0;
	if (!card->dev->dma_mask)
		card->dev->dma_mask = &aml_pcm_dmamask;
	if (!card->dev->coherent_dma_mask)
		card->dev->coherent_dma_mask = 0xffffffff;

	if (dai->playback.channels_min) {
		ret = aml_pcm_preallocate_dma_buffer(pcm,
			SNDRV_PCM_STREAM_PLAYBACK);
		if (ret)
			goto out;
	}

	if (dai->capture.channels_min) {
		pr_debug("aml-pcm:"
				"Allocating PCM capture DMA buffer\n");
		ret = aml_pcm_preallocate_dma_buffer(pcm,
			SNDRV_PCM_STREAM_CAPTURE);
		if (ret)
			goto out;
	}
	
 out:
	return ret;
}

static void aml_pcm_free_dma_buffers(struct snd_pcm *pcm)
{
	struct snd_pcm_substream *substream;
	struct snd_dma_buffer *buf;
	int stream;
	for (stream = 0; stream < 2; stream++) {
		substream = pcm->streams[stream].substream;
		if (!substream)
			continue;

		buf = &substream->dma_buffer;
		if (!buf->area)
			continue;
		dma_free_coherent(pcm->card->dev, buf->bytes,
				  buf->area, buf->addr);
		buf->area = NULL;
	}
}

#ifdef CONFIG_PM
static int aml_pcm_suspend(struct snd_soc_dai *dai)
{
	struct snd_pcm_runtime *runtime = dai->runtime;
	struct aml_runtime_data *prtd;
	struct aml_pcm_dma_params *params;
	if (!runtime)
		return 0;

	prtd = runtime->private_data;
	params = prtd->params;

	/* disable the PDC and save the PDC registers */
	// TODO
	printk("aml pcm suspend\n");	

	return 0;
}

static int aml_pcm_resume(struct snd_soc_dai *dai)
{
	struct snd_pcm_runtime *runtime = dai->runtime;
	struct aml_runtime_data *prtd;
	struct aml_pcm_dma_params *params;
	if (!runtime)
		return 0;

	prtd = runtime->private_data;
	params = prtd->params;

	/* restore the PDC registers and enable the PDC */
	// TODO
	printk("aml pcm resume\n");
	return 0;
}
#else
#define aml_pcm_suspend	NULL
#define aml_pcm_resume	NULL
#endif

#ifdef CONFIG_DEBUG_FS

static struct dentry *debugfs_root;
static struct dentry *debugfs_regs;
static struct dentry *debugfs_mems;

static int regs_open_file(struct inode *inode, struct file *file)
{
	return 0;
}

/**
 *	cat regs
 */
static ssize_t regs_read_file(struct file *file, char __user *user_buf,
			       size_t count, loff_t *ppos)
{
	ssize_t ret;
	char *buf = kmalloc(PAGE_SIZE, GFP_KERNEL);
	if (!buf)
		return -ENOMEM;
		
	ret = sprintf(buf, "Usage: \n"
										 "	echo base reg val >regs\t(set the register)\n"
										 "	echo base reg >regs\t(show the register)\n"
										 "	base -> c(cbus), x(aix), p(apb), h(ahb) \n"
									);
		
	if (ret >= 0)
		ret = simple_read_from_buffer(user_buf, count, ppos, buf, ret);
	kfree(buf);	
	
	return ret;
}

static int read_regs(char base, int reg)
{
	int val = 0;
	switch(base){
		case 'c':
			val = READ_CBUS_REG(reg);
			break;
		case 'x':
			val = READ_AXI_REG(reg);
			break;
		case 'p':
			val = READ_APB_REG(reg);
			break;
		case 'h':
			val = READ_AHB_REG(reg);
			break;
		default:
			break;
	};
	printk("\tReg %x = %x\n", reg, val);
	return val;
}

static void write_regs(char base, int reg, int val)
{
	switch(base){
		case 'c':
			WRITE_CBUS_REG(reg, val);
			break;
		case 'x':
			WRITE_AXI_REG(reg, val);
			break;
		case 'p':
			WRITE_APB_REG(reg, val);
			break;
		case 'h':
			WRITE_AHB_REG(reg, val);
			break;
		default:
			break;
	};
	printk("Write reg:%x = %x\n", reg, val);
}
static ssize_t regs_write_file(struct file *file,
		const char __user *user_buf, size_t count, loff_t *ppos)
{
	char buf[32];
	int buf_size = 0;
	char *start = buf;
	unsigned long reg, value;
	int step = 1;
	char base;
	
	buf_size = min(count, (sizeof(buf)-1));
	
	if (copy_from_user(buf, user_buf, buf_size))
		return -EFAULT;
	buf[buf_size] = 0;
	while (*start == ' ')
		start++;
		
	base = *start;
	start ++;
	if(!(base =='c' || base == 'x' || base == 'p' || base == 'h')){
		return -EINVAL;
	}
	
	while (*start == ' ')
		start++;
		
	reg = simple_strtoul(start, &start, 16);
	
	while (*start == ' ')
		start++;
		
	if (strict_strtoul(start, 16, &value))
	{
			read_regs(base, reg);
			return -EINVAL;
	}
	
	write_regs(base, reg, value);
	
	return buf_size;
}

static const struct file_operations regs_fops = {
	.open = regs_open_file,
	.read = regs_read_file,
	.write = regs_write_file,
};

static int mems_open_file(struct inode *inode, struct file *file)
{
	return 0;
}
static ssize_t mems_read_file(struct file *file, char __user *user_buf,
			       size_t count, loff_t *ppos)
{
	ssize_t ret;
	char *buf = kmalloc(PAGE_SIZE, GFP_KERNEL);
	if (!buf)
		return -ENOMEM;
		
	ret = sprintf(buf, "Usage: \n"
										 "	echo vmem >mems\t(read 64 bytes from vmem)\n"
										 "	echo vmem val >mems (write int value to vmem\n"
									);
		
	if (ret >= 0)
		ret = simple_read_from_buffer(user_buf, count, ppos, buf, ret);
	kfree(buf);	
	
	return ret;
}

static ssize_t mems_write_file(struct file *file,
		const char __user *user_buf, size_t count, loff_t *ppos)
{
	char buf[256];
	int buf_size = 0;
	char *start = buf;
	unsigned long mem, value;
	int i=0;
	unsigned* addr = 0;
		
	buf_size = min(count, (sizeof(buf)-1));
	
	if (copy_from_user(buf, user_buf, buf_size))
		return -EFAULT;
	buf[buf_size] = 0;
	
	while (*start == ' ')
		start++;
	
	mem = simple_strtoul(start, &start, 16);
	
	while (*start == ' ')
		start++;
		
	if (strict_strtoul(start, 16, &value))
	{
			addr = (unsigned*)mem;
			printk("%p: ", addr);
			for(i = 0; i< 8; i++){
				printk("%08x, ", addr[i]);
			}
			printk("\n");
			return -EINVAL;
	}
	addr = (unsigned*)mem;
	printk("%p: %08x\n", addr, *addr);
	*addr = value;
	printk("%p: %08x^\n", addr, *addr);
	
	return buf_size;
}
static const struct file_operations mems_fops={
	.open = mems_open_file,
	.read = mems_read_file,
	.write = mems_write_file,
};

static void aml_pcm_init_debugfs()
{
		debugfs_root = debugfs_create_dir("aml",NULL);
		if (IS_ERR(debugfs_root) || !debugfs_root) {
			printk("aml: Failed to create debugfs directory\n");
			debugfs_root = NULL;
		}
		
		debugfs_regs = debugfs_create_file("regs", 0644, debugfs_root, NULL, &regs_fops);
		if(!debugfs_regs){
			printk("aml: Failed to create debugfs file\n");
		}
		
		debugfs_mems = debugfs_create_file("mems", 0644, debugfs_root, NULL, &mems_fops);
		if(!debugfs_mems){
			printk("aml: Failed to create debugfs file\n");
		}
}
static void aml_pcm_cleanup_debugfs()
{
	debugfs_remove_recursive(debugfs_root);
}
#else
static void aml_pcm_init_debugfs()
{
}
static void aml_pcm_cleanup_debugfs()
{
}
#endif

typedef struct {
	unsigned int in_op_ptr;
	unsigned int out_op_ptr;
	unsigned int type;
	unsigned int in_start;
	unsigned int out_start;	
}amaudio_t;

static ssize_t amaudio_write(struct file *file, const char *buf,
                                size_t count, loff_t * ppos)
{
	amaudio_t * amaudio = (amaudio_t *)file->private_data;
	int len = 0;

    if(count <=0 )
      return -EINVAL;
	if(amaudio->type == 1){
		if(!if_audio_in_i2s_enable()){
			printk("amaudio input can not write now\n");
			return -EINVAL;
		}
		copy_from_user((void*)(amaudio->in_op_ptr+amaudio->in_start), (void*)buf, count);
	}else{
		if(!if_audio_out_enable()){
			printk("amaudio output can not write now\n");
			return -EINVAL;
		}
		copy_from_user((void*)(amaudio->out_op_ptr+amaudio->out_start), (void*)buf, count);
	}
	return count;
}
static ssize_t amaudio_read(struct file *file, char __user *buf, 
															size_t count, loff_t * ppos)
{
	amaudio_t * amaudio = (amaudio_t *)file->private_data;
	int len = 0;

    if(count <= 0)
      return -EINVAL;
	if(amaudio->type == 1){
		if(!if_audio_in_i2s_enable()){
			printk("amaudio input can not read now\n");
			return -EINVAL;
		}
        len = copy_to_user((void*)buf, (void*)(amaudio->in_op_ptr+amaudio->in_start), count);
		if(len){
			printk("amaudio read i2s in data failed\n");
		}
        memset((void*)(amaudio->in_op_ptr+amaudio->in_start), 0x78, count);
	}
	else{
		if(!if_audio_out_enable()){
			printk("amaudio output can not read now\n");
			return -EINVAL;
		}

		len = copy_to_user((void*)buf, (void*)(amaudio->out_op_ptr+amaudio->out_start), count);
		if(len){
			printk("amaudio read i2s out data failed\n");
		}
	}
	return count - len;
}

static int amaudio_open(struct inode *inode, struct file *file)
{
		amaudio_t * amaudio = kzalloc(sizeof(amaudio_t), GFP_KERNEL);
		if (if_audio_in_i2s_enable()){
			//amaudio->in_start = ioremap_nocache(READ_MPEG_REG(AUDIN_FIFO0_START), 65536);
            amaudio->in_start = addr_audioin;
			printk("amaudio->in_start = %x \n", amaudio->in_start);
			printk("(AUDIN_FIFO0_START) = %x \n", READ_MPEG_REG(AUDIN_FIFO0_START));
		}
		if (if_audio_out_enable()){
			//amaudio->out_start = ioremap_nocache(READ_MPEG_REG(AIU_MEM_I2S_START_PTR), 32768);
            amaudio->out_start = addr_audioout;
			printk("amaudio->out_start = %x \n", amaudio->out_start);
			printk("(AIU_MEM_I2S_START_PTR) = %x \n", READ_MPEG_REG(AIU_MEM_I2S_START_PTR));
		}
		if(iminor(inode) == 0){ // audio out
				printk("open audio out\n");
				amaudio->type = 0;				
		}else{									// audio in
				printk("open audio in\n");
				amaudio->type = 1;
		}
		file->private_data = amaudio;
		return 0;
}
static int amaudio_release(struct inode *inode, struct file *file)
{
	amaudio_t * amaudio = (amaudio_t *)file->private_data;
	kfree(amaudio);
	if (if_audio_in_i2s_enable()){
	//	iounmap(amaudio->in_start);
	}
	if (if_audio_out_enable()){
	//	iounmap(amaudio->out_start);
	}	
	return 0;
}
static int amaudio_ioctl(struct inode *inode, struct file *file,
                        unsigned int cmd, ulong arg)
{
	s32 r = 0;
	amaudio_t * amaudio = (amaudio_t *)file->private_data;
    switch(cmd){
		case AMAUDIO_IOC_GET_I2S_OUT_SIZE:
			if(if_audio_out_enable()){
				r = READ_MPEG_REG(AIU_MEM_I2S_END_PTR) - READ_MPEG_REG(AIU_MEM_I2S_START_PTR) + 64;
			}else{
				r = -EINVAL;
			}
			break;
		case AMAUDIO_IOC_GET_I2S_OUT_PTR:
			if(if_audio_out_enable()){
				r = read_i2s_rd_ptr() - READ_MPEG_REG(AIU_MEM_I2S_START_PTR);
			}else{
				r = -EINVAL;
			}
			break;
		case AMAUDIO_IOC_SET_I2S_OUT_OP_PTR:
			if(if_audio_out_enable()){
				if(arg < 0 || arg > (READ_MPEG_REG(AIU_MEM_I2S_END_PTR) - READ_MPEG_REG(AIU_MEM_I2S_START_PTR)+64)){
					r = -EINVAL;
				}else{
					amaudio->out_op_ptr = arg;
				}
			}else{
				r = -EINVAL;
			}
			break;
		case AMAUDIO_IOC_GET_I2S_IN_SIZE:
			if(if_audio_in_i2s_enable()){
				r = READ_MPEG_REG(AUDIN_FIFO0_END) - READ_MPEG_REG(AUDIN_FIFO0_START) + 8;
			}else{
				r = -EINVAL;
			}
			break;
		case AMAUDIO_IOC_GET_I2S_IN_PTR:
			if(if_audio_in_i2s_enable()){
				r = audio_in_i2s_wr_ptr() - READ_MPEG_REG(AUDIN_FIFO0_START);
			}else{
				r = -EINVAL;
			}
			break;
		case AMAUDIO_IOC_SET_I2S_IN_OP_PTR:
			if(if_audio_in_i2s_enable()){
				if(arg < 0 || arg > (READ_MPEG_REG(AUDIN_FIFO0_END)- READ_MPEG_REG(AUDIN_FIFO0_START)+8)){
					r = -EINVAL;
				}else{
					amaudio->in_op_ptr = arg;
				}
			}else{
				r = -EINVAL;
			}
			break;
		default:
			break;
		
	};
	return r;
}
														
static const struct file_operations amaudio_fops =
{
	.owner	= THIS_MODULE,
	.open	= amaudio_open,
	.release  = amaudio_release,
	.read 		= amaudio_read,
	.write		= amaudio_write,
	.ioctl    = amaudio_ioctl,
};

struct snd_soc_platform aml_soc_platform = {
	.name		= "aml-audio",
	.pcm_ops 	= &aml_pcm_ops,
	.pcm_new	= aml_pcm_new,
	.pcm_free	= aml_pcm_free_dma_buffers,
	.suspend	= aml_pcm_suspend,
	.resume		= aml_pcm_resume,
};

EXPORT_SYMBOL_GPL(aml_soc_platform);

static struct class *aml_pcm_class;
static int __init aml_alsa_audio_init(void)
{
		int res = 0;
		aml_pcm_init_debugfs();
		
		res = register_chrdev(AMAUDIO_MAJOR, "amaudio", &amaudio_fops);
		if(res<0){
			printk("register audio out chardev failed!!!\n");
			goto error;
		}
		
		aml_pcm_class = class_create(THIS_MODULE, "amaudio");
		device_create(aml_pcm_class, NULL,MKDEV(AMAUDIO_MAJOR, 0), NULL, "amaudio_out");
		device_create(aml_pcm_class, NULL,MKDEV(AMAUDIO_MAJOR, 1), NULL, "amaudio_in");
		
		return snd_soc_register_platform(&aml_soc_platform);

error:
		unregister_chrdev(AMAUDIO_MAJOR,"amaudio");		
		
		device_destroy(aml_pcm_class, MKDEV(AMAUDIO_MAJOR, 0));
		device_destroy(aml_pcm_class, MKDEV(AMAUDIO_MAJOR, 1));
		class_destroy(aml_pcm_class);
		
		snd_soc_unregister_platform(&aml_soc_platform);
		
		return res;
}

static void __exit aml_alsa_audio_exit(void)
{
		unregister_chrdev(AMAUDIO_MAJOR,"audio_out");
		device_destroy(aml_pcm_class, MKDEV(AMAUDIO_MAJOR, 0));
		device_destroy(aml_pcm_class, MKDEV(AMAUDIO_MAJOR, 1));
		class_destroy(aml_pcm_class);
		
		aml_pcm_cleanup_debugfs();
        snd_soc_unregister_platform(&aml_soc_platform);
}

module_init(aml_alsa_audio_init);
module_exit(aml_alsa_audio_exit);

MODULE_AUTHOR("AMLogic, Inc.");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("AML driver for ALSA");

//module_param(id, charp, 0444);
MODULE_PARM_DESC(id, "ID string for AML soundcard.");
