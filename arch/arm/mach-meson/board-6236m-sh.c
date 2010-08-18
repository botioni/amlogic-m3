/*
 *
 * arch/arm/mach-meson/meson.c
 *
 *  Copyright (C) 2010 AMLOGIC, INC.
 *
 * License terms: GNU General Public License (GPL) version 2
 * Platform machine definition.
 */
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <mach/hardware.h>
#include <mach/platform.h>
#include <mach/memory.h>
#include <mach/memory.h>
#include <mach/pinmux.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/mach/map.h>
#include <mach/am_regs.h>
#include <mach/am_eth_pinmux.h>
#include <asm/setup.h>
#include <linux/delay.h>
#ifdef CONFIG_CACHE_L2X0
#include <asm/hardware/cache-l2x0.h>
#endif
#include <mach/pinmux.h>
#include <mach/gpio.h>
#include "board-6236m-sh.h"
#include <mach/clk_set.h>

#if defined(CONFIG_JPEGLOGO)
static struct resource jpeglogo_resources[] = {
    [0] = {
        .start = CONFIG_JPEGLOGO_ADDR,
        .end   = CONFIG_JPEGLOGO_ADDR + CONFIG_JPEGLOGO_SIZE - 1,
        .flags = IORESOURCE_MEM,
    },
    [1] = {
        .start = CODEC_ADDR_START,
        .end   = CODEC_ADDR_END,
        .flags = IORESOURCE_MEM,
    },
};

static struct platform_device jpeglogo_dev = {
	.name = "jpeglogo-dev",
	.id   = 0,
    .num_resources = ARRAY_SIZE(jpeglogo_resources),
    .resource      = jpeglogo_resources,
};
#endif
static struct resource intput_resources[] = {
	{
		.start = 0x0,
		.end = 0x0,
		.name="6236",
		.flags = IORESOURCE_IO,
	},
};

static struct platform_device input_device = {
	.name = "m1-kp",
	.id = 0,
	.num_resources = ARRAY_SIZE(intput_resources),
	.resource = intput_resources,
	
};
#ifdef CONFIG_FB_AM
static struct resource fb_device_resources[] = {
    [0] = {
        .start = OSD1_ADDR_START,
        .end   = OSD1_ADDR_END,
        .flags = IORESOURCE_MEM,
    },
    [1] ={ //for osd2
        .start = OSD2_ADDR_START,
        .end   =OSD2_ADDR_END,
        .flags = IORESOURCE_MEM,
    },
};

static struct platform_device fb_device = {
    .name       = "apollofb",
    .id         = 0,
    .num_resources = ARRAY_SIZE(fb_device_resources),
    .resource      = fb_device_resources,
};
#endif

#if defined(CONFIG_AM_STREAMING)
static struct resource apollo_codec_resources[] = {
    [0] = {
        .start =  CODEC_ADDR_START,
        .end   = CODEC_ADDR_END,
        .flags = IORESOURCE_MEM,
    },
};

static struct platform_device apollo_codec = {
    .name       = "amstream",
    .id         = 0,
    .num_resources = ARRAY_SIZE(apollo_codec_resources),
    .resource      = apollo_codec_resources,
};
#endif
static struct resource apollo_audiodsp_resources[] = {
    [0] = {
        .start =  AUDIODSP_ADDR_START,
        .end   = AUDIODSP_ADDR_END,
        .flags = IORESOURCE_MEM,
    },
};

static struct platform_device apollo_audiodsp = {
    .name       = "audiodsp",
    .id         = 0,
    .num_resources = ARRAY_SIZE(apollo_audiodsp_resources),
    .resource      = apollo_audiodsp_resources,
};
static struct resource apollo_mali_resources[] = {
    [0] = {
        .start = MALI_ADDR_START,
        .end   = MALI_ADDR_END,
        .flags = IORESOURCE_MEM,
    },
};

static struct platform_device apollo_mali= {
    .name       = "mali400",
    .id         = 0,
    .num_resources = ARRAY_SIZE(apollo_mali_resources),
    .resource      = apollo_mali_resources,
};


static struct platform_device __initdata *platform_devs[] = {
    #if defined(CONFIG_JPEGLOGO)
		&jpeglogo_dev,
	#endif
    #if defined(CONFIG_KEYPADS_AM)||defined(CONFIG_VIRTUAL_REMOTE)
	&input_device,
    #endif		
    #if defined(CONFIG_FB_AM)
    	&fb_device,
    #endif
    #if defined(CONFIG_AM_STREAMING)
	&apollo_codec,
    #endif
	&apollo_audiodsp,
	&apollo_mali,
};

static void eth_pinmux_init(void)
{
    	//eth_clk_set(ETH_CLKSRC_SYS_D3,900*CLK_1M/3,50*CLK_1M);
	/*for dpf_sz with ethernet*/	
    	eth_set_pinmux(ETH_BANK0_GPIOC3_C12,ETH_CLK_OUT_GPIOC12_REG3_1,0);
	CLEAR_CBUS_REG_MASK(PREG_ETHERNET_ADDR0, 1);
	SET_CBUS_REG_MASK(PREG_ETHERNET_ADDR0, (1 << 1));
	SET_CBUS_REG_MASK(PREG_ETHERNET_ADDR0, 1);
	udelay(100);
	/*reset*/
	set_gpio_mode(PREG_FGPIO,0,GPIO_OUTPUT_MODE);
	set_gpio_val(PREG_FGPIO,0,0);
	udelay(100);	//waiting reset end;
	set_gpio_val(PREG_FGPIO,0,1);
}
static void __init device_pinmux_init(void )
{
	clearall_pinmux();
	//uart_set_pinmux(UART_PORT_A,UART_A_GPIO_C21_D22);
	uart_set_pinmux(UART_PORT_B,UART_B_GPIO_C13_C14);
	/*pinmux of eth*/
	eth_pinmux_init();
}

static void __init  device_clk_setting(void)
{
	/*Demod CLK for eth and sata*/
	demod_apll_setting(0,1200*CLK_1M);
	/*eth clk*/

    	//eth_clk_set(ETH_CLKSRC_SYS_D3,900*CLK_1M/3,50*CLK_1M);
    	eth_clk_set(ETH_CLKSRC_APLL_CLK,400*CLK_1M,50*CLK_1M);
}

static __init void m1_init_machine(void)
{
#ifdef CONFIG_CACHE_L2X0
		/* 128kb (16KB/way), 8-way associativity, evmon/parity/share disabled
		 * Bits:  .... .... .000 0010 0000 .... .... .... */
		l2x0_init((void __iomem *)IO_PL310_BASE, 0x00020000, 0xff800fff);
#endif
	device_clk_setting();
	device_pinmux_init();
	platform_add_devices(platform_devs, ARRAY_SIZE(platform_devs));
}
/*VIDEO MEMORY MAPING*/
static __initdata struct map_desc meson_video_mem_desc[] = {
	{
		/*FIXME:map the video memory to other.*/
		.virtual		=	PAGE_ALIGN(VIDEO_MEM_START),
		.pfn			= 	__phys_to_pfn(VIDEO_MEM_START),
		.length		= 	VIDEO_MEM_END-VIDEO_MEM_START+1,
		.type		= 	MT_DEVICE,
	},
};

static __init void m1_map_io(void)
{
	meson_map_io();
	iotable_init(meson_video_mem_desc, ARRAY_SIZE(meson_video_mem_desc));
}

static __init void m1_irq_init(void)
{
	meson_init_irq();
}

static __init void m1_fixup(struct machine_desc *mach, struct tag *tag, char **cmdline, struct meminfo *m)
{
	struct membank *pbank;
	m->nr_banks=0;
	pbank=&m->bank[m->nr_banks];
	pbank->start = PAGE_ALIGN(PHYS_MEM_START);
	pbank->size  = SZ_64M & PAGE_MASK;
	pbank->node  = PHYS_TO_NID(PHYS_MEM_START);
	m->nr_banks++;
	pbank=&m->bank[m->nr_banks];
	pbank->start = PAGE_ALIGN(VIDEO_MEM_END+1);
	pbank->size  = (PHYS_MEM_END-VIDEO_MEM_END) & PAGE_MASK;
	pbank->node  = PHYS_TO_NID(VIDEO_MEM_END+1);
	m->nr_banks++;
}

MACHINE_START(MESON_6236M_SH, "AMLOGIC MESON-M1-SH 6236M")
	.phys_io		= MESON_PERIPHS1_PHYS_BASE,
	.io_pg_offst	= (MESON_PERIPHS1_PHYS_BASE >> 18) & 0xfffc,
	.boot_params	= BOOT_PARAMS_OFFSET,
	.map_io			= m1_map_io,
	.init_irq		= m1_irq_init,
	.timer			= &meson_sys_timer,
	.init_machine	= m1_init_machine,
	.fixup		=m1_fixup,
	.video_start	=VIDEO_MEM_START,
	.video_end	=VIDEO_MEM_END,
MACHINE_END
