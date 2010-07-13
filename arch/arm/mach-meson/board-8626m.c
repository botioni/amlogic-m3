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
#include <asm/mach-types.h>
#include <asm/mach/arch.h>

#ifdef CONFIG_CACHE_L2X0
#include <asm/hardware/cache-l2x0.h>
#endif

static __init void m1_init_machine(void)
{
#ifdef CONFIG_CACHE_L2X0
		/* 128kb (16KB/way), 8-way associativity, evmon/parity/share disabled
		 * Bits:  .... .... .000 0010 0000 .... .... .... */
		l2x0_init((void __iomem *)IO_PL310_BASE, 0x00020000, 0xff800fff);
#endif

	/* todo: load device drivers */
}

static __init void m1_map_io(void)
{
	meson_map_io();
}

static __init void m1_irq_init(void)
{
	meson_init_irq();
}

static struct resource apollo_codec_resources[] = {
    [0] = {
        .start =  0x86000000,
        .end   = 0x88000000,
        .flags = IORESOURCE_MEM,
    },
};

static struct platform_device apollo_codec = {
    .name       = "amstream",
    .id         = 0,
    .num_resources = ARRAY_SIZE(apollo_codec_resources),
    .resource      = apollo_codec_resources,
};

static struct platform_device __initdata *platform_devs[] = {
	&apollo_codec,
};

int __init  platform_io_init(void)
{
 	printk("start platform init\r\n");
	platform_add_devices(platform_devs, ARRAY_SIZE(platform_devs));
	return 0;
}
arch_initcall(platform_io_init);

MACHINE_START(MESON_8626M, "AMLOGIC MESON-M1 8626M")
	.phys_io		= MESON_PERIPHS1_PHYS_BASE,
	.io_pg_offst	= (MESON_PERIPHS1_PHYS_BASE >> 18) & 0xfffc,
	.boot_params	= BOOT_PARAMS_OFFSET,
	.map_io			= m1_map_io,
	.init_irq		= m1_irq_init,
	.timer			= &meson_sys_timer,
	.init_machine	= m1_init_machine,
MACHINE_END
