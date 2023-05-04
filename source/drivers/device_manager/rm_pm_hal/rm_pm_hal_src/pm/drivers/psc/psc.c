/*
 * System Firmware
 *
 * Cortex-M3 (CM3) firmware for power management
 *
 * Copyright (C) 2014-2021, Texas Instruments Incorporated
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
#include <stddef.h>
#include <lib/container_of.h>
#include <types/errno.h>
#include <types/array_size.h>
#include <lib/ioremap.h>
#include <types/short_types.h>
#include <list.h>
#include <lib/trace.h>
#include <device.h>
#include <device_pm.h>
#include <resource.h>
#include <clk.h>
#include <pm_types.h>
#include <osal/osal_clock_user.h>
#include <config.h>

#define PSC_PID                 0x000U
#define PSC_GBLCTL              0x010U
#define PSC_GBLSTAT             0x014U
#define PSC_INTEVAL             0x018U
#define PSC_IPWKCNT             0x01cU
#define PSC_MERRPR0             0x040U
#define PSC_MERRPR1             0x044U
#define PSC_MERRCR0             0x050U
#define PSC_MERRCR1             0x054U
#define PSC_PERRPR              0x060U
#define PSC_PERRCR              0x068U
#define PSC_EPCPR               0x070U
#define PSC_EPCRR               0x078U
#define PSC_RAILSTAT            0x100U
#define PSC_RAILCTL             0x104U
#define PSC_RAILSET             0x108U
#define PSC_PTCMD               0x120U
#define PSC_PTCMD_H             0x124U
#define PSC_PTSTAT              0x128U
#define PSC_PTSTAT_H            0x12CU
#define PSC_PDSTAT(domain)      (0x200U + (4U * (domain)))
#define PSC_PDCTL(domain)       (0x300U + (4U * (domain)))
#define PSC_PDCFG(domain)       (0x400U + (4U * (domain)))
#define PSC_MDCFG(id)           (0x600U + (4U * (id)))
#define PSC_MDSTAT(id)          (0x800U + (4U * (id)))
#define PSC_MDCTL(id)           (0xa00U + (4U * (id)))

#define MDSTAT_STATE_MASK               0x3fU
#define MDSTAT_BUSY_MASK                0x30U
#define MDSTAT_STATE_SWRSTDISABLE       0x00U
#define MDSTAT_STATE_SYNCRST            0x01U
#define MDSTAT_STATE_DISABLE            0x02U
#define MDSTAT_STATE_ENABLE             0x03U
#define MDSTAT_STATE_AUTO_SLEEP         0x04U
#define MDSTAT_STATE_AUTO_WAKE          0x05U
#define MDSTAT_STATE_DISABLE_CLK_ON     0x21U
#define MDSTAT_STATE_DISABLE_IN_PROG    0x22U
#define MDSTAT_STATE_RETRY_DISABLE      0x23U
#define MDSTAT_STATE_ENABLE_IN_PROG     0x24U
#define MDSTAT_STATE_SLEEP_IN_PROG      0x25U
#define MDSTAT_STATE_CLK_ON1            0x26U
#define MDSTAT_STATE_CLK_OFF1           0x27U
#define MDSTAT_STATE_CLK_ON2            0x28U
#define MDSTAT_STATE_CLK_OFF2           0x29U
#define MDSTAT_STATE_CLK_ON3            0x2aU
#define MDSTAT_STATE_CLK_OFF3           0x2bU

#define MDSTAT_EMUIHB           BIT(17) /* EMU alters module state */
#define MDSTAT_EMURST           BIT(16) /* EMU alters reset to module */
#define MDSTAT_MCKOUT           BIT(12) /* Module clock output status */
#define MDSTAT_MRSTDONE         BIT(11) /* Module reset done */
#define MDSTAT_MRST             BIT(10) /* Module reset deasserted */
#define MDSTAT_LRSTDONE         BIT(9)  /* Local reset done */
#define MDSTAT_LRST             BIT(8)  /* Local reset deasserted */

#define MDCTL_FORCE             BIT(31)
#define MDCTL_RESET_ISO         BIT(12)         /* Enable reset isolation */
#define MDCTL_EMUIHBIE          BIT(10)         /* EMU alters module IE */
#define MDCTL_EMURSTIE          BIT(9)          /* EMU alter reset IE */
#define MDCTL_LRST              BIT(8)          /* Assert local reset when 0 */

#define PDSTAT_EMUIHB                   BIT(11) /* EMU alters domain state */
#define PDSTAT_PWRBAD                   BIT(10) /* Power bad error */
#define PDSTAT_PORDONE                  BIT(9)  /* Power on reset done */
#define PDSTAT_POR                      BIT(8)  /* Power on reset deasserted */
#define PDSTAT_STATE_MASK               0x1fU
#define PDSTAT_STATE_OFF                0x00U
#define PDSTAT_STATE_ON                 0x01U
#define PDSTAT_STATE_ON_ARB             0x10U
#define PDSTAT_STATE_SWITCH_ON          0x11U
#define PDSTAT_STATE_ON_RAIL_CNT        0x12U
#define PDSTAT_STATE_WAIT_ALL_SYNC_RST  0x13U
#define PDSTAT_STATE_STRETCH_POR        0x14U
#define PDSTAT_STATE_WAIT_POR_DONE      0x15U
#define PDSTAT_STATE_COUNT16            0x16U
#define PDSTAT_STATE_WAIT_ALL_SWRST_DIS 0x17U
#define PDSTAT_STATE_OFF_ARB            0x18U
#define PDSTAT_STATE_SWITCH_OFF         0x19U
#define PDSTAT_STATE_OFF_RAIL_CNT       0x1aU

#define PDCTL_STATE_MASK        BIT(0)
#define PDCTL_STATE_OFF         0U
#define PDCTL_STATE_ON          1U
#define PDCTL_EPCGOOD           BIT(8)  /* Externel power control on */
#define PDCTL_EMUIHBIE          BIT(9)  /* EMU alters domain state IE */
#define PDCTL_FORCE             BIT(31)

#define PSC_TRANSITION_TIMEOUT  10000

/* Extra delay time between powerdomain off and on in nanoseconds */
#define PSC_PTSTAT_ERRATA_DELAY_NS 10000U

static struct device *psc_devs;

static void lpsc_module_get_internal(struct device *dev, struct lpsc_module *module, sbool use, sbool ret);
static void lpsc_module_put_internal(struct device *dev, struct lpsc_module *module, sbool use, sbool ret);

#ifdef CONFIG_PSC_PTSTAT_ERRATA
/* Timestamp of last powerdomain poweroff for use in PTSTAT errata check */
static u64 psc_last_pd_off_ns;
#endif

static u32 psc_read(struct device *dev, u32 reg)
{
	const struct resource_mem *mem = device_resource_mem(dev, 0U);
	u32 ret = 0U;

	if (mem != NULL) {
		ret = mem_readl(mem, reg);
	}

	return ret;
}

static void psc_write(struct device *dev, u32 val, u32 reg)
{
	const struct resource_mem *mem = device_resource_mem(dev, 0U);

	if (mem != NULL) {
		mem_writel(mem, val, reg);
	}
}

pd_idx_t psc_pd_idx(struct device *dev, struct psc_pd *pd)
{
	const struct psc_drv_data *psc = to_psc_drv_data(get_drv_data(dev));

	return (pd_idx_t) (pd - psc->powerdomains);
}

static inline struct psc_pd *psc_idx2pd(const struct psc_drv_data	*psc,
					pd_idx_t			id)
{
	return psc->powerdomains + id;
}

static const struct psc_pd_data *get_psc_pd_data(struct device	*dev,
						 struct psc_pd	*pd)
{
	const struct psc_drv_data *psc = to_psc_drv_data(get_drv_data(dev));

	return psc->pd_data + psc_pd_idx(dev, pd);
}

lpsc_idx_t lpsc_module_idx(struct device *dev, struct lpsc_module *module)
{
	const struct psc_drv_data *psc = to_psc_drv_data(get_drv_data(dev));

	return (lpsc_idx_t) (module - psc->modules);
}

static inline struct lpsc_module *psc_idx2mod(const struct psc_drv_data *psc,
					      lpsc_idx_t		id)
{
	return psc->modules + id;
}

/* FIXME: Timeout behavior */
void psc_pd_wait(struct device *dev, struct psc_pd *pd)
{
	u32      psc_ptstat = PSC_PTSTAT;
	pd_idx_t psc_idx    = psc_pd_idx(dev, pd);

	if (!(get_psc_pd_data(dev, pd)->flags & PSC_PD_ALWAYSON)) {
		s32 i = PSC_TRANSITION_TIMEOUT;
		/* power domain >= 32 uses PSC_PTSTAT_H register */
		if (psc_idx >= 32U) {
			psc_ptstat = PSC_PTSTAT_H;
		}
		while (((psc_read(dev, psc_ptstat) &
			 BIT(psc_idx % 32U)) != 0U) && (--i != 0)) {
		}
		if (!i) {
			/* Directly convert to psc to get psc_idx */
			pm_trace(TRACE_PM_ACTION_PSC_TRANSITION_TIMEOUT | TRACE_PM_ACTION_FAIL,
				 (((to_psc_drv_data(get_drv_data(dev)))->psc_idx) << TRACE_PM_VAL_PSC_SHIFT) |
				 (psc_pd_idx(dev, pd) << TRACE_PM_VAL_PD_SHIFT) | TRACE_PM_VAL_PD_POS1);
		}
	}
}

static void pd_initiate(struct device *dev, struct psc_pd *pd)
{
	u32      psc_ptcmd = PSC_PTCMD;
	pd_idx_t psc_idx   = psc_pd_idx(dev, pd);
	/* power domain >= 32 uses PSC_PTCMD_H register */
	if (psc_idx >= 32U) {
		psc_ptcmd = PSC_PTCMD_H;
	}

	/* Note: This is a state machine reg */
	psc_write(dev, BIT(psc_idx % 32U), psc_ptcmd);
}

/**
 * \brief Enable clocks necessary for power domain
 *
 * This function calls clk_get on each of the clocks listed
 * under clock_dep for a given power domain. Some power domains
 * require certain clocks to be running while the power domain
 * is in transition or on.
 *
 * \param data
 * The const data for the power domain
 */
static void psc_pd_clk_get(const struct psc_pd_data *data)
{
	u32 i;

	for (i = 0U; i < ARRAY_SIZE(data->clock_dep); i++) {
		struct clk *clkp = clk_lookup(data->clock_dep[i]);
		if (clkp != NULL) {
			clk_get(clkp);
		}
	}
}

void psc_pd_get(struct device *dev, struct psc_pd *pd)
{
	const struct psc_drv_data *psc = to_psc_drv_data(get_drv_data(dev));
	u32 idx = psc_pd_idx(dev, pd);
	u32 pdctl;

	pm_trace(TRACE_PM_ACTION_PD_GET,
		 (psc->psc_idx << TRACE_PM_VAL_PSC_SHIFT) |
		 (idx << TRACE_PM_VAL_PD_SHIFT) |
		 (pd->use_count & TRACE_PM_VAL_MAX_PSC_DATA));

	if (pd->use_count++) {
		return;
	}

	if (psc->pd_data[idx].flags & PSC_PD_ALWAYSON) {
		return;
	}

	/* Verify any previous transitions have completed */
	psc_pd_wait(dev, pd);

	if (psc->pd_data[idx].flags & PSC_PD_DEPENDS) {
		psc_pd_get(dev, psc_idx2pd(psc,
					   (pd_idx_t) psc->pd_data[idx].depends));
	}

	psc_pd_clk_get(&psc->pd_data[idx]);

	pdctl = psc_read(dev, PSC_PDCTL(idx));

	if ((pdctl & PDCTL_STATE_MASK) != PDCTL_STATE_ON) {
		/* Avoid redundant power-up transitions */
		pdctl &= ~PDCTL_STATE_MASK;
		pdctl |= PDCTL_STATE_ON;

#ifdef CONFIG_PSC_PTSTAT_ERRATA
		/*
		 * Handle PTSTAT errata.
		 *
		 * We need to delay an extra 10uS before turning on a power
		 * domain after turning off that power domain. Note that in
		 * order to conserve memory, we only store one time stamp
		 * rather than a time stamp per powerdomain.
		 */
		if (psc_last_pd_off_ns != 0ULL) {
			u64 delta_ns;
			/*
			 * Calculate how long it's been since the last time
			 * we turned off a powerdomain.
			 * osal_timestamp64 returns a nanosecond timestamp
			 */
			delta_ns = osal_timestamp64() - psc_last_pd_off_ns;
			if (delta_ns < ((u64) PSC_PTSTAT_ERRATA_DELAY_NS)) {
				u32 delay_us;
				/*
				 * Calculate the delay we need to complete
				 * the wait period. The osal_delay argument
				 * is in microseconds. In the calculation we
				 * convert to microseconds and use the
				 * ceiling value to ensure we wait the
				 * minimum required time.
				 */
				delay_us = (PSC_PTSTAT_ERRATA_DELAY_NS - ((u32) delta_ns) + 999U) / 1000U;
				osal_delay(delay_us);
			}
			/*
			 * Reset the timer so we can avoid bothering with
			 * unnecessary osal_timestamp64 calls and comparisons.
			 */
			psc_last_pd_off_ns = 0ULL;
		}
#endif


		/* Note: This is a state machine reg */
		psc_write(dev, pdctl, PSC_PDCTL(idx));

		pd_initiate(dev, pd);
		psc_pd_wait(dev, pd);
	}

	psc->data->pds_enabled |= BIT(idx);
}

/**
 * \brief Disable clocks necessary for power domain
 *
 * This function calls clk_put on each of the clocks listed
 * under clock_dep for a given power domain. Some power domains
 * require certain clocks to be running while the power domain
 * is in transition or on.
 *
 * \param data
 * The const data for the power domain
 */
static void psc_pd_clk_put(const struct psc_pd_data *data)
{
	u32 i;

	for (i = 0U; i < ARRAY_SIZE(data->clock_dep); i++) {
		struct clk *clkp = clk_lookup(data->clock_dep[i]);
		if (clkp != NULL) {
			clk_put(clkp);
		}
	}
}

void psc_pd_put(struct device *dev, struct psc_pd *pd)
{
	const struct psc_drv_data *psc = to_psc_drv_data(get_drv_data(dev));
	u32 idx = psc_pd_idx(dev, pd);
	u32 pdctl;

	pm_trace(TRACE_PM_ACTION_PD_PUT,
		 (psc->psc_idx << TRACE_PM_VAL_PSC_SHIFT) |
		 (idx << TRACE_PM_VAL_PD_SHIFT) |
		 (pd->use_count & TRACE_PM_VAL_MAX_PSC_DATA));

	if (--pd->use_count) {
		return;
	}

	if (psc->pd_data[idx].flags & PSC_PD_ALWAYSON) {
		return;
	}

	/* Verify any previous transitions have completed */
	psc_pd_wait(dev, pd);

	pdctl = psc_read(dev, PSC_PDCTL(idx));
	if ((pdctl & PDCTL_STATE_MASK) != PDCTL_STATE_OFF) {
		/* Avoid redundant power-up transitions */
		pdctl &= ~PDCTL_STATE_MASK;
		pdctl |= PDCTL_STATE_OFF;
		/* Note: This is a state machine reg */
		psc_write(dev, pdctl, PSC_PDCTL(idx));

		pd_initiate(dev, pd);
		psc_pd_wait(dev, pd);

#ifdef CONFIG_PSC_PTSTAT_ERRATA
		/*
		 * Store the last powerdomain poweroff time for use in the
		 * PTSTAT errata check.
		 */
		psc_last_pd_off_ns = osal_timestamp64();
#endif
	}

	psc_pd_clk_put(&psc->pd_data[idx]);

	if (psc->pd_data[idx].flags & PSC_PD_DEPENDS) {
		psc_pd_put(dev, psc_idx2pd(psc,
					   (pd_idx_t) psc->pd_data[idx].depends));
	}

	psc->data->pds_enabled &= ~BIT(idx);
}

u32 psc_pd_get_state(struct device *dev, struct psc_pd *pd)
{
	u32      psc_ptstat = PSC_PTSTAT;
	pd_idx_t psc_idx    = psc_pd_idx(dev, pd);

	u8 state;

	/* power domain >= 32 uses PSC_PTSTAT_H register */
	if (psc_idx >= 32U) {
		psc_ptstat = PSC_PTSTAT_H;
	}

	if (psc_read(dev, psc_ptstat) & BIT(psc_idx % 32U)) {
		return 2;
	}

	state = (u8) (psc_read(dev, PSC_PDSTAT(psc_idx)) & PDSTAT_STATE_MASK);
	return (state == PDCTL_STATE_ON) ? 1U : 0U;
}

static void lpsc_module_notify_suspend(struct device		*dev,
				       struct lpsc_module	*mod)
{
	const struct psc_drv_data *psc = to_psc_drv_data(get_drv_data(dev));
	u32 i;
	u32 idx = lpsc_module_idx(dev, mod);
	const struct lpsc_module_data *data = psc->mod_data + idx;
	struct device *sub_dev;

	/*
	 * All our devices are going into reset/losing power, notify them so
	 * they can call their suspend functions.
	 *
	 * FIXME: Block transition if suspend fails
	 */
	if (data->flags & LPSC_DEVICES_LIST) {
		for (i = 0U; data->lpsc_dev.dev_list[i] != DEV_ID_NONE; i++) {
			sub_dev = device_lookup(data->lpsc_dev.dev_list[i]);
			if (sub_dev != NULL) {
				device_suspend(sub_dev);
			}
		}
	} else {
		for (i = 0U; (i < ARRAY_SIZE(data->lpsc_dev.dev_array)) &&
		     (data->lpsc_dev.dev_array[i] != DEV_ID_NONE); i++) {
			sub_dev = device_lookup(data->lpsc_dev.dev_array[i]);
			if (sub_dev != NULL) {
				device_suspend(sub_dev);
			}
		}
	}


	psc->data->mods_enabled[idx / 32UL] |= BIT(idx % 32UL);
}

static void lpsc_module_notify_resume(struct device		*dev,
				      struct lpsc_module	*module)
{
	const struct psc_drv_data *psc = to_psc_drv_data(get_drv_data(dev));
	u32 idx = lpsc_module_idx(dev, module);

	/* No resume functionality yet required */

	psc->data->mods_enabled[idx / 32UL] &= ~BIT(idx % 32UL);
}

/**
 * \brief Sync the hardware state of a module with it's software state.
 *
 * This function examines the current state of a given LPSC module and
 * compares it with the last programmed state. If there have been changes it
 * programs the hardware appropriately. It also enables/disables any
 * dependencies as necessary.
 *
 * \param dev
 * The device struct for this PSC.
 * \param module
 * The module data for this module.
 * \param domain_reset
 * True if we are syncing the state just prior to a domain reset. In this
 * case we avoid actual transitions.
 */
static void lpsc_module_sync_state(struct device	*dev,
				   struct lpsc_module	*module,
				   sbool		domain_reset)
{
	const struct psc_drv_data *psc = to_psc_drv_data(get_drv_data(dev));
	u32 mdctl;
	u32 idx = lpsc_module_idx(dev, module);
	const struct lpsc_module_data *data = psc->mod_data + idx;
	struct psc_pd *pd = psc_idx2pd(psc, (pd_idx_t) data->powerdomain);
	u8 state;               /* Target module state */
	u8 old_state;           /* Original module state */
	sbool new_mrst_ret;     /* MRST induced retention */
	sbool old_msrt_ret;
	sbool old_pwr;          /* Power domain should be enabled */
	sbool old_ret;          /* Retention of any kind */
	sbool old_en;           /* Enabled (clocks running) */
	sbool old_rst;
	sbool new_pwr;
	sbool new_ret;
	sbool new_en;
	sbool new_rst;
	sbool transition;       /* A state transition is necessary */
	sbool get_en;           /* Moving from disabled to enabled */
	sbool get_ret;          /* Moving from off to retention */
	sbool get_pwr;          /* Moving from power domain off to on */
	sbool put_en;           /* Moving from enabled to disabled */
	sbool put_ret;          /* Moving from retention to off */
	sbool put_pwr;          /* Moving from power domain on to off */

	/*
	 * Determine target state based on usage counts and module reset:
	 *
	 * +---------+-----------+-------+-------------------------------+
	 * + Enabled | Retention | Reset | State                         |
	 * +=========+===========+=======+===============================+
	 * | No      | No        | NA    | SwRstDisabled (may power off) |
	 * +---------+-----------+-------+-------------------------------+
	 * | No      | Yes       | No    | Disabled                      |
	 * +---------+-----------+-------+-------------------------------+
	 * | No      | Yes       | Yes   | SwRstDisabled (powered-on)    |
	 * +---------+-----------+-------+-------------------------------+
	 * | Yes     | NA        | No    | Enabled                       |
	 * +---------+-----------+-------+-------------------------------+
	 * | Yes     | NA        | Yes   | SyncReset                     |
	 * +---------+-----------+-------+-------------------------------+
	 */
	new_mrst_ret = SFALSE;
	if ((module->use_count == 0U) && (module->ret_count == 0U)) {
		state = MDSTAT_STATE_SWRSTDISABLE;
	} else if (module->use_count == 0U) {
		/* Retention enabled, but module disabled */
		if (module->mrst_active == 0U) {
			state = MDSTAT_STATE_DISABLE;
		} else {
			new_mrst_ret = STRUE;
			state = MDSTAT_STATE_SWRSTDISABLE;
		}
	} else {
		/* Module enabled (retention setting is don't care) */
		if (module->mrst_active == 0U) {
			state = MDSTAT_STATE_ENABLE;
		} else {
			state = MDSTAT_STATE_SYNCRST;
		}
	}

	/* Promote target state based on disallowed states */
	if ((state == MDSTAT_STATE_SWRSTDISABLE) && ((data->flags & LPSC_NO_MODULE_RESET) != 0U)) {
		state = MDSTAT_STATE_DISABLE;
		new_mrst_ret = SFALSE;
	}
	if ((state == MDSTAT_STATE_DISABLE) && ((data->flags & LPSC_NO_CLOCK_GATING) != 0U)) {
		state = MDSTAT_STATE_ENABLE;
	}
	if ((state == MDSTAT_STATE_SYNCRST) && ((data->flags & LPSC_NO_MODULE_RESET) != 0U)) {
		state = MDSTAT_STATE_ENABLE;
	}

	/* Track transition of old state to new state */
	old_state = module->sw_state;
	module->sw_state = state;
	old_msrt_ret = (sbool) module->sw_mrst_ret;
	module->sw_mrst_ret = (u8) new_mrst_ret;

	/* Previous setting of retention, enable, and reset */
	old_ret = old_state != MDSTAT_STATE_SWRSTDISABLE;
	old_pwr = old_ret || old_msrt_ret;
	old_en = (old_state == MDSTAT_STATE_SYNCRST) || (old_state == MDSTAT_STATE_ENABLE);
	old_rst = (old_state != MDSTAT_STATE_ENABLE) && (old_state != MDSTAT_STATE_DISABLE);

	/* New setting of retention, enable, and reset */
	new_ret = state != MDSTAT_STATE_SWRSTDISABLE;
	new_pwr = new_ret || new_mrst_ret;
	new_en = (state == MDSTAT_STATE_SYNCRST) || (state == MDSTAT_STATE_ENABLE);
	new_rst = (state != MDSTAT_STATE_ENABLE) && (state != MDSTAT_STATE_DISABLE);

	/* Are we transitioning from no retention/enable to retention/enable? */
	get_ret = !old_ret && new_ret;
	get_pwr = !old_pwr && new_pwr;
	get_en = !old_en && new_en;

	/* Are we transitioning from retention/enable to no retention/enable? */
	put_ret = old_ret && !new_ret;
	put_pwr = old_pwr && !new_pwr;
	put_en = old_en && !new_en;

	/* Notify of loss of functionality transitions before we do them */
	if (domain_reset) {
		/* no action */
	} else if (put_ret || put_en) {
		lpsc_module_notify_suspend(dev, module);
	}

	/* Make sure our parent LPSC is enabled as necessary */
	if ((get_en || get_ret) && ((data->flags & LPSC_DEPENDS) != 0UL)) {
		const struct psc_drv_data *depends_psc = psc;
		struct device *depends_dev = dev;
		if (data->depends_psc_idx != psc->psc_idx) {
			depends_dev = psc_lookup((psc_idx_t) data->depends_psc_idx);
			depends_psc = to_psc_drv_data(get_drv_data(depends_dev));
		}
		if (!depends_dev) {
			pm_trace(TRACE_PM_ACTION_PSC_INVALID_DEP_DATA | TRACE_PM_ACTION_FAIL,
				 (psc->psc_idx << TRACE_PM_VAL_PSC_SHIFT) |
				 (data->depends_psc_idx << TRACE_PM_VAL_PD_SHIFT) | TRACE_PM_VAL_PD_POS1);
		} else {
			/*
			 * Moving from a clock stop state to a clock enabled
			 * state or from a retention disable to a retention
			 * enabled state, bump the reference count on our
			 * dependency.
			 */
			lpsc_module_get_internal(depends_dev,
						 psc_idx2mod(depends_psc, data->depends),
						 get_en, get_ret);
		}
	}

	if (old_rst && !new_rst) {
		/* Coming out of reset, bump the context loss count. */
		module->loss_count++;
	}

	if (get_pwr) {
		/* Make sure pd is on. */
		psc_pd_get(dev, pd);
	}

	mdctl = psc_read(dev, PSC_MDCTL(idx));
	transition = (mdctl & MDSTAT_STATE_MASK) != state;
	if (transition && !domain_reset) {
		/* Verify any previous transitions have completed */
		psc_pd_wait(dev, pd);

		mdctl &= ~MDSTAT_STATE_MASK;
		mdctl |= (u32) state;
		/* Note: This is a state machine reg */
		psc_write(dev, mdctl, PSC_MDCTL(idx));
	}

	if (domain_reset) {
		/* Do nothing */
	} else if (put_pwr) {
		/* Module is ready for power down, drop ref count on pd */
		psc_pd_put(dev, pd);
		if ((pd->use_count != 0U) && transition) {
			/*
			 * If psc_pd_put has a use count of zero, it already
			 * initaited the transition, otherwise we need to
			 * do the transition ourselves.
			 */
			pd_initiate(dev, pd);
		}
	} else if (transition) {
		/*
		 * Initiate transition
		 * FIXME: defer (for coalescing) if wait not requested
		 */
		pd_initiate(dev, pd);
	}

	/* Notify of gain of functionality transitions after we do them */
	if (get_en || get_ret) {
		lpsc_module_notify_resume(dev, module);
	}

	/* Allow our parent LPSC to be disabled as necessary */
	if ((put_en || put_ret) && ((data->flags & LPSC_DEPENDS) != 0UL)) {
		const struct psc_drv_data *depends_psc = psc;
		struct device *depends_dev = dev;
		if (data->depends_psc_idx != psc->psc_idx) {
			depends_dev = psc_lookup((psc_idx_t) data->depends_psc_idx);
			depends_psc = to_psc_drv_data(get_drv_data(depends_dev));
		}
		if (!depends_dev) {
			pm_trace(TRACE_PM_ACTION_PSC_INVALID_DEP_DATA | TRACE_PM_ACTION_FAIL,
				 (psc->psc_idx << TRACE_PM_VAL_PSC_SHIFT) |
				 (data->depends_psc_idx << TRACE_PM_VAL_PD_SHIFT) | TRACE_PM_VAL_PD_POS2);
		} else if (domain_reset && (depends_dev == dev)) {
			/* Ignore self dependencies during domain reset */
		} else {
			/*
			 * Moving from a clock enabled state to a clock stop
			 * state or from a retention enable to a retention
			 * disabled state, drop the reference count on our
			 * dependency.
			 */
			lpsc_module_put_internal(depends_dev,
						 psc_idx2mod(depends_psc, data->depends),
						 put_en, put_ret);
		}
	}
}

u32 lpsc_module_get_state(struct device		*dev,
			  struct lpsc_module	*module)
{
	u32 idx = lpsc_module_idx(dev, module);
	u8 state;
	u32 ret;

	state = (u8) (psc_read(dev, PSC_MDCTL(idx)) & MDSTAT_STATE_MASK);

	if (state == MDSTAT_STATE_SWRSTDISABLE) {
		ret = 0U; /* Disabled */
	} else if ((state == MDSTAT_STATE_DISABLE) ||
		   (state == MDSTAT_STATE_ENABLE) ||
		   (state == MDSTAT_STATE_SYNCRST)) {
		ret = 1U;       /* Enabled or retention */
	} else {
		ret = 2U;       /* Transition (other) */
	}

	return ret;
}

/*
 * FIXME: Block context loss count increments if applicable when reset
 * iso is enabled.
 */
void lpsc_module_set_reset_iso(struct device *dev, struct lpsc_module *module,
			       sbool enable)
{
	const struct psc_drv_data *psc = to_psc_drv_data(get_drv_data(dev));
	u32 idx = lpsc_module_idx(dev, module);
	const struct lpsc_module_data *data = psc->mod_data + idx;
	sbool is_enabled;
	u32 mdctl;

	if (!(data->flags & LPSC_HAS_RESET_ISO)) {
		return;
	}

	mdctl = psc_read(dev, PSC_MDCTL(idx));
	is_enabled = !!(mdctl & MDCTL_RESET_ISO);

	if (enable != is_enabled) {
		if (enable) {
			mdctl |= MDCTL_RESET_ISO;
		} else {
			mdctl &= ~MDCTL_RESET_ISO;
		}
		/* Note: This is a state machine reg */
		psc_write(dev, mdctl, PSC_MDCTL(idx));
	}
}

sbool lpsc_module_get_reset_iso(struct device *dev, struct lpsc_module *module)
{
	const struct psc_drv_data *psc = to_psc_drv_data(get_drv_data(dev));
	u32 idx = lpsc_module_idx(dev, module);
	const struct lpsc_module_data *data = psc->mod_data + idx;

	if (!(data->flags & LPSC_HAS_RESET_ISO)) {
		return SFALSE;
	}

	return !!(psc_read(dev, PSC_MDCTL(idx)) & MDCTL_RESET_ISO);
}

/* Does not bump context loss count */
void lpsc_module_set_local_reset(struct device *dev,
				 struct lpsc_module *module, sbool enable)
{
	const struct psc_drv_data *psc = to_psc_drv_data(get_drv_data(dev));
	u32 idx = lpsc_module_idx(dev, module);
	const struct lpsc_module_data *data = psc->mod_data + idx;
	sbool is_enabled;
	u32 mdctl;

	if (!(data->flags & LPSC_HAS_LOCAL_RESET)) {
		return;
	}

	mdctl = psc_read(dev, (u32) PSC_MDCTL(idx));
	is_enabled = !(mdctl & MDCTL_LRST);

	if (enable != is_enabled) {
		pm_trace(TRACE_PM_ACTION_SET_LOCAL_RESET,
			 (psc->psc_idx << TRACE_PM_VAL_PSC_SHIFT) |
			 (idx << TRACE_PM_VAL_PD_SHIFT) |
			 (u32) enable);

		if (enable) {
			mdctl &= ~MDCTL_LRST;
		} else {
			mdctl |= MDCTL_LRST;
		}

		/* Note: This is a state machine reg */
		psc_write(dev, mdctl, PSC_MDCTL(idx));
	}
}

void lpsc_module_set_module_reset(struct device *dev,
				  struct lpsc_module *module, sbool enable)
{
	const struct psc_drv_data *psc = to_psc_drv_data(get_drv_data(dev));
	u32 idx = lpsc_module_idx(dev, module);
	const struct lpsc_module_data *data = &psc->mod_data[idx];
	sbool is_enabled;

	is_enabled = module->mrst_active != 0U;

	if ((enable != is_enabled) && ((data->flags & LPSC_NO_MODULE_RESET) == 0U)) {
		pm_trace(TRACE_PM_ACTION_SET_MODULE_RESET,
			 (psc->psc_idx << TRACE_PM_VAL_PSC_SHIFT) |
			 (idx << TRACE_PM_VAL_PD_SHIFT) |
			 (u32) enable);

		if (enable) {
			module->mrst_active = 1U;
		} else {
			module->mrst_active = 0U;
		}

		lpsc_module_sync_state(dev, module, SFALSE);
		lpsc_module_wait(dev, module);
	}
}

sbool lpsc_module_get_local_reset(struct device *dev, struct lpsc_module *module)
{
	const struct psc_drv_data *psc = to_psc_drv_data(get_drv_data(dev));
	u32 idx = lpsc_module_idx(dev, module);
	const struct lpsc_module_data *data = psc->mod_data + idx;

	if (!(data->flags & LPSC_HAS_LOCAL_RESET)) {
		return SFALSE;
	}

	return !(psc_read(dev, (u32) PSC_MDCTL(idx)) & MDCTL_LRST);
}

sbool lpsc_module_get_module_reset(struct device *dev UNUSED, struct lpsc_module *module)
{
	return module->mrst_active == 1U;
}

void lpsc_module_wait(struct device *dev, struct lpsc_module *module)
{
	const struct psc_drv_data *psc = to_psc_drv_data(get_drv_data(dev));
	u32 idx = lpsc_module_idx(dev, module);
	const struct lpsc_module_data *data = psc->mod_data + idx;
	struct psc_pd *pd = psc_idx2pd(psc, (pd_idx_t) data->powerdomain);

	s32 i = PSC_TRANSITION_TIMEOUT;

	psc_pd_wait(dev, pd);

	while (((psc_read(dev, PSC_MDSTAT(idx)) & MDSTAT_BUSY_MASK) != 0U) &&
	       (--i != 0)) {
	}
	if (!i) {
		pm_trace(TRACE_PM_ACTION_PSC_TRANSITION_TIMEOUT | TRACE_PM_ACTION_FAIL,
			 ((psc->psc_idx) << TRACE_PM_VAL_PSC_SHIFT) |
			 (idx << TRACE_PM_VAL_PD_SHIFT) | TRACE_PM_VAL_PD_POS2);
	}

	/* Only wait for reset to complete if module is in use */
	if (module->use_count) {
		u32 mask = MDSTAT_MRSTDONE;
		i = PSC_TRANSITION_TIMEOUT;

		if (!lpsc_module_get_local_reset(dev, module)) {
			mask |= MDSTAT_LRSTDONE;
		}
		while (!(psc_read(dev, PSC_MDSTAT(idx)) & mask) && (--i != 0)) {
		}
		if (!i) {
			pm_trace(TRACE_PM_ACTION_PSC_RST_DONE_TIMEOUT | TRACE_PM_ACTION_FAIL,
				 ((psc->psc_idx) << TRACE_PM_VAL_PSC_SHIFT) |
				 (idx << TRACE_PM_VAL_PD_SHIFT) | TRACE_PM_VAL_PD_POS1);
		}
	}
}

static void lpsc_module_clk_get(struct device *dev, struct lpsc_module *mod)
{
	const struct psc_drv_data *psc = to_psc_drv_data(get_drv_data(dev));
	u32 idx = lpsc_module_idx(dev, mod);
	const struct lpsc_module_data *data = psc->mod_data + idx;
	u32 i;

	for (i = 0U; i < ARRAY_SIZE(data->clock_dep); i++) {
		struct clk *clkp = clk_lookup(data->clock_dep[i]);
		if (clkp != NULL) {
			clk_get(clkp);
		}
	}
}

/**
 * \brief Disable clocks necessary for LPSC module
 *
 * This function calls clk_put on each of the clocks listed
 * under clock_dep for a given LPSC module. Some modules
 * require certain clocks to be running while the module
 * is in transition or on.
 *
 * \param dev
 * The device for this PSC.
 * \param mod
 * The const data for the LPSC module.
 * \param wait
 * True if we should wait for the module to complete transitioning before
 * disabling the clocks. This gets set to false when called as part of a
 * domain reset.
 */
static void lpsc_module_clk_put(struct device *dev, struct lpsc_module *mod, sbool wait)
{
	const struct psc_drv_data *psc = to_psc_drv_data(get_drv_data(dev));
	u32 idx = lpsc_module_idx(dev, mod);
	const struct lpsc_module_data *data = psc->mod_data + idx;
	u32 i;

	for (i = 0U; i < ARRAY_SIZE(data->clock_dep); i++) {
		struct clk *clkp = clk_lookup(data->clock_dep[i]);
		if (clkp != NULL) {
			/*
			 * We have to wait for the transition to complete
			 * taking a clock away.
			 */
			if (wait) {
				lpsc_module_wait(dev, mod);
				wait = SFALSE;
			}
			clk_put(clkp);
		}
	}
}

static void lpsc_module_get_internal(struct device *dev,
				     struct lpsc_module *module, sbool use, sbool ret)
{
#ifdef CONFIG_TRACE
	const struct psc_drv_data *psc = to_psc_drv_data(get_drv_data(dev));
#endif
	sbool modify = SFALSE;

	if (use) {
		pm_trace(TRACE_PM_ACTION_MODULE_GET,
			 (psc->psc_idx << TRACE_PM_VAL_PSC_SHIFT) |
			 (lpsc_module_idx(dev, module) << TRACE_PM_VAL_LPSC_SHIFT) |
			 (module->use_count & TRACE_PM_VAL_MAX_PSC_DATA));
		module->use_count++;
		if (module->use_count == 1U) {
			lpsc_module_clk_get(dev, module);
			modify = STRUE;
		}
	}

	if (ret) {
		pm_trace(TRACE_PM_ACTION_RETENTION_GET,
			 (psc->psc_idx << TRACE_PM_VAL_PSC_SHIFT) |
			 (lpsc_module_idx(dev, module) << TRACE_PM_VAL_LPSC_SHIFT) |
			 (module->ret_count & TRACE_PM_VAL_MAX_PSC_DATA));
		module->ret_count++;
		if (module->ret_count == 1U) {
			lpsc_module_clk_get(dev, module);
			modify = STRUE;
		}
	}

	if (modify) {
		lpsc_module_sync_state(dev, module, SFALSE);
		lpsc_module_wait(dev, module);
	}
}

static void lpsc_module_put_internal(struct device *dev,
				     struct lpsc_module *module, sbool use, sbool ret)
{
#ifdef CONFIG_TRACE
	const struct psc_drv_data *psc = to_psc_drv_data(get_drv_data(dev));
#endif
	sbool modify = SFALSE;

	if (use) {
		pm_trace(TRACE_PM_ACTION_MODULE_PUT,
			 (psc->psc_idx << TRACE_PM_VAL_PSC_SHIFT) |
			 (lpsc_module_idx(dev, module) << TRACE_PM_VAL_LPSC_SHIFT) |
			 (module->use_count & TRACE_PM_VAL_MAX_PSC_DATA));
		module->use_count--;
		if (module->use_count == 0U) {
			modify = STRUE;
		}
	}

	if (ret) {
		pm_trace(TRACE_PM_ACTION_RETENTION_PUT,
			 (psc->psc_idx << TRACE_PM_VAL_PSC_SHIFT) |
			 (lpsc_module_idx(dev, module) << TRACE_PM_VAL_LPSC_SHIFT) |
			 (module->ret_count & TRACE_PM_VAL_MAX_PSC_DATA));
		module->ret_count--;
		if (module->ret_count == 0U) {
			modify = STRUE;
		}
	}

	if (modify) {
		lpsc_module_sync_state(dev, module, SFALSE);
		if ((module->use_count == 0U) && use) {
			lpsc_module_clk_put(dev, module, STRUE);
		}
		if ((module->ret_count == 0U) && ret) {
			lpsc_module_clk_put(dev, module, STRUE);
		}
	}
}

void lpsc_module_get(struct device *dev, struct lpsc_module *module)
{
	lpsc_module_get_internal(dev, module, STRUE, SFALSE);
}

void lpsc_module_put(struct device *dev, struct lpsc_module *module)
{
	lpsc_module_put_internal(dev, module, STRUE, SFALSE);
}

void lpsc_module_ret_get(struct device *dev, struct lpsc_module *module)
{
	lpsc_module_get_internal(dev, module, SFALSE, STRUE);
}

void lpsc_module_ret_put(struct device *dev, struct lpsc_module *module)
{
	lpsc_module_put_internal(dev, module, SFALSE, STRUE);
}

/* Drop power up ref counts */
static void psc_pd_drop_pwr_up_ref(struct device *dev)
{
	const struct psc_drv_data *psc = to_psc_drv_data(get_drv_data(dev));
	pd_idx_t idx;

	for (idx = 0U; idx < psc->pd_count; idx++) {
		struct psc_pd *pd = psc_idx2pd(psc, idx);
		if (pd->pwr_up_enabled) {
			pd->pwr_up_enabled = (u8) SFALSE;
			psc_pd_put(dev, pd);
		}
	}
}

struct device *psc_lookup(psc_idx_t id)
{
	struct device *dev;

	dev = psc_devs;
	while (dev) {
		const struct psc_drv_data *psc;
		psc = to_psc_drv_data(get_drv_data(dev));

		if ((psc_idx_t) psc->psc_idx == id) {
			break;
		}

		dev = psc->data->next;
	}

	return dev;
}

struct psc_pd *psc_lookup_pd(struct device *dev, pd_idx_t id)
{
	const struct psc_drv_data *psc = to_psc_drv_data(get_drv_data(dev));
	struct psc_pd *pd = NULL;

	if ((id < psc->pd_count) &&
	    ((psc->pd_data[id].flags & PSC_PD_EXISTS) != 0U)) {
		pd = psc_idx2pd(psc, id);
	}
	return pd;
}

struct lpsc_module *psc_lookup_lpsc(struct device *dev, pd_idx_t id)
{
	const struct psc_drv_data *psc = to_psc_drv_data(get_drv_data(dev));
	struct lpsc_module *mod = NULL;

	if ((id < psc->module_count) &&
	    ((psc->mod_data[id].flags & LPSC_MODULE_EXISTS) != 0U)) {
		mod = psc_idx2mod(psc, id);
	}
	return mod;
}

/**
 * \brief Initialize all the PSC powerdomains.
 *
 * This initializes the powerdomains by waiting for them to finish any
 * active transitions, reading their state, and syncronizing it with the
 * in memory state.
 *
 * \return
 * 0 on success, <0 on failure.
 */
static s32 psc_initialize_pds(struct device *dev)
{
	const struct psc_drv_data *psc = to_psc_drv_data(get_drv_data(dev));
	pd_idx_t idx;

	/* First pass, find out which domains are on */
	for (idx = 0U; idx < psc->pd_count; idx++) {
		struct psc_pd *pd = psc_idx2pd(psc, idx);
		u8 state;

		if (!(psc->pd_data[idx].flags & PSC_PD_EXISTS)) {
			continue;
		}

		pm_trace(TRACE_PM_ACTION_PD_INIT,
			 (psc->psc_idx << TRACE_PM_VAL_PSC_SHIFT) |
			 (idx << TRACE_PM_VAL_PD_SHIFT));
		psc_pd_wait(dev, pd);
		state = (u8) (psc_read(dev, PSC_PDSTAT(idx)) & PDSTAT_STATE_MASK);

		/*
		 * Mark a PD as power up in use so we don't power everything
		 * off before PMMC startup is complete
		 */
		pd->pwr_up_enabled = (u8)
				     ((state == PDCTL_STATE_ON) ||
				      ((psc->pd_data[idx].flags & PSC_PD_ALWAYSON) != 0U));
	}

	/* Second pass, sync use count and impossible hardware states */
	for (idx = 0U; idx < psc->pd_count; idx++) {
		struct psc_pd *pd = psc_idx2pd(psc, idx);
		if (pd->pwr_up_enabled) {
			psc_pd_get(dev, pd);
		}
	}

	return 0;
}

/**
 * \brief Unititialize all the power domains of a PSC.
 *
 * This sets all the power domains in a PSC to a pre-initialized state
 * in preperation for the reset of it's reset domain. Because power
 * domains can only have dependencies on other domains within the same
 * PSC, this just means ensuring that the clock references are dropped.
 *
 * \param dev
 * The device associated with this PSC.
 */
static void psc_uninitialize_pds(struct device *dev)
{
	const struct psc_drv_data *psc = to_psc_drv_data(get_drv_data(dev));
	pd_idx_t idx;

	for (idx = 0U; idx < psc->pd_count; idx++) {
		struct psc_pd *pd = psc_idx2pd(psc, idx);

		if (((psc->pd_data[idx].flags & PSC_PD_EXISTS) != 0U) &&
		    (pd->use_count != 0U) &&
		    ((psc->pd_data[idx].flags & PSC_PD_ALWAYSON) == 0U)) {
			psc_pd_clk_put(&psc->pd_data[idx]);
		}

		pd->use_count = 0U;
		pd->pwr_up_enabled = 0U;
	}

	psc->data->pds_enabled = 0U;
}

/* Drop power up ref counts */
void psc_drop_pwr_up_ref(void)
{
	struct device *dev = psc_devs;

	while (dev) {
		const struct psc_drv_data *psc;
		lpsc_idx_t idx;

		psc = to_psc_drv_data(get_drv_data(dev));

		for (idx = 0U; idx < psc->module_count; idx++) {
			struct lpsc_module *mod = psc_idx2mod(psc, idx);
			if (mod->pwr_up_enabled) {
				mod->pwr_up_enabled = (u8) SFALSE;
				lpsc_module_put(dev, mod);
			}
			if (mod->pwr_up_ret) {
				mod->pwr_up_ret = (u8) SFALSE;
				lpsc_module_ret_put(dev, mod);
			}
		}
		dev = psc->data->next;
	}
}

/**
 * \brief Initialize all the PSC modules.
 *
 * This initializes the modules by waiting for them to finish any active
 * transitions and reading their state.
 *
 * \return
 * 0 on success, <0 on failure.
 */
static s32 psc_initialize_modules(struct device *dev)
{
	const struct psc_drv_data *psc = to_psc_drv_data(get_drv_data(dev));
	lpsc_idx_t idx;

	/* First pass, find out which modules are enabled */
	for (idx = 0U; idx < psc->module_count; idx++) {
		struct lpsc_module *mod = psc_idx2mod(psc, idx);
		u32 v;
		s32 i;

		if (!(psc->mod_data[idx].flags & LPSC_MODULE_EXISTS)) {
			continue;
		}

		v = psc_read(dev, PSC_MDCTL(idx));

		i = PSC_TRANSITION_TIMEOUT;
		while ((((v = psc_read(dev, PSC_MDSTAT(idx))) &
			 MDSTAT_BUSY_MASK) != 0U) && (--i != 0)) {
		}
		if (i == 0) {
			pm_trace(TRACE_PM_ACTION_PSC_TRANSITION_TIMEOUT | TRACE_PM_ACTION_FAIL,
				 ((psc->psc_idx) << TRACE_PM_VAL_PSC_SHIFT) |
				 (idx << TRACE_PM_VAL_PD_SHIFT) | TRACE_PM_VAL_PD_POS3);
		}

		v = psc_read(dev, PSC_MDSTAT(idx));
		v &= MDSTAT_STATE_MASK;

		/* Ref count as if we are moving out of off state */
		mod->sw_state = MDSTAT_STATE_SWRSTDISABLE;
		mod->sw_mrst_ret = (u8) SFALSE;

		if ((v == MDSTAT_STATE_ENABLE) || (v == MDSTAT_STATE_SYNCRST)) {
			mod->pwr_up_enabled = (u8) STRUE;
			mod->pwr_up_ret = (u8) STRUE;
		} else if (v == MDSTAT_STATE_DISABLE) {
			mod->pwr_up_enabled = (u8) SFALSE;
			mod->pwr_up_ret = (u8) STRUE;
		} else if (v == MDSTAT_STATE_SWRSTDISABLE) {
			mod->pwr_up_enabled = (u8) SFALSE;
			mod->pwr_up_ret = (u8) SFALSE;
		} else {
			/* Invalid initial state, try turning everything on */
			mod->pwr_up_ret = (u8) STRUE;
			mod->pwr_up_enabled = (u8) STRUE;
		}
	}

	return 0;
}

/**
 * \brief Finish initializing all the PSC modules.
 *
 * This finishes the initialization of modules by syncronizing their
 * state with the in memory state.
 *
 * \return
 * 0 on success, <0 on failure.
 */
static s32 psc_initialize_modules_finish(struct device *dev)
{
	const struct psc_drv_data *psc = to_psc_drv_data(get_drv_data(dev));
	lpsc_idx_t idx;

	/* Second pass, sync ref counts */
	for (idx = 0U; idx < psc->module_count; idx++) {
		struct lpsc_module *mod = psc_idx2mod(psc, idx);
		lpsc_module_get_internal(dev, mod, (sbool) mod->pwr_up_enabled,
					 (sbool) mod->pwr_up_ret);
	}

	psc_pd_drop_pwr_up_ref(dev);

	return 0;
}

/**
 * \brief Unititialize all the LPSC modules of a PSC.
 *
 * This sets all the LPSC modules in a PSC to a pre-initialized state
 * in preperation for the reset of it's reset domain. Because LPSC
 * modules can have dependencies on other domains, those dependencies
 * need to be droppe as appropriate. Any clock dependencies are also
 * dropped.
 *
 * \param dev
 * The device associated with this PSC.
 */
static void psc_uninitialize_modules(struct device *dev)
{
	const struct psc_drv_data *psc = to_psc_drv_data(get_drv_data(dev));
	lpsc_idx_t idx;
	u32 i;

	/* First pass, find out which modules are enabled */
	for (idx = 0U; idx < psc->module_count; idx++) {
		struct lpsc_module *mod = psc_idx2mod(psc, idx);
		if ((psc->mod_data[idx].flags & LPSC_MODULE_EXISTS) == 0U) {
			continue;
		}

		if (mod->use_count != 0U) {
			lpsc_module_clk_put(dev, mod, SFALSE);
		}
		if (mod->ret_count != 0U) {
			lpsc_module_clk_put(dev, mod, SFALSE);
		}

		mod->use_count = 0U;
		mod->ret_count = 0U;
		mod->pwr_up_enabled = (u8) SFALSE;
		mod->pwr_up_ret = (u8) SFALSE;

		lpsc_module_sync_state(dev, mod, STRUE);
		mod->sw_state = MDSTAT_STATE_SWRSTDISABLE;
		mod->sw_mrst_ret = (u8) SFALSE;
	}

	for (i = 0U; i < ARRAY_SIZE(psc->data->mods_enabled); i++) {
		psc->data->mods_enabled[i] = 0U;
	}
}

/**
 * \brief Check if PSCs we depend on have completed their initial config
 *
 * PSCs can have domains that depend on domains in other PSCs. We break up
 * initialization into two stages because PSCs can have cross dependencies.
 * Once all PSCs in a cross dependency set have completed, each one can
 * move to the second stage of initialization. We use the -EDEFER system
 * to manage this.
 *
 * \return
 * 0 on success, <0 on failure.
 */
static s32 psc_module_deps_ready(struct device *dev)
{
	const struct psc_drv_data *psc = to_psc_drv_data(get_drv_data(dev));
	u32 idx;
	sbool not_ready = SFALSE;
	s32 ret = 0;

	for (idx = 0; !not_ready && (idx < psc->module_count); idx++) {
		if (((psc->mod_data[idx].flags & LPSC_DEPENDS) != 0UL) && (psc->mod_data[idx].depends_psc_idx != psc->psc_idx)) {
			if (psc_lookup((psc_idx_t) psc->mod_data[idx].depends_psc_idx) == NULL) {
				not_ready = STRUE;
				ret = -EDEFER;
			}
		}
	}

	return ret;
}

static s32 psc_pre_init(struct device *dev)
{
	const struct drv_data *data;
	const struct psc_drv_data *psc;
	s32 ret = 0;

	data = get_drv_data(dev);
	psc = to_psc_drv_data(data);

	/* Only perform initial configuration if it has not been completed */
	if (psc_lookup((psc_idx_t) psc->psc_idx) == NULL) {
		const struct resource_mem *mem;

		mem = device_resource_mem(dev, 0);
		if (!mem) {
			ret = -EINVAL;
		}

		if (ret == 0) {
			ret = psc_initialize_pds(dev);
		}

		if (ret == 0) {
			ret = psc_initialize_modules(dev);
		}

		if (ret == 0) {
			/* This marks initial config as complete */
			psc->data->next = psc_devs;
			psc_devs = dev;
		}
	}

	return ret;
}

static s32 psc_post_init(struct device *dev)
{
	s32 ret;

	ret = psc_module_deps_ready(dev);

	if (ret == SUCCESS) {
		ret = psc_initialize_modules_finish(dev);
	}

	return ret;
}

/**
 * \brief Uninitialize this PSC.
 *
 * Perform the steps necessary to bring this PSC back to a pre-init state.
 * This is performed before a reset domain reset so that the PSC can be
 * re-initialized after the reset is complete.
 *
 * \param dev
 * The device associated with this PSC.
 */
static void psc_uninit(struct device *dev)
{
	const struct psc_drv_data *psc;
	struct device *curr_dev;
	struct device **last_dev;

	psc = to_psc_drv_data(get_drv_data(dev));

	psc_uninitialize_modules(dev);
	psc_uninitialize_pds(dev);

	/* Remove from list of unitialized PSCs */
	curr_dev = psc_devs;
	last_dev = &psc_devs;
	while ((curr_dev != NULL) && (curr_dev != dev)) {
		const struct psc_drv_data *curr_psc;
		curr_psc = to_psc_drv_data(get_drv_data(curr_dev));
		curr_dev = curr_psc->data->next;
		last_dev = &curr_psc->data->next;
	}
	if (curr_dev == dev) {
		*last_dev = psc->data->next;
		psc->data->next = NULL;
	}
}

const struct drv psc_drv = {
	.pre_init	= psc_pre_init,
	.post_init	= psc_post_init,
	.uninit		= psc_uninit,
};
