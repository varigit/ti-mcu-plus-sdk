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

#include <psc.h>
#include <lib/container_of.h>
#include <device.h>
#include <types/errno.h>
#include <types/array_size.h>
#include <lib/trace.h>

static u32 soc_device_get_state_internal(const struct soc_device_data *dev)
{
	struct device *psc_dev = psc_lookup((psc_idx_t) dev->psc_idx);
	u32 ret = 1;

	if (psc_dev) {
		struct lpsc_module *module;
		module = psc_lookup_lpsc(psc_dev, dev->mod);
		if (module) {
			ret = lpsc_module_get_state(psc_dev, module);
		}
	}

	return ret;
}

u32 soc_device_get_state(struct device *dev)
{
	const struct dev_data *data = get_dev_data(dev);
	u32 ret = 2;

	if (data->soc.psc_idx == PSC_DEV_MULTIPLE) {
		u32 i;
		const struct soc_device_data *domains;
		domains = soc_psc_multiple_domains[data->soc.mod];
		for (i = 0; domains[i].psc_idx != PSC_DEV_NONE; i++) {
			u32 this_ret;
			this_ret = soc_device_get_state_internal(&domains[i]);
			if (i == 0U) {
				ret = this_ret;
			} else if (ret != this_ret) {
				/* Mixed state of our domains, label as transition */
				ret = 2;
			}
		}
	} else {
		ret = soc_device_get_state_internal(&data->soc);
	}

	return ret;
}

/*
 * FIXME: Reset ISO and local reset are per module, but those are often
 * shared by several devices.
 */
static void soc_device_set_reset_iso_internal(const struct soc_device_data *dev, sbool enable)
{
	struct device *psc_dev = psc_lookup((psc_idx_t) dev->psc_idx);

	if (psc_dev) {
		struct lpsc_module *module;
		module = psc_lookup_lpsc(psc_dev, dev->mod);
		if (module) {
			lpsc_module_set_reset_iso(psc_dev, module, enable);
		}
	}
}

void soc_device_set_reset_iso(struct device *dev, sbool enable)
{
	const struct dev_data *data = get_dev_data(dev);

	if (data->soc.psc_idx == PSC_DEV_MULTIPLE) {
		/* Reset ISO not supported for devices with multiple domains */
	} else {
		soc_device_set_reset_iso_internal(&data->soc, enable);
	}
}

static sbool soc_device_get_reset_iso_internal(const struct soc_device_data *dev)
{
	struct device *psc_dev = psc_lookup((psc_idx_t) dev->psc_idx);
	sbool ret = SFALSE;

	if (psc_dev) {
		struct lpsc_module *module;
		module = psc_lookup_lpsc(psc_dev, dev->mod);
		if (module) {
			ret = lpsc_module_get_reset_iso(psc_dev, module);
		}
	}

	return ret;
}

sbool soc_device_get_reset_iso(struct device *dev)
{
	const struct dev_data *data = get_dev_data(dev);
	sbool ret = SFALSE;

	if (data->soc.psc_idx == PSC_DEV_MULTIPLE) {
		/* Reset ISO not supported for devices with multiple domains */
	} else {
		ret = soc_device_get_reset_iso_internal(&data->soc);
	}

	return ret;
}

static void soc_device_set_resets_internal(const struct soc_device_data *dev, u32 resets)
{
	struct device *psc_dev = psc_lookup((psc_idx_t) dev->psc_idx);

	if (psc_dev) {
		struct lpsc_module *module;
		module = psc_lookup_lpsc(psc_dev, dev->mod);
		if (module) {
			sbool local_reset = SFALSE;
			sbool module_reset = SFALSE;
			if ((resets & BIT(0)) != 0U) {
				local_reset = STRUE;
			}
			if ((resets & BIT(1)) != 0U) {
				module_reset = STRUE;
			}
			lpsc_module_set_local_reset(psc_dev, module, local_reset);
			lpsc_module_set_module_reset(psc_dev, module, module_reset);
		}
	}
}

void soc_device_set_resets(struct device *dev, u32 resets)
{
	const struct dev_data *data = get_dev_data(dev);

	if (data->soc.psc_idx == PSC_DEV_MULTIPLE) {
		/* Resets not supported for devices with multiple domains */
	} else {
		soc_device_set_resets_internal(&data->soc, resets);
	}
}

static u32 soc_device_get_resets_internal(const struct soc_device_data *dev)
{
	struct device *psc_dev = psc_lookup((psc_idx_t) dev->psc_idx);
	u32 resets = 0U;

	if (psc_dev) {
		struct lpsc_module *module;
		module = psc_lookup_lpsc(psc_dev, dev->mod);
		if (module) {
			sbool ret;
			ret = lpsc_module_get_local_reset(psc_dev, module);
			if (ret) {
				resets |= BIT(0);
			}
			ret = lpsc_module_get_module_reset(psc_dev, module);
			if (ret) {
				resets |= BIT(1);
			}
		}
	}

	return resets;
}

u32 soc_device_get_resets(struct device *dev)
{
	const struct dev_data *data = get_dev_data(dev);
	u32 resets = 0;

	if (data->soc.psc_idx == PSC_DEV_MULTIPLE) {
		/* Resets not supported for devices with multiple domains */
	} else {
		resets = soc_device_get_resets_internal(&data->soc);
	}

	return resets;
}

static u32 soc_device_get_context_loss_count_internal(const struct soc_device_data *dev)
{
	struct device *psc_dev = psc_lookup((psc_idx_t) dev->psc_idx);
	u32 ret = 0;

	if (psc_dev) {
		struct lpsc_module *module;
		module = psc_lookup_lpsc(psc_dev, dev->mod);
		if (module) {
			ret = module->loss_count;
		}
	}

	return ret;
}

u32 soc_device_get_context_loss_count(struct device *dev)
{
	const struct dev_data *data = get_dev_data(dev);
	u32 ret = 0;

	if (data->soc.psc_idx == PSC_DEV_MULTIPLE) {
		u32 i;
		const struct soc_device_data *domains;
		domains = soc_psc_multiple_domains[data->soc.mod];
		for (i = 0; domains[i].psc_idx != PSC_DEV_NONE; i++) {
			ret += soc_device_get_context_loss_count_internal(&domains[i]);
		}
	} else {
		ret = soc_device_get_context_loss_count_internal(&data->soc);
	}

	return ret;
}

static void soc_device_enable_internal(const struct soc_device_data *dev)
{
	struct device *psc_dev = psc_lookup((psc_idx_t) dev->psc_idx);

	if (psc_dev) {
		struct lpsc_module *module;
		module = psc_lookup_lpsc(psc_dev, dev->mod);
		if (module) {
			lpsc_module_get(psc_dev, module);
		}
	}
}

void soc_device_enable(struct device *dev)
{
	const struct dev_data *data = get_dev_data(dev);

	pm_trace(TRACE_PM_ACTION_DEVICE_ON, device_id(dev));

	if (data->soc.psc_idx == PSC_DEV_MULTIPLE) {
		u32 i;
		const struct soc_device_data *domains;
		domains = soc_psc_multiple_domains[data->soc.mod];
		for (i = 0; domains[i].psc_idx != PSC_DEV_NONE; i++) {
			soc_device_enable_internal(&domains[i]);
		}
	} else {
		soc_device_enable_internal(&data->soc);
	}
}

static void soc_device_disable_internal(const struct soc_device_data *dev)
{
	struct device *psc_dev = psc_lookup((psc_idx_t) dev->psc_idx);

	if (psc_dev) {
		struct lpsc_module *module;
		module = psc_lookup_lpsc(psc_dev, dev->mod);
		if (module) {
			lpsc_module_put(psc_dev, module);
		}
	}
}

/**
 * @brief This function clear all flags associated with the module and its dependent modules
 *
 * @param psc_dev PSC associated with module
 * @param module module which the flag needs to be cleared
 */
static void soc_device_disable_internal_flags_iterate(struct device *psc_dev, struct lpsc_module *module)
{
	const struct psc_drv_data *psc = to_psc_drv_data(get_drv_data(psc_dev));
	u32 idx;

	if (module != NULL) {
		module->use_count = 0U;
		module->ret_count = 0U;
		module->pwr_up_enabled = (u8) SFALSE;
		module->pwr_up_ret = (u8) SFALSE;
		module->sw_state = 0;
		module->loss_count = 0;
		module->mrst_active = SFALSE;
		module->sw_mrst_ret = (u8) SFALSE;
		for (idx = 0U; idx < ARRAY_SIZE(psc->data->mods_enabled); idx++) {
			psc->data->mods_enabled[idx] = 0U;
		}
	}
	for (idx = 0U; idx < psc->pd_count; idx++) {
		struct psc_pd *pd = psc->powerdomains + idx;
		pd->use_count = 0U;
		pd->pwr_up_enabled = 0U;
	}
	for (idx = 0; idx < psc->module_count; idx++) {
		struct lpsc_module *temp = psc->modules + idx;
		temp->use_count = 0U;
		temp->ret_count = 0U;
		temp->pwr_up_enabled = (u8) SFALSE;
		temp->pwr_up_ret = (u8) SFALSE;
		temp->sw_state = 0;
		temp->sw_mrst_ret = (u8) SFALSE;
		temp->loss_count = 0;
		temp->mrst_active = SFALSE;
	}

	psc->data->pds_enabled = 0U;
	idx = lpsc_module_idx(psc_dev, module);
	const struct lpsc_module_data *data = psc->mod_data + idx;

	if ((data->flags & LPSC_DEPENDS) != 0UL) {
		const struct psc_drv_data *depends_psc = psc;
		struct device *depends_dev = psc_dev;

		depends_dev = psc_lookup((psc_idx_t) data->depends_psc_idx);
		depends_psc = to_psc_drv_data(get_drv_data(depends_dev));

		if (depends_dev && module) {
			module = depends_psc->modules + (lpsc_idx_t) data->depends;
			soc_device_disable_internal_flags_iterate(depends_dev, module);
		}
	}
}

/**
 * @brief Clear all initialization flags associated with a device
 *
 * @param dev The device that flags needs to be cleared
 */
static void soc_device_disable_internal_flags(const struct soc_device_data *dev)
{
	struct device *psc_dev = psc_lookup((psc_idx_t) dev->psc_idx);

	if (psc_dev != NULL) {
		struct lpsc_module *module;
		module = psc_lookup_lpsc(psc_dev, dev->mod);

		soc_device_disable_internal_flags_iterate(psc_dev, module);
	}
}

void soc_device_disable(struct device *dev, sbool domain_reset)
{
	const struct dev_data *data = get_dev_data(dev);

	pm_trace(TRACE_PM_ACTION_DEVICE_OFF, device_id(dev));

	if (domain_reset) {
		/* No action */
	} else if (data->soc.psc_idx == PSC_DEV_MULTIPLE) {
		u32 i;
		const struct soc_device_data *domains;
		domains = soc_psc_multiple_domains[data->soc.mod];
		for (i = 0; domains[i].psc_idx != PSC_DEV_NONE; i++) {
			soc_device_disable_internal(&domains[i]);
		}
	} else {
		soc_device_disable_internal(&data->soc);
	}
}

void soc_device_clear_flags(struct device *dev)
{
	const struct dev_data *data = get_dev_data(dev);

	pm_trace(TRACE_PM_ACTION_DEVICE_OFF, device_id(dev));

	if (data->soc.psc_idx == PSC_DEV_MULTIPLE) {
		u32 i;
		const struct soc_device_data *domains;
		domains = soc_psc_multiple_domains[data->soc.mod];
		for (i = 0; domains[i].psc_idx != PSC_DEV_NONE; i++) {
			soc_device_disable_internal_flags(&domains[i]);
		}
	} else {
		soc_device_disable_internal_flags(&data->soc);
	}
}

static void soc_device_ret_enable_internal(const struct soc_device_data *dev)
{
	struct device *psc_dev = psc_lookup((psc_idx_t) dev->psc_idx);

	if (psc_dev) {
		struct psc_pd *pd;
		struct lpsc_module *module;

		pd = psc_lookup_pd(psc_dev, (pd_idx_t) dev->pd);
		if (pd) {
			psc_pd_get(psc_dev, pd);
		}

		module = psc_lookup_lpsc(psc_dev, dev->mod);
		if (module) {
			lpsc_module_ret_get(psc_dev, module);
		}
	}
}

void soc_device_ret_enable(struct device *dev)
{
	const struct dev_data *data = get_dev_data(dev);

	if (data->soc.psc_idx == PSC_DEV_MULTIPLE) {
		u32 i;
		const struct soc_device_data *domains;
		domains = soc_psc_multiple_domains[data->soc.mod];
		for (i = 0; domains[i].psc_idx != PSC_DEV_NONE; i++) {
			soc_device_ret_enable_internal(&domains[i]);
		}
	} else {
		soc_device_ret_enable_internal(&data->soc);
	}
}

static void soc_device_ret_disable_internal(const struct soc_device_data *dev)
{
	struct device *psc_dev = psc_lookup((psc_idx_t) dev->psc_idx);

	if (psc_dev) {
		struct lpsc_module *module;
		struct psc_pd *pd;

		module = psc_lookup_lpsc(psc_dev, dev->mod);
		if (module) {
			lpsc_module_ret_put(psc_dev, module);
		}

		pd = psc_lookup_pd(psc_dev, (pd_idx_t) dev->pd);
		if (pd) {
			psc_pd_put(psc_dev, pd);
		}
	}
}

void soc_device_ret_disable(struct device *dev)
{
	const struct dev_data *data = get_dev_data(dev);

	if (data->soc.psc_idx == PSC_DEV_MULTIPLE) {
		u32 i;
		const struct soc_device_data *domains;
		domains = soc_psc_multiple_domains[data->soc.mod];
		for (i = 0; domains[i].psc_idx != PSC_DEV_NONE; i++) {
			soc_device_ret_disable_internal(&domains[i]);
		}
	} else {
		soc_device_ret_disable_internal(&data->soc);
	}
}

void soc_device_init_complete(void)
{
	psc_drop_pwr_up_ref();
}

static s32 soc_device_verify_mapping(const struct psc_drv_data *psc,
				     u32 dev_id, const struct soc_device_data *dev)
{
	s32 ret = 0;

	/*
	 * Make sure the redundant const data stored in the PSC data is
	 * correct. This redundant data would idealy be checked or even
	 * generated at compile time.
	 */
	if (dev->mod != PSC_LPSC_NONE) {
		const struct lpsc_module_data *mdata = psc->mod_data + dev->mod;
		u32 i;

		if ((mdata->flags & LPSC_DEVICES_LIST) != 0UL) {
			for (i = 0U; mdata->lpsc_dev.dev_list[i] != dev_id; i++) {
				if (mdata->lpsc_dev.dev_list[i] == DEV_ID_NONE) {
					ret = -EINVAL;
					break;
				}
			}
		} else {
			for (i = 0U; mdata->lpsc_dev.dev_array[i] != dev_id; i++) {
				if ((i >= (ARRAY_SIZE(mdata->lpsc_dev.dev_array) - 1UL)) ||
				    (mdata->lpsc_dev.dev_array[i] == DEV_ID_NONE)) {
					ret = -EINVAL;
					break;
				}
			}
		}
	}

	if (ret != 0L) {
		pm_trace(TRACE_PM_ACTION_INVALID_PSC_DATA | TRACE_PM_ACTION_FAIL, 0UL);
	}

	return ret;
}

static s32 soc_device_init_internal(struct device *dev)
{
	const struct dev_data *devdata;
	struct device *psc_dev = NULL;
	const struct psc_drv_data *psc = NULL;
	u32 dev_id;
	s32 ret = 0;

	dev_id = device_id(dev);
	devdata = get_dev_data(dev);

	/* Check if this PSC manages it's own power domain */
	if (devdata->flags & DEVD_FLAG_DRV_DATA) {
		const struct drv_data *drvdata;
		drvdata = to_drv_data(devdata);
		if (drvdata->drv == &psc_drv) {
			psc = to_psc_drv_data(drvdata);
			if (psc->psc_idx == devdata->soc.psc_idx) {
				psc_dev = dev;
			} else {
				psc = NULL;
			}
		}
	}

	if (psc) {
		/* We are our own PSC */
		ret = soc_device_verify_mapping(psc, dev_id, &devdata->soc);
	} else if (devdata->soc.psc_idx == PSC_DEV_MULTIPLE) {
		/* Find all the PSCs this device needs */
		u32 i;
		const struct soc_device_data *domains;
		domains = soc_psc_multiple_domains[devdata->soc.mod];
		for (i = 0; domains[i].psc_idx != PSC_DEV_NONE; i++) {
			psc_dev = psc_lookup((psc_idx_t) domains[i].psc_idx);
			if (ret != 0) {
				/* Do nothing */
			} else if (!psc_dev) {
				ret = -EDEFER;
			} else {
				psc = to_psc_drv_data(get_drv_data(psc_dev));
				ret = soc_device_verify_mapping(psc, dev_id, &domains[i]);
			}
		}
	} else {
		/* We just need the one PSC */
		psc_dev = psc_lookup((psc_idx_t) devdata->soc.psc_idx);
		if (!psc_dev) {
			/*
			 * Don't try to bring this dev up before calling init
			 * on the device's PSC.
			 */
			ret = -EDEFER;
		} else {
			psc = to_psc_drv_data(get_drv_data(psc_dev));
			ret = soc_device_verify_mapping(psc, dev_id, &devdata->soc);
		}
	}

	return ret;
}

/* Defer all other device intialization until PSC initializes */
s32 soc_device_init(struct device *dev)
{
	const struct dev_data *devdata;
	s32 ret = 0;

	devdata = get_dev_data(dev);

	if (devdata->soc.psc_idx != PSC_DEV_NONE) {
		ret = soc_device_init_internal(dev);
	}

	return ret;
}
