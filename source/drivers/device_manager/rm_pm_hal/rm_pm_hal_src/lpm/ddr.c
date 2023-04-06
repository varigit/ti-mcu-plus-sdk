/*
 * System Firmware
 *
 * LPM DDR driver
 *
 * Copyright (C) 2021-2022, Texas Instruments Incorporated
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
#include <lib/io.h>
#include <types/errno.h>
#include <types/short_types.h>
#include <baseaddress.h>
#include <wkup_ctrl_mmr.h>
#include "ddr.h"

#include "lpm_trace.h"

#define DDR_RETRAIN_TIMEOUT 10000000

/* Cadence DDR registers */
#define CDNS_DENALI_PHY_1333					0x54d4
#define CDNS_DENALI_PHY_1333_PHY_CAL_START_0			0x01000000

#define CDNS_DENALI_PHY_1336					0x54e0
#define CDNS_DENALI_PHY_1336_PHY_CAL_RESULT_OBS_0		(BIT(23))

#define CDNS_DENALI_PHY_1364					0x5550
#define CDNS_DENALI_PHY_1364_PHY_INIT_UPDATE_CONFIG_MASK	0x7
#define CDNS_DENALI_PHY_1364_PHY_INIT_UPDATE_CONFIG_SHIFT	0x8

#define CDNS_DENALI_PHY_1369					0x5564
#define CDNS_DENALI_PHY_1369_PHY_UPDATE_MASK			0x1

#define CDNS_DENALI_PI_33					0x2084
#define CDNS_DENALI_PI_33_RDLVL_GATE_REQ			0x01000000

#define CDNS_DENALI_PI_34					0x2088
#define CDNS_DENALI_PI_34_RDLVL_CS				0x00000100

#define CDNS_DENALI_PI_67    					0x210C
#define CDNS_DENALI_PI_67_WDQLVL_CS				0x01000000
#define CDNS_DENALI_PI_67_WDQLVL_REQ				0x00000100

#define CDNS_DENALI_PI_83					0x214C
#define CDNS_DENALI_PI_83_LVL_DONE_BIT				0x2000

#define CDNS_DENALI_PI_84					0x2150
#define CDNS_DENALI_PI_84_INT_ACK_REG_MASK			0x1FFFFFFF

#define CDNS_DENALI_CTL_0    					0x0000
#define CDNS_DENALI_CTL_0_DRAM_CLASS_MASK                       0x00000F00
#define CDNS_DENALI_CTL_0_DRAM_CLASS_SHIFT                      0x00000008

#define CDNS_DENALI_CTL_0_DRAM_CLASS_DDR4			0xA
#define CDNS_DENALI_CTL_0_DRAM_CLASS_LPDDR4			0xB

static u32 ddr_read_ddr_type(void)
{
	u32 dram_class, val;

	val = readl(DDR_CTRL_BASE + CDNS_DENALI_CTL_0);
	dram_class = (val & CDNS_DENALI_CTL_0_DRAM_CLASS_MASK) >>
		     CDNS_DENALI_CTL_0_DRAM_CLASS_SHIFT;

	return dram_class;
}

void ddr_enter_low_power_mode(void)
{
	u32 val;

	/*
	 * - Set CDNS_DENALI_PHY_1369:PHY UPDATE MASK
	 * - Clear 0x7 in CDNS_DENALI_PHY_1364:PHY_INIT_UPDATE_CONFIG
	 */

	val = readl(DDR_CTRL_BASE + CDNS_DENALI_PHY_1369);
	val |= CDNS_DENALI_PHY_1369_PHY_UPDATE_MASK;
	writel(val, DDR_CTRL_BASE + CDNS_DENALI_PHY_1369);

	val = readl(DDR_CTRL_BASE + CDNS_DENALI_PHY_1364);
	val &= ~(CDNS_DENALI_PHY_1364_PHY_INIT_UPDATE_CONFIG_MASK <<
		 CDNS_DENALI_PHY_1364_PHY_INIT_UPDATE_CONFIG_SHIFT);
	writel(val, DDR_CTRL_BASE + CDNS_DENALI_PHY_1364);
}

void ddr_exit_low_power_mode(void)
{
	u32 val;

	/*
	 * - Clear CDNS_DENALI_PHY_1369:PHY UPDATE MASK
	 * - Set 0x7 in CDNS_DENALI_PHY_1364:PHY_INIT_UPDATE_CONFIG
	 */

	val = readl(DDR_CTRL_BASE + CDNS_DENALI_PHY_1369);
	val &= ~CDNS_DENALI_PHY_1369_PHY_UPDATE_MASK;
	writel(val, DDR_CTRL_BASE + CDNS_DENALI_PHY_1369);

	val = readl(DDR_CTRL_BASE + CDNS_DENALI_PHY_1364);
	val |= (CDNS_DENALI_PHY_1364_PHY_INIT_UPDATE_CONFIG_MASK <<
		CDNS_DENALI_PHY_1364_PHY_INIT_UPDATE_CONFIG_SHIFT);
	writel(val, DDR_CTRL_BASE + CDNS_DENALI_PHY_1364);
}

/* This only handles single rank, must dynamically detect dual rank */
s32 ddr_deepsleep_exit_training(void)
{
	u32 timeout = DDR_RETRAIN_TIMEOUT;
	s32 ret = SUCCESS;
	u32 val;

	// Write phy_cal_start_0 = 1
	val = readl(DDR_CTRL_BASE + CDNS_DENALI_PHY_1333);
	val |= CDNS_DENALI_PHY_1333_PHY_CAL_START_0;
	writel(val, DDR_CTRL_BASE + CDNS_DENALI_PHY_1333);

	// Poll on phy_cal_result_obs_0 bit 23 = 1
	while ((--timeout > 0) && (readl(DDR_CTRL_BASE + CDNS_DENALI_PHY_1336) & (CDNS_DENALI_PHY_1336_PHY_CAL_RESULT_OBS_0)) == 0);
	if (timeout == 0) {
		lpm_seq_trace_fail(0x92);
		ret = -EFAIL;
	}

	/* Soft trigger read gate training */
	// Program PI_INT_ACK={`ddr32_ew_PI_NUM_INT_SOURCES{1'b1}}
	writel(CDNS_DENALI_PI_84_INT_ACK_REG_MASK, DDR_CTRL_BASE + CDNS_DENALI_PI_84);

	// Program PI_RDLVL_CS=0
	val = readl(DDR_CTRL_BASE + CDNS_DENALI_PI_34);
	val &= ~CDNS_DENALI_PI_34_RDLVL_CS;
	writel(val, DDR_CTRL_BASE + CDNS_DENALI_PI_34);

	// Program PI_RDLVL_GATE_REQ=1
	val = readl(DDR_CTRL_BASE + CDNS_DENALI_PI_33);
	val |= CDNS_DENALI_PI_33_RDLVL_GATE_REQ;
	writel(val, DDR_CTRL_BASE + CDNS_DENALI_PI_33);

	// Polling PI_INT_STATUS[`ddr32_ew_PI_LVL_DONE_BIT]=1
	timeout = DDR_RETRAIN_TIMEOUT;
	while ((--timeout > 0) && (readl(DDR_CTRL_BASE + CDNS_DENALI_PI_83) & (CDNS_DENALI_PI_83_LVL_DONE_BIT)) == 0);
	if (timeout == 0) {
		lpm_seq_trace_fail(0x93);
		ret = -EFAIL;
	}

	// Program PI_INT_ACK={`ddr32_ew_PI_NUM_INT_SOURCES{1'b1}}
	writel(CDNS_DENALI_PI_84_INT_ACK_REG_MASK, DDR_CTRL_BASE + CDNS_DENALI_PI_84);

	/* LPDDR4 Only: Soft trigger write DQ training */
	if (ddr_read_ddr_type() == CDNS_DENALI_CTL_0_DRAM_CLASS_LPDDR4) {
		// Program PI_WDQLVL_CS=0
		val = readl(DDR_CTRL_BASE + CDNS_DENALI_PI_67);
		val &= ~CDNS_DENALI_PI_67_WDQLVL_CS;
		writel(val, DDR_CTRL_BASE + CDNS_DENALI_PI_67);

		// Program PI_WDQLVL_REQ=1
		val = readl(DDR_CTRL_BASE + CDNS_DENALI_PI_67);
		val |= CDNS_DENALI_PI_67_WDQLVL_REQ;
		writel(val, DDR_CTRL_BASE + CDNS_DENALI_PI_67);

		// Polling PI_INT_STATUS[`ddr32_ew_PI_LVL_DONE_BIT]=1
		timeout = DDR_RETRAIN_TIMEOUT;
		while ((--timeout > 0) && (readl(DDR_CTRL_BASE + CDNS_DENALI_PI_83) & (CDNS_DENALI_PI_83_LVL_DONE_BIT)) == 0);
		if (timeout == 0) {
			lpm_seq_trace_fail(0x94);
			ret = -EFAIL;
		}

		// Program PI_INT_ACK={`ddr32_ew_PI_NUM_INT_SOURCES{1'b1}}
		writel(CDNS_DENALI_PI_84_INT_ACK_REG_MASK, DDR_CTRL_BASE + CDNS_DENALI_PI_84);
	}

	return ret;
}

void ddr_enable_retention(void)
{
	/* TODO finish the sequence once it is consolidated */

	writel(DDR16SS_RETENTION_EN, WKUP_CTRL_MMR_BASE + DDR16SS_PMCTRL);
}

s32 ddr_disable_retention(void)
{

	writel(DDR16SS_RETENTION_DIS, WKUP_CTRL_MMR_BASE + DDR16SS_PMCTRL);

	return 0;
}
