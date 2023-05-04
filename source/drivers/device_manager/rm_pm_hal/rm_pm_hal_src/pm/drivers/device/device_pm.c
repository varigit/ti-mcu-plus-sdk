/*
 * System Firmware
 *
 * Cortex-M3 (CM3) firmware for power management
 *
 * Copyright (C) 2015-2023, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 * its contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <types/errno.h>
#include <device.h>
#include <device_pm.h>
#include <device_clk.h>
#include <types/short_types.h>
#include <lib/trace.h>
#include <list.h>
#include <notifier.h>

static LIST_HEAD(device_notifiers);

static void device_notify(struct device *dev)
{
	notifier_call(&device_notifiers, dev);
}

void register_device_notifier(struct notifier *n)
{
	notifier_register(&device_notifiers, n);
}

void device_suspend(struct device *dev)
{
	const struct dev_data *data = get_dev_data(dev);

	if (data->flags & DEVD_FLAG_DRV_DATA) {
		const struct drv *drvp = to_drv_data(data)->drv;
		if (drvp && drvp->suspend) {
			drvp->suspend(dev);
		}
	}
}

/**
 * \brief Enables a device.
 *
 * Performs the steps necessary to enable a device.
 *
 * \param device
 * The device to modify.
 */
static void device_enable(struct device *dev)
{
	const struct dev_data *data = get_dev_data(dev);
	u32 i;

	for (i = 0UL; i < data->n_clocks; i++) {
		/* FIXME: Error handling */
		device_clk_enable(dev, i);
	}
	soc_device_enable(dev);

	device_notify(dev);
}

/**
 * \brief Disables a device.
 *
 * Performs the steps necessary to disable a device.
 *
 * \param device
 * The device to modify.
 * \param domain_reset
 * True if the device is being disabled due to a domain reset.
 */
void device_disable(struct device *dev, sbool domain_reset)
{
	const struct dev_data *data = get_dev_data(dev);
	s32 i;

	device_notify(dev);

	soc_device_disable(dev, domain_reset);
	for (i = (s32) data->n_clocks - 1L; i >= 0L; i--) {
		device_clk_disable(dev, i);
	}
}

/**
 * \brief Clear device init flags
 *
 * \param device The device to modify.
 *
 */
void device_clear_flags(struct device *dev)
{
	const struct dev_data *data = get_dev_data(dev);

	device_notify(dev);

	soc_device_clear_flags(dev);
}

void device_set_state(struct device *device, u8 host_idx, sbool enable)
{
	sbool was_enabled;
	sbool is_enabled;

	was_enabled = (device->flags & DEV_FLAG_ENABLED_MASK) != 0UL;

	if (enable) {
		device->flags |= DEV_FLAG_ENABLED(host_idx);
	} else {
		device->flags &= ~DEV_FLAG_ENABLED(host_idx);
	}

	/*
	 * As soon as any host gets or puts a device, we drop the power
	 * on enabled flag.
	 */
	if (host_idx != DEV_POWER_ON_ENABLED_HOST_IDX) {
		device->flags &= ~DEV_FLAG_POWER_ON_ENABLED;
	}

	is_enabled = (device->flags & DEV_FLAG_ENABLED_MASK) != 0UL;

	if (was_enabled != is_enabled) {
		if (is_enabled) {
			device_enable(device);
		} else {
			device_disable(device, SFALSE);
		}
	}
}

void device_set_retention(struct device *device, sbool retention)
{
	sbool is_retention = !!(device->flags & DEV_FLAG_RETENTION);

	if (retention == is_retention) {
		return;
	}

	if (retention) {
		device->flags |= DEV_FLAG_RETENTION;
		soc_device_ret_enable(device);
	} else {
		device->flags &= ~DEV_FLAG_RETENTION;
		soc_device_ret_disable(device);
	}
}
