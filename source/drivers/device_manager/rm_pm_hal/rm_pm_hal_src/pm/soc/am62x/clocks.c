/*
 * Data version: 221107_151433
 *
 * Copyright (C) 2017-2023, Texas Instruments Incorporated
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
#include <clk.h>
#include <clk_mux.h>
#include <clk_dev.h>
#include <clk_div.h>
#include <clk_pll.h>
#include <types/array_size.h>
#include <types/devgrps.h>
#include <soc/am62x/control.h>
#include <soc/am62x/clk_ids.h>
#include <soc/am62x/devices.h>
#include <soc/am62x/clocks.h>
#include <soc/am62x/clk_soc_hfosc0.h>
#include <soc/am62x/clk_soc_lfosc0.h>
#include <clk_fixed.h>
#include <clk_pll_16fft.h>
#include <clk_div.h>
#include <clk_pllctrl.h>


enum {
	AM62X_FREQ_VALUE_BOARD_0_AUDIO_EXT_REFCLK0_OUT,
	AM62X_FREQ_VALUE_BOARD_0_AUDIO_EXT_REFCLK1_OUT,
	AM62X_FREQ_VALUE_BOARD_0_CP_GEMAC_CPTS0_RFT_CLK_OUT,
	AM62X_FREQ_VALUE_BOARD_0_DDR0_CK0_OUT,
	AM62X_FREQ_VALUE_BOARD_0_EXT_REFCLK1_OUT,
	AM62X_FREQ_VALUE_BOARD_0_GPMC0_CLKLB_OUT,
	AM62X_FREQ_VALUE_BOARD_0_I2C0_SCL_OUT,
	AM62X_FREQ_VALUE_BOARD_0_I2C1_SCL_OUT,
	AM62X_FREQ_VALUE_BOARD_0_I2C2_SCL_OUT,
	AM62X_FREQ_VALUE_BOARD_0_I2C3_SCL_OUT,
	AM62X_FREQ_VALUE_BOARD_0_MCASP0_ACLKR_OUT,
	AM62X_FREQ_VALUE_BOARD_0_MCASP0_ACLKX_OUT,
	AM62X_FREQ_VALUE_BOARD_0_MCASP1_ACLKR_OUT,
	AM62X_FREQ_VALUE_BOARD_0_MCASP1_ACLKX_OUT,
	AM62X_FREQ_VALUE_BOARD_0_MCASP2_ACLKR_OUT,
	AM62X_FREQ_VALUE_BOARD_0_MCASP2_ACLKX_OUT,
	AM62X_FREQ_VALUE_BOARD_0_MCU_EXT_REFCLK0_OUT,
	AM62X_FREQ_VALUE_BOARD_0_MCU_I2C0_SCL_OUT,
	AM62X_FREQ_VALUE_BOARD_0_MCU_SPI0_CLK_OUT,
	AM62X_FREQ_VALUE_BOARD_0_MCU_SPI1_CLK_OUT,
	AM62X_FREQ_VALUE_BOARD_0_MMC0_CLKLB_OUT,
	AM62X_FREQ_VALUE_BOARD_0_MMC0_CLK_OUT,
	AM62X_FREQ_VALUE_BOARD_0_MMC1_CLKLB_OUT,
	AM62X_FREQ_VALUE_BOARD_0_MMC1_CLK_OUT,
	AM62X_FREQ_VALUE_BOARD_0_MMC2_CLKLB_OUT,
	AM62X_FREQ_VALUE_BOARD_0_MMC2_CLK_OUT,
	AM62X_FREQ_VALUE_BOARD_0_OSPI0_DQS_OUT,
	AM62X_FREQ_VALUE_BOARD_0_OSPI0_LBCLKO_OUT,
	AM62X_FREQ_VALUE_BOARD_0_RGMII1_RXC_OUT,
	AM62X_FREQ_VALUE_BOARD_0_RGMII1_TXC_OUT,
	AM62X_FREQ_VALUE_BOARD_0_RGMII2_RXC_OUT,
	AM62X_FREQ_VALUE_BOARD_0_RGMII2_TXC_OUT,
	AM62X_FREQ_VALUE_BOARD_0_RMII1_REF_CLK_OUT,
	AM62X_FREQ_VALUE_BOARD_0_RMII2_REF_CLK_OUT,
	AM62X_FREQ_VALUE_BOARD_0_SPI0_CLK_OUT,
	AM62X_FREQ_VALUE_BOARD_0_SPI1_CLK_OUT,
	AM62X_FREQ_VALUE_BOARD_0_SPI2_CLK_OUT,
	AM62X_FREQ_VALUE_BOARD_0_TCK_OUT,
	AM62X_FREQ_VALUE_BOARD_0_VOUT0_EXTPCLKIN_OUT,
	AM62X_FREQ_VALUE_CPSW_3GUSS_MAIN_0_CPTS_GENF0,
	AM62X_FREQ_VALUE_CPSW_3GUSS_MAIN_0_CPTS_GENF1,
	AM62X_FREQ_VALUE_CPSW_3GUSS_MAIN_0_MDIO_MDCLK_O,
	AM62X_FREQ_VALUE_CPSW_3GUSS_MAIN_0_RGMII1_TXC_O,
	AM62X_FREQ_VALUE_CPSW_3GUSS_MAIN_0_RGMII2_TXC_O,
	AM62X_FREQ_VALUE_DEBUGSS_K3_WRAP_CV0_MAIN_0_CSTPIU_TRACECLK,
	AM62X_FREQ_VALUE_DMTIMER_DMC1MS_MAIN_0_TIMER_PWM,
	AM62X_FREQ_VALUE_DMTIMER_DMC1MS_MAIN_1_TIMER_PWM,
	AM62X_FREQ_VALUE_DMTIMER_DMC1MS_MAIN_2_TIMER_PWM,
	AM62X_FREQ_VALUE_DMTIMER_DMC1MS_MAIN_3_TIMER_PWM,
	AM62X_FREQ_VALUE_DMTIMER_DMC1MS_MAIN_4_TIMER_PWM,
	AM62X_FREQ_VALUE_DMTIMER_DMC1MS_MAIN_5_TIMER_PWM,
	AM62X_FREQ_VALUE_DMTIMER_DMC1MS_MAIN_6_TIMER_PWM,
	AM62X_FREQ_VALUE_DMTIMER_DMC1MS_MAIN_7_TIMER_PWM,
	AM62X_FREQ_VALUE_DMTIMER_DMC1MS_MCU_0_TIMER_PWM,
	AM62X_FREQ_VALUE_DMTIMER_DMC1MS_MCU_1_TIMER_PWM,
	AM62X_FREQ_VALUE_DMTIMER_DMC1MS_MCU_2_TIMER_PWM,
	AM62X_FREQ_VALUE_DMTIMER_DMC1MS_MCU_3_TIMER_PWM,
	AM62X_FREQ_VALUE_EMMCSD4SS_MAIN_0_EMMCSDSS_IO_CLK_O,
	AM62X_FREQ_VALUE_EMMCSD4SS_MAIN_1_EMMCSDSS_IO_CLK_O,
	AM62X_FREQ_VALUE_EMMCSD8SS_MAIN_0_EMMCSDSS_IO_CLK_O,
	AM62X_FREQ_VALUE_FSS_UL_MAIN_0_OSPI_0_OSPI_OCLK_CLK,
	AM62X_FREQ_VALUE_GPMC_MAIN_0_PO_GPMC_DEV_CLK,
	AM62X_FREQ_VALUE_K3_DPHY_RX_MAIN_0_IO_RX_CL_L_M,
	AM62X_FREQ_VALUE_K3_DPHY_RX_MAIN_0_IO_RX_CL_L_P,
	AM62X_FREQ_VALUE_K3_DPHY_RX_MAIN_0_PPI_RX_BYTE_CLK,
	AM62X_FREQ_VALUE_K3_DSS_UL_MAIN_0_DPI_1_OUT_CLK,
	AM62X_FREQ_VALUE_MCASP_MAIN_0_MCASP_ACLKR_POUT,
	AM62X_FREQ_VALUE_MCASP_MAIN_0_MCASP_ACLKX_POUT,
	AM62X_FREQ_VALUE_MCASP_MAIN_0_MCASP_AFSR_POUT,
	AM62X_FREQ_VALUE_MCASP_MAIN_0_MCASP_AFSX_POUT,
	AM62X_FREQ_VALUE_MCASP_MAIN_0_MCASP_AHCLKR_POUT,
	AM62X_FREQ_VALUE_MCASP_MAIN_0_MCASP_AHCLKX_POUT,
	AM62X_FREQ_VALUE_MCASP_MAIN_1_MCASP_ACLKR_POUT,
	AM62X_FREQ_VALUE_MCASP_MAIN_1_MCASP_ACLKX_POUT,
	AM62X_FREQ_VALUE_MCASP_MAIN_1_MCASP_AFSR_POUT,
	AM62X_FREQ_VALUE_MCASP_MAIN_1_MCASP_AFSX_POUT,
	AM62X_FREQ_VALUE_MCASP_MAIN_1_MCASP_AHCLKR_POUT,
	AM62X_FREQ_VALUE_MCASP_MAIN_1_MCASP_AHCLKX_POUT,
	AM62X_FREQ_VALUE_MCASP_MAIN_2_MCASP_ACLKR_POUT,
	AM62X_FREQ_VALUE_MCASP_MAIN_2_MCASP_ACLKX_POUT,
	AM62X_FREQ_VALUE_MCASP_MAIN_2_MCASP_AFSR_POUT,
	AM62X_FREQ_VALUE_MCASP_MAIN_2_MCASP_AFSX_POUT,
	AM62X_FREQ_VALUE_MCASP_MAIN_2_MCASP_AHCLKR_POUT,
	AM62X_FREQ_VALUE_MCASP_MAIN_2_MCASP_AHCLKX_POUT,
	AM62X_FREQ_VALUE_MSHSI2C_MAIN_0_PORSCL,
	AM62X_FREQ_VALUE_MSHSI2C_MAIN_1_PORSCL,
	AM62X_FREQ_VALUE_MSHSI2C_MAIN_2_PORSCL,
	AM62X_FREQ_VALUE_MSHSI2C_MAIN_3_PORSCL,
	AM62X_FREQ_VALUE_MSHSI2C_MCU_0_PORSCL,
	AM62X_FREQ_VALUE_MSHSI2C_WKUP_0_PORSCL,
	AM62X_FREQ_VALUE_PLLFRACF2_SSMOD_16FFT_MAIN_0,
	AM62X_FREQ_VALUE_PLLFRACF2_SSMOD_16FFT_MAIN_1,
	AM62X_FREQ_VALUE_PLLFRACF2_SSMOD_16FFT_MAIN_12,
	AM62X_FREQ_VALUE_PLLFRACF2_SSMOD_16FFT_MAIN_15,
	AM62X_FREQ_VALUE_PLLFRACF2_SSMOD_16FFT_MAIN_16,
	AM62X_FREQ_VALUE_PLLFRACF2_SSMOD_16FFT_MAIN_17,
	AM62X_FREQ_VALUE_PLLFRACF2_SSMOD_16FFT_MAIN_2,
	AM62X_FREQ_VALUE_PLLFRACF2_SSMOD_16FFT_MAIN_8,
	AM62X_FREQ_VALUE_PLLFRACF2_SSMOD_16FFT_MCU_0,
	AM62X_FREQ_VALUE_SAM62_A53_512KB_WRAP_MAIN_0_ARM_COREPACK_0_A53_DIVH_CLK4_OBSCLK_OUT_CLK,
	AM62X_FREQ_VALUE_SPI_MAIN_0_IO_CLKSPIO_CLK,
	AM62X_FREQ_VALUE_SPI_MAIN_1_IO_CLKSPIO_CLK,
	AM62X_FREQ_VALUE_SPI_MAIN_2_IO_CLKSPIO_CLK,
	AM62X_FREQ_VALUE_SPI_MCU_0_IO_CLKSPIO_CLK,
	AM62X_FREQ_VALUE_SPI_MCU_1_IO_CLKSPIO_CLK,
	AM62X_FREQ_VALUE_COUNT,
};

enum {
	AM62X_FREQ_RANGE_ANY,
	AM62X_FREQ_RANGE_GLUELOGIC_RCOSC_CLKOUT,
	AM62X_FREQ_RANGE_GLUELOGIC_RCOSC_CLK_1P0V_97P65K,
	AM62X_FREQ_RANGE_VCO_PLLFRACF2_SSMOD_16FFT_MAIN_0,
	AM62X_FREQ_RANGE_VCO_IN_PLLFRACF2_SSMOD_16FFT_MAIN_0,
	AM62X_FREQ_RANGE_ID_MAX,
};

enum {
	PLL_ENTRY_MCU_24MHZ_2400MHZ,
	PLL_ENTRY_MCU_25MHZ_2400MHZ,
	PLL_ENTRY_MCU_26MHZ_2400MHZ_F24BIT,
	PLL_ENTRY_MAIN_24MHZ_1000MHZ_F24BIT,
	PLL_ENTRY_MAIN_25MHZ_1000MHZ,
	PLL_ENTRY_MAIN_26MHZ_1000MHZ_F24BIT,
	PLL_ENTRY_PER0_24MHZ_960MHZ,
	PLL_ENTRY_PER0_25MHZ_960MHZ,
	PLL_ENTRY_PER0_26MHZ_960MHZ_F24BIT,
	PLL_ENTRY_ARM0_24MHZ_2500MHZ_F24BIT,
	PLL_ENTRY_ARM0_25MHZ_2500MHZ,
	PLL_ENTRY_ARM0_26MHZ_2500MHZ_F24BIT,
	PLL_ENTRY_DDR_24MHZ_1600MHZ_F24BIT,
	PLL_ENTRY_DDR_25MHZ_1600MHZ,
	PLL_ENTRY_DDR_26MHZ_1600MHZ_F24BIT,
	PLL_ENTRY_SMS_24MHZ_2000MHZ_F24BIT,
	PLL_ENTRY_SMS_25MHZ_2000MHZ,
	PLL_ENTRY_SMS_26MHZ_2000MHZ_F24BIT,
	PLL_ENTRY_DSS0_24MHZ_2380MHZ_F24BIT,
	PLL_ENTRY_DSS0_25MHZ_2380MHZ_F24BIT,
	PLL_ENTRY_DSS0_26MHZ_2380MHZ_F24BIT,
	PLL_ENTRY_DSS1_24MHZ_2040MHZ,
	PLL_ENTRY_DSS1_25MHZ_2040MHZ_F24BIT,
	PLL_ENTRY_DSS1_26MHZ_2040MHZ_F24BIT,
};

enum {
	NO_DEFAULT_FREQ,
	FREQ_PLLFRACF2_SSMOD_16FFT_MAIN_0_DEFAULT,
	FREQ_PLLFRACF2_SSMOD_16FFT_MAIN_1_DEFAULT,
	FREQ_PLLFRACF2_SSMOD_16FFT_MAIN_12_DEFAULT,
	FREQ_PLLFRACF2_SSMOD_16FFT_MAIN_15_DEFAULT,
	FREQ_PLLFRACF2_SSMOD_16FFT_MAIN_16_DEFAULT,
	FREQ_PLLFRACF2_SSMOD_16FFT_MAIN_17_DEFAULT,
	FREQ_PLLFRACF2_SSMOD_16FFT_MAIN_8_DEFAULT,
	FREQ_PLLFRACF2_SSMOD_16FFT_MCU_0_DEFAULT,
};

u32 soc_clock_values[AM62X_FREQ_VALUE_COUNT];

const struct clk_range soc_clock_ranges[AM62X_FREQ_RANGE_ID_MAX] = {
	CLK_RANGE(AM62X_FREQ_RANGE_ANY,					0,	   ULONG_MAX),
	CLK_RANGE(AM62X_FREQ_RANGE_GLUELOGIC_RCOSC_CLKOUT,		12500000,  12500000),
	CLK_RANGE(AM62X_FREQ_RANGE_GLUELOGIC_RCOSC_CLK_1P0V_97P65K,	97656,	   97656),
	CLK_RANGE(AM62X_FREQ_RANGE_VCO_IN_PLLFRACF2_SSMOD_16FFT_MAIN_0, 5000000,   1200000000),
	CLK_RANGE(AM62X_FREQ_RANGE_VCO_PLLFRACF2_SSMOD_16FFT_MAIN_0,	800000000, 3200000000),
};

const struct clk_default soc_clock_freq_defaults[] = {
	CLK_DEFAULT(FREQ_PLLFRACF2_SSMOD_16FFT_MAIN_0_DEFAULT,	1000000000UL, 1000000000UL, 1000000000UL),
	CLK_DEFAULT(FREQ_PLLFRACF2_SSMOD_16FFT_MAIN_1_DEFAULT,	960000000UL,  960000000UL,  960000000UL),
	CLK_DEFAULT(FREQ_PLLFRACF2_SSMOD_16FFT_MAIN_12_DEFAULT, 1600000000UL, 1600000000UL, 1600000000UL),
	CLK_DEFAULT(FREQ_PLLFRACF2_SSMOD_16FFT_MAIN_15_DEFAULT, 2000000000UL, 2000000000UL, 2000000000UL),
	CLK_DEFAULT(FREQ_PLLFRACF2_SSMOD_16FFT_MAIN_16_DEFAULT, 2380000000UL, 2380000000UL, 2380000000UL),
	CLK_DEFAULT(FREQ_PLLFRACF2_SSMOD_16FFT_MAIN_17_DEFAULT, 2040000000UL, 2040000000UL, 2040000000UL),
	CLK_DEFAULT(FREQ_PLLFRACF2_SSMOD_16FFT_MAIN_8_DEFAULT,	2500000000UL, 2500000000UL, 2500000000UL),
	CLK_DEFAULT(FREQ_PLLFRACF2_SSMOD_16FFT_MCU_0_DEFAULT,	2400000000UL, 2400000000UL, 2400000000UL),
};

const struct pll_table_entry soc_pll_table[] = {
	[PLL_ENTRY_MCU_24MHZ_2400MHZ] =		{
		.freq_min_hz	= 2400000000U,
		.freq_max_hz	= 2400000000U,
		.plld		= 1U,
		.pllm		= 100U,
		.pllfm		= 0U,
		.clkod		= 1U,
	},
	[PLL_ENTRY_MCU_25MHZ_2400MHZ] =		{
		.freq_min_hz	= 2400000000U,
		.freq_max_hz	= 2400000000U,
		.plld		= 1U,
		.pllm		= 96U,
		.pllfm		= 0U,
		.clkod		= 1U,
	},
	[PLL_ENTRY_MCU_26MHZ_2400MHZ_F24BIT] =	{
		.freq_min_hz	= 2400000000U,
		.freq_max_hz	= 2400000001U,
		.plld		= 1U,
		.pllm		= 92U,
		.pllfm		= 5162221U,
		.clkod		= 1U,
	},
	[PLL_ENTRY_MAIN_24MHZ_1000MHZ_F24BIT] = {
		.freq_min_hz	= 1000000000U,
		.freq_max_hz	= 1000000000U,
		.plld		= 1U,
		.pllm		= 83U,
		.pllfm		= 5592406U,
		.clkod		= 2U,
	},
	[PLL_ENTRY_MAIN_25MHZ_1000MHZ] =	{
		.freq_min_hz	= 1000000000U,
		.freq_max_hz	= 1000000000U,
		.plld		= 1U,
		.pllm		= 80U,
		.pllfm		= 0U,
		.clkod		= 2U,
	},
	[PLL_ENTRY_MAIN_26MHZ_1000MHZ_F24BIT] = {
		.freq_min_hz	= 1000000000U,
		.freq_max_hz	= 1000000000U,
		.plld		= 1U,
		.pllm		= 76U,
		.pllfm		= 15486661U,
		.clkod		= 2U,
	},
	[PLL_ENTRY_PER0_24MHZ_960MHZ] =		{
		.freq_min_hz	= 960000000U,
		.freq_max_hz	= 960000000U,
		.plld		= 1U,
		.pllm		= 80U,
		.pllfm		= 0U,
		.clkod		= 2U,
	},
	[PLL_ENTRY_PER0_25MHZ_960MHZ] =		{
		.freq_min_hz	= 960000000U,
		.freq_max_hz	= 960000000U,
		.plld		= 5U,
		.pllm		= 384U,
		.pllfm		= 0U,
		.clkod		= 2U,
	},
	[PLL_ENTRY_PER0_26MHZ_960MHZ_F24BIT] =	{
		.freq_min_hz	= 960000000U,
		.freq_max_hz	= 960000000U,
		.plld		= 1U,
		.pllm		= 73U,
		.pllfm		= 14196106U,
		.clkod		= 2U,
	},
	[PLL_ENTRY_ARM0_24MHZ_2500MHZ_F24BIT] = {
		.freq_min_hz	= 2500000000U,
		.freq_max_hz	= 2500000000U,
		.plld		= 1U,
		.pllm		= 104U,
		.pllfm		= 2796203U,
		.clkod		= 1U,
	},
	[PLL_ENTRY_ARM0_25MHZ_2500MHZ] =	{
		.freq_min_hz	= 2500000000U,
		.freq_max_hz	= 2500000000U,
		.plld		= 1U,
		.pllm		= 100U,
		.pllfm		= 0U,
		.clkod		= 1U,
	},
	[PLL_ENTRY_ARM0_26MHZ_2500MHZ_F24BIT] = {
		.freq_min_hz	= 2500000000U,
		.freq_max_hz	= 2500000001U,
		.plld		= 1U,
		.pllm		= 96U,
		.pllfm		= 2581111U,
		.clkod		= 1U,
	},
	[PLL_ENTRY_DDR_24MHZ_1600MHZ_F24BIT] =	{
		.freq_min_hz	= 1600000000U,
		.freq_max_hz	= 1600000000U,
		.plld		= 1U,
		.pllm		= 66U,
		.pllfm		= 11184811U,
		.clkod		= 1U,
	},
	[PLL_ENTRY_DDR_25MHZ_1600MHZ] =		{
		.freq_min_hz	= 1600000000U,
		.freq_max_hz	= 1600000000U,
		.plld		= 1U,
		.pllm		= 64U,
		.pllfm		= 0U,
		.clkod		= 1U,
	},
	[PLL_ENTRY_DDR_26MHZ_1600MHZ_F24BIT] =	{
		.freq_min_hz	= 1600000000U,
		.freq_max_hz	= 1600000000U,
		.plld		= 1U,
		.pllm		= 61U,
		.pllfm		= 9033886U,
		.clkod		= 1U,
	},
	[PLL_ENTRY_SMS_24MHZ_2000MHZ_F24BIT] =	{
		.freq_min_hz	= 2000000000U,
		.freq_max_hz	= 2000000000U,
		.plld		= 1U,
		.pllm		= 83U,
		.pllfm		= 5592406U,
		.clkod		= 1U,
	},
	[PLL_ENTRY_SMS_25MHZ_2000MHZ] =		{
		.freq_min_hz	= 2000000000U,
		.freq_max_hz	= 2000000000U,
		.plld		= 1U,
		.pllm		= 80U,
		.pllfm		= 0U,
		.clkod		= 1U,
	},
	[PLL_ENTRY_SMS_26MHZ_2000MHZ_F24BIT] =	{
		.freq_min_hz	= 2000000000U,
		.freq_max_hz	= 2000000000U,
		.plld		= 1U,
		.pllm		= 76U,
		.pllfm		= 15486661U,
		.clkod		= 1U,
	},
	[PLL_ENTRY_DSS0_24MHZ_2380MHZ_F24BIT] = {
		.freq_min_hz	= 2380000000U,
		.freq_max_hz	= 2380000000U,
		.plld		= 1U,
		.pllm		= 99U,
		.pllfm		= 2796203U,
		.clkod		= 1U,
	},
	[PLL_ENTRY_DSS0_25MHZ_2380MHZ_F24BIT] = {
		.freq_min_hz	= 2380000000U,
		.freq_max_hz	= 2380000000U,
		.plld		= 2U,
		.pllm		= 190U,
		.pllfm		= 6710887U,
		.clkod		= 1U,
	},
	[PLL_ENTRY_DSS0_26MHZ_2380MHZ_F24BIT] = {
		.freq_min_hz	= 2380000000U,
		.freq_max_hz	= 2380000000U,
		.plld		= 1U,
		.pllm		= 91U,
		.pllfm		= 9033886U,
		.clkod		= 1U,
	},
	[PLL_ENTRY_DSS1_24MHZ_2040MHZ] =	{
		.freq_min_hz	= 2040000000U,
		.freq_max_hz	= 2040000000U,
		.plld		= 1U,
		.pllm		= 85U,
		.pllfm		= 0U,
		.clkod		= 1U,
	},
	[PLL_ENTRY_DSS1_25MHZ_2040MHZ_F24BIT] = {
		.freq_min_hz	= 2040000000U,
		.freq_max_hz	= 2040000000U,
		.plld		= 1U,
		.pllm		= 81U,
		.pllfm		= 10066330U,
		.clkod		= 1U,
	},
	[PLL_ENTRY_DSS1_26MHZ_2040MHZ_F24BIT] = {
		.freq_min_hz	= 2040000000U,
		.freq_max_hz	= 2040000000U,
		.plld		= 1U,
		.pllm		= 78U,
		.pllfm		= 7743331U,
		.clkod		= 1U,
	},
};

static const u8 pllfracf2_ssmod_16fft_main_0_entries[] = {
	PLL_ENTRY_MAIN_24MHZ_1000MHZ_F24BIT,
	PLL_ENTRY_MAIN_25MHZ_1000MHZ,
	PLL_ENTRY_MAIN_26MHZ_1000MHZ_F24BIT,
	PLL_TABLE_LAST,
};
static const u8 pllfracf2_ssmod_16fft_main_1_entries[] = {
	PLL_ENTRY_PER0_24MHZ_960MHZ,
	PLL_ENTRY_PER0_25MHZ_960MHZ,
	PLL_ENTRY_PER0_26MHZ_960MHZ_F24BIT,
	PLL_TABLE_LAST,
};
static const u8 pllfracf2_ssmod_16fft_main_12_entries[] = {
	PLL_ENTRY_DDR_24MHZ_1600MHZ_F24BIT,
	PLL_ENTRY_DDR_25MHZ_1600MHZ,
	PLL_ENTRY_DDR_26MHZ_1600MHZ_F24BIT,
	PLL_TABLE_LAST,
};
static const u8 pllfracf2_ssmod_16fft_main_15_entries[] = {
	PLL_ENTRY_SMS_24MHZ_2000MHZ_F24BIT,
	PLL_ENTRY_SMS_25MHZ_2000MHZ,
	PLL_ENTRY_SMS_26MHZ_2000MHZ_F24BIT,
	PLL_TABLE_LAST,
};
static const u8 pllfracf2_ssmod_16fft_main_16_entries[] = {
	PLL_ENTRY_DSS0_24MHZ_2380MHZ_F24BIT,
	PLL_ENTRY_DSS0_25MHZ_2380MHZ_F24BIT,
	PLL_ENTRY_DSS0_26MHZ_2380MHZ_F24BIT,
	PLL_TABLE_LAST,
};
static const u8 pllfracf2_ssmod_16fft_main_17_entries[] = {
	PLL_ENTRY_DSS1_24MHZ_2040MHZ,
	PLL_ENTRY_DSS1_25MHZ_2040MHZ_F24BIT,
	PLL_ENTRY_DSS1_26MHZ_2040MHZ_F24BIT,
	PLL_TABLE_LAST,
};
static const u8 pllfracf2_ssmod_16fft_main_8_entries[] = {
	PLL_ENTRY_ARM0_24MHZ_2500MHZ_F24BIT,
	PLL_ENTRY_ARM0_25MHZ_2500MHZ,
	PLL_ENTRY_ARM0_26MHZ_2500MHZ_F24BIT,
	PLL_TABLE_LAST,
};
static const u8 pllfracf2_ssmod_16fft_mcu_0_entries[] = {
	PLL_ENTRY_MCU_24MHZ_2400MHZ,
	PLL_ENTRY_MCU_25MHZ_2400MHZ,
	PLL_ENTRY_MCU_26MHZ_2400MHZ_F24BIT,
	PLL_TABLE_LAST,
};

static const struct clk_parent clk_AUDIO_REFCLKn_out0_parents[] = {
	{
		CLK_AM62X_MCASP_MAIN_0_MCASP_AHCLKR_POUT,
		1,
	},
	{
		CLK_AM62X_MCASP_MAIN_1_MCASP_AHCLKR_POUT,
		1,
	},
	{
		CLK_AM62X_MCASP_MAIN_2_MCASP_AHCLKR_POUT,
		1,
	},
	{
		CLK_AM62X_MCASP_MAIN_0_MCASP_AHCLKX_POUT,
		1,
	},
	{
		CLK_AM62X_MCASP_MAIN_1_MCASP_AHCLKX_POUT,
		1,
	},
	{
		CLK_AM62X_MCASP_MAIN_2_MCASP_AHCLKX_POUT,
		1,
	},
	{
		CLK_AM62X_POSTDIV1_16FFT_MAIN_1_HSDIVOUT6_CLK,
		1,
	},
	{
		CLK_AM62X_POSTDIV4_16FF_MAIN_2_HSDIVOUT8_CLK,
		1,
	},
};
static const struct clk_data_mux_reg clk_data_AUDIO_REFCLKn_out0 = {
	.data_mux		= {
		.parents	= clk_AUDIO_REFCLKn_out0_parents,
		.n		= ARRAY_SIZE(clk_AUDIO_REFCLKn_out0_parents),
	},
	.reg			= 0x00100000 + 33504,
	.bit			= 0,
};
static const struct clk_data_mux_reg clk_data_AUDIO_REFCLKn_out1 = {
	.data_mux		= {
		.parents	= clk_AUDIO_REFCLKn_out0_parents,
		.n		= ARRAY_SIZE(clk_AUDIO_REFCLKn_out0_parents),
	},
	.reg			= 0x00100000 + 33508,
	.bit			= 0,
};
static const struct clk_parent clk_CLKOUT0_CTRL_parents[] = {
	{
		CLK_AM62X_HSDIV4_16FFT_MAIN_2_HSDIVOUT1_CLK,
		5,
	},
	{
		CLK_AM62X_HSDIV4_16FFT_MAIN_2_HSDIVOUT1_CLK,
		10,
	},
};
static const struct clk_data_mux_reg clk_data_CLKOUT0_CTRL_out0 = {
	.data_mux		= {
		.parents	= clk_CLKOUT0_CTRL_parents,
		.n		= ARRAY_SIZE(clk_CLKOUT0_CTRL_parents),
	},
	.reg			= 0x00100000 + 32784,
	.bit			= 0,
};
static const struct clk_parent clk_CLK_32K_RC_SEL_parents[] = {
	{
		CLK_AM62X_GLUELOGIC_RCOSC_CLK_1P0V_97P65K,
		3,
	},
	{
		CLK_AM62X_HSDIV0_16FFT_MCU_32KHZ_GEN_0_HSDIVOUT0_CLK,
		8,
	},
	{
		CLK_AM62X_CLK_32K_RC_SEL_DIV_CLKOUT,
		1,
	},
	{
		CLK_AM62X_GLUELOGIC_LFOSC0_CLKOUT,
		1,
	},
};
static const struct clk_data_mux_reg clk_data_CLK_32K_RC_SEL_out0 = {
	.data_mux		= {
		.parents	= clk_CLK_32K_RC_SEL_parents,
		.n		= ARRAY_SIZE(clk_CLK_32K_RC_SEL_parents),
	},
	.reg			= 0x04500000 + 32856,
	.bit			= 0,
};
static const struct clk_data_div clk_data_CLK_32K_RC_SEL_div_in2 = {
	.n	= 384,
};
static const struct clk_data_from_dev clk_data_K3_DPHY_RX_main_0_io_rx_cl_l_m = {
	.dev		= AM62X_DEV_DPHY_RX0,
	.clk_idx	= AM62X_DEV_DPHY_RX0_IO_RX_CL_L_M,
};
static const struct clk_data_from_dev clk_data_K3_DPHY_RX_main_0_io_rx_cl_l_p = {
	.dev		= AM62X_DEV_DPHY_RX0,
	.clk_idx	= AM62X_DEV_DPHY_RX0_IO_RX_CL_L_P,
};
static const struct clk_data_from_dev clk_data_K3_DPHY_RX_main_0_ppi_rx_byte_clk = {
	.dev		= AM62X_DEV_DPHY_RX0,
	.clk_idx	= AM62X_DEV_DPHY_RX0_PPI_RX_BYTE_CLK,
};
static const struct clk_parent clk_MAIN_CP_GEMAC_CPTS_CLK_SEL_parents[] = {
	{
		CLK_AM62X_POSTDIV4_16FF_MAIN_2_HSDIVOUT5_CLK,
		1,
	},
	{
		CLK_AM62X_POSTDIV4_16FF_MAIN_0_HSDIVOUT6_CLK,
		1,
	},
	{
		CLK_AM62X_BOARD_0_CP_GEMAC_CPTS0_RFT_CLK_OUT,
		1,
	},
	{
		0,
		1,
	},
	{
		CLK_AM62X_BOARD_0_MCU_EXT_REFCLK0_OUT,
		1,
	},
	{
		CLK_AM62X_BOARD_0_EXT_REFCLK1_OUT,
		1,
	},
	{
		CLK_AM62X_SAM62_PLL_CTRL_WRAP_MCU_0_CHIP_DIV1_CLK_CLK,
		1,
	},
	{
		CLK_AM62X_SAM62_PLL_CTRL_WRAP_MAIN_0_CHIP_DIV1_CLK_CLK,
		1,
	},
};
static const struct clk_data_mux_reg clk_data_MAIN_CP_GEMAC_CPTS_CLK_SEL_out0 = {
	.data_mux		= {
		.parents	= clk_MAIN_CP_GEMAC_CPTS_CLK_SEL_parents,
		.n		= ARRAY_SIZE(clk_MAIN_CP_GEMAC_CPTS_CLK_SEL_parents),
	},
	.reg			= 0x00100000 + 33088,
	.bit			= 0,
};
static const struct clk_parent clk_MAIN_DSS_DPI1_parents[] = {
	{
		CLK_AM62X_HSDIV0_16FFT_MAIN_17_HSDIVOUT0_CLK,
		1,
	},
	{
		CLK_AM62X_BOARD_0_VOUT0_EXTPCLKIN_OUT,
		1,
	},
};
static const struct clk_data_mux_reg clk_data_MAIN_DSS_DPI1_out0 = {
	.data_mux		= {
		.parents	= clk_MAIN_DSS_DPI1_parents,
		.n		= ARRAY_SIZE(clk_MAIN_DSS_DPI1_parents),
	},
	.reg			= 0x00100000 + 33568,
	.bit			= 0,
};
static const struct clk_parent clk_MAIN_EMMCSD0_IO_CLKLB_SEL_parents[] = {
	{
		CLK_AM62X_BOARD_0_MMC0_CLKLB_OUT,
		1,
	},
	{
		CLK_AM62X_BOARD_0_MMC0_CLK_OUT,
		1,
	},
};
static const struct clk_data_mux_reg clk_data_MAIN_EMMCSD0_IO_CLKLB_SEL_out0 = {
	.data_mux		= {
		.parents	= clk_MAIN_EMMCSD0_IO_CLKLB_SEL_parents,
		.n		= ARRAY_SIZE(clk_MAIN_EMMCSD0_IO_CLKLB_SEL_parents),
	},
	.reg			= 0x00100000 + 33120,
	.bit			= 16,
};
static const struct clk_parent clk_MAIN_EMMCSD0_REFCLK_SEL_parents[] = {
	{
		CLK_AM62X_POSTDIV4_16FF_MAIN_0_HSDIVOUT5_CLK,
		1,
	},
	{
		CLK_AM62X_HSDIV4_16FFT_MAIN_2_HSDIVOUT2_CLK,
		1,
	},
};
static const struct clk_data_mux_reg clk_data_MAIN_EMMCSD0_REFCLK_SEL_out0 = {
	.data_mux		= {
		.parents	= clk_MAIN_EMMCSD0_REFCLK_SEL_parents,
		.n		= ARRAY_SIZE(clk_MAIN_EMMCSD0_REFCLK_SEL_parents),
	},
	.reg			= 0x00100000 + 33120,
	.bit			= 0,
};
static const struct clk_parent clk_MAIN_EMMCSD1_IO_CLKLB_SEL_parents[] = {
	{
		CLK_AM62X_BOARD_0_MMC1_CLKLB_OUT,
		1,
	},
	{
		CLK_AM62X_BOARD_0_MMC1_CLK_OUT,
		1,
	},
};
static const struct clk_data_mux_reg clk_data_MAIN_EMMCSD1_IO_CLKLB_SEL_out0 = {
	.data_mux		= {
		.parents	= clk_MAIN_EMMCSD1_IO_CLKLB_SEL_parents,
		.n		= ARRAY_SIZE(clk_MAIN_EMMCSD1_IO_CLKLB_SEL_parents),
	},
	.reg			= 0x00100000 + 33128,
	.bit			= 16,
};
static const struct clk_data_mux_reg clk_data_MAIN_EMMCSD1_REFCLK_SEL_out0 = {
	.data_mux		= {
		.parents	= clk_MAIN_EMMCSD0_REFCLK_SEL_parents,
		.n		= ARRAY_SIZE(clk_MAIN_EMMCSD0_REFCLK_SEL_parents),
	},
	.reg			= 0x00100000 + 33128,
	.bit			= 0,
};
static const struct clk_parent clk_MAIN_EMMCSD2_IO_CLKLB_SEL_parents[] = {
	{
		CLK_AM62X_BOARD_0_MMC2_CLKLB_OUT,
		1,
	},
	{
		CLK_AM62X_BOARD_0_MMC2_CLK_OUT,
		1,
	},
};
static const struct clk_data_mux_reg clk_data_MAIN_EMMCSD2_IO_CLKLB_SEL_out0 = {
	.data_mux		= {
		.parents	= clk_MAIN_EMMCSD2_IO_CLKLB_SEL_parents,
		.n		= ARRAY_SIZE(clk_MAIN_EMMCSD2_IO_CLKLB_SEL_parents),
	},
	.reg			= 0x00100000 + 33132,
	.bit			= 16,
};
static const struct clk_data_mux_reg clk_data_MAIN_EMMCSD2_REFCLK_SEL_out0 = {
	.data_mux		= {
		.parents	= clk_MAIN_EMMCSD0_REFCLK_SEL_parents,
		.n		= ARRAY_SIZE(clk_MAIN_EMMCSD0_REFCLK_SEL_parents),
	},
	.reg			= 0x00100000 + 33132,
	.bit			= 0,
};
static const struct clk_parent clk_MAIN_GPMC_FCLK_SEL_parents[] = {
	{
		CLK_AM62X_HSDIV4_16FFT_MAIN_0_HSDIVOUT3_CLK,
		1,
	},
	{
		CLK_AM62X_POSTDIV4_16FF_MAIN_2_HSDIVOUT7_CLK,
		1,
	},
};
static const struct clk_data_mux_reg clk_data_MAIN_GPMC_FCLK_SEL_out0 = {
	.data_mux		= {
		.parents	= clk_MAIN_GPMC_FCLK_SEL_parents,
		.n		= ARRAY_SIZE(clk_MAIN_GPMC_FCLK_SEL_parents),
	},
	.reg			= 0x00100000 + 33152,
	.bit			= 0,
};
static const struct clk_data_mux_reg clk_data_MAIN_GTCCLK_SEL_out0 = {
	.data_mux		= {
		.parents	= clk_MAIN_CP_GEMAC_CPTS_CLK_SEL_parents,
		.n		= ARRAY_SIZE(clk_MAIN_CP_GEMAC_CPTS_CLK_SEL_parents),
	},
	.reg			= 0x43000000 + 32816,
	.bit			= 0,
};
static const struct clk_parent clk_MAIN_ICSSM_CORE_CLK_SEL_parents[] = {
	{
		CLK_AM62X_HSDIV4_16FFT_MAIN_2_HSDIVOUT0_CLK,
		1,
	},
	{
		CLK_AM62X_POSTDIV4_16FF_MAIN_0_HSDIVOUT9_CLK,
		1,
	},
};
static const struct clk_data_mux_reg clk_data_MAIN_ICSSM_CORE_CLK_SEL_out0 = {
	.data_mux		= {
		.parents	= clk_MAIN_ICSSM_CORE_CLK_SEL_parents,
		.n		= ARRAY_SIZE(clk_MAIN_ICSSM_CORE_CLK_SEL_parents),
	},
	.reg			= 0x00100000 + 32832,
	.bit			= 0,
};
static const struct clk_data_mux_reg clk_data_MAIN_ICSSM_IEPCLK_SEL_out0 = {
	.data_mux		= {
		.parents	= clk_MAIN_CP_GEMAC_CPTS_CLK_SEL_parents,
		.n		= ARRAY_SIZE(clk_MAIN_CP_GEMAC_CPTS_CLK_SEL_parents),
	},
	.reg			= 0x00100000 + 32832,
	.bit			= 16,
};
static const struct clk_parent clk_MAIN_MCANn_CLK_SEL_parents[] = {
	{
		CLK_AM62X_HSDIV4_16FFT_MAIN_0_HSDIVOUT4_CLK,
		1,
	},
	{
		CLK_AM62X_BOARD_0_MCU_EXT_REFCLK0_OUT,
		1,
	},
	{
		CLK_AM62X_BOARD_0_EXT_REFCLK1_OUT,
		1,
	},
	{
		CLK_AM62X_GLUELOGIC_HFOSC0_CLKOUT,
		1,
	},
};
static const struct clk_data_mux_reg clk_data_MAIN_MCANn_CLK_SEL_out0 = {
	.data_mux		= {
		.parents	= clk_MAIN_MCANn_CLK_SEL_parents,
		.n		= ARRAY_SIZE(clk_MAIN_MCANn_CLK_SEL_parents),
	},
	.reg			= 0x00100000 + 33920,
	.bit			= 0,
};
static const struct clk_parent clk_MAIN_OBSCLK0_MUX_SEL_parents[] = {
	{
		CLK_AM62X_HSDIV4_16FFT_MAIN_0_HSDIVOUT0_CLK,
		1,
	},
	{
		CLK_AM62X_HSDIV4_16FFT_MAIN_1_HSDIVOUT0_CLK,
		1,
	},
	{
		CLK_AM62X_HSDIV4_16FFT_MAIN_2_HSDIVOUT0_CLK,
		1,
	},
	{
		CLK_AM62X_SAM62_A53_512KB_WRAP_MAIN_0_ARM_COREPACK_0_A53_DIVH_CLK4_OBSCLK_OUT_CLK,
		1,
	},
	{
		CLK_AM62X_HSDIV0_16FFT_MAIN_12_HSDIVOUT0_CLK,
		1,
	},
	{
		CLK_AM62X_GLUELOGIC_RCOSC_CLKOUT,
		1,
	},
	{
		CLK_AM62X_HSDIV0_16FFT_MCU_32KHZ_GEN_0_HSDIVOUT0_CLK,
		8,
	},
	{
		CLK_AM62X_HSDIV4_16FFT_MAIN_0_HSDIVOUT0_CLK,
		1,
	},
	{
		CLK_AM62X_GLUELOGIC_HFOSC0_CLKOUT,
		1,
	},
	{
		CLK_AM62X_MAIN_OBSCLK0_MUX_SEL_DIV_CLKOUT,
		1,
	},
	{
		CLK_AM62X_CPSW_3GUSS_MAIN_0_CPTS_GENF0,
		1,
	},
	{
		CLK_AM62X_CPSW_3GUSS_MAIN_0_CPTS_GENF1,
		1,
	},
	{
		CLK_AM62X_HSDIV4_16FFT_MCU_0_HSDIVOUT0_CLK,
		1,
	},
	{
		CLK_AM62X_HSDIV1_16FFT_MAIN_15_HSDIVOUT0_CLK,
		1,
	},
	{
		CLK_AM62X_HSDIV0_16FFT_MAIN_16_HSDIVOUT0_CLK,
		1,
	},
	{
		CLK_AM62X_HSDIV0_16FFT_MAIN_17_HSDIVOUT0_CLK,
		1,
	},
	{
		CLK_AM62X_SAM62_PLL_CTRL_WRAP_MAIN_0_CHIP_DIV1_CLK_CLK,
		1,
	},
	{
		CLK_AM62X_CLK_32K_RC_SEL_OUT0,
		1,
	},
	{
		0,
		1,
	},
	{
		0,
		1,
	},
	{
		0,
		1,
	},
	{
		0,
		1,
	},
	{
		0,
		1,
	},
	{
		0,
		1,
	},
	{
		0,
		1,
	},
	{
		0,
		1,
	},
	{
		0,
		1,
	},
	{
		0,
		1,
	},
	{
		0,
		1,
	},
	{
		0,
		1,
	},
	{
		0,
		1,
	},
	{
		0,
		0,
	},
};
static const struct clk_data_mux_reg clk_data_MAIN_OBSCLK0_MUX_SEL_out0 = {
	.data_mux		= {
		.parents	= clk_MAIN_OBSCLK0_MUX_SEL_parents,
		.n		= ARRAY_SIZE(clk_MAIN_OBSCLK0_MUX_SEL_parents),
	},
	.reg			= 0x00100000 + 32768,
	.bit			= 0,
};
static const struct clk_data_div clk_data_MAIN_OBSCLK0_MUX_SEL_div_in9 = {
	.n	= 384,
};
static const struct clk_parent clk_MAIN_OSPI_LOOPBACK_CLK_SEL_parents[] = {
	{
		CLK_AM62X_BOARD_0_OSPI0_DQS_OUT,
		1,
	},
	{
		CLK_AM62X_BOARD_0_OSPI0_LBCLKO_OUT,
		1,
	},
};
static const struct clk_data_mux_reg clk_data_MAIN_OSPI_LOOPBACK_CLK_SEL_out0 = {
	.data_mux		= {
		.parents	= clk_MAIN_OSPI_LOOPBACK_CLK_SEL_parents,
		.n		= ARRAY_SIZE(clk_MAIN_OSPI_LOOPBACK_CLK_SEL_parents),
	},
	.reg			= 0x00100000 + 34048,
	.bit			= 4,
};
static const struct clk_parent clk_MAIN_OSPI_REF_CLK_SEL_parents[] = {
	{
		CLK_AM62X_HSDIV4_16FFT_MAIN_0_HSDIVOUT1_CLK,
		1,
	},
	{
		CLK_AM62X_POSTDIV1_16FFT_MAIN_1_HSDIVOUT5_CLK,
		1,
	},
};
static const struct clk_data_mux_reg clk_data_MAIN_OSPI_REF_CLK_SEL_out0 = {
	.data_mux		= {
		.parents	= clk_MAIN_OSPI_REF_CLK_SEL_parents,
		.n		= ARRAY_SIZE(clk_MAIN_OSPI_REF_CLK_SEL_parents),
	},
	.reg			= 0x00100000 + 34048,
	.bit			= 0,
};
static const struct clk_parent clk_MAIN_TIMERCLKn_SEL_out0_parents[] = {
	{
		CLK_AM62X_GLUELOGIC_HFOSC0_CLKOUT,
		1,
	},
	{
		CLK_AM62X_CLK_32K_RC_SEL_OUT0,
		1,
	},
	{
		CLK_AM62X_POSTDIV4_16FF_MAIN_0_HSDIVOUT7_CLK,
		1,
	},
	{
		CLK_AM62X_GLUELOGIC_RCOSC_CLKOUT,
		1,
	},
	{
		CLK_AM62X_BOARD_0_MCU_EXT_REFCLK0_OUT,
		1,
	},
	{
		CLK_AM62X_BOARD_0_EXT_REFCLK1_OUT,
		1,
	},
	{
		0,
		1,
	},
	{
		CLK_AM62X_BOARD_0_CP_GEMAC_CPTS0_RFT_CLK_OUT,
		1,
	},
	{
		CLK_AM62X_HSDIV4_16FFT_MAIN_1_HSDIVOUT3_CLK,
		1,
	},
	{
		CLK_AM62X_POSTDIV4_16FF_MAIN_2_HSDIVOUT6_CLK,
		1,
	},
	{
		CLK_AM62X_CPSW_3GUSS_MAIN_0_CPTS_GENF0,
		1,
	},
	{
		CLK_AM62X_CPSW_3GUSS_MAIN_0_CPTS_GENF1,
		1,
	},
	{
		0,
		1,
	},
	{
		0,
		1,
	},
	{
		0,
		1,
	},
	{
		0,
		1,
	},
};
static const struct clk_data_mux_reg clk_data_MAIN_TIMERCLKn_SEL_out0 = {
	.data_mux		= {
		.parents	= clk_MAIN_TIMERCLKn_SEL_out0_parents,
		.n		= ARRAY_SIZE(clk_MAIN_TIMERCLKn_SEL_out0_parents),
	},
	.reg			= 0x00100000 + 33200,
	.bit			= 0,
};
static const struct clk_data_mux_reg clk_data_MAIN_TIMERCLKn_SEL_out1 = {
	.data_mux		= {
		.parents	= clk_MAIN_TIMERCLKn_SEL_out0_parents,
		.n		= ARRAY_SIZE(clk_MAIN_TIMERCLKn_SEL_out0_parents),
	},
	.reg			= 0x00100000 + 33204,
	.bit			= 0,
};
static const struct clk_data_mux_reg clk_data_MAIN_TIMERCLKn_SEL_out2 = {
	.data_mux		= {
		.parents	= clk_MAIN_TIMERCLKn_SEL_out0_parents,
		.n		= ARRAY_SIZE(clk_MAIN_TIMERCLKn_SEL_out0_parents),
	},
	.reg			= 0x00100000 + 33208,
	.bit			= 0,
};
static const struct clk_data_mux_reg clk_data_MAIN_TIMERCLKn_SEL_out3 = {
	.data_mux		= {
		.parents	= clk_MAIN_TIMERCLKn_SEL_out0_parents,
		.n		= ARRAY_SIZE(clk_MAIN_TIMERCLKn_SEL_out0_parents),
	},
	.reg			= 0x00100000 + 33212,
	.bit			= 0,
};
static const struct clk_data_mux_reg clk_data_MAIN_TIMERCLKn_SEL_out4 = {
	.data_mux		= {
		.parents	= clk_MAIN_TIMERCLKn_SEL_out0_parents,
		.n		= ARRAY_SIZE(clk_MAIN_TIMERCLKn_SEL_out0_parents),
	},
	.reg			= 0x00100000 + 33216,
	.bit			= 0,
};
static const struct clk_data_mux_reg clk_data_MAIN_TIMERCLKn_SEL_out5 = {
	.data_mux		= {
		.parents	= clk_MAIN_TIMERCLKn_SEL_out0_parents,
		.n		= ARRAY_SIZE(clk_MAIN_TIMERCLKn_SEL_out0_parents),
	},
	.reg			= 0x00100000 + 33220,
	.bit			= 0,
};
static const struct clk_data_mux_reg clk_data_MAIN_TIMERCLKn_SEL_out6 = {
	.data_mux		= {
		.parents	= clk_MAIN_TIMERCLKn_SEL_out0_parents,
		.n		= ARRAY_SIZE(clk_MAIN_TIMERCLKn_SEL_out0_parents),
	},
	.reg			= 0x00100000 + 33224,
	.bit			= 0,
};
static const struct clk_data_mux_reg clk_data_MAIN_TIMERCLKn_SEL_out7 = {
	.data_mux		= {
		.parents	= clk_MAIN_TIMERCLKn_SEL_out0_parents,
		.n		= ARRAY_SIZE(clk_MAIN_TIMERCLKn_SEL_out0_parents),
	},
	.reg			= 0x00100000 + 33228,
	.bit			= 0,
};
static const struct clk_parent clk_MAIN_USB0_REFCLK_SEL_parents[] = {
	{
		CLK_AM62X_GLUELOGIC_HFOSC0_CLKOUT,
		1,
	},
	{
		CLK_AM62X_POSTDIV4_16FF_MAIN_0_HSDIVOUT8_CLK,
		1,
	},
};
static const struct clk_data_mux_reg clk_data_MAIN_USB0_REFCLK_SEL_out0 = {
	.data_mux		= {
		.parents	= clk_MAIN_USB0_REFCLK_SEL_parents,
		.n		= ARRAY_SIZE(clk_MAIN_USB0_REFCLK_SEL_parents),
	},
	.reg			= 0x43000000 + 33168,
	.bit			= 0,
};
static const struct clk_data_mux_reg clk_data_MAIN_USB1_REFCLK_SEL_out0 = {
	.data_mux		= {
		.parents	= clk_MAIN_USB0_REFCLK_SEL_parents,
		.n		= ARRAY_SIZE(clk_MAIN_USB0_REFCLK_SEL_parents),
	},
	.reg			= 0x43000000 + 33172,
	.bit			= 0,
};
static const struct clk_parent clk_MAIN_WWDTCLKn_SEL_out0_parents[] = {
	{
		CLK_AM62X_GLUELOGIC_HFOSC0_CLKOUT,
		1,
	},
	{
		CLK_AM62X_CLK_32K_RC_SEL_OUT0,
		1,
	},
	{
		CLK_AM62X_GLUELOGIC_RCOSC_CLKOUT,
		1,
	},
	{
		CLK_AM62X_MAIN_WWDTCLKN_SEL_OUT0_DIV_CLKOUT,
		1,
	},
};
static const struct clk_data_mux_reg clk_data_MAIN_WWDTCLKn_SEL_out0 = {
	.data_mux		= {
		.parents	= clk_MAIN_WWDTCLKn_SEL_out0_parents,
		.n		= ARRAY_SIZE(clk_MAIN_WWDTCLKn_SEL_out0_parents),
	},
	.reg			= 0x00100000 + 33664,
	.bit			= 0,
};
static const struct clk_data_div clk_data_MAIN_WWDTCLKn_SEL_out0_div_in3 = {
	.n	= 384,
};
static const struct clk_parent clk_MAIN_WWDTCLKn_SEL_out1_parents[] = {
	{
		CLK_AM62X_GLUELOGIC_HFOSC0_CLKOUT,
		1,
	},
	{
		CLK_AM62X_CLK_32K_RC_SEL_OUT0,
		1,
	},
	{
		CLK_AM62X_GLUELOGIC_RCOSC_CLKOUT,
		1,
	},
	{
		CLK_AM62X_MAIN_WWDTCLKN_SEL_OUT1_DIV_CLKOUT,
		1,
	},
};
static const struct clk_data_mux_reg clk_data_MAIN_WWDTCLKn_SEL_out1 = {
	.data_mux		= {
		.parents	= clk_MAIN_WWDTCLKn_SEL_out1_parents,
		.n		= ARRAY_SIZE(clk_MAIN_WWDTCLKn_SEL_out1_parents),
	},
	.reg			= 0x00100000 + 33668,
	.bit			= 0,
};
static const struct clk_data_div clk_data_MAIN_WWDTCLKn_SEL_out1_div_in3 = {
	.n	= 384,
};
static const struct clk_parent clk_MAIN_WWDTCLKn_SEL_out2_parents[] = {
	{
		CLK_AM62X_GLUELOGIC_HFOSC0_CLKOUT,
		1,
	},
	{
		CLK_AM62X_CLK_32K_RC_SEL_OUT0,
		1,
	},
	{
		CLK_AM62X_GLUELOGIC_RCOSC_CLKOUT,
		1,
	},
	{
		CLK_AM62X_MAIN_WWDTCLKN_SEL_OUT2_DIV_CLKOUT,
		1,
	},
};
static const struct clk_data_mux_reg clk_data_MAIN_WWDTCLKn_SEL_out2 = {
	.data_mux		= {
		.parents	= clk_MAIN_WWDTCLKn_SEL_out2_parents,
		.n		= ARRAY_SIZE(clk_MAIN_WWDTCLKn_SEL_out2_parents),
	},
	.reg			= 0x00100000 + 33672,
	.bit			= 0,
};
static const struct clk_data_div clk_data_MAIN_WWDTCLKn_SEL_out2_div_in3 = {
	.n	= 384,
};
static const struct clk_parent clk_MAIN_WWDTCLKn_SEL_out3_parents[] = {
	{
		CLK_AM62X_GLUELOGIC_HFOSC0_CLKOUT,
		1,
	},
	{
		CLK_AM62X_CLK_32K_RC_SEL_OUT0,
		1,
	},
	{
		CLK_AM62X_GLUELOGIC_RCOSC_CLKOUT,
		1,
	},
	{
		CLK_AM62X_MAIN_WWDTCLKN_SEL_OUT3_DIV_CLKOUT,
		1,
	},
};
static const struct clk_data_mux_reg clk_data_MAIN_WWDTCLKn_SEL_out3 = {
	.data_mux		= {
		.parents	= clk_MAIN_WWDTCLKn_SEL_out3_parents,
		.n		= ARRAY_SIZE(clk_MAIN_WWDTCLKn_SEL_out3_parents),
	},
	.reg			= 0x00100000 + 33676,
	.bit			= 0,
};
static const struct clk_data_div clk_data_MAIN_WWDTCLKn_SEL_out3_div_in3 = {
	.n	= 384,
};
static const struct clk_parent clk_MAIN_WWDTCLKn_SEL_out4_parents[] = {
	{
		CLK_AM62X_GLUELOGIC_HFOSC0_CLKOUT,
		1,
	},
	{
		CLK_AM62X_CLK_32K_RC_SEL_OUT0,
		1,
	},
	{
		CLK_AM62X_GLUELOGIC_RCOSC_CLKOUT,
		1,
	},
	{
		CLK_AM62X_MAIN_WWDTCLKN_SEL_OUT4_DIV_CLKOUT,
		1,
	},
};
static const struct clk_data_mux_reg clk_data_MAIN_WWDTCLKn_SEL_out4 = {
	.data_mux		= {
		.parents	= clk_MAIN_WWDTCLKn_SEL_out4_parents,
		.n		= ARRAY_SIZE(clk_MAIN_WWDTCLKn_SEL_out4_parents),
	},
	.reg			= 0x00100000 + 33724,
	.bit			= 0,
};
static const struct clk_data_div clk_data_MAIN_WWDTCLKn_SEL_out4_div_in3 = {
	.n	= 384,
};
static const struct clk_parent clk_MAIN_spi0_mstr_lp_clksel_parents[] = {
	{
		CLK_AM62X_BOARD_0_SPI0_CLK_OUT,
		1,
	},
	{
		CLK_AM62X_SPI_MAIN_0_IO_CLKSPIO_CLK,
		1,
	},
};
static const struct clk_data_mux_reg clk_data_MAIN_spi0_mstr_lp_clksel_out0 = {
	.data_mux		= {
		.parents	= clk_MAIN_spi0_mstr_lp_clksel_parents,
		.n		= ARRAY_SIZE(clk_MAIN_spi0_mstr_lp_clksel_parents),
	},
	.reg			= 0x00100000 + 33280,
	.bit			= 16,
};
static const struct clk_parent clk_MAIN_spi1_mstr_lp_clksel_parents[] = {
	{
		CLK_AM62X_BOARD_0_SPI1_CLK_OUT,
		1,
	},
	{
		CLK_AM62X_SPI_MAIN_1_IO_CLKSPIO_CLK,
		1,
	},
};
static const struct clk_data_mux_reg clk_data_MAIN_spi1_mstr_lp_clksel_out0 = {
	.data_mux		= {
		.parents	= clk_MAIN_spi1_mstr_lp_clksel_parents,
		.n		= ARRAY_SIZE(clk_MAIN_spi1_mstr_lp_clksel_parents),
	},
	.reg			= 0x00100000 + 33284,
	.bit			= 16,
};
static const struct clk_parent clk_MAIN_spi2_mstr_lp_clksel_parents[] = {
	{
		CLK_AM62X_BOARD_0_SPI2_CLK_OUT,
		1,
	},
	{
		CLK_AM62X_SPI_MAIN_2_IO_CLKSPIO_CLK,
		1,
	},
};
static const struct clk_data_mux_reg clk_data_MAIN_spi2_mstr_lp_clksel_out0 = {
	.data_mux		= {
		.parents	= clk_MAIN_spi2_mstr_lp_clksel_parents,
		.n		= ARRAY_SIZE(clk_MAIN_spi2_mstr_lp_clksel_parents),
	},
	.reg			= 0x00100000 + 33288,
	.bit			= 16,
};
static const struct clk_parent clk_MAIN_usart0_fclk_sel_parents[] = {
	{
		CLK_AM62X_USART_PROGRAMMABLE_CLOCK_DIVIDER_OUT0,
		1,
	},
	{
		CLK_AM62X_HSDIV4_16FFT_MAIN_1_HSDIVOUT1_CLK,
		1,
	},
};
static const struct clk_data_mux_reg clk_data_MAIN_usart0_fclk_sel_out0 = {
	.data_mux		= {
		.parents	= clk_MAIN_usart0_fclk_sel_parents,
		.n		= ARRAY_SIZE(clk_MAIN_usart0_fclk_sel_parents),
	},
	.reg			= 0x00100000 + 33408,
	.bit			= 0,
};
static const struct clk_parent clk_MAIN_usart1_fclk_sel_parents[] = {
	{
		CLK_AM62X_USART_PROGRAMMABLE_CLOCK_DIVIDER_OUT1,
		1,
	},
	{
		CLK_AM62X_HSDIV4_16FFT_MAIN_1_HSDIVOUT1_CLK,
		1,
	},
};
static const struct clk_data_mux_reg clk_data_MAIN_usart1_fclk_sel_out0 = {
	.data_mux		= {
		.parents	= clk_MAIN_usart1_fclk_sel_parents,
		.n		= ARRAY_SIZE(clk_MAIN_usart1_fclk_sel_parents),
	},
	.reg			= 0x00100000 + 33412,
	.bit			= 0,
};
static const struct clk_parent clk_MAIN_usart2_fclk_sel_parents[] = {
	{
		CLK_AM62X_USART_PROGRAMMABLE_CLOCK_DIVIDER_OUT2,
		1,
	},
	{
		CLK_AM62X_HSDIV4_16FFT_MAIN_1_HSDIVOUT1_CLK,
		1,
	},
};
static const struct clk_data_mux_reg clk_data_MAIN_usart2_fclk_sel_out0 = {
	.data_mux		= {
		.parents	= clk_MAIN_usart2_fclk_sel_parents,
		.n		= ARRAY_SIZE(clk_MAIN_usart2_fclk_sel_parents),
	},
	.reg			= 0x00100000 + 33416,
	.bit			= 0,
};
static const struct clk_parent clk_MAIN_usart3_fclk_sel_parents[] = {
	{
		CLK_AM62X_USART_PROGRAMMABLE_CLOCK_DIVIDER_OUT3,
		1,
	},
	{
		CLK_AM62X_HSDIV4_16FFT_MAIN_1_HSDIVOUT1_CLK,
		1,
	},
};
static const struct clk_data_mux_reg clk_data_MAIN_usart3_fclk_sel_out0 = {
	.data_mux		= {
		.parents	= clk_MAIN_usart3_fclk_sel_parents,
		.n		= ARRAY_SIZE(clk_MAIN_usart3_fclk_sel_parents),
	},
	.reg			= 0x00100000 + 33420,
	.bit			= 0,
};
static const struct clk_parent clk_MAIN_usart4_fclk_sel_parents[] = {
	{
		CLK_AM62X_USART_PROGRAMMABLE_CLOCK_DIVIDER_OUT4,
		1,
	},
	{
		CLK_AM62X_HSDIV4_16FFT_MAIN_1_HSDIVOUT1_CLK,
		1,
	},
};
static const struct clk_data_mux_reg clk_data_MAIN_usart4_fclk_sel_out0 = {
	.data_mux		= {
		.parents	= clk_MAIN_usart4_fclk_sel_parents,
		.n		= ARRAY_SIZE(clk_MAIN_usart4_fclk_sel_parents),
	},
	.reg			= 0x00100000 + 33424,
	.bit			= 0,
};
static const struct clk_parent clk_MAIN_usart5_fclk_sel_parents[] = {
	{
		CLK_AM62X_USART_PROGRAMMABLE_CLOCK_DIVIDER_OUT5,
		1,
	},
	{
		CLK_AM62X_HSDIV4_16FFT_MAIN_1_HSDIVOUT1_CLK,
		1,
	},
};
static const struct clk_data_mux_reg clk_data_MAIN_usart5_fclk_sel_out0 = {
	.data_mux		= {
		.parents	= clk_MAIN_usart5_fclk_sel_parents,
		.n		= ARRAY_SIZE(clk_MAIN_usart5_fclk_sel_parents),
	},
	.reg			= 0x00100000 + 33428,
	.bit			= 0,
};
static const struct clk_parent clk_MAIN_usart6_fclk_sel_parents[] = {
	{
		CLK_AM62X_USART_PROGRAMMABLE_CLOCK_DIVIDER_OUT6,
		1,
	},
	{
		CLK_AM62X_HSDIV4_16FFT_MAIN_1_HSDIVOUT1_CLK,
		1,
	},
};
static const struct clk_data_mux_reg clk_data_MAIN_usart6_fclk_sel_out0 = {
	.data_mux		= {
		.parents	= clk_MAIN_usart6_fclk_sel_parents,
		.n		= ARRAY_SIZE(clk_MAIN_usart6_fclk_sel_parents),
	},
	.reg			= 0x00100000 + 33432,
	.bit			= 0,
};
static const struct clk_parent clk_MCASPn_AHCLKSEL_AHCLKR_out0_parents[] = {
	{
		CLK_AM62X_BOARD_0_EXT_REFCLK1_OUT,
		1,
	},
	{
		CLK_AM62X_GLUELOGIC_HFOSC0_CLKOUT,
		1,
	},
	{
		CLK_AM62X_BOARD_0_AUDIO_EXT_REFCLK0_OUT,
		1,
	},
	{
		CLK_AM62X_BOARD_0_AUDIO_EXT_REFCLK1_OUT,
		1,
	},
};
static const struct clk_data_mux_reg clk_data_MCASPn_AHCLKSEL_AHCLKR_out0 = {
	.data_mux		= {
		.parents	= clk_MCASPn_AHCLKSEL_AHCLKR_out0_parents,
		.n		= ARRAY_SIZE(clk_MCASPn_AHCLKSEL_AHCLKR_out0_parents),
	},
	.reg			= 0x00100000 + 33616,
	.bit			= 0,
};
static const struct clk_data_mux_reg clk_data_MCASPn_AHCLKSEL_AHCLKR_out1 = {
	.data_mux		= {
		.parents	= clk_MCASPn_AHCLKSEL_AHCLKR_out0_parents,
		.n		= ARRAY_SIZE(clk_MCASPn_AHCLKSEL_AHCLKR_out0_parents),
	},
	.reg			= 0x00100000 + 33620,
	.bit			= 0,
};
static const struct clk_data_mux_reg clk_data_MCASPn_AHCLKSEL_AHCLKR_out2 = {
	.data_mux		= {
		.parents	= clk_MCASPn_AHCLKSEL_AHCLKR_out0_parents,
		.n		= ARRAY_SIZE(clk_MCASPn_AHCLKSEL_AHCLKR_out0_parents),
	},
	.reg			= 0x00100000 + 33624,
	.bit			= 0,
};
static const struct clk_data_mux_reg clk_data_MCASPn_AHCLKSEL_AHCLKX_out0 = {
	.data_mux		= {
		.parents	= clk_MCASPn_AHCLKSEL_AHCLKR_out0_parents,
		.n		= ARRAY_SIZE(clk_MCASPn_AHCLKSEL_AHCLKR_out0_parents),
	},
	.reg			= 0x00100000 + 33616,
	.bit			= 8,
};
static const struct clk_data_mux_reg clk_data_MCASPn_AHCLKSEL_AHCLKX_out1 = {
	.data_mux		= {
		.parents	= clk_MCASPn_AHCLKSEL_AHCLKR_out0_parents,
		.n		= ARRAY_SIZE(clk_MCASPn_AHCLKSEL_AHCLKR_out0_parents),
	},
	.reg			= 0x00100000 + 33620,
	.bit			= 8,
};
static const struct clk_data_mux_reg clk_data_MCASPn_AHCLKSEL_AHCLKX_out2 = {
	.data_mux		= {
		.parents	= clk_MCASPn_AHCLKSEL_AHCLKR_out0_parents,
		.n		= ARRAY_SIZE(clk_MCASPn_AHCLKSEL_AHCLKR_out0_parents),
	},
	.reg			= 0x00100000 + 33624,
	.bit			= 8,
};
static const struct clk_parent clk_MCASPn_CLKSEL_AUXCLK_out0_parents[] = {
	{
		CLK_AM62X_POSTDIV4_16FF_MAIN_2_HSDIVOUT8_CLK,
		1,
	},
	{
		CLK_AM62X_POSTDIV1_16FFT_MAIN_1_HSDIVOUT6_CLK,
		1,
	},
};
static const struct clk_data_mux_reg clk_data_MCASPn_CLKSEL_AUXCLK_out0 = {
	.data_mux		= {
		.parents	= clk_MCASPn_CLKSEL_AUXCLK_out0_parents,
		.n		= ARRAY_SIZE(clk_MCASPn_CLKSEL_AUXCLK_out0_parents),
	},
	.reg			= 0x00100000 + 33584,
	.bit			= 0,
};
static const struct clk_data_mux_reg clk_data_MCASPn_CLKSEL_AUXCLK_out1 = {
	.data_mux		= {
		.parents	= clk_MCASPn_CLKSEL_AUXCLK_out0_parents,
		.n		= ARRAY_SIZE(clk_MCASPn_CLKSEL_AUXCLK_out0_parents),
	},
	.reg			= 0x00100000 + 33588,
	.bit			= 0,
};
static const struct clk_data_mux_reg clk_data_MCASPn_CLKSEL_AUXCLK_out2 = {
	.data_mux		= {
		.parents	= clk_MCASPn_CLKSEL_AUXCLK_out0_parents,
		.n		= ARRAY_SIZE(clk_MCASPn_CLKSEL_AUXCLK_out0_parents),
	},
	.reg			= 0x00100000 + 33592,
	.bit			= 0,
};
static const struct clk_parent clk_MCU_GPIO0_CLKSEL_parents[] = {
	{
		CLK_AM62X_SAM62_PLL_CTRL_WRAP_MCU_0_CHIP_DIV1_CLK_CLK,
		4,
	},
	{
		CLK_AM62X_GLUELOGIC_LFOSC0_CLKOUT,
		1,
	},
	{
		CLK_AM62X_GLUELOGIC_RCOSC_CLK_1P0V_97P65K,
		3,
	},
	{
		CLK_AM62X_GLUELOGIC_RCOSC_CLKOUT,
		1,
	},
};
static const struct clk_data_mux_reg clk_data_MCU_GPIO0_CLKSEL_out0 = {
	.data_mux		= {
		.parents	= clk_MCU_GPIO0_CLKSEL_parents,
		.n		= ARRAY_SIZE(clk_MCU_GPIO0_CLKSEL_parents),
	},
	.reg			= 0x04500000 + 32880,
	.bit			= 0,
};
static const struct clk_parent clk_MCU_M4FSS_CLKSEL_parents[] = {
	{
		CLK_AM62X_SAM62_PLL_CTRL_WRAP_MCU_0_CHIP_DIV1_CLK_CLK,
		1,
	},
	{
		CLK_AM62X_SAM62_PLL_CTRL_WRAP_MCU_0_CHIP_DIV1_CLK_CLK,
		2,
	},
};
static const struct clk_data_mux_reg clk_data_MCU_M4FSS_CLKSEL_out0 = {
	.data_mux		= {
		.parents	= clk_MCU_M4FSS_CLKSEL_parents,
		.n		= ARRAY_SIZE(clk_MCU_M4FSS_CLKSEL_parents),
	},
	.reg			= 0x04500000 + 32832,
	.bit			= 0,
};
static const struct clk_parent clk_MCU_MCANn_CLK_SEL_out0_parents[] = {
	{
		CLK_AM62X_HSDIV4_16FFT_MCU_0_HSDIVOUT4_CLK,
		1,
	},
	{
		CLK_AM62X_BOARD_0_MCU_EXT_REFCLK0_OUT,
		1,
	},
	{
		CLK_AM62X_GLUELOGIC_HFOSC0_CLKOUT,
		1,
	},
	{
		CLK_AM62X_GLUELOGIC_HFOSC0_CLKOUT,
		1,
	},
};
static const struct clk_data_mux_reg clk_data_MCU_MCANn_CLK_SEL_out0 = {
	.data_mux		= {
		.parents	= clk_MCU_MCANn_CLK_SEL_out0_parents,
		.n		= ARRAY_SIZE(clk_MCU_MCANn_CLK_SEL_out0_parents),
	},
	.reg			= 0x04500000 + 32896,
	.bit			= 0,
};
static const struct clk_data_mux_reg clk_data_MCU_MCANn_CLK_SEL_out1 = {
	.data_mux		= {
		.parents	= clk_MCU_MCANn_CLK_SEL_out0_parents,
		.n		= ARRAY_SIZE(clk_MCU_MCANn_CLK_SEL_out0_parents),
	},
	.reg			= 0x04500000 + 32900,
	.bit			= 0,
};
static const struct clk_parent clk_MCU_OBSCLK_MUX_SEL_parents[] = {
	{
		CLK_AM62X_GLUELOGIC_RCOSC_CLKOUT,
		1,
	},
	{
		0,
		0,
	},
	{
		CLK_AM62X_HSDIV4_16FFT_MCU_0_HSDIVOUT0_CLK,
		1,
	},
	{
		CLK_AM62X_HSDIV4_16FFT_MCU_0_HSDIVOUT4_CLK,
		1,
	},
	{
		CLK_AM62X_HSDIV4_16FFT_MCU_0_HSDIVOUT0_CLK,
		1,
	},
	{
		CLK_AM62X_MCU_OBSCLK_MUX_SEL_DIV_CLKOUT,
		1,
	},
	{
		CLK_AM62X_GLUELOGIC_HFOSC0_CLKOUT,
		1,
	},
	{
		CLK_AM62X_HSDIV0_16FFT_MCU_32KHZ_GEN_0_HSDIVOUT0_CLK,
		8,
	},
	{
		CLK_AM62X_SAM62_PLL_CTRL_WRAP_MCU_0_CHIP_DIV1_CLK_CLK,
		1,
	},
	{
		CLK_AM62X_CLK_32K_RC_SEL_OUT0,
		1,
	},
	{
		0,
		0,
	},
	{
		0,
		0,
	},
	{
		0,
		0,
	},
	{
		0,
		0,
	},
	{
		0,
		0,
	},
	{
		0,
		0,
	},
};
static const struct clk_data_mux_reg clk_data_MCU_OBSCLK_MUX_SEL_out0 = {
	.data_mux		= {
		.parents	= clk_MCU_OBSCLK_MUX_SEL_parents,
		.n		= ARRAY_SIZE(clk_MCU_OBSCLK_MUX_SEL_parents),
	},
	.reg			= 0x04500000 + 32768,
	.bit			= 0,
};
static const struct clk_data_div clk_data_MCU_OBSCLK_MUX_SEL_div_in5 = {
	.n	= 384,
};
static const struct clk_parent clk_MCU_OBSCLK_OUTMUX_SEL_parents[] = {
	{
		CLK_AM62X_MCU_OBSCLK_DIV_OUT0,
		1,
	},
	{
		CLK_AM62X_GLUELOGIC_HFOSC0_CLKOUT,
		1,
	},
};
static const struct clk_data_mux_reg clk_data_MCU_OBSCLK_OUTMUX_SEL_out0 = {
	.data_mux		= {
		.parents	= clk_MCU_OBSCLK_OUTMUX_SEL_parents,
		.n		= ARRAY_SIZE(clk_MCU_OBSCLK_OUTMUX_SEL_parents),
	},
	.reg			= 0x04500000 + 32768,
	.bit			= 24,
};
static const struct clk_parent clk_MCU_TIMERCLKn_SEL_out0_parents[] = {
	{
		CLK_AM62X_GLUELOGIC_HFOSC0_CLKOUT,
		1,
	},
	{
		CLK_AM62X_SAM62_PLL_CTRL_WRAP_MCU_0_CHIP_DIV1_CLK_CLK,
		4,
	},
	{
		CLK_AM62X_GLUELOGIC_RCOSC_CLKOUT,
		1,
	},
	{
		CLK_AM62X_HSDIV4_16FFT_MCU_0_HSDIVOUT3_CLK,
		1,
	},
	{
		CLK_AM62X_BOARD_0_MCU_EXT_REFCLK0_OUT,
		1,
	},
	{
		CLK_AM62X_CLK_32K_RC_SEL_OUT0,
		1,
	},
	{
		CLK_AM62X_CPSW_3GUSS_MAIN_0_CPTS_GENF0,
		1,
	},
	{
		CLK_AM62X_MCU_TIMERCLKN_SEL_OUT0_DIV_CLKOUT,
		1,
	},
};
static const struct clk_data_mux_reg clk_data_MCU_TIMERCLKn_SEL_out0 = {
	.data_mux		= {
		.parents	= clk_MCU_TIMERCLKn_SEL_out0_parents,
		.n		= ARRAY_SIZE(clk_MCU_TIMERCLKn_SEL_out0_parents),
	},
	.reg			= 0x04500000 + 32864,
	.bit			= 0,
};
static const struct clk_data_div clk_data_MCU_TIMERCLKn_SEL_out0_div_in7 = {
	.n	= 384,
};
static const struct clk_parent clk_MCU_TIMERCLKn_SEL_out1_parents[] = {
	{
		CLK_AM62X_GLUELOGIC_HFOSC0_CLKOUT,
		1,
	},
	{
		CLK_AM62X_SAM62_PLL_CTRL_WRAP_MCU_0_CHIP_DIV1_CLK_CLK,
		4,
	},
	{
		CLK_AM62X_GLUELOGIC_RCOSC_CLKOUT,
		1,
	},
	{
		CLK_AM62X_HSDIV4_16FFT_MCU_0_HSDIVOUT3_CLK,
		1,
	},
	{
		CLK_AM62X_BOARD_0_MCU_EXT_REFCLK0_OUT,
		1,
	},
	{
		CLK_AM62X_CLK_32K_RC_SEL_OUT0,
		1,
	},
	{
		CLK_AM62X_CPSW_3GUSS_MAIN_0_CPTS_GENF0,
		1,
	},
	{
		CLK_AM62X_MCU_TIMERCLKN_SEL_OUT1_DIV_CLKOUT,
		1,
	},
};
static const struct clk_data_mux_reg clk_data_MCU_TIMERCLKn_SEL_out1 = {
	.data_mux		= {
		.parents	= clk_MCU_TIMERCLKn_SEL_out1_parents,
		.n		= ARRAY_SIZE(clk_MCU_TIMERCLKn_SEL_out1_parents),
	},
	.reg			= 0x04500000 + 32868,
	.bit			= 0,
};
static const struct clk_data_div clk_data_MCU_TIMERCLKn_SEL_out1_div_in7 = {
	.n	= 384,
};
static const struct clk_parent clk_MCU_TIMERCLKn_SEL_out2_parents[] = {
	{
		CLK_AM62X_GLUELOGIC_HFOSC0_CLKOUT,
		1,
	},
	{
		CLK_AM62X_SAM62_PLL_CTRL_WRAP_MCU_0_CHIP_DIV1_CLK_CLK,
		4,
	},
	{
		CLK_AM62X_GLUELOGIC_RCOSC_CLKOUT,
		1,
	},
	{
		CLK_AM62X_HSDIV4_16FFT_MCU_0_HSDIVOUT3_CLK,
		1,
	},
	{
		CLK_AM62X_BOARD_0_MCU_EXT_REFCLK0_OUT,
		1,
	},
	{
		CLK_AM62X_CLK_32K_RC_SEL_OUT0,
		1,
	},
	{
		CLK_AM62X_CPSW_3GUSS_MAIN_0_CPTS_GENF0,
		1,
	},
	{
		CLK_AM62X_MCU_TIMERCLKN_SEL_OUT2_DIV_CLKOUT,
		1,
	},
};
static const struct clk_data_mux_reg clk_data_MCU_TIMERCLKn_SEL_out2 = {
	.data_mux		= {
		.parents	= clk_MCU_TIMERCLKn_SEL_out2_parents,
		.n		= ARRAY_SIZE(clk_MCU_TIMERCLKn_SEL_out2_parents),
	},
	.reg			= 0x04500000 + 32872,
	.bit			= 0,
};
static const struct clk_data_div clk_data_MCU_TIMERCLKn_SEL_out2_div_in7 = {
	.n	= 384,
};
static const struct clk_parent clk_MCU_TIMERCLKn_SEL_out3_parents[] = {
	{
		CLK_AM62X_GLUELOGIC_HFOSC0_CLKOUT,
		1,
	},
	{
		CLK_AM62X_SAM62_PLL_CTRL_WRAP_MCU_0_CHIP_DIV1_CLK_CLK,
		4,
	},
	{
		CLK_AM62X_GLUELOGIC_RCOSC_CLKOUT,
		1,
	},
	{
		CLK_AM62X_HSDIV4_16FFT_MCU_0_HSDIVOUT3_CLK,
		1,
	},
	{
		CLK_AM62X_BOARD_0_MCU_EXT_REFCLK0_OUT,
		1,
	},
	{
		CLK_AM62X_CLK_32K_RC_SEL_OUT0,
		1,
	},
	{
		CLK_AM62X_CPSW_3GUSS_MAIN_0_CPTS_GENF0,
		1,
	},
	{
		CLK_AM62X_MCU_TIMERCLKN_SEL_OUT3_DIV_CLKOUT,
		1,
	},
};
static const struct clk_data_mux_reg clk_data_MCU_TIMERCLKn_SEL_out3 = {
	.data_mux		= {
		.parents	= clk_MCU_TIMERCLKn_SEL_out3_parents,
		.n		= ARRAY_SIZE(clk_MCU_TIMERCLKn_SEL_out3_parents),
	},
	.reg			= 0x04500000 + 32876,
	.bit			= 0,
};
static const struct clk_data_div clk_data_MCU_TIMERCLKn_SEL_out3_div_in7 = {
	.n	= 384,
};
static const struct clk_parent clk_MCU_WWDTCLK_SEL_parents[] = {
	{
		CLK_AM62X_GLUELOGIC_HFOSC0_CLKOUT,
		1,
	},
	{
		CLK_AM62X_CLK_32K_RC_SEL_OUT0,
		1,
	},
	{
		CLK_AM62X_GLUELOGIC_RCOSC_CLKOUT,
		1,
	},
	{
		CLK_AM62X_MCU_WWDTCLK_SEL_DIV_CLKOUT,
		1,
	},
};
static const struct clk_data_mux_reg clk_data_MCU_WWDTCLK_SEL_out0 = {
	.data_mux		= {
		.parents	= clk_MCU_WWDTCLK_SEL_parents,
		.n		= ARRAY_SIZE(clk_MCU_WWDTCLK_SEL_parents),
	},
	.reg			= 0x04500000 + 32944,
	.bit			= 0,
};
static const struct clk_data_div clk_data_MCU_WWDTCLK_SEL_div_in3 = {
	.n	= 384,
};
static const struct clk_parent clk_MCU_spi0_mstr_lp_clksel_parents[] = {
	{
		CLK_AM62X_BOARD_0_MCU_SPI0_CLK_OUT,
		1,
	},
	{
		CLK_AM62X_SPI_MCU_0_IO_CLKSPIO_CLK,
		1,
	},
};
static const struct clk_data_mux_reg clk_data_MCU_spi0_mstr_lp_clksel_out0 = {
	.data_mux		= {
		.parents	= clk_MCU_spi0_mstr_lp_clksel_parents,
		.n		= ARRAY_SIZE(clk_MCU_spi0_mstr_lp_clksel_parents),
	},
	.reg			= 0x04500000 + 32928,
	.bit			= 16,
};
static const struct clk_parent clk_MCU_spi1_mstr_lp_clksel_parents[] = {
	{
		CLK_AM62X_BOARD_0_MCU_SPI1_CLK_OUT,
		1,
	},
	{
		CLK_AM62X_SPI_MCU_1_IO_CLKSPIO_CLK,
		1,
	},
};
static const struct clk_data_mux_reg clk_data_MCU_spi1_mstr_lp_clksel_out0 = {
	.data_mux		= {
		.parents	= clk_MCU_spi1_mstr_lp_clksel_parents,
		.n		= ARRAY_SIZE(clk_MCU_spi1_mstr_lp_clksel_parents),
	},
	.reg			= 0x04500000 + 32932,
	.bit			= 16,
};
static const struct clk_parent clk_RTC_CLK_SEL_parents[] = {
	{
		CLK_AM62X_CLK_32K_RC_SEL_OUT0,
		1,
	},
	{
		CLK_AM62X_RTC_CLK_SEL_DIV_CLKOUT,
		1,
	},
};
static const struct clk_data_mux_reg clk_data_RTC_CLK_SEL_out0 = {
	.data_mux		= {
		.parents	= clk_RTC_CLK_SEL_parents,
		.n		= ARRAY_SIZE(clk_RTC_CLK_SEL_parents),
	},
	.reg			= 0x43000000 + 34304,
	.bit			= 0,
};
static const struct clk_data_div clk_data_RTC_CLK_SEL_div_in1 = {
	.n	= 384,
};
static const struct clk_data_div_reg_go clk_data_USART_Programmable_Clock_Divider_out0 = {
	.data_div	= {
		.n	= 4,
	},
	.reg		= 0x00100000 + 33344,
	.bit		= 0,
	.go		= 16,
};
static const struct clk_data_div_reg_go clk_data_USART_Programmable_Clock_Divider_out1 = {
	.data_div	= {
		.n	= 4,
	},
	.reg		= 0x00100000 + 33348,
	.bit		= 0,
	.go		= 16,
};
static const struct clk_data_div_reg_go clk_data_USART_Programmable_Clock_Divider_out2 = {
	.data_div	= {
		.n	= 4,
	},
	.reg		= 0x00100000 + 33352,
	.bit		= 0,
	.go		= 16,
};
static const struct clk_data_div_reg_go clk_data_USART_Programmable_Clock_Divider_out3 = {
	.data_div	= {
		.n	= 4,
	},
	.reg		= 0x00100000 + 33356,
	.bit		= 0,
	.go		= 16,
};
static const struct clk_data_div_reg_go clk_data_USART_Programmable_Clock_Divider_out4 = {
	.data_div	= {
		.n	= 4,
	},
	.reg		= 0x00100000 + 33360,
	.bit		= 0,
	.go		= 16,
};
static const struct clk_data_div_reg_go clk_data_USART_Programmable_Clock_Divider_out5 = {
	.data_div	= {
		.n	= 4,
	},
	.reg		= 0x00100000 + 33364,
	.bit		= 0,
	.go		= 16,
};
static const struct clk_data_div_reg_go clk_data_USART_Programmable_Clock_Divider_out6 = {
	.data_div	= {
		.n	= 4,
	},
	.reg		= 0x00100000 + 33368,
	.bit		= 0,
	.go		= 16,
};
static const struct clk_parent clk_WKUP_CLKOUT_SEL_parents[] = {
	{
		CLK_AM62X_GLUELOGIC_HFOSC0_CLKOUT,
		1,
	},
	{
		CLK_AM62X_GLUELOGIC_LFOSC0_CLKOUT,
		1,
	},
	{
		CLK_AM62X_HSDIV4_16FFT_MAIN_0_HSDIVOUT2_CLK,
		1,
	},
	{
		CLK_AM62X_HSDIV4_16FFT_MAIN_1_HSDIVOUT2_CLK,
		1,
	},
	{
		CLK_AM62X_POSTDIV4_16FF_MAIN_2_HSDIVOUT9_CLK,
		1,
	},
	{
		CLK_AM62X_CLK_32K_RC_SEL_OUT0,
		1,
	},
	{
		CLK_AM62X_GLUELOGIC_RCOSC_CLKOUT,
		1,
	},
	{
		CLK_AM62X_GLUELOGIC_HFOSC0_CLKOUT,
		1,
	},
};
static const struct clk_data_mux_reg clk_data_WKUP_CLKOUT_SEL_out0 = {
	.data_mux		= {
		.parents	= clk_WKUP_CLKOUT_SEL_parents,
		.n		= ARRAY_SIZE(clk_WKUP_CLKOUT_SEL_parents),
	},
	.reg			= 0x43000000 + 32800,
	.bit			= 0,
};
static const struct clk_parent clk_WKUP_CLKSEL_parents[] = {
	{
		CLK_AM62X_HSDIV1_16FFT_MAIN_15_HSDIVOUT0_CLK,
		1,
	},
	{
		CLK_AM62X_HSDIV4_16FFT_MCU_0_HSDIVOUT0_CLK,
		1,
	},
};
static const struct clk_data_mux_reg clk_data_WKUP_CLKSEL_out0 = {
	.data_mux		= {
		.parents	= clk_WKUP_CLKSEL_parents,
		.n		= ARRAY_SIZE(clk_WKUP_CLKSEL_parents),
	},
	.reg			= 0x43000000 + 32784,
	.bit			= 0,
};
static const struct clk_parent clk_WKUP_TIMERCLKn_SEL_out0_parents[] = {
	{
		CLK_AM62X_GLUELOGIC_HFOSC0_CLKOUT,
		1,
	},
	{
		CLK_AM62X_WKUP_CLKSEL_OUT0,
		4,
	},
	{
		CLK_AM62X_GLUELOGIC_RCOSC_CLKOUT,
		1,
	},
	{
		CLK_AM62X_HSDIV4_16FFT_MCU_0_HSDIVOUT3_CLK,
		1,
	},
	{
		CLK_AM62X_BOARD_0_MCU_EXT_REFCLK0_OUT,
		1,
	},
	{
		CLK_AM62X_CLK_32K_RC_SEL_OUT0,
		1,
	},
	{
		CLK_AM62X_CPSW_3GUSS_MAIN_0_CPTS_GENF0,
		1,
	},
	{
		CLK_AM62X_WKUP_TIMERCLKN_SEL_OUT0_DIV_CLKOUT,
		1,
	},
};
static const struct clk_data_mux_reg clk_data_WKUP_TIMERCLKn_SEL_out0 = {
	.data_mux		= {
		.parents	= clk_WKUP_TIMERCLKn_SEL_out0_parents,
		.n		= ARRAY_SIZE(clk_WKUP_TIMERCLKn_SEL_out0_parents),
	},
	.reg			= 0x43000000 + 33200,
	.bit			= 0,
};
static const struct clk_data_div clk_data_WKUP_TIMERCLKn_SEL_out0_div_in7 = {
	.n	= 384,
};
static const struct clk_parent clk_WKUP_TIMERCLKn_SEL_out1_parents[] = {
	{
		CLK_AM62X_GLUELOGIC_HFOSC0_CLKOUT,
		1,
	},
	{
		CLK_AM62X_WKUP_CLKSEL_OUT0,
		4,
	},
	{
		CLK_AM62X_GLUELOGIC_RCOSC_CLKOUT,
		1,
	},
	{
		CLK_AM62X_HSDIV4_16FFT_MCU_0_HSDIVOUT3_CLK,
		1,
	},
	{
		CLK_AM62X_BOARD_0_MCU_EXT_REFCLK0_OUT,
		1,
	},
	{
		CLK_AM62X_CLK_32K_RC_SEL_OUT0,
		1,
	},
	{
		CLK_AM62X_CPSW_3GUSS_MAIN_0_CPTS_GENF0,
		1,
	},
	{
		CLK_AM62X_WKUP_TIMERCLKN_SEL_OUT1_DIV_CLKOUT,
		1,
	},
};
static const struct clk_data_mux_reg clk_data_WKUP_TIMERCLKn_SEL_out1 = {
	.data_mux		= {
		.parents	= clk_WKUP_TIMERCLKn_SEL_out1_parents,
		.n		= ARRAY_SIZE(clk_WKUP_TIMERCLKn_SEL_out1_parents),
	},
	.reg			= 0x43000000 + 33204,
	.bit			= 0,
};
static const struct clk_data_div clk_data_WKUP_TIMERCLKn_SEL_out1_div_in7 = {
	.n	= 384,
};
static const struct clk_parent clk_WKUP_WWDTCLK_SEL_parents[] = {
	{
		CLK_AM62X_GLUELOGIC_HFOSC0_CLKOUT,
		1,
	},
	{
		CLK_AM62X_CLK_32K_RC_SEL_OUT0,
		1,
	},
	{
		CLK_AM62X_GLUELOGIC_RCOSC_CLKOUT,
		1,
	},
	{
		CLK_AM62X_WKUP_WWDTCLK_SEL_DIV_CLKOUT,
		1,
	},
};
static const struct clk_data_mux_reg clk_data_WKUP_WWDTCLK_SEL_out0 = {
	.data_mux		= {
		.parents	= clk_WKUP_WWDTCLK_SEL_parents,
		.n		= ARRAY_SIZE(clk_WKUP_WWDTCLK_SEL_parents),
	},
	.reg			= 0x43000000 + 33664,
	.bit			= 0,
};
static const struct clk_data_div clk_data_WKUP_WWDTCLK_SEL_div_in3 = {
	.n	= 384,
};
static const struct clk_data_from_dev clk_data_board_0_AUDIO_EXT_REFCLK0_out = {
	.dev		= AM62X_DEV_BOARD0,
	.clk_idx	= AM62X_DEV_BOARD0_AUDIO_EXT_REFCLK0_OUT,
};
static const struct clk_data_from_dev clk_data_board_0_AUDIO_EXT_REFCLK1_out = {
	.dev		= AM62X_DEV_BOARD0,
	.clk_idx	= AM62X_DEV_BOARD0_AUDIO_EXT_REFCLK1_OUT,
};
static const struct clk_data_from_dev clk_data_board_0_CP_GEMAC_CPTS0_RFT_CLK_out = {
	.dev		= AM62X_DEV_BOARD0,
	.clk_idx	= AM62X_DEV_BOARD0_CP_GEMAC_CPTS0_RFT_CLK_OUT,
};
static const struct clk_data_from_dev clk_data_board_0_DDR0_CK0_out = {
	.dev		= AM62X_DEV_BOARD0,
	.clk_idx	= AM62X_DEV_BOARD0_DDR0_CK0_OUT,
};
static const struct clk_data_from_dev clk_data_board_0_EXT_REFCLK1_out = {
	.dev		= AM62X_DEV_BOARD0,
	.clk_idx	= AM62X_DEV_BOARD0_EXT_REFCLK1_OUT,
};
static const struct clk_data_from_dev clk_data_board_0_GPMC0_CLKLB_out = {
	.dev		= AM62X_DEV_BOARD0,
	.clk_idx	= AM62X_DEV_BOARD0_GPMC0_CLKLB_OUT,
};
static const struct clk_data_from_dev clk_data_board_0_I2C0_SCL_out = {
	.dev		= AM62X_DEV_BOARD0,
	.clk_idx	= AM62X_DEV_BOARD0_I2C0_SCL_OUT,
};
static const struct clk_data_from_dev clk_data_board_0_I2C1_SCL_out = {
	.dev		= AM62X_DEV_BOARD0,
	.clk_idx	= AM62X_DEV_BOARD0_I2C1_SCL_OUT,
};
static const struct clk_data_from_dev clk_data_board_0_I2C2_SCL_out = {
	.dev		= AM62X_DEV_BOARD0,
	.clk_idx	= AM62X_DEV_BOARD0_I2C2_SCL_OUT,
};
static const struct clk_data_from_dev clk_data_board_0_I2C3_SCL_out = {
	.dev		= AM62X_DEV_BOARD0,
	.clk_idx	= AM62X_DEV_BOARD0_I2C3_SCL_OUT,
};
static const struct clk_data_from_dev clk_data_board_0_MCASP0_ACLKR_out = {
	.dev		= AM62X_DEV_BOARD0,
	.clk_idx	= AM62X_DEV_BOARD0_MCASP0_ACLKR_OUT,
};
static const struct clk_data_from_dev clk_data_board_0_MCASP0_ACLKX_out = {
	.dev		= AM62X_DEV_BOARD0,
	.clk_idx	= AM62X_DEV_BOARD0_MCASP0_ACLKX_OUT,
};
static const struct clk_data_from_dev clk_data_board_0_MCASP1_ACLKR_out = {
	.dev		= AM62X_DEV_BOARD0,
	.clk_idx	= AM62X_DEV_BOARD0_MCASP1_ACLKR_OUT,
};
static const struct clk_data_from_dev clk_data_board_0_MCASP1_ACLKX_out = {
	.dev		= AM62X_DEV_BOARD0,
	.clk_idx	= AM62X_DEV_BOARD0_MCASP1_ACLKX_OUT,
};
static const struct clk_data_from_dev clk_data_board_0_MCASP2_ACLKR_out = {
	.dev		= AM62X_DEV_BOARD0,
	.clk_idx	= AM62X_DEV_BOARD0_MCASP2_ACLKR_OUT,
};
static const struct clk_data_from_dev clk_data_board_0_MCASP2_ACLKX_out = {
	.dev		= AM62X_DEV_BOARD0,
	.clk_idx	= AM62X_DEV_BOARD0_MCASP2_ACLKX_OUT,
};
static const struct clk_data_from_dev clk_data_board_0_MCU_EXT_REFCLK0_out = {
	.dev		= AM62X_DEV_BOARD0,
	.clk_idx	= AM62X_DEV_BOARD0_MCU_EXT_REFCLK0_OUT,
};
static const struct clk_data_from_dev clk_data_board_0_MCU_I2C0_SCL_out = {
	.dev		= AM62X_DEV_BOARD0,
	.clk_idx	= AM62X_DEV_BOARD0_MCU_I2C0_SCL_OUT,
};
static const struct clk_data_from_dev clk_data_board_0_MCU_SPI0_CLK_out = {
	.dev		= AM62X_DEV_BOARD0,
	.clk_idx	= AM62X_DEV_BOARD0_MCU_SPI0_CLK_OUT,
};
static const struct clk_data_from_dev clk_data_board_0_MCU_SPI1_CLK_out = {
	.dev		= AM62X_DEV_BOARD0,
	.clk_idx	= AM62X_DEV_BOARD0_MCU_SPI1_CLK_OUT,
};
static const struct clk_data_from_dev clk_data_board_0_MMC0_CLKLB_out = {
	.dev		= AM62X_DEV_BOARD0,
	.clk_idx	= AM62X_DEV_BOARD0_MMC0_CLKLB_OUT,
};
static const struct clk_data_from_dev clk_data_board_0_MMC0_CLK_out = {
	.dev		= AM62X_DEV_BOARD0,
	.clk_idx	= AM62X_DEV_BOARD0_MMC0_CLK_OUT,
};
static const struct clk_data_from_dev clk_data_board_0_MMC1_CLKLB_out = {
	.dev		= AM62X_DEV_BOARD0,
	.clk_idx	= AM62X_DEV_BOARD0_MMC1_CLKLB_OUT,
};
static const struct clk_data_from_dev clk_data_board_0_MMC1_CLK_out = {
	.dev		= AM62X_DEV_BOARD0,
	.clk_idx	= AM62X_DEV_BOARD0_MMC1_CLK_OUT,
};
static const struct clk_data_from_dev clk_data_board_0_MMC2_CLKLB_out = {
	.dev		= AM62X_DEV_BOARD0,
	.clk_idx	= AM62X_DEV_BOARD0_MMC2_CLKLB_OUT,
};
static const struct clk_data_from_dev clk_data_board_0_MMC2_CLK_out = {
	.dev		= AM62X_DEV_BOARD0,
	.clk_idx	= AM62X_DEV_BOARD0_MMC2_CLK_OUT,
};
static const struct clk_data_from_dev clk_data_board_0_OSPI0_DQS_out = {
	.dev		= AM62X_DEV_BOARD0,
	.clk_idx	= AM62X_DEV_BOARD0_OSPI0_DQS_OUT,
};
static const struct clk_data_from_dev clk_data_board_0_OSPI0_LBCLKO_out = {
	.dev		= AM62X_DEV_BOARD0,
	.clk_idx	= AM62X_DEV_BOARD0_OSPI0_LBCLKO_OUT,
};
static const struct clk_data_from_dev clk_data_board_0_RGMII1_RXC_out = {
	.dev		= AM62X_DEV_BOARD0,
	.clk_idx	= AM62X_DEV_BOARD0_RGMII1_RXC_OUT,
};
static const struct clk_data_from_dev clk_data_board_0_RGMII1_TXC_out = {
	.dev		= AM62X_DEV_BOARD0,
	.clk_idx	= AM62X_DEV_BOARD0_RGMII1_TXC_OUT,
};
static const struct clk_data_from_dev clk_data_board_0_RGMII2_RXC_out = {
	.dev		= AM62X_DEV_BOARD0,
	.clk_idx	= AM62X_DEV_BOARD0_RGMII2_RXC_OUT,
};
static const struct clk_data_from_dev clk_data_board_0_RGMII2_TXC_out = {
	.dev		= AM62X_DEV_BOARD0,
	.clk_idx	= AM62X_DEV_BOARD0_RGMII2_TXC_OUT,
};
static const struct clk_data_from_dev clk_data_board_0_RMII1_REF_CLK_out = {
	.dev		= AM62X_DEV_BOARD0,
	.clk_idx	= AM62X_DEV_BOARD0_RMII1_REF_CLK_OUT,
};
static const struct clk_data_from_dev clk_data_board_0_RMII2_REF_CLK_out = {
	.dev		= AM62X_DEV_BOARD0,
	.clk_idx	= AM62X_DEV_BOARD0_RMII2_REF_CLK_OUT,
};
static const struct clk_data_from_dev clk_data_board_0_SPI0_CLK_out = {
	.dev		= AM62X_DEV_BOARD0,
	.clk_idx	= AM62X_DEV_BOARD0_SPI0_CLK_OUT,
};
static const struct clk_data_from_dev clk_data_board_0_SPI1_CLK_out = {
	.dev		= AM62X_DEV_BOARD0,
	.clk_idx	= AM62X_DEV_BOARD0_SPI1_CLK_OUT,
};
static const struct clk_data_from_dev clk_data_board_0_SPI2_CLK_out = {
	.dev		= AM62X_DEV_BOARD0,
	.clk_idx	= AM62X_DEV_BOARD0_SPI2_CLK_OUT,
};
static const struct clk_data_from_dev clk_data_board_0_TCK_out = {
	.dev		= AM62X_DEV_BOARD0,
	.clk_idx	= AM62X_DEV_BOARD0_TCK_OUT,
};
static const struct clk_data_from_dev clk_data_board_0_VOUT0_EXTPCLKIN_out = {
	.dev		= AM62X_DEV_BOARD0,
	.clk_idx	= AM62X_DEV_BOARD0_VOUT0_EXTPCLKIN_OUT,
};
static const struct clk_data_from_dev clk_data_cpsw_3guss_main_0_cpts_genf0 = {
	.dev		= AM62X_DEV_CPSW0,
	.clk_idx	= AM62X_DEV_CPSW0_CPTS_GENF0,
};
static const struct clk_data_from_dev clk_data_cpsw_3guss_main_0_cpts_genf1 = {
	.dev		= AM62X_DEV_CPSW0,
	.clk_idx	= AM62X_DEV_CPSW0_CPTS_GENF1,
};
static const struct clk_data_from_dev clk_data_cpsw_3guss_main_0_mdio_mdclk_o = {
	.dev		= AM62X_DEV_CPSW0,
	.clk_idx	= AM62X_DEV_CPSW0_MDIO_MDCLK_O,
};
static const struct clk_data_from_dev clk_data_cpsw_3guss_main_0_rgmii1_txc_o = {
	.dev		= AM62X_DEV_CPSW0,
	.clk_idx	= AM62X_DEV_CPSW0_RGMII1_TXC_O,
};
static const struct clk_data_from_dev clk_data_cpsw_3guss_main_0_rgmii2_txc_o = {
	.dev		= AM62X_DEV_CPSW0,
	.clk_idx	= AM62X_DEV_CPSW0_RGMII2_TXC_O,
};
static const struct clk_data_from_dev clk_data_debugss_k3_wrap_cv0_main_0_cstpiu_traceclk = {
	.dev		= AM62X_DEV_DEBUGSS_WRAP0,
	.clk_idx	= AM62X_DEV_DEBUGSS_WRAP0_CSTPIU_TRACECLK,
};
static const struct clk_data_from_dev clk_data_dmtimer_dmc1ms_main_0_timer_pwm = {
	.dev		= AM62X_DEV_TIMER0,
	.clk_idx	= AM62X_DEV_TIMER0_TIMER_PWM,
};
static const struct clk_data_from_dev clk_data_dmtimer_dmc1ms_main_1_timer_pwm = {
	.dev		= AM62X_DEV_TIMER1,
	.clk_idx	= AM62X_DEV_TIMER1_TIMER_PWM,
};
static const struct clk_data_from_dev clk_data_dmtimer_dmc1ms_main_2_timer_pwm = {
	.dev		= AM62X_DEV_TIMER2,
	.clk_idx	= AM62X_DEV_TIMER2_TIMER_PWM,
};
static const struct clk_data_from_dev clk_data_dmtimer_dmc1ms_main_3_timer_pwm = {
	.dev		= AM62X_DEV_TIMER3,
	.clk_idx	= AM62X_DEV_TIMER3_TIMER_PWM,
};
static const struct clk_data_from_dev clk_data_dmtimer_dmc1ms_main_4_timer_pwm = {
	.dev		= AM62X_DEV_TIMER4,
	.clk_idx	= AM62X_DEV_TIMER4_TIMER_PWM,
};
static const struct clk_data_from_dev clk_data_dmtimer_dmc1ms_main_5_timer_pwm = {
	.dev		= AM62X_DEV_TIMER5,
	.clk_idx	= AM62X_DEV_TIMER5_TIMER_PWM,
};
static const struct clk_data_from_dev clk_data_dmtimer_dmc1ms_main_6_timer_pwm = {
	.dev		= AM62X_DEV_TIMER6,
	.clk_idx	= AM62X_DEV_TIMER6_TIMER_PWM,
};
static const struct clk_data_from_dev clk_data_dmtimer_dmc1ms_main_7_timer_pwm = {
	.dev		= AM62X_DEV_TIMER7,
	.clk_idx	= AM62X_DEV_TIMER7_TIMER_PWM,
};
static const struct clk_data_from_dev clk_data_dmtimer_dmc1ms_mcu_0_timer_pwm = {
	.dev		= AM62X_DEV_MCU_TIMER0,
	.clk_idx	= AM62X_DEV_MCU_TIMER0_TIMER_PWM,
};
static const struct clk_data_from_dev clk_data_dmtimer_dmc1ms_mcu_1_timer_pwm = {
	.dev		= AM62X_DEV_MCU_TIMER1,
	.clk_idx	= AM62X_DEV_MCU_TIMER1_TIMER_PWM,
};
static const struct clk_data_from_dev clk_data_dmtimer_dmc1ms_mcu_2_timer_pwm = {
	.dev		= AM62X_DEV_MCU_TIMER2,
	.clk_idx	= AM62X_DEV_MCU_TIMER2_TIMER_PWM,
};
static const struct clk_data_from_dev clk_data_dmtimer_dmc1ms_mcu_3_timer_pwm = {
	.dev		= AM62X_DEV_MCU_TIMER3,
	.clk_idx	= AM62X_DEV_MCU_TIMER3_TIMER_PWM,
};
static const struct clk_data_from_dev clk_data_emmcsd4ss_main_0_emmcsdss_io_clk_o = {
	.dev		= AM62X_DEV_MMCSD1,
	.clk_idx	= AM62X_DEV_MMCSD1_EMMCSDSS_IO_CLK_O,
};
static const struct clk_data_from_dev clk_data_emmcsd4ss_main_1_emmcsdss_io_clk_o = {
	.dev		= AM62X_DEV_MMCSD2,
	.clk_idx	= AM62X_DEV_MMCSD2_EMMCSDSS_IO_CLK_O,
};
static const struct clk_data_from_dev clk_data_emmcsd8ss_main_0_emmcsdss_io_clk_o = {
	.dev		= AM62X_DEV_MMCSD0,
	.clk_idx	= AM62X_DEV_MMCSD0_EMMCSDSS_IO_CLK_O,
};
static const struct clk_data_from_dev clk_data_fss_ul_main_0_ospi_0_ospi_oclk_clk = {
	.dev		= AM62X_DEV_FSS0_OSPI_0,
	.clk_idx	= AM62X_DEV_FSS0_OSPI_0_OSPI_OCLK_CLK,
};
static const struct clk_data_from_dev clk_data_gpmc_main_0_po_gpmc_dev_clk = {
	.dev		= AM62X_DEV_GPMC0,
	.clk_idx	= AM62X_DEV_GPMC0_PO_GPMC_DEV_CLK,
};
static const struct clk_data_div_reg clk_data_hsdiv0_16fft_main_12_hsdiv0 = {
	.data_div		= {
		.n		= 128,
		.default_div	= 4,
	},
	.reg			= 0x00680000UL + (0x1000UL * 12UL) + 0x80UL + (0x4UL * 0UL),
	.bit			= 0,
};
static const struct clk_data_div_reg clk_data_hsdiv0_16fft_main_16_hsdiv0 = {
	.data_div		= {
		.n		= 128,
		.default_div	= 2,
	},
	.reg			= 0x00680000UL + (0x1000UL * 16UL) + 0x80UL + (0x4UL * 0UL),
	.bit			= 0,
};
static const struct clk_data_div_reg clk_data_hsdiv0_16fft_main_17_hsdiv0 = {
	.data_div		= {
		.n		= 128,
		.default_div	= 12,
	},
	.reg			= 0x00680000UL + (0x1000UL * 17UL) + 0x80UL + (0x4UL * 0UL),
	.bit			= 0,
};
static const struct clk_data_div_reg clk_data_hsdiv0_16fft_main_8_hsdiv0 = {
	.data_div		= {
		.n		= 128,
		.default_div	= 2,
	},
	.reg			= 0x00680000UL + (0x1000UL * 8UL) + 0x80UL + (0x4UL * 0UL),
	.bit			= 0,
};
static const struct clk_data_div_reg clk_data_hsdiv0_16fft_mcu_32khz_gen_0_hsdiv0 = {
	.data_div	= {
		.n	= 128,
	},
	.reg		= 0x04500000UL + 0x8030UL + (0x4UL * 0UL),
	.bit		= 0,
};
static const struct clk_data_div_reg clk_data_hsdiv1_16fft_main_15_hsdiv0 = {
	.data_div		= {
		.n		= 128,
		.default_div	= 5,
	},
	.reg			= 0x00680000UL + (0x1000UL * 15UL) + 0x80UL + (0x4UL * 0UL),
	.bit			= 0,
};
static const struct clk_data_div_reg clk_data_hsdiv1_16fft_main_15_hsdiv1 = {
	.data_div		= {
		.n		= 128,
		.default_div	= 5,
	},
	.reg			= 0x00680000UL + (0x1000UL * 15UL) + 0x80UL + (0x4UL * 1UL),
	.bit			= 0,
};
static const struct clk_data_div_reg clk_data_hsdiv4_16fft_main_0_hsdiv0 = {
	.data_div		= {
		.n		= 128,
		.default_div	= 4,
	},
	.reg			= 0x00680000UL + (0x1000UL * 0UL) + 0x80UL + (0x4UL * 0UL),
	.bit			= 0,
};
static const struct clk_data_div_reg clk_data_hsdiv4_16fft_main_0_hsdiv1 = {
	.data_div		= {
		.n		= 128,
		.default_div	= 10,
	},
	.reg			= 0x00680000UL + (0x1000UL * 0UL) + 0x80UL + (0x4UL * 1UL),
	.bit			= 0,
};
static const struct clk_data_div_reg clk_data_hsdiv4_16fft_main_0_hsdiv2 = {
	.data_div		= {
		.n		= 128,
		.default_div	= 10,
	},
	.reg			= 0x00680000UL + (0x1000UL * 0UL) + 0x80UL + (0x4UL * 2UL),
	.bit			= 0,
};
static const struct clk_data_div_reg clk_data_hsdiv4_16fft_main_0_hsdiv3 = {
	.data_div		= {
		.n		= 128,
		.default_div	= 15,
	},
	.reg			= 0x00680000UL + (0x1000UL * 0UL) + 0x80UL + (0x4UL * 3UL),
	.bit			= 0,
};
static const struct clk_data_div_reg clk_data_hsdiv4_16fft_main_0_hsdiv4 = {
	.data_div		= {
		.n		= 128,
		.default_div	= 25,
	},
	.reg			= 0x00680000UL + (0x1000UL * 0UL) + 0x80UL + (0x4UL * 4UL),
	.bit			= 0,
};
static const struct clk_data_div_reg clk_data_hsdiv4_16fft_main_1_hsdiv0 = {
	.data_div		= {
		.n		= 128,
		.default_div	= 10,
	},
	.reg			= 0x00680000UL + (0x1000UL * 1UL) + 0x80UL + (0x4UL * 0UL),
	.bit			= 0,
};
static const struct clk_data_div_reg clk_data_hsdiv4_16fft_main_1_hsdiv1 = {
	.data_div		= {
		.n		= 128,
		.default_div	= 12,
	},
	.reg			= 0x00680000UL + (0x1000UL * 1UL) + 0x80UL + (0x4UL * 1UL),
	.bit			= 0,
};
static const struct clk_data_div_reg clk_data_hsdiv4_16fft_main_1_hsdiv2 = {
	.data_div		= {
		.n		= 128,
		.default_div	= 10,
	},
	.reg			= 0x00680000UL + (0x1000UL * 1UL) + 0x80UL + (0x4UL * 2UL),
	.bit			= 0,
};
static const struct clk_data_div_reg clk_data_hsdiv4_16fft_main_1_hsdiv3 = {
	.data_div		= {
		.n		= 128,
		.default_div	= 10,
	},
	.reg			= 0x00680000UL + (0x1000UL * 1UL) + 0x80UL + (0x4UL * 3UL),
	.bit			= 0,
};
static const struct clk_data_div_reg clk_data_hsdiv4_16fft_main_1_hsdiv4 = {
	.data_div		= {
		.n		= 128,
		.default_div	= 80,
	},
	.reg			= 0x00680000UL + (0x1000UL * 1UL) + 0x80UL + (0x4UL * 4UL),
	.bit			= 0,
};
static const struct clk_data_div_reg clk_data_hsdiv4_16fft_main_2_hsdiv0 = {
	.data_div		= {
		.n		= 128,
		.default_div	= 6,
	},
	.reg			= 0x00680000UL + (0x1000UL * 2UL) + 0x80UL + (0x4UL * 0UL),
	.bit			= 0,
};
static const struct clk_data_div_reg clk_data_hsdiv4_16fft_main_2_hsdiv1 = {
	.data_div		= {
		.n		= 128,
		.default_div	= 8,
	},
	.reg			= 0x00680000UL + (0x1000UL * 2UL) + 0x80UL + (0x4UL * 1UL),
	.bit			= 0,
};
static const struct clk_data_div_reg clk_data_hsdiv4_16fft_main_2_hsdiv2 = {
	.data_div		= {
		.n		= 128,
		.default_div	= 10,
	},
	.reg			= 0x00680000UL + (0x1000UL * 2UL) + 0x80UL + (0x4UL * 2UL),
	.bit			= 0,
};
static const struct clk_data_div_reg clk_data_hsdiv4_16fft_main_2_hsdiv3 = {
	.data_div		= {
		.n		= 128,
		.default_div	= 6,
	},
	.reg			= 0x00680000UL + (0x1000UL * 2UL) + 0x80UL + (0x4UL * 3UL),
	.bit			= 0,
};
static const struct clk_data_div_reg clk_data_hsdiv4_16fft_main_2_hsdiv4 = {
	.data_div		= {
		.n		= 128,
		.default_div	= 4,
	},
	.reg			= 0x00680000UL + (0x1000UL * 2UL) + 0x80UL + (0x4UL * 4UL),
	.bit			= 0,
};
static const struct clk_data_div_reg clk_data_hsdiv4_16fft_mcu_0_hsdiv0 = {
	.data_div		= {
		.n		= 128,
		.default_div	= 6,
	},
	.reg			= 0x04040000UL + (0x1000UL * 0UL) + 0x80UL + (0x4UL * 0UL),
	.bit			= 0,
};
static const struct clk_data_div_reg clk_data_hsdiv4_16fft_mcu_0_hsdiv1 = {
	.data_div		= {
		.n		= 128,
		.default_div	= 25,
	},
	.reg			= 0x04040000UL + (0x1000UL * 0UL) + 0x80UL + (0x4UL * 1UL),
	.bit			= 0,
};
static const struct clk_data_div_reg clk_data_hsdiv4_16fft_mcu_0_hsdiv2 = {
	.data_div		= {
		.n		= 128,
		.default_div	= 50,
	},
	.reg			= 0x04040000UL + (0x1000UL * 0UL) + 0x80UL + (0x4UL * 2UL),
	.bit			= 0,
};
static const struct clk_data_div_reg clk_data_hsdiv4_16fft_mcu_0_hsdiv3 = {
	.data_div		= {
		.n		= 128,
		.default_div	= 12,
	},
	.reg			= 0x04040000UL + (0x1000UL * 0UL) + 0x80UL + (0x4UL * 3UL),
	.bit			= 0,
};
static const struct clk_data_div_reg clk_data_hsdiv4_16fft_mcu_0_hsdiv4 = {
	.data_div		= {
		.n		= 128,
		.default_div	= 120,
	},
	.reg			= 0x04040000UL + (0x1000UL * 0UL) + 0x80UL + (0x4UL * 4UL),
	.bit			= 0,
};
static const struct clk_data_from_dev clk_data_k3_dss_ul_main_0_dpi_1_out_clk = {
	.dev		= AM62X_DEV_DSS0,
	.clk_idx	= AM62X_DEV_DSS0_DPI_1_OUT_CLK,
};
static const struct clk_data_from_dev clk_data_mcasp_main_0_mcasp_aclkr_pout = {
	.dev		= AM62X_DEV_MCASP0,
	.clk_idx	= AM62X_DEV_MCASP0_MCASP_ACLKR_POUT,
};
static const struct clk_data_from_dev clk_data_mcasp_main_0_mcasp_aclkx_pout = {
	.dev		= AM62X_DEV_MCASP0,
	.clk_idx	= AM62X_DEV_MCASP0_MCASP_ACLKX_POUT,
};
static const struct clk_data_from_dev clk_data_mcasp_main_0_mcasp_afsr_pout = {
	.dev		= AM62X_DEV_MCASP0,
	.clk_idx	= AM62X_DEV_MCASP0_MCASP_AFSR_POUT,
};
static const struct clk_data_from_dev clk_data_mcasp_main_0_mcasp_afsx_pout = {
	.dev		= AM62X_DEV_MCASP0,
	.clk_idx	= AM62X_DEV_MCASP0_MCASP_AFSX_POUT,
};
static const struct clk_data_from_dev clk_data_mcasp_main_0_mcasp_ahclkr_pout = {
	.dev		= AM62X_DEV_MCASP0,
	.clk_idx	= AM62X_DEV_MCASP0_MCASP_AHCLKR_POUT,
};
static const struct clk_data_from_dev clk_data_mcasp_main_0_mcasp_ahclkx_pout = {
	.dev		= AM62X_DEV_MCASP0,
	.clk_idx	= AM62X_DEV_MCASP0_MCASP_AHCLKX_POUT,
};
static const struct clk_data_from_dev clk_data_mcasp_main_1_mcasp_aclkr_pout = {
	.dev		= AM62X_DEV_MCASP1,
	.clk_idx	= AM62X_DEV_MCASP1_MCASP_ACLKR_POUT,
};
static const struct clk_data_from_dev clk_data_mcasp_main_1_mcasp_aclkx_pout = {
	.dev		= AM62X_DEV_MCASP1,
	.clk_idx	= AM62X_DEV_MCASP1_MCASP_ACLKX_POUT,
};
static const struct clk_data_from_dev clk_data_mcasp_main_1_mcasp_afsr_pout = {
	.dev		= AM62X_DEV_MCASP1,
	.clk_idx	= AM62X_DEV_MCASP1_MCASP_AFSR_POUT,
};
static const struct clk_data_from_dev clk_data_mcasp_main_1_mcasp_afsx_pout = {
	.dev		= AM62X_DEV_MCASP1,
	.clk_idx	= AM62X_DEV_MCASP1_MCASP_AFSX_POUT,
};
static const struct clk_data_from_dev clk_data_mcasp_main_1_mcasp_ahclkr_pout = {
	.dev		= AM62X_DEV_MCASP1,
	.clk_idx	= AM62X_DEV_MCASP1_MCASP_AHCLKR_POUT,
};
static const struct clk_data_from_dev clk_data_mcasp_main_1_mcasp_ahclkx_pout = {
	.dev		= AM62X_DEV_MCASP1,
	.clk_idx	= AM62X_DEV_MCASP1_MCASP_AHCLKX_POUT,
};
static const struct clk_data_from_dev clk_data_mcasp_main_2_mcasp_aclkr_pout = {
	.dev		= AM62X_DEV_MCASP2,
	.clk_idx	= AM62X_DEV_MCASP2_MCASP_ACLKR_POUT,
};
static const struct clk_data_from_dev clk_data_mcasp_main_2_mcasp_aclkx_pout = {
	.dev		= AM62X_DEV_MCASP2,
	.clk_idx	= AM62X_DEV_MCASP2_MCASP_ACLKX_POUT,
};
static const struct clk_data_from_dev clk_data_mcasp_main_2_mcasp_afsr_pout = {
	.dev		= AM62X_DEV_MCASP2,
	.clk_idx	= AM62X_DEV_MCASP2_MCASP_AFSR_POUT,
};
static const struct clk_data_from_dev clk_data_mcasp_main_2_mcasp_afsx_pout = {
	.dev		= AM62X_DEV_MCASP2,
	.clk_idx	= AM62X_DEV_MCASP2_MCASP_AFSX_POUT,
};
static const struct clk_data_from_dev clk_data_mcasp_main_2_mcasp_ahclkr_pout = {
	.dev		= AM62X_DEV_MCASP2,
	.clk_idx	= AM62X_DEV_MCASP2_MCASP_AHCLKR_POUT,
};
static const struct clk_data_from_dev clk_data_mcasp_main_2_mcasp_ahclkx_pout = {
	.dev		= AM62X_DEV_MCASP2,
	.clk_idx	= AM62X_DEV_MCASP2_MCASP_AHCLKX_POUT,
};
static const struct clk_data_div_reg_go clk_data_mcu_obsclk_div_out0 = {
	.data_div	= {
		.n	= 16,
	},
	.reg		= 0x04500000 + 32768,
	.bit		= 8,
	.go		= 16,
};
static const struct clk_data_from_dev clk_data_mshsi2c_main_0_porscl = {
	.dev		= AM62X_DEV_I2C0,
	.clk_idx	= AM62X_DEV_I2C0_PORSCL,
};
static const struct clk_data_from_dev clk_data_mshsi2c_main_1_porscl = {
	.dev		= AM62X_DEV_I2C1,
	.clk_idx	= AM62X_DEV_I2C1_PORSCL,
};
static const struct clk_data_from_dev clk_data_mshsi2c_main_2_porscl = {
	.dev		= AM62X_DEV_I2C2,
	.clk_idx	= AM62X_DEV_I2C2_PORSCL,
};
static const struct clk_data_from_dev clk_data_mshsi2c_main_3_porscl = {
	.dev		= AM62X_DEV_I2C3,
	.clk_idx	= AM62X_DEV_I2C3_PORSCL,
};
static const struct clk_data_from_dev clk_data_mshsi2c_mcu_0_porscl = {
	.dev		= AM62X_DEV_MCU_I2C0,
	.clk_idx	= AM62X_DEV_MCU_I2C0_PORSCL,
};
static const struct clk_data_from_dev clk_data_mshsi2c_wkup_0_porscl = {
	.dev		= AM62X_DEV_WKUP_I2C0,
	.clk_idx	= AM62X_DEV_WKUP_I2C0_PORSCL,
};
static const struct clk_data_div_reg_go clk_data_osbclk0_div_out0 = {
	.data_div	= {
		.n	= 256,
	},
	.reg		= 0x00100000 + 32768,
	.bit		= 8,
	.go		= 16,
};
static const struct clk_data_pll_16fft clk_data_pllfracf2_ssmod_16fft_main_0 = {
	.data_pll			= {
		.vco_range_idx		= AM62X_FREQ_RANGE_VCO_PLLFRACF2_SSMOD_16FFT_MAIN_0,
		.vco_in_range_idx	= AM62X_FREQ_RANGE_VCO_IN_PLLFRACF2_SSMOD_16FFT_MAIN_0,
		.fractional_support	= STRUE,
		.devgrp			= DEVGRP_00,
		.pll_entries		= pllfracf2_ssmod_16fft_main_0_entries,
		.default_freq_idx	= FREQ_PLLFRACF2_SSMOD_16FFT_MAIN_0_DEFAULT,
	},
	.idx				= 0,
	.base				= 0x00680000,
};
static const struct clk_data_div clk_data_pllfracf2_ssmod_16fft_main_0_postdiv = {
	.n	= 49,
};
static const struct clk_data_pll_16fft clk_data_pllfracf2_ssmod_16fft_main_1 = {
	.data_pll			= {
		.vco_range_idx		= AM62X_FREQ_RANGE_VCO_PLLFRACF2_SSMOD_16FFT_MAIN_0,
		.vco_in_range_idx	= AM62X_FREQ_RANGE_VCO_IN_PLLFRACF2_SSMOD_16FFT_MAIN_0,
		.fractional_support	= STRUE,
		.devgrp			= DEVGRP_00,
		.pll_entries		= pllfracf2_ssmod_16fft_main_1_entries,
		.default_freq_idx	= FREQ_PLLFRACF2_SSMOD_16FFT_MAIN_1_DEFAULT,
	},
	.idx				= 1,
	.base				= 0x00680000,
};
static const struct clk_data_div clk_data_pllfracf2_ssmod_16fft_main_1_postdiv = {
	.n	= 49,
};
static const struct clk_data_pll_16fft clk_data_pllfracf2_ssmod_16fft_main_12 = {
	.data_pll			= {
		.vco_range_idx		= AM62X_FREQ_RANGE_VCO_PLLFRACF2_SSMOD_16FFT_MAIN_0,
		.vco_in_range_idx	= AM62X_FREQ_RANGE_VCO_IN_PLLFRACF2_SSMOD_16FFT_MAIN_0,
		.fractional_support	= STRUE,
		.devgrp			= DEVGRP_00,
		.pll_entries		= pllfracf2_ssmod_16fft_main_12_entries,
		.default_freq_idx	= FREQ_PLLFRACF2_SSMOD_16FFT_MAIN_12_DEFAULT,
	},
	.idx				= 12,
	.base				= 0x00680000,
};
static const struct clk_data_pll_16fft clk_data_pllfracf2_ssmod_16fft_main_15 = {
	.data_pll			= {
		.vco_range_idx		= AM62X_FREQ_RANGE_VCO_PLLFRACF2_SSMOD_16FFT_MAIN_0,
		.vco_in_range_idx	= AM62X_FREQ_RANGE_VCO_IN_PLLFRACF2_SSMOD_16FFT_MAIN_0,
		.fractional_support	= STRUE,
		.devgrp			= DEVGRP_00,
		.pll_entries		= pllfracf2_ssmod_16fft_main_15_entries,
		.default_freq_idx	= FREQ_PLLFRACF2_SSMOD_16FFT_MAIN_15_DEFAULT,
	},
	.idx				= 15,
	.base				= 0x00680000,
};
static const struct clk_data_pll_16fft clk_data_pllfracf2_ssmod_16fft_main_16 = {
	.data_pll			= {
		.vco_range_idx		= AM62X_FREQ_RANGE_VCO_PLLFRACF2_SSMOD_16FFT_MAIN_0,
		.vco_in_range_idx	= AM62X_FREQ_RANGE_VCO_IN_PLLFRACF2_SSMOD_16FFT_MAIN_0,
		.fractional_support	= STRUE,
		.devgrp			= DEVGRP_00,
		.pll_entries		= pllfracf2_ssmod_16fft_main_16_entries,
		.default_freq_idx	= FREQ_PLLFRACF2_SSMOD_16FFT_MAIN_16_DEFAULT,
	},
	.idx				= 16,
	.base				= 0x00680000,
};
static const struct clk_data_pll_16fft clk_data_pllfracf2_ssmod_16fft_main_17 = {
	.data_pll			= {
		.vco_range_idx		= AM62X_FREQ_RANGE_VCO_PLLFRACF2_SSMOD_16FFT_MAIN_0,
		.vco_in_range_idx	= AM62X_FREQ_RANGE_VCO_IN_PLLFRACF2_SSMOD_16FFT_MAIN_0,
		.fractional_support	= STRUE,
		.devgrp			= DEVGRP_00,
		.pll_entries		= pllfracf2_ssmod_16fft_main_17_entries,
		.default_freq_idx	= FREQ_PLLFRACF2_SSMOD_16FFT_MAIN_17_DEFAULT,
	},
	.idx				= 17,
	.base				= 0x00680000,
};
static const struct clk_data_pll_16fft clk_data_pllfracf2_ssmod_16fft_main_2 = {
	.data_pll			= {
		.vco_range_idx		= AM62X_FREQ_RANGE_VCO_PLLFRACF2_SSMOD_16FFT_MAIN_0,
		.vco_in_range_idx	= AM62X_FREQ_RANGE_VCO_IN_PLLFRACF2_SSMOD_16FFT_MAIN_0,
		.fractional_support	= STRUE,
		.devgrp			= DEVGRP_00,
		.pll_entries		= pllfracf2_ssmod_16fft_main_0_entries,
		.default_freq_idx	= FREQ_PLLFRACF2_SSMOD_16FFT_MAIN_0_DEFAULT,
	},
	.idx				= 2,
	.base				= 0x00680000,
};
static const struct clk_data_div clk_data_pllfracf2_ssmod_16fft_main_2_postdiv = {
	.n	= 49,
};
static const struct clk_data_pll_16fft clk_data_pllfracf2_ssmod_16fft_main_8 = {
	.data_pll			= {
		.vco_range_idx		= AM62X_FREQ_RANGE_VCO_PLLFRACF2_SSMOD_16FFT_MAIN_0,
		.vco_in_range_idx	= AM62X_FREQ_RANGE_VCO_IN_PLLFRACF2_SSMOD_16FFT_MAIN_0,
		.fractional_support	= STRUE,
		.devgrp			= DEVGRP_00,
		.pll_entries		= pllfracf2_ssmod_16fft_main_8_entries,
		.default_freq_idx	= FREQ_PLLFRACF2_SSMOD_16FFT_MAIN_8_DEFAULT,
	},
	.idx				= 8,
	.base				= 0x00680000,
};
static const struct clk_data_pll_16fft clk_data_pllfracf2_ssmod_16fft_mcu_0 = {
	.data_pll			= {
		.vco_range_idx		= AM62X_FREQ_RANGE_VCO_PLLFRACF2_SSMOD_16FFT_MAIN_0,
		.vco_in_range_idx	= AM62X_FREQ_RANGE_VCO_IN_PLLFRACF2_SSMOD_16FFT_MAIN_0,
		.fractional_support	= STRUE,
		.devgrp			= DEVGRP_01,
		.pll_entries		= pllfracf2_ssmod_16fft_mcu_0_entries,
		.default_freq_idx	= FREQ_PLLFRACF2_SSMOD_16FFT_MCU_0_DEFAULT,
	},
	.idx				= 0,
	.base				= 0x04040000,
};
static const struct clk_data_div_reg clk_data_postdiv1_16fft_main_1_hsdiv5 = {
	.data_div		= {
		.n		= 128,
		.default_div	= 6,
	},
	.reg			= 0x00680000UL + (0x1000UL * 1UL) + 0x80UL + (0x4UL * 5UL),
	.bit			= 0,
};
static const struct clk_data_div_reg clk_data_postdiv1_16fft_main_1_hsdiv6 = {
	.data_div		= {
		.n		= 128,
		.default_div	= 10,
	},
	.reg			= 0x00680000UL + (0x1000UL * 1UL) + 0x80UL + (0x4UL * 6UL),
	.bit			= 0,
};
static const struct clk_data_div_reg clk_data_postdiv4_16ff_main_0_hsdiv5 = {
	.data_div		= {
		.n		= 128,
		.default_div	= 5,
	},
	.reg			= 0x00680000UL + (0x1000UL * 0UL) + 0x80UL + (0x4UL * 5UL),
	.bit			= 0,
};
static const struct clk_data_div_reg clk_data_postdiv4_16ff_main_0_hsdiv6 = {
	.data_div		= {
		.n		= 128,
		.default_div	= 5,
	},
	.reg			= 0x00680000UL + (0x1000UL * 0UL) + 0x80UL + (0x4UL * 6UL),
	.bit			= 0,
};
static const struct clk_data_div_reg clk_data_postdiv4_16ff_main_0_hsdiv7 = {
	.data_div		= {
		.n		= 128,
		.default_div	= 5,
	},
	.reg			= 0x00680000UL + (0x1000UL * 0UL) + 0x80UL + (0x4UL * 7UL),
	.bit			= 0,
};
static const struct clk_data_div_reg clk_data_postdiv4_16ff_main_0_hsdiv8 = {
	.data_div		= {
		.n		= 128,
		.default_div	= 20,
	},
	.reg			= 0x00680000UL + (0x1000UL * 0UL) + 0x80UL + (0x4UL * 8UL),
	.bit			= 0,
};
static const struct clk_data_div_reg clk_data_postdiv4_16ff_main_0_hsdiv9 = {
	.data_div		= {
		.n		= 128,
		.default_div	= 3,
	},
	.reg			= 0x00680000UL + (0x1000UL * 0UL) + 0x80UL + (0x4UL * 9UL),
	.bit			= 0,
};
static const struct clk_data_div_reg clk_data_postdiv4_16ff_main_2_hsdiv5 = {
	.data_div		= {
		.n		= 128,
		.default_div	= 4,
	},
	.reg			= 0x00680000UL + (0x1000UL * 2UL) + 0x80UL + (0x4UL * 5UL),
	.bit			= 0,
};
static const struct clk_data_div_reg clk_data_postdiv4_16ff_main_2_hsdiv6 = {
	.data_div		= {
		.n		= 128,
		.default_div	= 4,
	},
	.reg			= 0x00680000UL + (0x1000UL * 2UL) + 0x80UL + (0x4UL * 6UL),
	.bit			= 0,
};
static const struct clk_data_div_reg clk_data_postdiv4_16ff_main_2_hsdiv7 = {
	.data_div		= {
		.n		= 128,
		.default_div	= 10,
	},
	.reg			= 0x00680000UL + (0x1000UL * 2UL) + 0x80UL + (0x4UL * 7UL),
	.bit			= 0,
};
static const struct clk_data_div_reg clk_data_postdiv4_16ff_main_2_hsdiv8 = {
	.data_div		= {
		.n		= 128,
		.default_div	= 10,
	},
	.reg			= 0x00680000UL + (0x1000UL * 2UL) + 0x80UL + (0x4UL * 8UL),
	.bit			= 0,
};
static const struct clk_data_div_reg clk_data_postdiv4_16ff_main_2_hsdiv9 = {
	.data_div		= {
		.n		= 128,
		.default_div	= 20,
	},
	.reg			= 0x00680000UL + (0x1000UL * 2UL) + 0x80UL + (0x4UL * 9UL),
	.bit			= 0,
};
static const struct clk_data_from_dev clk_data_sam62_a53_512kb_wrap_main_0_arm_corepack_0_a53_divh_clk4_obsclk_out_clk = {
	.dev		= AM62X_DEV_A53SS0,
	.clk_idx	= AM62X_DEV_A53SS0_A53_DIVH_CLK4_OBSCLK_OUT_CLK,
};
static const struct clk_parent clk_sam62_pll_ctrl_wrap_main_0_parents[] = {
	{
		CLK_AM62X_GLUELOGIC_HFOSC0_CLKOUT,
		1,
	},
	{
		CLK_AM62X_HSDIV4_16FFT_MAIN_0_HSDIVOUT0_CLK,
		1,
	},
};
static const struct clk_data_mux_reg clk_data_sam62_pll_ctrl_wrap_main_0_sysclkout_clk = {
	.data_mux		= {
		.parents	= clk_sam62_pll_ctrl_wrap_main_0_parents,
		.n		= ARRAY_SIZE(clk_sam62_pll_ctrl_wrap_main_0_parents),
	},
	.reg			= 0x00410000,
};
static const struct clk_data_div_reg clk_data_sam62_pll_ctrl_wrap_main_0_chip_div1_clk_clk = {
	.data_div	= {
		.n	= 32,
	},
	.reg		= 0x00410000 + 0x118,
	.bit		= 0,
};
static const struct clk_data_div_reg clk_data_sam62_pll_ctrl_wrap_main_0_chip_div24_clk_clk = {
	.data_div	= {
		.n	= 32,
	},
	.reg		= 0x00410000 + 0x11c,
	.bit		= 0,
};
static const struct clk_parent clk_sam62_pll_ctrl_wrap_mcu_0_parents[] = {
	{
		CLK_AM62X_GLUELOGIC_HFOSC0_CLKOUT,
		1,
	},
	{
		CLK_AM62X_HSDIV4_16FFT_MCU_0_HSDIVOUT0_CLK,
		1,
	},
};
static const struct clk_data_mux_reg clk_data_sam62_pll_ctrl_wrap_mcu_0_sysclkout_clk = {
	.data_mux		= {
		.parents	= clk_sam62_pll_ctrl_wrap_mcu_0_parents,
		.n		= ARRAY_SIZE(clk_sam62_pll_ctrl_wrap_mcu_0_parents),
	},
	.reg			= 0x04020000,
};
static const struct clk_data_div_reg clk_data_sam62_pll_ctrl_wrap_mcu_0_chip_div1_clk_clk = {
	.data_div	= {
		.n	= 32,
	},
	.reg		= 0x04020000 + 0x118,
	.bit		= 0,
};
static const struct clk_data_div_reg clk_data_sam62_pll_ctrl_wrap_mcu_0_chip_div24_clk_clk = {
	.data_div	= {
		.n	= 32,
	},
	.reg		= 0x04020000 + 0x11c,
	.bit		= 0,
};
static const struct clk_data_from_dev clk_data_spi_main_0_io_clkspio_clk = {
	.dev		= AM62X_DEV_MCSPI0,
	.clk_idx	= AM62X_DEV_MCSPI0_IO_CLKSPIO_CLK,
};
static const struct clk_data_from_dev clk_data_spi_main_1_io_clkspio_clk = {
	.dev		= AM62X_DEV_MCSPI1,
	.clk_idx	= AM62X_DEV_MCSPI1_IO_CLKSPIO_CLK,
};
static const struct clk_data_from_dev clk_data_spi_main_2_io_clkspio_clk = {
	.dev		= AM62X_DEV_MCSPI2,
	.clk_idx	= AM62X_DEV_MCSPI2_IO_CLKSPIO_CLK,
};
static const struct clk_data_from_dev clk_data_spi_mcu_0_io_clkspio_clk = {
	.dev		= AM62X_DEV_MCU_MCSPI0,
	.clk_idx	= AM62X_DEV_MCU_MCSPI0_IO_CLKSPIO_CLK,
};
static const struct clk_data_from_dev clk_data_spi_mcu_1_io_clkspio_clk = {
	.dev		= AM62X_DEV_MCU_MCSPI1,
	.clk_idx	= AM62X_DEV_MCU_MCSPI1_IO_CLKSPIO_CLK,
};

const struct clk_data soc_clock_data[] = {
	[CLK_AM62X_GLUELOGIC_HFOSC0_CLKOUT] =						      {
		.drv	= &clk_drv_soc_hfosc0,
		.flags	= 0,
	},
	[CLK_AM62X_GLUELOGIC_LFOSC0_CLKOUT] =						      {
		.drv	= &clk_drv_soc_lfosc0,
		.flags	= 0,
	},
	[CLK_AM62X_GLUELOGIC_RCOSC_CLKOUT] =						      {
		.drv		= &clk_drv_fixed,
		.flags		= 0,
		.range_idx	= AM62X_FREQ_RANGE_GLUELOGIC_RCOSC_CLKOUT,
	},
	[CLK_AM62X_GLUELOGIC_RCOSC_CLK_1P0V_97P65K] =					      {
		.drv		= &clk_drv_fixed,
		.flags		= 0,
		.range_idx	= AM62X_FREQ_RANGE_GLUELOGIC_RCOSC_CLK_1P0V_97P65K,
	},
	[CLK_AM62X_K3_DPHY_RX_MAIN_0_IO_RX_CL_L_M] =					      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_K3_DPHY_RX_main_0_io_rx_cl_l_m.data,
		.freq_idx	= AM62X_FREQ_VALUE_K3_DPHY_RX_MAIN_0_IO_RX_CL_L_M,
	},
	[CLK_AM62X_K3_DPHY_RX_MAIN_0_IO_RX_CL_L_P] =					      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_K3_DPHY_RX_main_0_io_rx_cl_l_p.data,
		.freq_idx	= AM62X_FREQ_VALUE_K3_DPHY_RX_MAIN_0_IO_RX_CL_L_P,
	},
	[CLK_AM62X_K3_DPHY_RX_MAIN_0_PPI_RX_BYTE_CLK] =					      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_K3_DPHY_RX_main_0_ppi_rx_byte_clk.data,
		.freq_idx	= AM62X_FREQ_VALUE_K3_DPHY_RX_MAIN_0_PPI_RX_BYTE_CLK,
	},
	[CLK_AM62X_MAIN_OBSCLK0_MUX_SEL_DIV_CLKOUT] =					      {
		.parent =								      {
			CLK_AM62X_GLUELOGIC_RCOSC_CLKOUT,
			1,
		},
		.drv	= &clk_drv_div_fixed.drv,
		.flags	= 0,
		.data	= &clk_data_MAIN_OBSCLK0_MUX_SEL_div_in9.data,
		.type	= CLK_TYPE_DIV,
	},
	[CLK_AM62X_MAIN_WWDTCLKN_SEL_OUT0_DIV_CLKOUT] =					      {
		.parent =								      {
			CLK_AM62X_GLUELOGIC_RCOSC_CLKOUT,
			1,
		},
		.drv	= &clk_drv_div_fixed.drv,
		.flags	= 0,
		.data	= &clk_data_MAIN_WWDTCLKn_SEL_out0_div_in3.data,
		.type	= CLK_TYPE_DIV,
	},
	[CLK_AM62X_MAIN_WWDTCLKN_SEL_OUT1_DIV_CLKOUT] =					      {
		.parent =								      {
			CLK_AM62X_GLUELOGIC_RCOSC_CLKOUT,
			1,
		},
		.drv	= &clk_drv_div_fixed.drv,
		.flags	= 0,
		.data	= &clk_data_MAIN_WWDTCLKn_SEL_out1_div_in3.data,
		.type	= CLK_TYPE_DIV,
	},
	[CLK_AM62X_MAIN_WWDTCLKN_SEL_OUT2_DIV_CLKOUT] =					      {
		.parent =								      {
			CLK_AM62X_GLUELOGIC_RCOSC_CLKOUT,
			1,
		},
		.drv	= &clk_drv_div_fixed.drv,
		.flags	= 0,
		.data	= &clk_data_MAIN_WWDTCLKn_SEL_out2_div_in3.data,
		.type	= CLK_TYPE_DIV,
	},
	[CLK_AM62X_MAIN_WWDTCLKN_SEL_OUT3_DIV_CLKOUT] =					      {
		.parent =								      {
			CLK_AM62X_GLUELOGIC_RCOSC_CLKOUT,
			1,
		},
		.drv	= &clk_drv_div_fixed.drv,
		.flags	= 0,
		.data	= &clk_data_MAIN_WWDTCLKn_SEL_out3_div_in3.data,
		.type	= CLK_TYPE_DIV,
	},
	[CLK_AM62X_MAIN_WWDTCLKN_SEL_OUT4_DIV_CLKOUT] =					      {
		.parent =								      {
			CLK_AM62X_GLUELOGIC_RCOSC_CLKOUT,
			1,
		},
		.drv	= &clk_drv_div_fixed.drv,
		.flags	= 0,
		.data	= &clk_data_MAIN_WWDTCLKn_SEL_out4_div_in3.data,
		.type	= CLK_TYPE_DIV,
	},
	[CLK_AM62X_MCU_OBSCLK_MUX_SEL_DIV_CLKOUT] =					      {
		.parent =								      {
			CLK_AM62X_GLUELOGIC_RCOSC_CLKOUT,
			1,
		},
		.drv	= &clk_drv_div_fixed.drv,
		.flags	= 0,
		.data	= &clk_data_MCU_OBSCLK_MUX_SEL_div_in5.data,
		.type	= CLK_TYPE_DIV,
	},
	[CLK_AM62X_RTC_CLK_SEL_DIV_CLKOUT] =						      {
		.parent =								      {
			CLK_AM62X_GLUELOGIC_RCOSC_CLKOUT,
			1,
		},
		.drv	= &clk_drv_div_fixed.drv,
		.flags	= 0,
		.data	= &clk_data_RTC_CLK_SEL_div_in1.data,
		.type	= CLK_TYPE_DIV,
	},
	[CLK_AM62X_BOARD_0_AUDIO_EXT_REFCLK0_OUT] =					      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_board_0_AUDIO_EXT_REFCLK0_out.data,
		.freq_idx	= AM62X_FREQ_VALUE_BOARD_0_AUDIO_EXT_REFCLK0_OUT,
	},
	[CLK_AM62X_BOARD_0_AUDIO_EXT_REFCLK1_OUT] =					      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_board_0_AUDIO_EXT_REFCLK1_out.data,
		.freq_idx	= AM62X_FREQ_VALUE_BOARD_0_AUDIO_EXT_REFCLK1_OUT,
	},
	[CLK_AM62X_BOARD_0_CP_GEMAC_CPTS0_RFT_CLK_OUT] =				      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_board_0_CP_GEMAC_CPTS0_RFT_CLK_out.data,
		.freq_idx	= AM62X_FREQ_VALUE_BOARD_0_CP_GEMAC_CPTS0_RFT_CLK_OUT,
	},
	[CLK_AM62X_BOARD_0_DDR0_CK0_OUT] =						      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_board_0_DDR0_CK0_out.data,
		.freq_idx	= AM62X_FREQ_VALUE_BOARD_0_DDR0_CK0_OUT,
	},
	[CLK_AM62X_BOARD_0_EXT_REFCLK1_OUT] =						      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_board_0_EXT_REFCLK1_out.data,
		.freq_idx	= AM62X_FREQ_VALUE_BOARD_0_EXT_REFCLK1_OUT,
	},
	[CLK_AM62X_BOARD_0_GPMC0_CLKLB_OUT] =						      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_board_0_GPMC0_CLKLB_out.data,
		.freq_idx	= AM62X_FREQ_VALUE_BOARD_0_GPMC0_CLKLB_OUT,
	},
	[CLK_AM62X_BOARD_0_I2C0_SCL_OUT] =						      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_board_0_I2C0_SCL_out.data,
		.freq_idx	= AM62X_FREQ_VALUE_BOARD_0_I2C0_SCL_OUT,
	},
	[CLK_AM62X_BOARD_0_I2C1_SCL_OUT] =						      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_board_0_I2C1_SCL_out.data,
		.freq_idx	= AM62X_FREQ_VALUE_BOARD_0_I2C1_SCL_OUT,
	},
	[CLK_AM62X_BOARD_0_I2C2_SCL_OUT] =						      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_board_0_I2C2_SCL_out.data,
		.freq_idx	= AM62X_FREQ_VALUE_BOARD_0_I2C2_SCL_OUT,
	},
	[CLK_AM62X_BOARD_0_I2C3_SCL_OUT] =						      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_board_0_I2C3_SCL_out.data,
		.freq_idx	= AM62X_FREQ_VALUE_BOARD_0_I2C3_SCL_OUT,
	},
	[CLK_AM62X_BOARD_0_MCASP0_ACLKR_OUT] =						      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_board_0_MCASP0_ACLKR_out.data,
		.freq_idx	= AM62X_FREQ_VALUE_BOARD_0_MCASP0_ACLKR_OUT,
	},
	[CLK_AM62X_BOARD_0_MCASP0_ACLKX_OUT] =						      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_board_0_MCASP0_ACLKX_out.data,
		.freq_idx	= AM62X_FREQ_VALUE_BOARD_0_MCASP0_ACLKX_OUT,
	},
	[CLK_AM62X_BOARD_0_MCASP1_ACLKR_OUT] =						      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_board_0_MCASP1_ACLKR_out.data,
		.freq_idx	= AM62X_FREQ_VALUE_BOARD_0_MCASP1_ACLKR_OUT,
	},
	[CLK_AM62X_BOARD_0_MCASP1_ACLKX_OUT] =						      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_board_0_MCASP1_ACLKX_out.data,
		.freq_idx	= AM62X_FREQ_VALUE_BOARD_0_MCASP1_ACLKX_OUT,
	},
	[CLK_AM62X_BOARD_0_MCASP2_ACLKR_OUT] =						      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_board_0_MCASP2_ACLKR_out.data,
		.freq_idx	= AM62X_FREQ_VALUE_BOARD_0_MCASP2_ACLKR_OUT,
	},
	[CLK_AM62X_BOARD_0_MCASP2_ACLKX_OUT] =						      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_board_0_MCASP2_ACLKX_out.data,
		.freq_idx	= AM62X_FREQ_VALUE_BOARD_0_MCASP2_ACLKX_OUT,
	},
	[CLK_AM62X_BOARD_0_MCU_EXT_REFCLK0_OUT] =					      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_board_0_MCU_EXT_REFCLK0_out.data,
		.freq_idx	= AM62X_FREQ_VALUE_BOARD_0_MCU_EXT_REFCLK0_OUT,
	},
	[CLK_AM62X_BOARD_0_MCU_I2C0_SCL_OUT] =						      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_board_0_MCU_I2C0_SCL_out.data,
		.freq_idx	= AM62X_FREQ_VALUE_BOARD_0_MCU_I2C0_SCL_OUT,
	},
	[CLK_AM62X_BOARD_0_MCU_SPI0_CLK_OUT] =						      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_board_0_MCU_SPI0_CLK_out.data,
		.freq_idx	= AM62X_FREQ_VALUE_BOARD_0_MCU_SPI0_CLK_OUT,
	},
	[CLK_AM62X_BOARD_0_MCU_SPI1_CLK_OUT] =						      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_board_0_MCU_SPI1_CLK_out.data,
		.freq_idx	= AM62X_FREQ_VALUE_BOARD_0_MCU_SPI1_CLK_OUT,
	},
	[CLK_AM62X_BOARD_0_MMC0_CLKLB_OUT] =						      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_board_0_MMC0_CLKLB_out.data,
		.freq_idx	= AM62X_FREQ_VALUE_BOARD_0_MMC0_CLKLB_OUT,
	},
	[CLK_AM62X_BOARD_0_MMC0_CLK_OUT] =						      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_board_0_MMC0_CLK_out.data,
		.freq_idx	= AM62X_FREQ_VALUE_BOARD_0_MMC0_CLK_OUT,
	},
	[CLK_AM62X_BOARD_0_MMC1_CLKLB_OUT] =						      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_board_0_MMC1_CLKLB_out.data,
		.freq_idx	= AM62X_FREQ_VALUE_BOARD_0_MMC1_CLKLB_OUT,
	},
	[CLK_AM62X_BOARD_0_MMC1_CLK_OUT] =						      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_board_0_MMC1_CLK_out.data,
		.freq_idx	= AM62X_FREQ_VALUE_BOARD_0_MMC1_CLK_OUT,
	},
	[CLK_AM62X_BOARD_0_MMC2_CLKLB_OUT] =						      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_board_0_MMC2_CLKLB_out.data,
		.freq_idx	= AM62X_FREQ_VALUE_BOARD_0_MMC2_CLKLB_OUT,
	},
	[CLK_AM62X_BOARD_0_MMC2_CLK_OUT] =						      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_board_0_MMC2_CLK_out.data,
		.freq_idx	= AM62X_FREQ_VALUE_BOARD_0_MMC2_CLK_OUT,
	},
	[CLK_AM62X_BOARD_0_OSPI0_DQS_OUT] =						      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_board_0_OSPI0_DQS_out.data,
		.freq_idx	= AM62X_FREQ_VALUE_BOARD_0_OSPI0_DQS_OUT,
	},
	[CLK_AM62X_BOARD_0_OSPI0_LBCLKO_OUT] =						      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_board_0_OSPI0_LBCLKO_out.data,
		.freq_idx	= AM62X_FREQ_VALUE_BOARD_0_OSPI0_LBCLKO_OUT,
	},
	[CLK_AM62X_BOARD_0_RGMII1_RXC_OUT] =						      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_board_0_RGMII1_RXC_out.data,
		.freq_idx	= AM62X_FREQ_VALUE_BOARD_0_RGMII1_RXC_OUT,
	},
	[CLK_AM62X_BOARD_0_RGMII1_TXC_OUT] =						      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_board_0_RGMII1_TXC_out.data,
		.freq_idx	= AM62X_FREQ_VALUE_BOARD_0_RGMII1_TXC_OUT,
	},
	[CLK_AM62X_BOARD_0_RGMII2_RXC_OUT] =						      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_board_0_RGMII2_RXC_out.data,
		.freq_idx	= AM62X_FREQ_VALUE_BOARD_0_RGMII2_RXC_OUT,
	},
	[CLK_AM62X_BOARD_0_RGMII2_TXC_OUT] =						      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_board_0_RGMII2_TXC_out.data,
		.freq_idx	= AM62X_FREQ_VALUE_BOARD_0_RGMII2_TXC_OUT,
	},
	[CLK_AM62X_BOARD_0_RMII1_REF_CLK_OUT] =						      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_board_0_RMII1_REF_CLK_out.data,
		.freq_idx	= AM62X_FREQ_VALUE_BOARD_0_RMII1_REF_CLK_OUT,
	},
	[CLK_AM62X_BOARD_0_RMII2_REF_CLK_OUT] =						      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_board_0_RMII2_REF_CLK_out.data,
		.freq_idx	= AM62X_FREQ_VALUE_BOARD_0_RMII2_REF_CLK_OUT,
	},
	[CLK_AM62X_BOARD_0_SPI0_CLK_OUT] =						      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_board_0_SPI0_CLK_out.data,
		.freq_idx	= AM62X_FREQ_VALUE_BOARD_0_SPI0_CLK_OUT,
	},
	[CLK_AM62X_BOARD_0_SPI1_CLK_OUT] =						      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_board_0_SPI1_CLK_out.data,
		.freq_idx	= AM62X_FREQ_VALUE_BOARD_0_SPI1_CLK_OUT,
	},
	[CLK_AM62X_BOARD_0_SPI2_CLK_OUT] =						      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_board_0_SPI2_CLK_out.data,
		.freq_idx	= AM62X_FREQ_VALUE_BOARD_0_SPI2_CLK_OUT,
	},
	[CLK_AM62X_BOARD_0_TCK_OUT] =							      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_board_0_TCK_out.data,
		.freq_idx	= AM62X_FREQ_VALUE_BOARD_0_TCK_OUT,
	},
	[CLK_AM62X_BOARD_0_VOUT0_EXTPCLKIN_OUT] =					      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_board_0_VOUT0_EXTPCLKIN_out.data,
		.freq_idx	= AM62X_FREQ_VALUE_BOARD_0_VOUT0_EXTPCLKIN_OUT,
	},
	[CLK_AM62X_CPSW_3GUSS_MAIN_0_CPTS_GENF0] =					      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_cpsw_3guss_main_0_cpts_genf0.data,
		.freq_idx	= AM62X_FREQ_VALUE_CPSW_3GUSS_MAIN_0_CPTS_GENF0,
	},
	[CLK_AM62X_CPSW_3GUSS_MAIN_0_CPTS_GENF1] =					      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_cpsw_3guss_main_0_cpts_genf1.data,
		.freq_idx	= AM62X_FREQ_VALUE_CPSW_3GUSS_MAIN_0_CPTS_GENF1,
	},
	[CLK_AM62X_CPSW_3GUSS_MAIN_0_MDIO_MDCLK_O] =					      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_cpsw_3guss_main_0_mdio_mdclk_o.data,
		.freq_idx	= AM62X_FREQ_VALUE_CPSW_3GUSS_MAIN_0_MDIO_MDCLK_O,
	},
	[CLK_AM62X_CPSW_3GUSS_MAIN_0_RGMII1_TXC_O] =					      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_cpsw_3guss_main_0_rgmii1_txc_o.data,
		.freq_idx	= AM62X_FREQ_VALUE_CPSW_3GUSS_MAIN_0_RGMII1_TXC_O,
	},
	[CLK_AM62X_CPSW_3GUSS_MAIN_0_RGMII2_TXC_O] =					      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_cpsw_3guss_main_0_rgmii2_txc_o.data,
		.freq_idx	= AM62X_FREQ_VALUE_CPSW_3GUSS_MAIN_0_RGMII2_TXC_O,
	},
	[CLK_AM62X_DEBUGSS_K3_WRAP_CV0_MAIN_0_CSTPIU_TRACECLK] =			      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_debugss_k3_wrap_cv0_main_0_cstpiu_traceclk.data,
		.freq_idx	= AM62X_FREQ_VALUE_DEBUGSS_K3_WRAP_CV0_MAIN_0_CSTPIU_TRACECLK,
	},
	[CLK_AM62X_DMTIMER_DMC1MS_MAIN_0_TIMER_PWM] =					      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_dmtimer_dmc1ms_main_0_timer_pwm.data,
		.freq_idx	= AM62X_FREQ_VALUE_DMTIMER_DMC1MS_MAIN_0_TIMER_PWM,
	},
	[CLK_AM62X_DMTIMER_DMC1MS_MAIN_1_TIMER_PWM] =					      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_dmtimer_dmc1ms_main_1_timer_pwm.data,
		.freq_idx	= AM62X_FREQ_VALUE_DMTIMER_DMC1MS_MAIN_1_TIMER_PWM,
	},
	[CLK_AM62X_DMTIMER_DMC1MS_MAIN_2_TIMER_PWM] =					      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_dmtimer_dmc1ms_main_2_timer_pwm.data,
		.freq_idx	= AM62X_FREQ_VALUE_DMTIMER_DMC1MS_MAIN_2_TIMER_PWM,
	},
	[CLK_AM62X_DMTIMER_DMC1MS_MAIN_3_TIMER_PWM] =					      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_dmtimer_dmc1ms_main_3_timer_pwm.data,
		.freq_idx	= AM62X_FREQ_VALUE_DMTIMER_DMC1MS_MAIN_3_TIMER_PWM,
	},
	[CLK_AM62X_DMTIMER_DMC1MS_MAIN_4_TIMER_PWM] =					      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_dmtimer_dmc1ms_main_4_timer_pwm.data,
		.freq_idx	= AM62X_FREQ_VALUE_DMTIMER_DMC1MS_MAIN_4_TIMER_PWM,
	},
	[CLK_AM62X_DMTIMER_DMC1MS_MAIN_5_TIMER_PWM] =					      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_dmtimer_dmc1ms_main_5_timer_pwm.data,
		.freq_idx	= AM62X_FREQ_VALUE_DMTIMER_DMC1MS_MAIN_5_TIMER_PWM,
	},
	[CLK_AM62X_DMTIMER_DMC1MS_MAIN_6_TIMER_PWM] =					      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_dmtimer_dmc1ms_main_6_timer_pwm.data,
		.freq_idx	= AM62X_FREQ_VALUE_DMTIMER_DMC1MS_MAIN_6_TIMER_PWM,
	},
	[CLK_AM62X_DMTIMER_DMC1MS_MAIN_7_TIMER_PWM] =					      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_dmtimer_dmc1ms_main_7_timer_pwm.data,
		.freq_idx	= AM62X_FREQ_VALUE_DMTIMER_DMC1MS_MAIN_7_TIMER_PWM,
	},
	[CLK_AM62X_DMTIMER_DMC1MS_MCU_0_TIMER_PWM] =					      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_dmtimer_dmc1ms_mcu_0_timer_pwm.data,
		.freq_idx	= AM62X_FREQ_VALUE_DMTIMER_DMC1MS_MCU_0_TIMER_PWM,
	},
	[CLK_AM62X_DMTIMER_DMC1MS_MCU_1_TIMER_PWM] =					      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_dmtimer_dmc1ms_mcu_1_timer_pwm.data,
		.freq_idx	= AM62X_FREQ_VALUE_DMTIMER_DMC1MS_MCU_1_TIMER_PWM,
	},
	[CLK_AM62X_DMTIMER_DMC1MS_MCU_2_TIMER_PWM] =					      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_dmtimer_dmc1ms_mcu_2_timer_pwm.data,
		.freq_idx	= AM62X_FREQ_VALUE_DMTIMER_DMC1MS_MCU_2_TIMER_PWM,
	},
	[CLK_AM62X_DMTIMER_DMC1MS_MCU_3_TIMER_PWM] =					      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_dmtimer_dmc1ms_mcu_3_timer_pwm.data,
		.freq_idx	= AM62X_FREQ_VALUE_DMTIMER_DMC1MS_MCU_3_TIMER_PWM,
	},
	[CLK_AM62X_EMMCSD4SS_MAIN_0_EMMCSDSS_IO_CLK_O] =				      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_emmcsd4ss_main_0_emmcsdss_io_clk_o.data,
		.freq_idx	= AM62X_FREQ_VALUE_EMMCSD4SS_MAIN_0_EMMCSDSS_IO_CLK_O,
	},
	[CLK_AM62X_EMMCSD4SS_MAIN_1_EMMCSDSS_IO_CLK_O] =				      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_emmcsd4ss_main_1_emmcsdss_io_clk_o.data,
		.freq_idx	= AM62X_FREQ_VALUE_EMMCSD4SS_MAIN_1_EMMCSDSS_IO_CLK_O,
	},
	[CLK_AM62X_EMMCSD8SS_MAIN_0_EMMCSDSS_IO_CLK_O] =				      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_emmcsd8ss_main_0_emmcsdss_io_clk_o.data,
		.freq_idx	= AM62X_FREQ_VALUE_EMMCSD8SS_MAIN_0_EMMCSDSS_IO_CLK_O,
	},
	[CLK_AM62X_FSS_UL_MAIN_0_OSPI_0_OSPI_OCLK_CLK] =				      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_fss_ul_main_0_ospi_0_ospi_oclk_clk.data,
		.freq_idx	= AM62X_FREQ_VALUE_FSS_UL_MAIN_0_OSPI_0_OSPI_OCLK_CLK,
	},
	[CLK_AM62X_GPMC_MAIN_0_PO_GPMC_DEV_CLK] =					      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_gpmc_main_0_po_gpmc_dev_clk.data,
		.freq_idx	= AM62X_FREQ_VALUE_GPMC_MAIN_0_PO_GPMC_DEV_CLK,
	},
	[CLK_AM62X_HSDIV0_16FFT_MCU_32KHZ_GEN_0_HSDIVOUT0_CLK] =			      {
		.parent =								      {
			CLK_AM62X_GLUELOGIC_HFOSC0_CLKOUT,
			1,
		},
		.drv	= &clk_drv_div_reg.drv,
		.flags	= 0,
		.type	= CLK_TYPE_DIV,
		.data	= &clk_data_hsdiv0_16fft_mcu_32khz_gen_0_hsdiv0.data_div.data,
	},
	[CLK_AM62X_K3_DSS_UL_MAIN_0_DPI_1_OUT_CLK] =					      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_k3_dss_ul_main_0_dpi_1_out_clk.data,
		.freq_idx	= AM62X_FREQ_VALUE_K3_DSS_UL_MAIN_0_DPI_1_OUT_CLK,
	},
	[CLK_AM62X_MCASP_MAIN_0_MCASP_ACLKR_POUT] =					      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_mcasp_main_0_mcasp_aclkr_pout.data,
		.freq_idx	= AM62X_FREQ_VALUE_MCASP_MAIN_0_MCASP_ACLKR_POUT,
	},
	[CLK_AM62X_MCASP_MAIN_0_MCASP_ACLKX_POUT] =					      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_mcasp_main_0_mcasp_aclkx_pout.data,
		.freq_idx	= AM62X_FREQ_VALUE_MCASP_MAIN_0_MCASP_ACLKX_POUT,
	},
	[CLK_AM62X_MCASP_MAIN_0_MCASP_AFSR_POUT] =					      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_mcasp_main_0_mcasp_afsr_pout.data,
		.freq_idx	= AM62X_FREQ_VALUE_MCASP_MAIN_0_MCASP_AFSR_POUT,
	},
	[CLK_AM62X_MCASP_MAIN_0_MCASP_AFSX_POUT] =					      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_mcasp_main_0_mcasp_afsx_pout.data,
		.freq_idx	= AM62X_FREQ_VALUE_MCASP_MAIN_0_MCASP_AFSX_POUT,
	},
	[CLK_AM62X_MCASP_MAIN_0_MCASP_AHCLKR_POUT] =					      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_mcasp_main_0_mcasp_ahclkr_pout.data,
		.freq_idx	= AM62X_FREQ_VALUE_MCASP_MAIN_0_MCASP_AHCLKR_POUT,
	},
	[CLK_AM62X_MCASP_MAIN_0_MCASP_AHCLKX_POUT] =					      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_mcasp_main_0_mcasp_ahclkx_pout.data,
		.freq_idx	= AM62X_FREQ_VALUE_MCASP_MAIN_0_MCASP_AHCLKX_POUT,
	},
	[CLK_AM62X_MCASP_MAIN_1_MCASP_ACLKR_POUT] =					      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_mcasp_main_1_mcasp_aclkr_pout.data,
		.freq_idx	= AM62X_FREQ_VALUE_MCASP_MAIN_1_MCASP_ACLKR_POUT,
	},
	[CLK_AM62X_MCASP_MAIN_1_MCASP_ACLKX_POUT] =					      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_mcasp_main_1_mcasp_aclkx_pout.data,
		.freq_idx	= AM62X_FREQ_VALUE_MCASP_MAIN_1_MCASP_ACLKX_POUT,
	},
	[CLK_AM62X_MCASP_MAIN_1_MCASP_AFSR_POUT] =					      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_mcasp_main_1_mcasp_afsr_pout.data,
		.freq_idx	= AM62X_FREQ_VALUE_MCASP_MAIN_1_MCASP_AFSR_POUT,
	},
	[CLK_AM62X_MCASP_MAIN_1_MCASP_AFSX_POUT] =					      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_mcasp_main_1_mcasp_afsx_pout.data,
		.freq_idx	= AM62X_FREQ_VALUE_MCASP_MAIN_1_MCASP_AFSX_POUT,
	},
	[CLK_AM62X_MCASP_MAIN_1_MCASP_AHCLKR_POUT] =					      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_mcasp_main_1_mcasp_ahclkr_pout.data,
		.freq_idx	= AM62X_FREQ_VALUE_MCASP_MAIN_1_MCASP_AHCLKR_POUT,
	},
	[CLK_AM62X_MCASP_MAIN_1_MCASP_AHCLKX_POUT] =					      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_mcasp_main_1_mcasp_ahclkx_pout.data,
		.freq_idx	= AM62X_FREQ_VALUE_MCASP_MAIN_1_MCASP_AHCLKX_POUT,
	},
	[CLK_AM62X_MCASP_MAIN_2_MCASP_ACLKR_POUT] =					      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_mcasp_main_2_mcasp_aclkr_pout.data,
		.freq_idx	= AM62X_FREQ_VALUE_MCASP_MAIN_2_MCASP_ACLKR_POUT,
	},
	[CLK_AM62X_MCASP_MAIN_2_MCASP_ACLKX_POUT] =					      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_mcasp_main_2_mcasp_aclkx_pout.data,
		.freq_idx	= AM62X_FREQ_VALUE_MCASP_MAIN_2_MCASP_ACLKX_POUT,
	},
	[CLK_AM62X_MCASP_MAIN_2_MCASP_AFSR_POUT] =					      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_mcasp_main_2_mcasp_afsr_pout.data,
		.freq_idx	= AM62X_FREQ_VALUE_MCASP_MAIN_2_MCASP_AFSR_POUT,
	},
	[CLK_AM62X_MCASP_MAIN_2_MCASP_AFSX_POUT] =					      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_mcasp_main_2_mcasp_afsx_pout.data,
		.freq_idx	= AM62X_FREQ_VALUE_MCASP_MAIN_2_MCASP_AFSX_POUT,
	},
	[CLK_AM62X_MCASP_MAIN_2_MCASP_AHCLKR_POUT] =					      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_mcasp_main_2_mcasp_ahclkr_pout.data,
		.freq_idx	= AM62X_FREQ_VALUE_MCASP_MAIN_2_MCASP_AHCLKR_POUT,
	},
	[CLK_AM62X_MCASP_MAIN_2_MCASP_AHCLKX_POUT] =					      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_mcasp_main_2_mcasp_ahclkx_pout.data,
		.freq_idx	= AM62X_FREQ_VALUE_MCASP_MAIN_2_MCASP_AHCLKX_POUT,
	},
	[CLK_AM62X_MSHSI2C_MAIN_0_PORSCL] =						      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_mshsi2c_main_0_porscl.data,
		.freq_idx	= AM62X_FREQ_VALUE_MSHSI2C_MAIN_0_PORSCL,
	},
	[CLK_AM62X_MSHSI2C_MAIN_1_PORSCL] =						      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_mshsi2c_main_1_porscl.data,
		.freq_idx	= AM62X_FREQ_VALUE_MSHSI2C_MAIN_1_PORSCL,
	},
	[CLK_AM62X_MSHSI2C_MAIN_2_PORSCL] =						      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_mshsi2c_main_2_porscl.data,
		.freq_idx	= AM62X_FREQ_VALUE_MSHSI2C_MAIN_2_PORSCL,
	},
	[CLK_AM62X_MSHSI2C_MAIN_3_PORSCL] =						      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_mshsi2c_main_3_porscl.data,
		.freq_idx	= AM62X_FREQ_VALUE_MSHSI2C_MAIN_3_PORSCL,
	},
	[CLK_AM62X_MSHSI2C_MCU_0_PORSCL] =						      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_mshsi2c_mcu_0_porscl.data,
		.freq_idx	= AM62X_FREQ_VALUE_MSHSI2C_MCU_0_PORSCL,
	},
	[CLK_AM62X_PLLFRACF2_SSMOD_16FFT_MAIN_0_FOUTVCOP_CLK] =				      {
		.parent		=							      {
			CLK_AM62X_GLUELOGIC_HFOSC0_CLKOUT,
			1,
		},
		.drv		= &clk_drv_pll_16fft,
		.freq_idx	= AM62X_FREQ_VALUE_PLLFRACF2_SSMOD_16FFT_MAIN_0,
		.data		= &clk_data_pllfracf2_ssmod_16fft_main_0.data_pll.data,
		.flags		= 0,
	},
	[CLK_AM62X_PLLFRACF2_SSMOD_16FFT_MAIN_0_FOUTPOSTDIV_CLK] =			      {
		.parent =								      {
			CLK_AM62X_PLLFRACF2_SSMOD_16FFT_MAIN_0_FOUTVCOP_CLK,
			1,
		},
		.drv	= &clk_drv_div_pll_16fft_postdiv.drv,
		.type	= CLK_TYPE_DIV,
		.data	= &clk_data_pllfracf2_ssmod_16fft_main_0_postdiv.data,
		.flags	= 0,
	},
	[CLK_AM62X_PLLFRACF2_SSMOD_16FFT_MAIN_1_FOUTVCOP_CLK] =				      {
		.parent		=							      {
			CLK_AM62X_GLUELOGIC_HFOSC0_CLKOUT,
			1,
		},
		.drv		= &clk_drv_pll_16fft,
		.freq_idx	= AM62X_FREQ_VALUE_PLLFRACF2_SSMOD_16FFT_MAIN_1,
		.data		= &clk_data_pllfracf2_ssmod_16fft_main_1.data_pll.data,
		.flags		= 0,
	},
	[CLK_AM62X_PLLFRACF2_SSMOD_16FFT_MAIN_1_FOUTPOSTDIV_CLK] =			      {
		.parent =								      {
			CLK_AM62X_PLLFRACF2_SSMOD_16FFT_MAIN_1_FOUTVCOP_CLK,
			1,
		},
		.drv	= &clk_drv_div_pll_16fft_postdiv.drv,
		.type	= CLK_TYPE_DIV,
		.data	= &clk_data_pllfracf2_ssmod_16fft_main_1_postdiv.data,
		.flags	= 0,
	},
	[CLK_AM62X_PLLFRACF2_SSMOD_16FFT_MAIN_12_FOUTVCOP_CLK] =			      {
		.parent		=							      {
			CLK_AM62X_GLUELOGIC_HFOSC0_CLKOUT,
			1,
		},
		.drv		= &clk_drv_pll_16fft,
		.freq_idx	= AM62X_FREQ_VALUE_PLLFRACF2_SSMOD_16FFT_MAIN_12,
		.data		= &clk_data_pllfracf2_ssmod_16fft_main_12.data_pll.data,
		.flags		= CLK_DATA_FLAG_NO_HW_REINIT,
	},
	[CLK_AM62X_PLLFRACF2_SSMOD_16FFT_MAIN_15_FOUTVCOP_CLK] =			      {
		.parent		=							      {
			CLK_AM62X_GLUELOGIC_HFOSC0_CLKOUT,
			1,
		},
		.drv		= &clk_drv_pll_16fft,
		.freq_idx	= AM62X_FREQ_VALUE_PLLFRACF2_SSMOD_16FFT_MAIN_15,
		.data		= &clk_data_pllfracf2_ssmod_16fft_main_15.data_pll.data,
		.flags		= 0,
	},
	[CLK_AM62X_PLLFRACF2_SSMOD_16FFT_MAIN_16_FOUTVCOP_CLK] =			      {
		.parent		=							      {
			CLK_AM62X_GLUELOGIC_HFOSC0_CLKOUT,
			1,
		},
		.drv		= &clk_drv_pll_16fft,
		.freq_idx	= AM62X_FREQ_VALUE_PLLFRACF2_SSMOD_16FFT_MAIN_16,
		.data		= &clk_data_pllfracf2_ssmod_16fft_main_16.data_pll.data,
		.flags		= 0,
	},
	[CLK_AM62X_PLLFRACF2_SSMOD_16FFT_MAIN_17_FOUTVCOP_CLK] =			      {
		.parent		=							      {
			CLK_AM62X_GLUELOGIC_HFOSC0_CLKOUT,
			1,
		},
		.drv		= &clk_drv_pll_16fft,
		.freq_idx	= AM62X_FREQ_VALUE_PLLFRACF2_SSMOD_16FFT_MAIN_17,
		.data		= &clk_data_pllfracf2_ssmod_16fft_main_17.data_pll.data,
		.flags		= 0,
	},
	[CLK_AM62X_PLLFRACF2_SSMOD_16FFT_MAIN_2_FOUTVCOP_CLK] =				      {
		.parent		=							      {
			CLK_AM62X_GLUELOGIC_HFOSC0_CLKOUT,
			1,
		},
		.drv		= &clk_drv_pll_16fft,
		.freq_idx	= AM62X_FREQ_VALUE_PLLFRACF2_SSMOD_16FFT_MAIN_2,
		.data		= &clk_data_pllfracf2_ssmod_16fft_main_2.data_pll.data,
		.flags		= 0,
	},
	[CLK_AM62X_PLLFRACF2_SSMOD_16FFT_MAIN_2_FOUTPOSTDIV_CLK] =			      {
		.parent =								      {
			CLK_AM62X_PLLFRACF2_SSMOD_16FFT_MAIN_2_FOUTVCOP_CLK,
			1,
		},
		.drv	= &clk_drv_div_pll_16fft_postdiv.drv,
		.type	= CLK_TYPE_DIV,
		.data	= &clk_data_pllfracf2_ssmod_16fft_main_2_postdiv.data,
		.flags	= 0,
	},
	[CLK_AM62X_PLLFRACF2_SSMOD_16FFT_MAIN_8_FOUTVCOP_CLK] =				      {
		.parent		=							      {
			CLK_AM62X_GLUELOGIC_HFOSC0_CLKOUT,
			1,
		},
		.drv		= &clk_drv_pll_16fft,
		.freq_idx	= AM62X_FREQ_VALUE_PLLFRACF2_SSMOD_16FFT_MAIN_8,
		.data		= &clk_data_pllfracf2_ssmod_16fft_main_8.data_pll.data,
		.flags		= 0,
	},
	[CLK_AM62X_PLLFRACF2_SSMOD_16FFT_MCU_0_FOUTVCOP_CLK] =				      {
		.parent		=							      {
			CLK_AM62X_GLUELOGIC_HFOSC0_CLKOUT,
			1,
		},
		.drv		= &clk_drv_pll_16fft,
		.freq_idx	= AM62X_FREQ_VALUE_PLLFRACF2_SSMOD_16FFT_MCU_0,
		.data		= &clk_data_pllfracf2_ssmod_16fft_mcu_0.data_pll.data,
		.flags		= 0,
	},
	[CLK_AM62X_POSTDIV1_16FFT_MAIN_1_HSDIVOUT5_CLK] =				      {
		.parent =								      {
			CLK_AM62X_PLLFRACF2_SSMOD_16FFT_MAIN_1_FOUTPOSTDIV_CLK,
			1,
		},
		.drv	= &clk_drv_div_reg.drv,
		.flags	= 0,
		.type	= CLK_TYPE_DIV,
		.data	= &clk_data_postdiv1_16fft_main_1_hsdiv5.data_div.data,
	},
	[CLK_AM62X_POSTDIV1_16FFT_MAIN_1_HSDIVOUT6_CLK] =				      {
		.parent =								      {
			CLK_AM62X_PLLFRACF2_SSMOD_16FFT_MAIN_1_FOUTPOSTDIV_CLK,
			1,
		},
		.drv	= &clk_drv_div_reg.drv,
		.flags	= 0,
		.type	= CLK_TYPE_DIV,
		.data	= &clk_data_postdiv1_16fft_main_1_hsdiv6.data_div.data,
	},
	[CLK_AM62X_POSTDIV4_16FF_MAIN_0_HSDIVOUT5_CLK] =				      {
		.parent =								      {
			CLK_AM62X_PLLFRACF2_SSMOD_16FFT_MAIN_0_FOUTPOSTDIV_CLK,
			1,
		},
		.drv	= &clk_drv_div_reg.drv,
		.flags	= 0,
		.type	= CLK_TYPE_DIV,
		.data	= &clk_data_postdiv4_16ff_main_0_hsdiv5.data_div.data,
	},
	[CLK_AM62X_POSTDIV4_16FF_MAIN_0_HSDIVOUT6_CLK] =				      {
		.parent =								      {
			CLK_AM62X_PLLFRACF2_SSMOD_16FFT_MAIN_0_FOUTPOSTDIV_CLK,
			1,
		},
		.drv	= &clk_drv_div_reg.drv,
		.flags	= CLK_DATA_FLAG_NO_HW_REINIT,
		.type	= CLK_TYPE_DIV,
		.data	= &clk_data_postdiv4_16ff_main_0_hsdiv6.data_div.data,
	},
	[CLK_AM62X_POSTDIV4_16FF_MAIN_0_HSDIVOUT7_CLK] =				      {
		.parent =								      {
			CLK_AM62X_PLLFRACF2_SSMOD_16FFT_MAIN_0_FOUTPOSTDIV_CLK,
			1,
		},
		.drv	= &clk_drv_div_reg.drv,
		.flags	= 0,
		.type	= CLK_TYPE_DIV,
		.data	= &clk_data_postdiv4_16ff_main_0_hsdiv7.data_div.data,
	},
	[CLK_AM62X_POSTDIV4_16FF_MAIN_0_HSDIVOUT8_CLK] =				      {
		.parent =								      {
			CLK_AM62X_PLLFRACF2_SSMOD_16FFT_MAIN_0_FOUTPOSTDIV_CLK,
			1,
		},
		.drv	= &clk_drv_div_reg.drv,
		.flags	= 0,
		.type	= CLK_TYPE_DIV,
		.data	= &clk_data_postdiv4_16ff_main_0_hsdiv8.data_div.data,
	},
	[CLK_AM62X_POSTDIV4_16FF_MAIN_0_HSDIVOUT9_CLK] =				      {
		.parent =								      {
			CLK_AM62X_PLLFRACF2_SSMOD_16FFT_MAIN_0_FOUTPOSTDIV_CLK,
			1,
		},
		.drv	= &clk_drv_div_reg.drv,
		.flags	= 0,
		.type	= CLK_TYPE_DIV,
		.data	= &clk_data_postdiv4_16ff_main_0_hsdiv9.data_div.data,
	},
	[CLK_AM62X_POSTDIV4_16FF_MAIN_2_HSDIVOUT5_CLK] =				      {
		.parent =								      {
			CLK_AM62X_PLLFRACF2_SSMOD_16FFT_MAIN_2_FOUTPOSTDIV_CLK,
			1,
		},
		.drv	= &clk_drv_div_reg.drv,
		.flags	= 0,
		.type	= CLK_TYPE_DIV,
		.data	= &clk_data_postdiv4_16ff_main_2_hsdiv5.data_div.data,
	},
	[CLK_AM62X_POSTDIV4_16FF_MAIN_2_HSDIVOUT6_CLK] =				      {
		.parent =								      {
			CLK_AM62X_PLLFRACF2_SSMOD_16FFT_MAIN_2_FOUTPOSTDIV_CLK,
			1,
		},
		.drv	= &clk_drv_div_reg.drv,
		.flags	= 0,
		.type	= CLK_TYPE_DIV,
		.data	= &clk_data_postdiv4_16ff_main_2_hsdiv6.data_div.data,
	},
	[CLK_AM62X_POSTDIV4_16FF_MAIN_2_HSDIVOUT7_CLK] =				      {
		.parent =								      {
			CLK_AM62X_PLLFRACF2_SSMOD_16FFT_MAIN_2_FOUTPOSTDIV_CLK,
			1,
		},
		.drv	= &clk_drv_div_reg.drv,
		.flags	= 0,
		.type	= CLK_TYPE_DIV,
		.data	= &clk_data_postdiv4_16ff_main_2_hsdiv7.data_div.data,
	},
	[CLK_AM62X_POSTDIV4_16FF_MAIN_2_HSDIVOUT8_CLK] =				      {
		.parent =								      {
			CLK_AM62X_PLLFRACF2_SSMOD_16FFT_MAIN_2_FOUTPOSTDIV_CLK,
			1,
		},
		.drv	= &clk_drv_div_reg.drv,
		.flags	= 0,
		.type	= CLK_TYPE_DIV,
		.data	= &clk_data_postdiv4_16ff_main_2_hsdiv8.data_div.data,
	},
	[CLK_AM62X_POSTDIV4_16FF_MAIN_2_HSDIVOUT9_CLK] =				      {
		.parent =								      {
			CLK_AM62X_PLLFRACF2_SSMOD_16FFT_MAIN_2_FOUTPOSTDIV_CLK,
			1,
		},
		.drv	= &clk_drv_div_reg.drv,
		.flags	= 0,
		.type	= CLK_TYPE_DIV,
		.data	= &clk_data_postdiv4_16ff_main_2_hsdiv9.data_div.data,
	},
	[CLK_AM62X_SAM62_A53_512KB_WRAP_MAIN_0_ARM_COREPACK_0_A53_DIVH_CLK4_OBSCLK_OUT_CLK] = {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_sam62_a53_512kb_wrap_main_0_arm_corepack_0_a53_divh_clk4_obsclk_out_clk.data,
		.freq_idx	= AM62X_FREQ_VALUE_SAM62_A53_512KB_WRAP_MAIN_0_ARM_COREPACK_0_A53_DIVH_CLK4_OBSCLK_OUT_CLK,
	},
	[CLK_AM62X_SPI_MAIN_0_IO_CLKSPIO_CLK] =						      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_spi_main_0_io_clkspio_clk.data,
		.freq_idx	= AM62X_FREQ_VALUE_SPI_MAIN_0_IO_CLKSPIO_CLK,
	},
	[CLK_AM62X_SPI_MAIN_1_IO_CLKSPIO_CLK] =						      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_spi_main_1_io_clkspio_clk.data,
		.freq_idx	= AM62X_FREQ_VALUE_SPI_MAIN_1_IO_CLKSPIO_CLK,
	},
	[CLK_AM62X_SPI_MAIN_2_IO_CLKSPIO_CLK] =						      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_spi_main_2_io_clkspio_clk.data,
		.freq_idx	= AM62X_FREQ_VALUE_SPI_MAIN_2_IO_CLKSPIO_CLK,
	},
	[CLK_AM62X_SPI_MCU_0_IO_CLKSPIO_CLK] =						      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_spi_mcu_0_io_clkspio_clk.data,
		.freq_idx	= AM62X_FREQ_VALUE_SPI_MCU_0_IO_CLKSPIO_CLK,
	},
	[CLK_AM62X_SPI_MCU_1_IO_CLKSPIO_CLK] =						      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_spi_mcu_1_io_clkspio_clk.data,
		.freq_idx	= AM62X_FREQ_VALUE_SPI_MCU_1_IO_CLKSPIO_CLK,
	},
	[CLK_AM62X_AUDIO_REFCLKN_OUT0] =						      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_AUDIO_REFCLKn_out0.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_AUDIO_REFCLKN_OUT1] =						      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_AUDIO_REFCLKn_out1.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_CLK_32K_RC_SEL_DIV_CLKOUT] =						      {
		.parent =								      {
			CLK_AM62X_GLUELOGIC_RCOSC_CLKOUT,
			1,
		},
		.drv	= &clk_drv_div_fixed.drv,
		.flags	= 0,
		.data	= &clk_data_CLK_32K_RC_SEL_div_in2.data,
		.type	= CLK_TYPE_DIV,
	},
	[CLK_AM62X_MAIN_EMMCSD0_IO_CLKLB_SEL_OUT0] =					      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_MAIN_EMMCSD0_IO_CLKLB_SEL_out0.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_MAIN_EMMCSD1_IO_CLKLB_SEL_OUT0] =					      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_MAIN_EMMCSD1_IO_CLKLB_SEL_out0.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_MAIN_EMMCSD2_IO_CLKLB_SEL_OUT0] =					      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_MAIN_EMMCSD2_IO_CLKLB_SEL_out0.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_MAIN_OSPI_LOOPBACK_CLK_SEL_OUT0] =					      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_MAIN_OSPI_LOOPBACK_CLK_SEL_out0.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_MAIN_USB0_REFCLK_SEL_OUT0] =						      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_MAIN_USB0_REFCLK_SEL_out0.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_MAIN_USB1_REFCLK_SEL_OUT0] =						      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_MAIN_USB1_REFCLK_SEL_out0.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_MAIN_SPI0_MSTR_LP_CLKSEL_OUT0] =					      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_MAIN_spi0_mstr_lp_clksel_out0.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_MAIN_SPI1_MSTR_LP_CLKSEL_OUT0] =					      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_MAIN_spi1_mstr_lp_clksel_out0.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_MAIN_SPI2_MSTR_LP_CLKSEL_OUT0] =					      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_MAIN_spi2_mstr_lp_clksel_out0.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_MCASPN_AHCLKSEL_AHCLKR_OUT0] =					      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_MCASPn_AHCLKSEL_AHCLKR_out0.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_MCASPN_AHCLKSEL_AHCLKR_OUT1] =					      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_MCASPn_AHCLKSEL_AHCLKR_out1.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_MCASPN_AHCLKSEL_AHCLKR_OUT2] =					      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_MCASPn_AHCLKSEL_AHCLKR_out2.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_MCASPN_AHCLKSEL_AHCLKX_OUT0] =					      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_MCASPn_AHCLKSEL_AHCLKX_out0.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_MCASPN_AHCLKSEL_AHCLKX_OUT1] =					      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_MCASPn_AHCLKSEL_AHCLKX_out1.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_MCASPN_AHCLKSEL_AHCLKX_OUT2] =					      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_MCASPn_AHCLKSEL_AHCLKX_out2.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_MCASPN_CLKSEL_AUXCLK_OUT0] =						      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_MCASPn_CLKSEL_AUXCLK_out0.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_MCASPN_CLKSEL_AUXCLK_OUT1] =						      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_MCASPn_CLKSEL_AUXCLK_out1.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_MCASPN_CLKSEL_AUXCLK_OUT2] =						      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_MCASPn_CLKSEL_AUXCLK_out2.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_HSDIV0_16FFT_MAIN_12_HSDIVOUT0_CLK] =				      {
		.parent =								      {
			CLK_AM62X_PLLFRACF2_SSMOD_16FFT_MAIN_12_FOUTVCOP_CLK,
			1,
		},
		.drv	= &clk_drv_div_pll_16fft_hsdiv.drv,
		.flags	= CLK_DATA_FLAG_MODIFY_PARENT_FREQ | CLK_DATA_FLAG_NO_HW_REINIT,
		.type	= CLK_TYPE_DIV,
		.data	= &clk_data_hsdiv0_16fft_main_12_hsdiv0.data_div.data,
	},
	[CLK_AM62X_HSDIV0_16FFT_MAIN_16_HSDIVOUT0_CLK] =				      {
		.parent =								      {
			CLK_AM62X_PLLFRACF2_SSMOD_16FFT_MAIN_16_FOUTVCOP_CLK,
			1,
		},
		.drv	= &clk_drv_div_pll_16fft_hsdiv.drv,
		.flags	= CLK_DATA_FLAG_MODIFY_PARENT_FREQ,
		.type	= CLK_TYPE_DIV,
		.data	= &clk_data_hsdiv0_16fft_main_16_hsdiv0.data_div.data,
	},
	[CLK_AM62X_HSDIV0_16FFT_MAIN_17_HSDIVOUT0_CLK] =				      {
		.parent =								      {
			CLK_AM62X_PLLFRACF2_SSMOD_16FFT_MAIN_17_FOUTVCOP_CLK,
			1,
		},
		.drv	= &clk_drv_div_pll_16fft_hsdiv.drv,
		.flags	= CLK_DATA_FLAG_MODIFY_PARENT_FREQ,
		.type	= CLK_TYPE_DIV,
		.data	= &clk_data_hsdiv0_16fft_main_17_hsdiv0.data_div.data,
	},
	[CLK_AM62X_HSDIV0_16FFT_MAIN_8_HSDIVOUT0_CLK] =					      {
		.parent =								      {
			CLK_AM62X_PLLFRACF2_SSMOD_16FFT_MAIN_8_FOUTVCOP_CLK,
			1,
		},
		.drv	= &clk_drv_div_pll_16fft_hsdiv.drv,
		.flags	= CLK_DATA_FLAG_MODIFY_PARENT_FREQ,
		.type	= CLK_TYPE_DIV,
		.data	= &clk_data_hsdiv0_16fft_main_8_hsdiv0.data_div.data,
	},
	[CLK_AM62X_HSDIV1_16FFT_MAIN_15_HSDIVOUT0_CLK] =				      {
		.parent =								      {
			CLK_AM62X_PLLFRACF2_SSMOD_16FFT_MAIN_15_FOUTVCOP_CLK,
			1,
		},
		.drv	= &clk_drv_div_pll_16fft_hsdiv.drv,
		.flags	= 0,
		.type	= CLK_TYPE_DIV,
		.data	= &clk_data_hsdiv1_16fft_main_15_hsdiv0.data_div.data,
	},
	[CLK_AM62X_HSDIV1_16FFT_MAIN_15_HSDIVOUT1_CLK] =				      {
		.parent =								      {
			CLK_AM62X_PLLFRACF2_SSMOD_16FFT_MAIN_15_FOUTVCOP_CLK,
			1,
		},
		.drv	= &clk_drv_div_pll_16fft_hsdiv.drv,
		.flags	= 0,
		.type	= CLK_TYPE_DIV,
		.data	= &clk_data_hsdiv1_16fft_main_15_hsdiv1.data_div.data,
	},
	[CLK_AM62X_HSDIV4_16FFT_MAIN_0_HSDIVOUT0_CLK] =					      {
		.parent =								      {
			CLK_AM62X_PLLFRACF2_SSMOD_16FFT_MAIN_0_FOUTVCOP_CLK,
			1,
		},
		.drv	= &clk_drv_div_pll_16fft_hsdiv.drv,
		.flags	= 0,
		.type	= CLK_TYPE_DIV,
		.data	= &clk_data_hsdiv4_16fft_main_0_hsdiv0.data_div.data,
	},
	[CLK_AM62X_HSDIV4_16FFT_MAIN_0_HSDIVOUT1_CLK] =					      {
		.parent =								      {
			CLK_AM62X_PLLFRACF2_SSMOD_16FFT_MAIN_0_FOUTVCOP_CLK,
			1,
		},
		.drv	= &clk_drv_div_pll_16fft_hsdiv.drv,
		.flags	= 0,
		.type	= CLK_TYPE_DIV,
		.data	= &clk_data_hsdiv4_16fft_main_0_hsdiv1.data_div.data,
	},
	[CLK_AM62X_HSDIV4_16FFT_MAIN_0_HSDIVOUT2_CLK] =					      {
		.parent =								      {
			CLK_AM62X_PLLFRACF2_SSMOD_16FFT_MAIN_0_FOUTVCOP_CLK,
			1,
		},
		.drv	= &clk_drv_div_pll_16fft_hsdiv.drv,
		.flags	= 0,
		.type	= CLK_TYPE_DIV,
		.data	= &clk_data_hsdiv4_16fft_main_0_hsdiv2.data_div.data,
	},
	[CLK_AM62X_HSDIV4_16FFT_MAIN_0_HSDIVOUT3_CLK] =					      {
		.parent =								      {
			CLK_AM62X_PLLFRACF2_SSMOD_16FFT_MAIN_0_FOUTVCOP_CLK,
			1,
		},
		.drv	= &clk_drv_div_pll_16fft_hsdiv.drv,
		.flags	= 0,
		.type	= CLK_TYPE_DIV,
		.data	= &clk_data_hsdiv4_16fft_main_0_hsdiv3.data_div.data,
	},
	[CLK_AM62X_HSDIV4_16FFT_MAIN_0_HSDIVOUT4_CLK] =					      {
		.parent =								      {
			CLK_AM62X_PLLFRACF2_SSMOD_16FFT_MAIN_0_FOUTVCOP_CLK,
			1,
		},
		.drv	= &clk_drv_div_pll_16fft_hsdiv.drv,
		.flags	= 0,
		.type	= CLK_TYPE_DIV,
		.data	= &clk_data_hsdiv4_16fft_main_0_hsdiv4.data_div.data,
	},
	[CLK_AM62X_HSDIV4_16FFT_MAIN_1_HSDIVOUT0_CLK] =					      {
		.parent =								      {
			CLK_AM62X_PLLFRACF2_SSMOD_16FFT_MAIN_1_FOUTVCOP_CLK,
			1,
		},
		.drv	= &clk_drv_div_pll_16fft_hsdiv.drv,
		.flags	= 0,
		.type	= CLK_TYPE_DIV,
		.data	= &clk_data_hsdiv4_16fft_main_1_hsdiv0.data_div.data,
	},
	[CLK_AM62X_HSDIV4_16FFT_MAIN_1_HSDIVOUT1_CLK] =					      {
		.parent =								      {
			CLK_AM62X_PLLFRACF2_SSMOD_16FFT_MAIN_1_FOUTVCOP_CLK,
			1,
		},
		.drv	= &clk_drv_div_pll_16fft_hsdiv.drv,
		.flags	= 0,
		.type	= CLK_TYPE_DIV,
		.data	= &clk_data_hsdiv4_16fft_main_1_hsdiv1.data_div.data,
	},
	[CLK_AM62X_HSDIV4_16FFT_MAIN_1_HSDIVOUT2_CLK] =					      {
		.parent =								      {
			CLK_AM62X_PLLFRACF2_SSMOD_16FFT_MAIN_1_FOUTVCOP_CLK,
			1,
		},
		.drv	= &clk_drv_div_pll_16fft_hsdiv.drv,
		.flags	= 0,
		.type	= CLK_TYPE_DIV,
		.data	= &clk_data_hsdiv4_16fft_main_1_hsdiv2.data_div.data,
	},
	[CLK_AM62X_HSDIV4_16FFT_MAIN_1_HSDIVOUT3_CLK] =					      {
		.parent =								      {
			CLK_AM62X_PLLFRACF2_SSMOD_16FFT_MAIN_1_FOUTVCOP_CLK,
			1,
		},
		.drv	= &clk_drv_div_pll_16fft_hsdiv.drv,
		.flags	= 0,
		.type	= CLK_TYPE_DIV,
		.data	= &clk_data_hsdiv4_16fft_main_1_hsdiv3.data_div.data,
	},
	[CLK_AM62X_HSDIV4_16FFT_MAIN_1_HSDIVOUT4_CLK] =					      {
		.parent =								      {
			CLK_AM62X_PLLFRACF2_SSMOD_16FFT_MAIN_1_FOUTVCOP_CLK,
			1,
		},
		.drv	= &clk_drv_div_pll_16fft_hsdiv.drv,
		.flags	= 0,
		.type	= CLK_TYPE_DIV,
		.data	= &clk_data_hsdiv4_16fft_main_1_hsdiv4.data_div.data,
	},
	[CLK_AM62X_HSDIV4_16FFT_MAIN_2_HSDIVOUT0_CLK] =					      {
		.parent =								      {
			CLK_AM62X_PLLFRACF2_SSMOD_16FFT_MAIN_2_FOUTVCOP_CLK,
			1,
		},
		.drv	= &clk_drv_div_pll_16fft_hsdiv.drv,
		.flags	= 0,
		.type	= CLK_TYPE_DIV,
		.data	= &clk_data_hsdiv4_16fft_main_2_hsdiv0.data_div.data,
	},
	[CLK_AM62X_HSDIV4_16FFT_MAIN_2_HSDIVOUT1_CLK] =					      {
		.parent =								      {
			CLK_AM62X_PLLFRACF2_SSMOD_16FFT_MAIN_2_FOUTVCOP_CLK,
			1,
		},
		.drv	= &clk_drv_div_pll_16fft_hsdiv.drv,
		.flags	= 0,
		.type	= CLK_TYPE_DIV,
		.data	= &clk_data_hsdiv4_16fft_main_2_hsdiv1.data_div.data,
	},
	[CLK_AM62X_HSDIV4_16FFT_MAIN_2_HSDIVOUT2_CLK] =					      {
		.parent =								      {
			CLK_AM62X_PLLFRACF2_SSMOD_16FFT_MAIN_2_FOUTVCOP_CLK,
			1,
		},
		.drv	= &clk_drv_div_pll_16fft_hsdiv.drv,
		.flags	= 0,
		.type	= CLK_TYPE_DIV,
		.data	= &clk_data_hsdiv4_16fft_main_2_hsdiv2.data_div.data,
	},
	[CLK_AM62X_HSDIV4_16FFT_MAIN_2_HSDIVOUT3_CLK] =					      {
		.parent =								      {
			CLK_AM62X_PLLFRACF2_SSMOD_16FFT_MAIN_2_FOUTVCOP_CLK,
			1,
		},
		.drv	= &clk_drv_div_pll_16fft_hsdiv.drv,
		.flags	= 0,
		.type	= CLK_TYPE_DIV,
		.data	= &clk_data_hsdiv4_16fft_main_2_hsdiv3.data_div.data,
	},
	[CLK_AM62X_HSDIV4_16FFT_MAIN_2_HSDIVOUT4_CLK] =					      {
		.parent =								      {
			CLK_AM62X_PLLFRACF2_SSMOD_16FFT_MAIN_2_FOUTVCOP_CLK,
			1,
		},
		.drv	= &clk_drv_div_pll_16fft_hsdiv.drv,
		.flags	= 0,
		.type	= CLK_TYPE_DIV,
		.data	= &clk_data_hsdiv4_16fft_main_2_hsdiv4.data_div.data,
	},
	[CLK_AM62X_HSDIV4_16FFT_MCU_0_HSDIVOUT0_CLK] =					      {
		.parent =								      {
			CLK_AM62X_PLLFRACF2_SSMOD_16FFT_MCU_0_FOUTVCOP_CLK,
			1,
		},
		.drv	= &clk_drv_div_pll_16fft_hsdiv.drv,
		.flags	= 0,
		.type	= CLK_TYPE_DIV,
		.data	= &clk_data_hsdiv4_16fft_mcu_0_hsdiv0.data_div.data,
	},
	[CLK_AM62X_HSDIV4_16FFT_MCU_0_HSDIVOUT4_CLK] =					      {
		.parent =								      {
			CLK_AM62X_PLLFRACF2_SSMOD_16FFT_MCU_0_FOUTVCOP_CLK,
			1,
		},
		.drv	= &clk_drv_div_pll_16fft_hsdiv.drv,
		.flags	= 0,
		.type	= CLK_TYPE_DIV,
		.data	= &clk_data_hsdiv4_16fft_mcu_0_hsdiv4.data_div.data,
	},
	[CLK_AM62X_SAM62_PLL_CTRL_WRAP_MAIN_0_SYSCLKOUT_CLK] =				      {
		.drv	= &clk_drv_pllctrl_mux_reg_ro.drv,
		.flags	= 0,
		.data	= &clk_data_sam62_pll_ctrl_wrap_main_0_sysclkout_clk.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_SAM62_PLL_CTRL_WRAP_MAIN_0_CHIP_DIV1_CLK_CLK] =			      {
		.drv	= &clk_drv_div_reg.drv,
		.flags	= 0,
		.data	= &clk_data_sam62_pll_ctrl_wrap_main_0_chip_div1_clk_clk.data_div.data,
		.parent =								      {
			CLK_AM62X_SAM62_PLL_CTRL_WRAP_MAIN_0_SYSCLKOUT_CLK,
			1,
		},
	},
	[CLK_AM62X_SAM62_PLL_CTRL_WRAP_MCU_0_SYSCLKOUT_CLK] =				      {
		.drv	= &clk_drv_pllctrl_mux_reg_ro.drv,
		.flags	= 0,
		.data	= &clk_data_sam62_pll_ctrl_wrap_mcu_0_sysclkout_clk.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_SAM62_PLL_CTRL_WRAP_MCU_0_CHIP_DIV1_CLK_CLK] =			      {
		.drv	= &clk_drv_div_reg.drv,
		.flags	= 0,
		.data	= &clk_data_sam62_pll_ctrl_wrap_mcu_0_chip_div1_clk_clk.data_div.data,
		.parent =								      {
			CLK_AM62X_SAM62_PLL_CTRL_WRAP_MCU_0_SYSCLKOUT_CLK,
			1,
		},
	},
	[CLK_AM62X_CLKOUT0_CTRL_OUT0] =							      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_CLKOUT0_CTRL_out0.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_CLK_32K_RC_SEL_OUT0] =						      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_CLK_32K_RC_SEL_out0.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_MAIN_CP_GEMAC_CPTS_CLK_SEL_OUT0] =					      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_MAIN_CP_GEMAC_CPTS_CLK_SEL_out0.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_MAIN_DSS_DPI1_OUT0] =						      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_MAIN_DSS_DPI1_out0.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_MAIN_EMMCSD0_REFCLK_SEL_OUT0] =					      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_MAIN_EMMCSD0_REFCLK_SEL_out0.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_MAIN_EMMCSD1_REFCLK_SEL_OUT0] =					      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_MAIN_EMMCSD1_REFCLK_SEL_out0.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_MAIN_EMMCSD2_REFCLK_SEL_OUT0] =					      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_MAIN_EMMCSD2_REFCLK_SEL_out0.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_MAIN_GPMC_FCLK_SEL_OUT0] =						      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_MAIN_GPMC_FCLK_SEL_out0.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_MAIN_GTCCLK_SEL_OUT0] =						      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_MAIN_GTCCLK_SEL_out0.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_MAIN_ICSSM_CORE_CLK_SEL_OUT0] =					      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_MAIN_ICSSM_CORE_CLK_SEL_out0.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_MAIN_ICSSM_IEPCLK_SEL_OUT0] =					      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_MAIN_ICSSM_IEPCLK_SEL_out0.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_MAIN_MCANN_CLK_SEL_OUT0] =						      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_MAIN_MCANn_CLK_SEL_out0.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_MAIN_OBSCLK0_MUX_SEL_OUT0] =						      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_MAIN_OBSCLK0_MUX_SEL_out0.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_MAIN_OSPI_REF_CLK_SEL_OUT0] =					      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_MAIN_OSPI_REF_CLK_SEL_out0.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_MAIN_TIMERCLKN_SEL_OUT0] =						      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_MAIN_TIMERCLKn_SEL_out0.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_MAIN_TIMERCLKN_SEL_OUT1] =						      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_MAIN_TIMERCLKn_SEL_out1.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_MAIN_TIMERCLKN_SEL_OUT2] =						      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_MAIN_TIMERCLKn_SEL_out2.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_MAIN_TIMERCLKN_SEL_OUT3] =						      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_MAIN_TIMERCLKn_SEL_out3.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_MAIN_TIMERCLKN_SEL_OUT4] =						      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_MAIN_TIMERCLKn_SEL_out4.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_MAIN_TIMERCLKN_SEL_OUT5] =						      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_MAIN_TIMERCLKn_SEL_out5.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_MAIN_TIMERCLKN_SEL_OUT6] =						      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_MAIN_TIMERCLKn_SEL_out6.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_MAIN_TIMERCLKN_SEL_OUT7] =						      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_MAIN_TIMERCLKn_SEL_out7.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_MAIN_WWDTCLKN_SEL_OUT0] =						      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_MAIN_WWDTCLKn_SEL_out0.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_MAIN_WWDTCLKN_SEL_OUT1] =						      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_MAIN_WWDTCLKn_SEL_out1.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_MAIN_WWDTCLKN_SEL_OUT2] =						      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_MAIN_WWDTCLKn_SEL_out2.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_MAIN_WWDTCLKN_SEL_OUT3] =						      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_MAIN_WWDTCLKn_SEL_out3.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_MAIN_WWDTCLKN_SEL_OUT4] =						      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_MAIN_WWDTCLKn_SEL_out4.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_MCU_M4FSS_CLKSEL_OUT0] =						      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_MCU_M4FSS_CLKSEL_out0.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_MCU_OBSCLK_MUX_SEL_OUT0] =						      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_MCU_OBSCLK_MUX_SEL_out0.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_RTC_CLK_SEL_OUT0] =							      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_RTC_CLK_SEL_out0.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_USART_PROGRAMMABLE_CLOCK_DIVIDER_OUT0] =				      {
		.parent =								      {
			CLK_AM62X_HSDIV4_16FFT_MAIN_1_HSDIVOUT0_CLK,
			1,
		},
		.drv	= &clk_drv_div_reg_go.drv,
		.flags	= 0,
		.data	= &clk_data_USART_Programmable_Clock_Divider_out0.data_div.data,
		.type	= CLK_TYPE_DIV,
	},
	[CLK_AM62X_USART_PROGRAMMABLE_CLOCK_DIVIDER_OUT1] =				      {
		.parent =								      {
			CLK_AM62X_HSDIV4_16FFT_MAIN_1_HSDIVOUT0_CLK,
			1,
		},
		.drv	= &clk_drv_div_reg_go.drv,
		.flags	= 0,
		.data	= &clk_data_USART_Programmable_Clock_Divider_out1.data_div.data,
		.type	= CLK_TYPE_DIV,
	},
	[CLK_AM62X_USART_PROGRAMMABLE_CLOCK_DIVIDER_OUT2] =				      {
		.parent =								      {
			CLK_AM62X_HSDIV4_16FFT_MAIN_1_HSDIVOUT0_CLK,
			1,
		},
		.drv	= &clk_drv_div_reg_go.drv,
		.flags	= 0,
		.data	= &clk_data_USART_Programmable_Clock_Divider_out2.data_div.data,
		.type	= CLK_TYPE_DIV,
	},
	[CLK_AM62X_USART_PROGRAMMABLE_CLOCK_DIVIDER_OUT3] =				      {
		.parent =								      {
			CLK_AM62X_HSDIV4_16FFT_MAIN_1_HSDIVOUT0_CLK,
			1,
		},
		.drv	= &clk_drv_div_reg_go.drv,
		.flags	= 0,
		.data	= &clk_data_USART_Programmable_Clock_Divider_out3.data_div.data,
		.type	= CLK_TYPE_DIV,
	},
	[CLK_AM62X_USART_PROGRAMMABLE_CLOCK_DIVIDER_OUT4] =				      {
		.parent =								      {
			CLK_AM62X_HSDIV4_16FFT_MAIN_1_HSDIVOUT0_CLK,
			1,
		},
		.drv	= &clk_drv_div_reg_go.drv,
		.flags	= 0,
		.data	= &clk_data_USART_Programmable_Clock_Divider_out4.data_div.data,
		.type	= CLK_TYPE_DIV,
	},
	[CLK_AM62X_USART_PROGRAMMABLE_CLOCK_DIVIDER_OUT5] =				      {
		.parent =								      {
			CLK_AM62X_HSDIV4_16FFT_MAIN_1_HSDIVOUT0_CLK,
			1,
		},
		.drv	= &clk_drv_div_reg_go.drv,
		.flags	= 0,
		.data	= &clk_data_USART_Programmable_Clock_Divider_out5.data_div.data,
		.type	= CLK_TYPE_DIV,
	},
	[CLK_AM62X_USART_PROGRAMMABLE_CLOCK_DIVIDER_OUT6] =				      {
		.parent =								      {
			CLK_AM62X_HSDIV4_16FFT_MAIN_1_HSDIVOUT0_CLK,
			1,
		},
		.drv	= &clk_drv_div_reg_go.drv,
		.flags	= 0,
		.data	= &clk_data_USART_Programmable_Clock_Divider_out6.data_div.data,
		.type	= CLK_TYPE_DIV,
	},
	[CLK_AM62X_WKUP_CLKOUT_SEL_OUT0] =						      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_WKUP_CLKOUT_SEL_out0.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_WKUP_CLKSEL_OUT0] =							      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_WKUP_CLKSEL_out0.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_MCU_OBSCLK_DIV_OUT0] =						      {
		.parent =								      {
			CLK_AM62X_MCU_OBSCLK_MUX_SEL_OUT0,
			1,
		},
		.drv	= &clk_drv_div_reg_go.drv,
		.flags	= 0,
		.data	= &clk_data_mcu_obsclk_div_out0.data_div.data,
		.type	= CLK_TYPE_DIV,
	},
	[CLK_AM62X_OSBCLK0_DIV_OUT0] =							      {
		.parent =								      {
			CLK_AM62X_MAIN_OBSCLK0_MUX_SEL_OUT0,
			1,
		},
		.drv	= &clk_drv_div_reg_go.drv,
		.flags	= 0,
		.data	= &clk_data_osbclk0_div_out0.data_div.data,
		.type	= CLK_TYPE_DIV,
	},
	[CLK_AM62X_MAIN_USART0_FCLK_SEL_OUT0] =						      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_MAIN_usart0_fclk_sel_out0.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_MAIN_USART1_FCLK_SEL_OUT0] =						      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_MAIN_usart1_fclk_sel_out0.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_MAIN_USART2_FCLK_SEL_OUT0] =						      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_MAIN_usart2_fclk_sel_out0.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_MAIN_USART3_FCLK_SEL_OUT0] =						      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_MAIN_usart3_fclk_sel_out0.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_MAIN_USART4_FCLK_SEL_OUT0] =						      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_MAIN_usart4_fclk_sel_out0.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_MAIN_USART5_FCLK_SEL_OUT0] =						      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_MAIN_usart5_fclk_sel_out0.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_MAIN_USART6_FCLK_SEL_OUT0] =						      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_MAIN_usart6_fclk_sel_out0.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_MCU_OBSCLK_OUTMUX_SEL_OUT0] =					      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_MCU_OBSCLK_OUTMUX_SEL_out0.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_MCU_GPIO0_CLKSEL_OUT0] =						      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_MCU_GPIO0_CLKSEL_out0.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_MCU_MCANN_CLK_SEL_OUT0] =						      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_MCU_MCANn_CLK_SEL_out0.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_MCU_MCANN_CLK_SEL_OUT1] =						      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_MCU_MCANn_CLK_SEL_out1.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_MCU_TIMERCLKN_SEL_OUT0_DIV_CLKOUT] =					      {
		.parent =								      {
			CLK_AM62X_GLUELOGIC_RCOSC_CLKOUT,
			1,
		},
		.drv	= &clk_drv_div_fixed.drv,
		.flags	= 0,
		.data	= &clk_data_MCU_TIMERCLKn_SEL_out0_div_in7.data,
		.type	= CLK_TYPE_DIV,
	},
	[CLK_AM62X_MCU_TIMERCLKN_SEL_OUT1_DIV_CLKOUT] =					      {
		.parent =								      {
			CLK_AM62X_GLUELOGIC_RCOSC_CLKOUT,
			1,
		},
		.drv	= &clk_drv_div_fixed.drv,
		.flags	= 0,
		.data	= &clk_data_MCU_TIMERCLKn_SEL_out1_div_in7.data,
		.type	= CLK_TYPE_DIV,
	},
	[CLK_AM62X_MCU_TIMERCLKN_SEL_OUT2_DIV_CLKOUT] =					      {
		.parent =								      {
			CLK_AM62X_GLUELOGIC_RCOSC_CLKOUT,
			1,
		},
		.drv	= &clk_drv_div_fixed.drv,
		.flags	= 0,
		.data	= &clk_data_MCU_TIMERCLKn_SEL_out2_div_in7.data,
		.type	= CLK_TYPE_DIV,
	},
	[CLK_AM62X_MCU_TIMERCLKN_SEL_OUT3_DIV_CLKOUT] =					      {
		.parent =								      {
			CLK_AM62X_GLUELOGIC_RCOSC_CLKOUT,
			1,
		},
		.drv	= &clk_drv_div_fixed.drv,
		.flags	= 0,
		.data	= &clk_data_MCU_TIMERCLKn_SEL_out3_div_in7.data,
		.type	= CLK_TYPE_DIV,
	},
	[CLK_AM62X_MCU_WWDTCLK_SEL_DIV_CLKOUT] =					      {
		.parent =								      {
			CLK_AM62X_GLUELOGIC_RCOSC_CLKOUT,
			1,
		},
		.drv	= &clk_drv_div_fixed.drv,
		.flags	= 0,
		.data	= &clk_data_MCU_WWDTCLK_SEL_div_in3.data,
		.type	= CLK_TYPE_DIV,
	},
	[CLK_AM62X_MCU_SPI0_MSTR_LP_CLKSEL_OUT0] =					      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_MCU_spi0_mstr_lp_clksel_out0.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_MCU_SPI1_MSTR_LP_CLKSEL_OUT0] =					      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_MCU_spi1_mstr_lp_clksel_out0.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_WKUP_TIMERCLKN_SEL_OUT0_DIV_CLKOUT] =				      {
		.parent =								      {
			CLK_AM62X_GLUELOGIC_RCOSC_CLKOUT,
			1,
		},
		.drv	= &clk_drv_div_fixed.drv,
		.flags	= 0,
		.data	= &clk_data_WKUP_TIMERCLKn_SEL_out0_div_in7.data,
		.type	= CLK_TYPE_DIV,
	},
	[CLK_AM62X_WKUP_TIMERCLKN_SEL_OUT1_DIV_CLKOUT] =				      {
		.parent =								      {
			CLK_AM62X_GLUELOGIC_RCOSC_CLKOUT,
			1,
		},
		.drv	= &clk_drv_div_fixed.drv,
		.flags	= 0,
		.data	= &clk_data_WKUP_TIMERCLKn_SEL_out1_div_in7.data,
		.type	= CLK_TYPE_DIV,
	},
	[CLK_AM62X_WKUP_WWDTCLK_SEL_DIV_CLKOUT] =					      {
		.parent =								      {
			CLK_AM62X_GLUELOGIC_RCOSC_CLKOUT,
			1,
		},
		.drv	= &clk_drv_div_fixed.drv,
		.flags	= 0,
		.data	= &clk_data_WKUP_WWDTCLK_SEL_div_in3.data,
		.type	= CLK_TYPE_DIV,
	},
	[CLK_AM62X_HSDIV4_16FFT_MCU_0_HSDIVOUT1_CLK] =					      {
		.parent =								      {
			CLK_AM62X_PLLFRACF2_SSMOD_16FFT_MCU_0_FOUTVCOP_CLK,
			1,
		},
		.drv	= &clk_drv_div_pll_16fft_hsdiv.drv,
		.flags	= 0,
		.type	= CLK_TYPE_DIV,
		.data	= &clk_data_hsdiv4_16fft_mcu_0_hsdiv1.data_div.data,
	},
	[CLK_AM62X_HSDIV4_16FFT_MCU_0_HSDIVOUT2_CLK] =					      {
		.parent =								      {
			CLK_AM62X_PLLFRACF2_SSMOD_16FFT_MCU_0_FOUTVCOP_CLK,
			1,
		},
		.drv	= &clk_drv_div_pll_16fft_hsdiv.drv,
		.flags	= 0,
		.type	= CLK_TYPE_DIV,
		.data	= &clk_data_hsdiv4_16fft_mcu_0_hsdiv2.data_div.data,
	},
	[CLK_AM62X_HSDIV4_16FFT_MCU_0_HSDIVOUT3_CLK] =					      {
		.parent =								      {
			CLK_AM62X_PLLFRACF2_SSMOD_16FFT_MCU_0_FOUTVCOP_CLK,
			1,
		},
		.drv	= &clk_drv_div_pll_16fft_hsdiv.drv,
		.flags	= 0,
		.type	= CLK_TYPE_DIV,
		.data	= &clk_data_hsdiv4_16fft_mcu_0_hsdiv3.data_div.data,
	},
	[CLK_AM62X_MSHSI2C_WKUP_0_PORSCL] =						      {
		.drv		= &clk_drv_from_device,
		.flags		= 0,
		.data		= &clk_data_mshsi2c_wkup_0_porscl.data,
		.freq_idx	= AM62X_FREQ_VALUE_MSHSI2C_WKUP_0_PORSCL,
	},
	[CLK_AM62X_MCU_TIMERCLKN_SEL_OUT0] =						      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_MCU_TIMERCLKn_SEL_out0.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_MCU_TIMERCLKN_SEL_OUT1] =						      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_MCU_TIMERCLKn_SEL_out1.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_MCU_TIMERCLKN_SEL_OUT2] =						      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_MCU_TIMERCLKn_SEL_out2.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_MCU_TIMERCLKN_SEL_OUT3] =						      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_MCU_TIMERCLKn_SEL_out3.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_MCU_WWDTCLK_SEL_OUT0] =						      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_MCU_WWDTCLK_SEL_out0.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_WKUP_TIMERCLKN_SEL_OUT0] =						      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_WKUP_TIMERCLKn_SEL_out0.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_WKUP_TIMERCLKN_SEL_OUT1] =						      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_WKUP_TIMERCLKn_SEL_out1.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_WKUP_WWDTCLK_SEL_OUT0] =						      {
		.drv	= &clk_drv_mux_reg.drv,
		.flags	= 0,
		.data	= &clk_data_WKUP_WWDTCLK_SEL_out0.data_mux.data,
		.type	= CLK_TYPE_MUX,
	},
	[CLK_AM62X_SAM62_PLL_CTRL_WRAP_MAIN_0_CHIP_DIV24_CLK_CLK] =			      {
		.drv	= &clk_drv_div_reg.drv,
		.flags	= 0,
		.data	= &clk_data_sam62_pll_ctrl_wrap_main_0_chip_div24_clk_clk.data_div.data,
		.parent =								      {
			CLK_AM62X_SAM62_PLL_CTRL_WRAP_MAIN_0_SYSCLKOUT_CLK,
			1,
		},
	},
	[CLK_AM62X_SAM62_PLL_CTRL_WRAP_MCU_0_CHIP_DIV24_CLK_CLK] =			      {
		.drv	= &clk_drv_div_reg.drv,
		.flags	= 0,
		.data	= &clk_data_sam62_pll_ctrl_wrap_mcu_0_chip_div24_clk_clk.data_div.data,
		.parent =								      {
			CLK_AM62X_SAM62_PLL_CTRL_WRAP_MCU_0_SYSCLKOUT_CLK,
			1,
		},
	},
};

struct clk soc_clocks[ARRAY_SIZE(soc_clock_data)];
const size_t soc_clock_count = ARRAY_SIZE(soc_clock_data);
