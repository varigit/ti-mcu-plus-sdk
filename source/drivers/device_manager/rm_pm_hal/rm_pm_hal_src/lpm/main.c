/*
 * DM Stub Firmware
 *
 * DM Stub main low power functionality
 *
 * Copyright (C) 2021-2023, Texas Instruments Incorporated
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

#include <lib/bitops.h>
#include <tisci/lpm/tisci_lpm.h>
#include <types/errno.h>
#include <baseaddress.h>
#include <mcu_ctrl_mmr.h>
#include <wkup_ctrl_mmr.h>
#include <wake_data.h>
#include <lpm_data.h>

#include "lpm_io.h"
#include "ctrlmmr_raw.h"
#include "ddr.h"
#include "lpm_serial_8250.h"
#include "lpm_string.h"
#include "lpm_trace.h"
#include "pll_16fft_raw.h"
#include "psc_raw.h"
#include "sec_proxy.h"
#include "timeout.h"
#include "vim_raw.h"
#include "lpm_string.h"
#include "string.h"

enum lpm_mode {
	LPM_DEEPSLEEP,
	LPM_MCU_ONLY,
	LPM_STANDBY,
};

struct main_pd_lpsc {
	u8	pd;
	u8	lpsc;
};

/* variable to store the last wakeup interrupt */
u32 wake_up_source = TISCI_MSG_VALUE_LPM_WAKE_SOURCE_INVALID;
static struct tisci_msg_prepare_sleep_req g_params;

/*
 * XXX: Define a setup time of 4x default for MOSC
 *
 * This allows for better clock stability once R5 begins
 * execution again.
 */
#define PMCTRL_MOSC_SETUP_TIME          0xFC000

/* FIXME IO_ISO_TIMEOUT should be about 10us */
#define IO_ISO_TIMEOUT  10000
#define PLLOFFSET(idx) (0x1000 * (idx))

static struct pll_raw_data mcu_pll = { .base = MCU_PLL_MMR_BASE, };
static struct pll_raw_data main_pll0 =
{ .base = MAIN_PLL_MMR_BASE + PLLOFFSET(0), };
static struct pll_raw_data main_pll1 =
{ .base = MAIN_PLL_MMR_BASE + PLLOFFSET(1), };
static struct pll_raw_data main_pll2 =
{ .base = MAIN_PLL_MMR_BASE + PLLOFFSET(2), };
static struct pll_raw_data main_pll8 =
{ .base = MAIN_PLL_MMR_BASE + PLLOFFSET(8), };
static struct pll_raw_data main_pll12 =
{ .base = MAIN_PLL_MMR_BASE + PLLOFFSET(12), };
static struct pll_raw_data main_pll16 =
{ .base = MAIN_PLL_MMR_BASE + PLLOFFSET(16), };
static struct pll_raw_data main_pll17 =
{ .base = MAIN_PLL_MMR_BASE + PLLOFFSET(17), };

/* MAIN /LPSCs to be disabled during Deepsleep phase 1 */
/* FIXME am62x specific, move to soc specific place */
static struct main_pd_lpsc main_lpscs_phase1[] = {
	{ PD_A53_0,	    LPSC_A53_0	       },
	{ PD_A53_CLUSTER_0, LPSC_A53_CLUSTER_0 },
	/* FIXME disabling LPSC_MAIN_IP causes problem on vlab, skip it for now */
/*	{ PD_MAIN_IP,		LPSC_MAIN_IP },*/
	/* FIXME Removing this lpsc since it is not getting turned off properly in second cycle of deep sleep */
/*	{ PD_GP_CORE_CTL,   LPSC_MAIN_TEST},*/
	{ PD_SMS,	    LPSC_HSM	       },
	{ PD_SMS,	    LPSC_TIFS	       },
	{ PD_SMS,	    LPSC_SA3UL	       },
	{ PD_SMS,	    LPSC_SMS_COMMON    }
};

/* MAIN LPSCs to be disabled during Deepsleep phase 2 */
/* FIXME am62x specific, move to soc specific place */
static struct main_pd_lpsc main_lpscs_phase2[] = {
	/* FIXME this times out, so skip it for now */
/*	{ PD_DEBUG,		LPSC_DEBUGSS },*/
};

/* MCU LPSCs to be disabled during Deepsleep */
/* FIXME am62x specific, move to soc specific place */
static int mcu_lpscs[] = {
	LPSC_MCU_TEST,
	LPSC_MCU_COMMON
};

void lpm_clear_all_wakeup_interrupt(void)
{
	int i;

	for (i = 0; i < WAKEUP_SOURCE_MAX; i++) {
		vim_clear_intr(soc_wake_sources_data[i].int_num);
	}
}

static void lpm_abort()
{
	volatile int a = 0x1234;

	while (a) {
	}
}

static void enter_ddr_low_power_mode()
{
	s32 ret;
	int i;

	psc_raw_lpsc_set_state(MAIN_PSC_BASE, LPSC_EMIF_DATA_ISO,
			       MDCTL_STATE_DISABLE, 0);
	psc_raw_pd_initiate(MAIN_PSC_BASE, PD_GP_CORE_CTL);
	ret = psc_raw_pd_wait(MAIN_PSC_BASE, PD_GP_CORE_CTL);

	ddr_enter_low_power_mode();

	psc_raw_lpsc_set_state(MAIN_PSC_BASE, LPSC_EMIF_LOCAL,
			       MDCTL_STATE_DISABLE, 0);
	psc_raw_pd_initiate(MAIN_PSC_BASE, PD_GP_CORE_CTL);
	ret = psc_raw_pd_wait(MAIN_PSC_BASE, PD_GP_CORE_CTL);

	ddr_enable_retention();
	psc_raw_lpsc_set_state(MAIN_PSC_BASE, LPSC_EMIF_CFG_ISO,
			       MDCTL_STATE_DISABLE, 0);
	psc_raw_pd_initiate(MAIN_PSC_BASE, PD_GP_CORE_CTL);
	psc_raw_pd_wait(MAIN_PSC_BASE, PD_GP_CORE_CTL);

	writel(DS_DDR0_RESET_MASK, WKUP_CTRL_MMR_BASE + DS_DDR0_RESET);
}

static void exit_ddr_low_power_mode()
{
	ddr_disable_retention();

	psc_raw_lpsc_set_state(MAIN_PSC_BASE, LPSC_EMIF_LOCAL,
			       MDCTL_STATE_ENABLE, 0);
	psc_raw_pd_initiate(MAIN_PSC_BASE, PD_GP_CORE_CTL);
	psc_raw_pd_wait(MAIN_PSC_BASE, PD_GP_CORE_CTL);

	psc_raw_lpsc_set_state(MAIN_PSC_BASE, LPSC_EMIF_CFG_ISO,
			       MDCTL_STATE_ENABLE, 0);
	psc_raw_pd_initiate(MAIN_PSC_BASE, PD_GP_CORE_CTL);
	psc_raw_pd_wait(MAIN_PSC_BASE, PD_GP_CORE_CTL);

	writel(DS_DDR0_RESET_UNMASK, WKUP_CTRL_MMR_BASE + DS_DDR0_RESET);

	ddr_exit_low_power_mode();

	psc_raw_lpsc_set_state(MAIN_PSC_BASE, LPSC_EMIF_DATA_ISO,
			       MDCTL_STATE_ENABLE, 0);
	psc_raw_pd_initiate(MAIN_PSC_BASE, PD_GP_CORE_CTL);
	psc_raw_pd_wait(MAIN_PSC_BASE, PD_GP_CORE_CTL);

	ddr_deepsleep_exit_training();
}

static void set_usb_reset_isolation()
{
	writel(DS_DDR0_RESET_MASK, WKUP_CTRL_MMR_BASE + DS_USB0_RESET);
	writel(DS_DDR0_RESET_MASK, WKUP_CTRL_MMR_BASE + DS_USB1_RESET);
}

static void release_usb_reset_isolation()
{
	/*
	 * Nothing is done here, as we cannot yet remove reset isolation
	 * until a method to enable USB LPSC before removing reset
	 * isolation can be determined.
	 */
}

static s32 disable_main_lpsc(struct main_pd_lpsc *lpscs, u32 n_lpscs)
{
	u32 i;
	int ret;

	for (i = 0; i < n_lpscs; i++) {
		psc_raw_lpsc_set_state(MAIN_PSC_BASE, lpscs[i].lpsc,
				       MDCTL_STATE_DISABLE, 0);
		psc_raw_pd_initiate(MAIN_PSC_BASE, lpscs[i].pd);
		ret = psc_raw_pd_wait(MAIN_PSC_BASE, lpscs[i].pd);
		if (ret) {
			return ret;
		}
	}
	return 0;
}

static void bypass_main_pll()
{
	/* disable all HSDIV in MAIN_PLLCTRL, bypass all MAIN PLL
	 * except clock for Debug, PLL0, PLL15
	 */
	pll_save(&main_pll0);
	pll_save(&main_pll1);
	pll_save(&main_pll2);
	pll_save(&main_pll8);
	pll_save(&main_pll12);
	pll_save(&main_pll16);
	pll_save(&main_pll17);

	pll_disable(&main_pll1, 0xFFFF);
	pll_disable(&main_pll2, 0xFFFF);
	pll_disable(&main_pll8, 0xFFFF);
	pll_disable(&main_pll12, 0xFFFF);
	pll_disable(&main_pll16, 0xFFFF);
	pll_disable(&main_pll17, 0xFFFF);
}


static void wait_for_debug(void)
{
	while (g_params.debug_flags) {
	}
}

static void config_wake_sources()
{
	u32 i;
	u32 val = 0;

	for (i = 0; i < WAKEUP_SOURCE_MAX; i++) {
		/* Enable interrupt */
		vim_set_intr_enable(soc_wake_sources_data[i].int_num,
				    INTR_ENABLE);

		/* Set WKUP0_EN BIT to be enabled */
		val |= BIT(soc_wake_sources_data[i].wkup_idx);
	}
	/* Write all bits to enable at once */
	writel(val, (WKUP_CTRL_MMR_BASE + WKUP0_EN));
}

static void disable_wake_sources()
{
	int i;

	/* disable all wake up interrupts */
	for (i = 0; i < WAKEUP_SOURCE_MAX; i++) {
		vim_set_intr_enable(soc_wake_sources_data[i].int_num, INTR_DISABLE);
	}
	/* Clear all bits in WKUP0_EN */
	writel(0, (WKUP_CTRL_MMR_BASE + WKUP0_EN));
}

static int enable_main_io_isolation()
{
	return 0;
}

static int disable_main_io_isolation()
{
	return 0;
}

static void config_clk_muxes()
{
	writel(MCU_CTRL_MMR_CFG0_MCU_GPIO_CLKSEL_CLK_32K, (MCU_CTRL_MMR_BASE + MCU_CTRL_MMR_CFG0_MCU_GPIO_CLKSEL));
}

static void enable_gpio_wake_up()
{
	writel(MCU_CTRL_MMR_CFG0_MCU_GPIO_WKUP_CTRL_ENABLE, (MCU_CTRL_MMR_BASE + MCU_CTRL_MMR_CFG0_MCU_GPIO_WKUP_CTRL));
}

static void disable_gpio_wake_up()
{
	writel(MCU_CTRL_MMR_CFG0_MCU_GPIO_WKUP_CTRL_DISABLE, (MCU_CTRL_MMR_BASE + MCU_CTRL_MMR_CFG0_MCU_GPIO_WKUP_CTRL));
}

static void disable_main_remain_pll()
{
	/* disable remaining HSDIV & PLL in MAIN */
	return;
}

static int enable_main_remain_pll()
{
	s32 ret = 0;

	ret = pll_restore(&main_pll0);
	if (ret) {
		return ret;
	}

	ret = pll_restore(&main_pll1);
	if (ret) {
		return ret;
	}

	ret = pll_restore(&main_pll2);
	if (ret) {
		return ret;
	}

	/* Restore pll used by A53 */
	ret = pll_restore(&main_pll8);
	if (ret) {
		return ret;
	}

	/* config additional MAIN PLLs and PSCs for EMIF */
	ret = pll_restore(&main_pll12);
	if (ret) {
		return ret;
	}

	ret = pll_restore(&main_pll16);
	if (ret) {
		return ret;
	}

	ret = pll_restore(&main_pll17);
	if (ret) {
		return ret;
	}

	return ret;
}

static s32 disable_mcu_domain()
{
	u32 i;
	s32 ret;

	for (i = 0; i < sizeof(mcu_lpscs) / sizeof(int); i++) {
		psc_raw_lpsc_set_state(MCU_PSC_BASE, mcu_lpscs[i],
				       MDCTL_STATE_DISABLE, 0);
	}

	psc_raw_pd_set_state(MCU_PSC_BASE, PD_GP_CORE_CTL_MCU,
			     PDCTL_STATE_OFF, 0);
	psc_raw_pd_initiate(MCU_PSC_BASE, PD_GP_CORE_CTL_MCU);
	ret = psc_raw_pd_wait(MCU_PSC_BASE, PD_GP_CORE_CTL_MCU);
	if (ret) {
		return ret;
	}

	psc_raw_pd_set_state(MCU_PSC_BASE, PD_MCU_M4F, PDCTL_STATE_OFF, 0);
	psc_raw_pd_initiate(MCU_PSC_BASE, PD_MCU_M4F);
	ret = psc_raw_pd_wait(MCU_PSC_BASE, PD_MCU_M4F);
	if (ret) {
		return ret;
	}
	return 0;
}

static void enable_mcu_remain_pll()
{
}

static s32 enable_dm_lpsc(void)
{
	s32 ret = SUCCESS;

	psc_raw_lpsc_set_state(MAIN_PSC_BASE, LPSC_MAIN_DM,
			       MDCTL_STATE_ENABLE, 0);
	psc_raw_pd_initiate(MAIN_PSC_BASE, PD_GP_CORE_CTL);
	ret = psc_raw_pd_wait(MAIN_PSC_BASE, PD_GP_CORE_CTL);

	return ret;
}

static void disable_mcu_io_isolation()
{
	/* disable WKUP IO Daisy Chain & IO isolation */
}

/* Send TISCI ROM Boot image message containing location
 * and boot address to load FS stub from SPS Memory
 */

#ifndef CONFIG_LPM_DM_STUB_NO_FS_STUB
static s32 send_tisci_msg_firmware_load()
{
	s32 ret = 0;

	struct tisci_msg_firmware_load_resp resp;
	struct tisci_msg_firmware_load_req req;

	req.hdr.type = TISCI_MSG_FIRMWARE_LOAD;
	req.hdr.flags = 8 << 24;
	req.image_addr = CONFIG_TIFSFW_SPS_BASE;
	req.image_size = CONFIG_TIFSFW_SPS_BASE;

	ret = sproxy_send_msg_rom(&req, sizeof(req));
	if (ret) {
		return ret;
	}

	lpm_memset(&resp, 0, sizeof(resp));
	ret = sproxy_receive_msg_rom(&resp, sizeof(resp));

	if (ret) {
		return ret;
	}

	if (resp.hdr.type != MSG_FIRMWARE_LOAD_RESULT ||
	    resp.hdr.flags != MSG_FLAG_CERT_AUTH_PASS) {
		ret = -EINVAL;
	}

	return ret;
}

static s32 receive_tisci_msg_continue_resume_req()
{
	struct tisci_msg_continue_resume_req req;
	s32 ret = 0;

	lpm_memset(&req, 0, sizeof(req));
	ret = sproxy_receive_msg_rom(&req, sizeof(req));

	if (ret) {
		return ret;
	}

	if (req.hdr.type != TISCI_MSG_CONTINUE_RESUME) {
		ret = -EINVAL;
	}

	return ret;
}

static s32 send_tisci_msg_continue_resume_resp()
{
	s32 ret = 0;
	struct tisci_msg_continue_resume_resp resp;

	resp.hdr.type = TISCI_MSG_CONTINUE_RESUME;
	resp.hdr.flags = TISCI_MSG_FLAG_ACK;

	resp.ctx_lo = g_params.ctx_lo;
	resp.ctx_hi = g_params.ctx_hi;

	ret = sproxy_send_msg_rom(&resp, sizeof(resp));
	if (ret) {
		return ret;
	}

	return ret;
}

static s32 receive_tisci_msg_sync_resume_req()
{
	struct tisci_msg_sync_resume_req req;
	s32 ret = 0;

	lpm_memset(&req, 0, sizeof(req));
	ret = sproxy_receive_msg_rom(&req, sizeof(req));

	if (ret) {
		return ret;
	}

	if (req.hdr.type != TISCI_MSG_SYNC_RESUME) {
		ret = -EINVAL;
	}

	return ret;
}
#else
static s32 send_tisci_msg_firmware_load()
{
	return 0;
}
static s32 receive_tisci_msg_continue_resume_req()
{
	return 0;
}
static s32 send_tisci_msg_continue_resume_resp()
{
	return 0;
}
static s32 receive_tisci_msg_sync_resume_req()
{
	return 0;
}
#endif

static void send_msg_bypass_sms_pll()
{
}
static void send_msg_restore_sms_pll()
{
}

static s32 wait_for_reset_statz(int stat)
{
	u32 val;
	int i = 0;

	do {
		val = readl(WKUP_CTRL_MMR_BASE + SLEEP_STATUS);
		if ((val & SLEEP_STATUS_MAIN_RESETSTATZ) == stat) {
			return 0;
		}
		delay_1us();
	} while (i++ < RETRY_CNT_MS);

	return -ETIMEDOUT;
}

static s32 wait_for_tifs_ready()
{
	u32 val;
	int i = 0;

	do {
		val = readl(WKUP_CTRL_MMR_BASE + DS_MAGIC_WORD);
		if (val == DS_MAGIC_WORD_RESUME_ROM) {
			writel(0, WKUP_CTRL_MMR_BASE + DS_MAGIC_WORD);
			return 0;
		}
		delay_1us();
	} while (i++ < RETRY_CNT_MS);

	return -ETIMEDOUT;
}

static void enable_pll_standby()
{
}
static void clear_prepare_sleep_data()
{
	lpm_memset(&g_params, 0, sizeof(g_params));
}

/*
 * lpm_populate_prepare_sleep_data() for DM to populate stub local data.
 * it should be called by DM before dm_stub_entry().
 */
void lpm_populate_prepare_sleep_data(struct tisci_msg_prepare_sleep_req *p)
{
	if (!p) {
		return;
	}
	lpm_memcpy(&g_params, p, sizeof(g_params));
}

s32 __attribute__((noinline)) dm_stub_entry(void)
{
	u32 reg;
	u32 n_lpscs;

	lpm_console_init();

	lpm_seq_trace(TRACE_PM_ACTION_LPM_SEQ_DM_STUB_START);

#ifdef CONFIG_LPM_DM_STUB_STANDALONE
	/* FIXME parameters should be passed in from DM */
	g_params.mode = LPM_DEEPSLEEP;
	g_params.debug_flags = 0;
#endif

	/* unlock wkup_ctrl_mmr region 2 & 6 */
	ctrlmmr_unlock(WKUP_CTRL_MMR_BASE, 2);
	ctrlmmr_unlock(WKUP_CTRL_MMR_BASE, 6);

	/* unlock mcu_ctrl_mmr region 2 */
	ctrlmmr_unlock(MCU_CTRL_MMR_BASE, 2);

	lpm_seq_trace(TRACE_PM_ACTION_LPM_SEQ_DM_STUB_MMR_UNLOCK);

	enter_ddr_low_power_mode();

	lpm_seq_trace(0x77);

	if (g_params.mode == LPM_DEEPSLEEP || g_params.mode == LPM_MCU_ONLY) {
		lpm_seq_trace(TRACE_PM_ACTION_LPM_SEQ_DM_STUB_DDR_RST_ISO);
		set_usb_reset_isolation();
		lpm_seq_trace(TRACE_PM_ACTION_LPM_SEQ_DM_STUB_USB_RST_ISO);

		/* Disable all LPSCs in MAIN except Debug, Always ON */
		n_lpscs = sizeof(main_lpscs_phase1) / sizeof(struct main_pd_lpsc);
		if (disable_main_lpsc(main_lpscs_phase1, n_lpscs)) {
			lpm_seq_trace_fail(TRACE_PM_ACTION_LPM_SEQ_DM_STUB_DIS_MAIN_LPSC);
			lpm_abort();
		}

		lpm_seq_trace(TRACE_PM_ACTION_LPM_SEQ_DM_STUB_DIS_MAIN_LPSC);

		/* Disable all HSDIVs in MAIN_PLL, bypass all MAIN PLLs,
		 * except clock for debug, PLL0_HSDIV0, PLL15
		 */
		bypass_main_pll();
		lpm_seq_trace(TRACE_PM_ACTION_LPM_SEQ_DM_STUB_BYPASS_MAIN_PLL);
	}

	wait_for_debug();

	if (g_params.mode == LPM_DEEPSLEEP || g_params.mode == LPM_MCU_ONLY) {
		/* Configure selected wake sources
		 * with writes to WKUP0_EN IN WKUP_CTRL
		 */
		config_wake_sources();
		lpm_seq_trace(TRACE_PM_ACTION_LPM_SEQ_DM_STUB_CONFIG_WAKE_SRC);

		if (enable_main_io_isolation()) {
			lpm_seq_trace_fail(TRACE_PM_ACTION_LPM_SEQ_DM_STUB_EN_MAIN_IO_ISO);
			lpm_abort();
		}

		lpm_seq_trace(TRACE_PM_ACTION_LPM_SEQ_DM_STUB_EN_MAIN_IO_ISO);

		/* Disable remaining MAIN LPSCs for debug */
		n_lpscs = sizeof(main_lpscs_phase2) / sizeof(struct main_pd_lpsc);
		if (disable_main_lpsc(main_lpscs_phase2, n_lpscs)) {
			lpm_seq_trace_fail(TRACE_PM_ACTION_LPM_SEQ_DM_STUB_DIS_MAIN_LPSC2);
			lpm_abort();
		}

		lpm_seq_trace(TRACE_PM_ACTION_LPM_SEQ_DM_STUB_DIS_MAIN_LPSC2);

		/* Modify WKUP_CLKSEL in WKUP_CTRL
		 * to use MCU_PLL instead of MAIN PLL
		 */
		writel(WKUP_CLKSEL_MCU, WKUP_CTRL_MMR_BASE + WKUP_CLKSEL);

		lpm_seq_trace(TRACE_PM_ACTION_LPM_SEQ_DM_STUB_WKUP_CLKSEL_MCU);

		/* Configure GPIO/WDT/32k Clock muxes */
		config_clk_muxes();

		/* Enable GPIO wake up */
		enable_gpio_wake_up();

		lpm_seq_trace(TRACE_PM_ACTION_LPM_SEQ_DM_STUB_CONF_CLK_MUXES);

		/* Disable remaining MAIN HSDIVs and PLLs */
		disable_main_remain_pll();

		lpm_seq_trace(TRACE_PM_ACTION_LPM_SEQ_DM_STUB_DIS_MAIN_PLLS);

		/* Set WKUP_CTRL DS_DM_RESET.mask to isolate DM
		 * from MAIN domain reset
		 */
		writel(DS_DM_RESET_MASK, WKUP_CTRL_MMR_BASE + DS_DM_RESET);

		lpm_seq_trace(TRACE_PM_ACTION_LPM_SEQ_DM_STUB_EN_DS_RST_MASK);

		/* Write to WKUP_CTRL DS_MAIN.por_pdoff bits to
		 * place Main domain in reset, gate PD_SMS and PD_DEBUG
		 */
		writel(DS_MAIN_OFF, WKUP_CTRL_MMR_BASE + DS_MAIN);

		if (wait_for_reset_statz(0)) {
			lpm_seq_trace_fail(TRACE_PM_ACTION_LPM_SEQ_DM_STUB_DS_MAIN_OFF);
			lpm_abort();
		}
		lpm_seq_trace(TRACE_PM_ACTION_LPM_SEQ_DM_STUB_DS_MAIN_OFF);
	}

	if (g_params.mode == LPM_DEEPSLEEP) {
		/* Disable MCU Domain LPSCs, PDs */
		if (disable_mcu_domain()) {
			lpm_seq_trace_fail(TRACE_PM_ACTION_LPM_SEQ_DM_STUB_DIS_MCU_DOM);
			lpm_abort();
		} else {
			lpm_seq_trace(TRACE_PM_ACTION_LPM_SEQ_DM_STUB_DIS_MCU_DOM);
		}
	}

	if (g_params.mode == LPM_DEEPSLEEP || g_params.mode == LPM_MCU_ONLY) {
		/* Set WKUP_CTRL.RST_CTRL.main_reset_iso_done_z to 1 to
		 * mask main reset in case of RESET_REQz wakeup
		 */
		reg = readl(WKUP_CTRL_MMR_BASE + RST_CTRL);
		reg |= RST_CTRL_MAIN_RST_ISO_DONE_Z;
		writel(reg, WKUP_CTRL_MMR_BASE + RST_CTRL);
		lpm_seq_trace(TRACE_PM_ACTION_LPM_SEQ_DM_STUB_EN_RST_ISO_DONE);
	} else {
		/* Send TISCI message to bypass SMS PLL */
		send_msg_bypass_sms_pll();
	}

	/* Use WKUP_CTRL.WKUP_WWD0_CTRL to gate clock to RTI */
	writel(WWD_STOP, WKUP_CTRL_MMR_BASE + WKUP_WWD0_CTRL);
	lpm_seq_trace(TRACE_PM_ACTION_LPM_SEQ_DM_STUB_MSK_WWD0_CTRL);

	if (g_params.mode == LPM_DEEPSLEEP) {
		pll_save(&mcu_pll);
		pll_bypass(&mcu_pll);
		lpm_console_bypass_init();
		lpm_seq_trace(TRACE_PM_ACTION_LPM_SEQ_DM_STUB_BYPASS_MCU_PLL);

		/* set MCU_MMR.MCU_PLL_CLKSEL.clkloss_switch_en to 0
		 *     to avoid clk switch
		 */
		reg = readl(MCU_CTRL_MMR_BASE + MCU_PLL_CLKSEL);
		reg &= ~MCU_PLL_CLKSEL_CLKLOSS_SWTCH_EN;
		writel(reg, MCU_CTRL_MMR_BASE + MCU_PLL_CLKSEL);
		lpm_seq_trace(TRACE_PM_ACTION_LPM_SEQ_DM_STUB_CLR_CLKLOSS_EN);

		/* set MCU_MMR.HFOSC0_CTRL.pd_c to power off of HFOSC PD */
		reg = readl(MCU_CTRL_MMR_BASE + HFOSC0_CTRL);
		reg |= HFOSC0_CTRL_PD_C;
		writel(reg, MCU_CTRL_MMR_BASE + HFOSC0_CTRL);
		lpm_seq_trace(TRACE_PM_ACTION_LPM_SEQ_DM_STUB_EN_HFOSC_PD_C);

		/* set OSC_CG_ON_WFI bit in WKUP_CTRL.PMCTRL_MOSC */
		reg = readl(WKUP_CTRL_MMR_BASE + PMCTRL_MOSC);
		reg |= PMCTRL_MOSC_OSC_CG_ON_WFI | PMCTRL_MOSC_SETUP_TIME;
		writel(reg, WKUP_CTRL_MMR_BASE + PMCTRL_MOSC);
		lpm_seq_trace(TRACE_PM_ACTION_LPM_SEQ_DM_STUB_EN_OSC_CG_WFI);
	}

	lpm_seq_trace(TRACE_PM_ACTION_LPM_SEQ_DM_STUB_PRE_WFI);

	/* enter WFI */
	__asm volatile ("\tWFI");
	/* enable global interrupt */
	__asm volatile ("\tCPSIE I");

	lpm_seq_trace(TRACE_PM_ACTION_LPM_SEQ_DM_STUB_POST_WFI);

	/* start resume */
	if (g_params.mode == LPM_DEEPSLEEP) {
		/* clear OSC_CG_ON_WFI bit in WKUP_CTRL.PMCTRL_MOSC */
		reg = readl(WKUP_CTRL_MMR_BASE + PMCTRL_MOSC);
		reg &= ~PMCTRL_MOSC_OSC_CG_ON_WFI;
		writel(reg, WKUP_CTRL_MMR_BASE + PMCTRL_MOSC);

		lpm_seq_trace(TRACE_PM_ACTION_LPM_SEQ_DM_STUB_CLR_OSC_CG_WFI);

		/* clear MCU_MMR.HFOSC0_CTRL.pd_c and
		 * set MCU_MMR.MCU_PLL_CLKSEL.clkloss_switch_en
		 */
		reg = readl(MCU_CTRL_MMR_BASE + HFOSC0_CTRL);
		reg &= ~HFOSC0_CTRL_PD_C;
		writel(reg, MCU_CTRL_MMR_BASE + HFOSC0_CTRL);

		lpm_seq_trace(TRACE_PM_ACTION_LPM_SEQ_DM_STUB_CLR_HFOSC_PD_C);

		reg = readl(MCU_CTRL_MMR_BASE + MCU_PLL_CLKSEL);
		reg |= MCU_PLL_CLKSEL_CLKLOSS_SWTCH_EN;
		writel(reg, MCU_CTRL_MMR_BASE + MCU_PLL_CLKSEL);

		lpm_seq_trace(TRACE_PM_ACTION_LPM_SEQ_DM_STUB_SET_CLKLOSS_EN);

		if (pll_restore(&mcu_pll)) {
			lpm_seq_trace_fail(TRACE_PM_ACTION_LPM_SEQ_DM_STUB_RESTORE_MCU_PLL);
			lpm_abort();
		} else {
			lpm_console_init();
			lpm_seq_trace(TRACE_PM_ACTION_LPM_SEQ_DM_STUB_RESTORE_MCU_PLL);
		}
	}

	/* Use WKUP_CTRL.WKUP_WWD0_CTRL to ungate clock to RTI */
	writel(WWD_RUN, WKUP_CTRL_MMR_BASE + WKUP_WWD0_CTRL);

	lpm_seq_trace(TRACE_PM_ACTION_LPM_SEQ_DM_STUB_UNMSK_WWD0_CTRL);

	if (g_params.mode == LPM_DEEPSLEEP || g_params.mode == LPM_MCU_ONLY) {
		/* Disable WKUP IO Daisy Chain and IO Isolation */
		disable_mcu_io_isolation();

		lpm_seq_trace(TRACE_PM_ACTION_LPM_SEQ_DIS_MCU_IO_ISO);

		/* Write 0xdee51ee5 to WKUP DS_MAGIC_WORD to
		 * indicate resume is required to TIFS ROM
		 */
		writel(DS_MAGIC_WORD_RESUME_TIFS,
		       WKUP_CTRL_MMR_BASE + DS_MAGIC_WORD);

		lpm_seq_trace(TRACE_PM_ACTION_LPM_SEQ_DM_STUB_SET_MGC_WRD);

		/* Write to WKUP_CTRL DS_MAIN.por_pdoff
		 * bits to release Main domain reset
		 */
		writel(DS_MAIN_ON, WKUP_CTRL_MMR_BASE + DS_MAIN);

		lpm_seq_trace(TRACE_PM_ACTION_LPM_SEQ_DM_STUB_DS_MAIN_ON);

		/* FIXME unmask WKUP_CTRL.DS_DM_REST? it is set in suspend */
	}

	wait_for_debug();

	if (g_params.mode == LPM_DEEPSLEEP || g_params.mode == LPM_MCU_ONLY) {
		if (wait_for_reset_statz(SLEEP_STATUS_MAIN_RESETSTATZ)) {
			lpm_seq_trace_fail(TRACE_PM_ACTION_LPM_SEQ_DM_STUB_WAIT_MAIN_RST);
			lpm_abort();
		}

		lpm_seq_trace(TRACE_PM_ACTION_LPM_SEQ_DM_STUB_WAIT_MAIN_RST);

		/*
		 * Set DM LPSC to enabled as early as possible as JTAG
		 * will not connect until this is done.
		 */
		if (enable_dm_lpsc()) {
			lpm_seq_trace_fail(TRACE_PM_ACTION_LPM_SEQ_DM_STUB_MAIN_DM_LPSC_EN);
			lpm_abort();
		} else {
			lpm_seq_trace(TRACE_PM_ACTION_LPM_SEQ_DM_STUB_MAIN_DM_LPSC_EN);
		}

		/* Modify WKUP_CLKSEL in WKUP_CTRL
		 * to use SMS_PLL instead of MCU PLL
		 */
		writel(WKUP_CLKSEL_MAIN, WKUP_CTRL_MMR_BASE + WKUP_CLKSEL);

		lpm_seq_trace(TRACE_PM_ACTION_LPM_SEQ_DM_STUB_WKUP_CLKSEL_MAIN);

		/* Configure additional MCU PLLs and PSCs to return to
		 * pre-DeepSleep state
		 */
		enable_mcu_remain_pll();

		lpm_seq_trace(TRACE_PM_ACTION_LPM_SEQ_DM_STUB_EN_MCU_PLLS);

		/* Poll on WKUP DS_MAGIC_WORD for 0x00d5d02e that indicates
		 * TIFS ROM has completed and execution can continue.
		 * Clear WKUP DS_MAGIC_WORD
		 */
		if (wait_for_tifs_ready()) {
			lpm_seq_trace_fail(TRACE_PM_ACTION_LPM_SEQ_DM_STUB_WAIT_TIFS);
			lpm_abort();
		}

		lpm_seq_trace(TRACE_PM_ACTION_LPM_SEQ_DM_STUB_WAIT_TIFS);

		/* Send TISCI ROM Boot image message containing location
		 * and boot address to load FS stub from SPS Memory
		 * TISCI_MSG_FIRMWARE_LOAD
		 */
		if (send_tisci_msg_firmware_load()) {
			lpm_seq_trace_fail(TRACE_PM_ACTION_LPM_SEQ_DM_STUB_FS_STUB_LD);
			lpm_abort();
		} else {
			lpm_seq_trace(TRACE_PM_ACTION_LPM_SEQ_DM_STUB_FS_STUB_LD);
		}

		/* Wait for TISCI Message to indicate DDR restore can resume */
		/* TISCI_MSG_CONTINUE_RESUME */
		if (receive_tisci_msg_continue_resume_req()) {
			lpm_seq_trace_fail(TRACE_PM_ACTION_LPM_SEQ_DM_STUB_TISCI_CONT_RES);
			lpm_abort();
		} else {
			lpm_seq_trace(TRACE_PM_ACTION_LPM_SEQ_DM_STUB_TISCI_CONT_RES);
		}

		/* Disable MAIN IO Daisy Chain and IO Isolation */
		if (disable_main_io_isolation()) {
			lpm_seq_trace_fail(TRACE_PM_ACTION_LPM_SEQ_DM_STUB_DIS_MAIN_IO_ISO);
			lpm_abort();
		} else {
			lpm_seq_trace(TRACE_PM_ACTION_LPM_SEQ_DM_STUB_DIS_MAIN_IO_ISO);
		}

		/* Configure additional MAIN PLLs and PSCs for EMIF operation */
		if (enable_main_remain_pll()) {
			lpm_seq_trace_fail(TRACE_PM_ACTION_LPM_SEQ_DM_STUB_EN_MAIN_PLLS);
			lpm_abort();
		} else {
			lpm_seq_trace(TRACE_PM_ACTION_LPM_SEQ_DM_STUB_EN_MAIN_PLLS);
		}


		lpm_seq_trace(TRACE_PM_ACTION_LPM_SEQ_DM_STUB_DIS_DDR_RST_ISO);
	} else {
		/* Send TISCI message to restore SMS PLL */
		send_msg_restore_sms_pll();

		/* Relock all PLLs as described in Copy of
		 * PSC_PLL_config_LPM_0p5_ch.xlsx
		 */
		enable_pll_standby();
	}

	/* Disable GPIO wake up */
	disable_gpio_wake_up();

	exit_ddr_low_power_mode();

	lpm_seq_trace(TRACE_PM_ACTION_LPM_SEQ_DM_STUB_DDR_SR_EXIT);

	if (g_params.mode == LPM_DEEPSLEEP || g_params.mode == LPM_MCU_ONLY) {
		release_usb_reset_isolation();

		lpm_seq_trace(TRACE_PM_ACTION_LPM_SEQ_DM_STUB_DIS_DDR_RST_ISO);

		/* Send TISCI Message to TIFS to indicate DDR is active and
		 * resume can proceed, include address of TIFS context
		 */
		if (send_tisci_msg_continue_resume_resp()) {
			lpm_seq_trace_fail(TRACE_PM_ACTION_LPM_SEQ_DM_STUB_RESP_CONT_RES);
			lpm_abort();
		} else {
			lpm_seq_trace(TRACE_PM_ACTION_LPM_SEQ_DM_STUB_RESP_CONT_RES);
		}

		/* Wait for TISCI_MSG_SYNC_RESUME msg */
		if (receive_tisci_msg_sync_resume_req()) {
			lpm_seq_trace_fail(TRACE_PM_ACTION_LPM_SEQ_DM_STUB_TISCI_SYNC_RES);
			lpm_abort();
		} else {
			lpm_seq_trace(TRACE_PM_ACTION_LPM_SEQ_DM_STUB_TISCI_SYNC_RES);
		}
	}


	clear_prepare_sleep_data();

	/* Return to standard firmware in DDR zero */
	return 0;
}

void dm_stub_irq_handler(void)
{
	u32 int_num;
	const struct wake_source_data *active_wake_source = NULL;
	int i;

	wake_up_source = TISCI_MSG_VALUE_LPM_WAKE_SOURCE_INVALID;

	/*
	 * XXX: Add an additional delay for oscillator to stablize.
	 */
	for (i = 0; i < 10; i++) {
		delay_1us();
	}

	int_num = vim_get_intr_number();

	for (i = 0; i < WAKEUP_SOURCE_MAX; i++) {
		if (soc_wake_sources_data[i].int_num == int_num) {
			active_wake_source = &soc_wake_sources_data[i];
			wake_up_source = soc_wake_sources_data[i].source_id;
			break;
		}
	}

	if (active_wake_source != NULL) {
		lpm_seq_trace_val(TRACE_PM_ACTION_LPM_SEQ_DM_STUB_WAKE_EVENT, active_wake_source->wkup_idx);
	} else {
		lpm_seq_trace_fail(TRACE_PM_ACTION_LPM_SEQ_DM_STUB_WAKE_EVENT);
	}

	vim_set_intr_enable(int_num, INTR_DISABLE);
	vim_clear_intr(int_num);

	disable_wake_sources();
	vim_irq_complete();
}

u32 lpm_get_wake_up_source(void)
{
	return wake_up_source;
}
