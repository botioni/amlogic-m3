/*
 * Common power driver for Amlogic Devices with one or two external
 * power supplies (AC/USB) connected to main and backup batteries,
 * and optional builtin charger.
 *
 * Copyright © 2010 Larson Jiang <larson.jiang@amlogic.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __AML_POWER_H__
#define __AML_POWER_H__

#define AML_POWER_CHARGE_AC  (1 << 0)
#define AML_POWER_CHARGE_USB (1 << 1)

struct device;

struct aml_power_pdata {
	int (*init)(struct device *dev);
	int (*is_ac_online)(void);
	int (*is_usb_online)(void);
	int (*get_bat_vol)(void);
	int (*get_charge_status)(void);
	void (*set_charge)(int flags);
	void (*exit)(struct device *dev);

	char **supplied_to;
	size_t num_supplicants;

	unsigned int wait_for_status; /* msecs, default is 500 */
	unsigned int wait_for_charger; /* msecs, default is 500 */
	unsigned int polling_interval; /* msecs, default is 2000 */

	unsigned long ac_max_uA; /* current to draw when on AC */
};

#endif /* __AML_POWER_H__ */
