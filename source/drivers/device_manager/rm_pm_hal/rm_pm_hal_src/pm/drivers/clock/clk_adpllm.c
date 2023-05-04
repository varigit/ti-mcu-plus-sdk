/*
 * DMSC firmware
 *
 * Copyright (C) 2017-2020, Texas Instruments Incorporated
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

#include <clk_adpllm.h>
#include <device.h>
#include <lib/container_of.h>
#include <lib/ioremap.h>
#include <clk_pll.h>
#include <clk_mux.h>
#include <div64.h>
#include <compiler.h>
#include <lib/trace.h>

/*
 * AM6 Notes (MAIN PLL5 is not present):
 *
 * ADPLLM - Type A
 *	MCU PLL0 (Uses CLKOUTBYPASS output) 2400MHz
 *		MCU_SYSCLK0 - 400MHz
 *		HSDIV connects to CLKDCOLDO (Same as VCO)
 *			MCUHSDIV_CLKOUT1 / 60 = 60MHz
 *			MCUHSDIV_CLKOUT2 / 80 = 60MHz
 *			MCUHSDIV_CLKOUT3 / 96 = 60MHz
 *			MCUHSDIV_CLKOUT4 / 18 = 133.3MHz
 *	MCU PLL1 (CPSW) - 2000MHz
 *		HSDIV connects to CLKDCOLDO (Same as VCO)
 *      MAIN PLL6 (ARM0) - 1600MHz
 *	MAIN PLL7 (ARM1) - 1600MHz
 * ADPLLJ - Type B
 *	MAIN PLL0 (MAIN) - 2500MHz
 *		HSDIV connects to CLKDCOLDO (Same as VCO)
 *	MAIN PLL1 (PER0) - 1920MHz
 *		CLKOUT - 960MHz
 *	MAIN PLL2 (PER1) - 1800MHz
 *		HSDIV connects to CLKDCOLDO (Same as VCO)
 *	MAIN PLL3 (DDR) - 1333/933/800MHz
 *	MAIN PLL4 (DSS) - 2310MHz
 *
 * No AM6 PLLs connect CLKINPULOW. ULOWCLKEN and M1 are tied to 0.
 * No AM6 PLLs connect CLKINPHIF. CLKINPHIF is tied to 0.
 * No AM6 PLLs use the CLKOUTX2 clock.
 * No AM6 PLLs use the CLKOUTHIF clock.
 * One PLL uses the CLKOUTBYPASS clock, MCU_PLL0.
 * One PLL uses CLKOUTLDO, MAIN PLL1 (PER0), to connect to USB
 *
 * It is preferred to minimize the value for N parameter (it minimizes lock
 * time and jitter). Then M and M.f are chosen to provide correct frequency
 * (with lowest delta as possible).
 *
 * When PLL is in bypass, all HSDIV outputs connect to CLKOUTBYPASS, which is
 * CLKNIP.
 */

/*
 * Clock input/output table:
 *
 * ADPLLJ - DC_CORRECTOR_EN is always 1, M4XEN always 0
 *
 * CLKINP -	Reference input clock
 *		ADPLLM - 32kHz to 52MHz
 *		ADPLLJ - 620kHz to 62MHz
 * REFCLK -	Input to VCO
 *		ADPLLM - 32kHz - 52MHz
 *		ADPLLJ - 620kHz to 2.5MHz
 *		CLKINP / (N+1)
 * CLKINPULOW Bypass input clock
 *		ADPLLM - 1kHz to 400MHz (200MHz at OPP50)
 *		ADPLLJ - 1kHz to 600MHz (300MHz at OPP50)
 * CLKINPHIF -	High frequency input clock to post-divider M3 (ADPLLM only)
 *		20MHz/M2 to 1400MHz (2000MHz OPPTurbo, 700MHz OPP50)
 * CLKOUT -	Primary output clock
 *		ADPLLM - 20MHz/M2 to 1400MHz (2500MHz at OPPTurbo, 700MHz at OPP50)
 *		ADDPLJ - 20MHz/M2 to 1450MHz (625MHz at OPP50)
 *		BYPASS=0, DC_CORRECTOR_EN=1 - CLKDCOLDO / M2 (M2=1 for ADPLLM)
 *		BYPASS=0, DC_CORRECTOR_EN=0 - CLKDCOLDO / (M2 * 2)
 *		BYPASS=1 - CLKOUTBYPASS
 * CLKOUTLDO -	Primary output clk in VDDLDOOUT domain (no bypass, ADPLLJ only),
 *		750MHz to 2500MHz
 *		CLKDCOLDO / M2
 * CLKOUTX2 -	Secondary 2x output clock (ADPLLM only)
 *		40MHz to 2200MHz (2800MHz at OPPTurbo, 1400MHz at OPP50)
 *		When CLKOUTX2 > 2.2GHz, it must be disabled with CLKOUTX2EN=0
 *		BYPASS=0 - CLKDCOLDO / M2
 *		BYPASS=1 - CLKOUTBYPASS
 * CLKOUTBYPASS Output of bypass mux
 *		ADPLLM - 1kHz/(M1+1) to 600MHz (1GHz at OPPTurbo, 300MHz at OPP50)
 *		ULOWCLKEN=0 - CLKINP
 *		ULOWCLKEN=1 - CLKINPULOW / (M1 + 1)
 * CLKDCOLDO -	Oscillator output clock before post-division (no bypass)
 *		Also known as DCOCLKLDO
 *		ADPLLM - 40MHz to 2800MHz
 *		ADPLLJ - 750MHz to 2500MHz
 *		M4XEN=0, DC_CORRECTOR_EN=1 - 1 * REFCLK * (M+M.f)
 *		M4XEN=0, DC_CORRECTOR_EN=0 - 2 * REFCLK * (M+M.f)
 *		M4XEN=1, DC_CORRECTOR_EN=1 - 4 * REFCLK * (M+M.f)
 *		M4XEN=1, DC_CORRECTOR_EN=0 - 8 * REFCLK * (M+M.f)
 * CLKOUTHIF -	High freq output clk from post-div M3 (no bypass, ADPLLM only)
 *		CLKINPHIFSEL=0 - CLKDCOLDO / M3
 *			20MHz/M3 - 1400MHz (2GHz at OPPTurbo, 700MHz at OPP50)
 *		CLKINPHIFSEL=1 - CLKINPHIF / M3
 *			40MHz/M3 - 2200MHz (2.8GHz at OPPTurbo, 1.4GHz at OPP50)
 * CLKINPHIFLDO HSDIV high frequency input clock, up to 2400MHz
 * CLKOUTx -	HSDIV output clocks up to 1.2GHz (1.9GHz OPPTurbo, 600MHZ OPP50)
 */

/*
 * APLLM Operating modes:
 *
 * Active & Locked
 *	INITZ=1, CLKINP On, IDLE = 0, M > 1
 * MN Bypass (2.5us + 500 refclk to lock)
 *      INITZ=1, CLKINP On, IDLE = 0, M <= 1
 * Low Power Idle Bypass (2.5us + 120 refclk to lock)
 *	INITZ=1, CLKINP On, IDLE = 1, LOWCURRSTDBY=1
 * Fast Relock Idle Bypass (0.05us + 120 refclk to lock)
 *	INITZ=1, CLKINP On, IDLE = 1, LOWCURRSTDBY=0
 * Low Power Lost clock bypass (2.5us + 500 refclk to lock)
 *	INITZ=1, CLKINP Off, LOWCURRSTDBY=1
 * Fast Relock Lost clock bypass (0.05us + 500 refclk to lock)
 *	INITZ=1, CLKINP Off, LOWCURRSTDBY=0
 * Off
 *	INITZ=0
 *
 * 1 <= plld <= 128
 * 2 <= pllm <= 2047 (*4 for M4X=1, *2 for DCC=0)
 * 1 <= clkod <= 31 (*2 for DCC=0, 1 for DCC=1)
 *
 * APLLJ Operating modes:
 *
 * Active & Locked
 *	INITZ=1, CLKINP On, IDLE = 0, M > 1
 * MN Bypass (500 refclk to lock)
 *      INITZ=1, CLKINP On, IDLE = 0, M <= 1
 * Idle Bypass (9.0us + 125 refclk to lock)
 *	INITZ=1, CLKINP On, IDLE = 1
 * Lost clock bypass (9.0us + 125 refclk to lock)
 *	INITZ=1, CLKINP Off
 * Off
 *	INITZ=0
 * Retention
 *	Entered from Idle/Lost clock bypass, no bypass clock.
 *
 * 1 <= plld <= 256
 * 2 <= pllm <= 4095
 * 1 <= clkod <= 127
 */

/* Registers */
#define ADPLLM_PID                      ((u32) 0x00U)
#define ADPLLM_MMR_CFG0                 ((u32) 0x08U)
#define ADPLLM_MMR_CFG1                 ((u32) 0x0cU)

#define ADPLLM_PLL_KICK0(idx)           ((idx * 0x1000U) + 0x10U)
#define ADPLLM_PLL_KICK1(idx)           ((idx * 0x1000U) + 0x14U)
#define ADPLLM_PLL_FREQ_CTRL0(idx)      ((idx * 0x1000U) + 0x20U)
#define ADPLLM_PLL_FREQ_CTRL1(idx)      ((idx * 0x1000U) + 0x24U)
#define ADPLLM_PLL_CLKDIV(idx)          ((idx * 0x1000U) + 0x28U)
#define ADPLLM_PLL_PROG(idx)            ((idx * 0x1000U) + 0x2cU)
#define ADPLLM_PLL_CTRL(idx)            ((idx * 0x1000U) + 0x30U)
#define ADPLLM_PLL_STAT(idx)            ((idx * 0x1000U) + 0x34U)
#define ADPLLM_PLL_PWR_CTRL(idx)        ((idx * 0x1000U) + 0x38U)
#define ADPLLM_PLL_PWR_STAT(idx)        ((idx * 0x1000U) + 0x3cU)
#define ADPLLM_PLL_SS_SPREAD(idx)       ((idx * 0x1000U) + 0x40U)
#define ADPLLM_PLL_SS_MODFREQ(idx)      ((idx * 0x1000U) + 0x44U)

#define ADPLLM_HSDIV_CLKDIV(idx)        ((idx * 0x1000U) + 0x120U)
#define ADPLLM_HSDIV_CTRL(idx)          ((idx * 0x1000U) + 0x124U)
#define ADPLLM_HSDIV_STAT(idx)          ((idx * 0x1000U) + 0x128U)
#define ADPLLM_HSDIV_PWR_CTRL(idx)      ((idx * 0x1000U) + 0x12cU)
#define ADPLLM_HSDIV_PWR_STAT(idx)      ((idx * 0x1000U) + 0x130U)

/* PLL_KICK */
#define ADPLLM_PLL_KICK0_VALUE          0x68ef3490U
#define ADPLLM_PLL_KICK1_VALUE          0xd172bc5aU

/* FREQ_CTRL0 */
/*
 * Sigma-Delta Divider is REGSD = ceil(((M+M.f)/(N+1)) * CLKINP / 250000000)
 * Ensure that the sigma-delta operation frequency is as close as possible,
 * but less than 250MHz for optimal performance.
 */
#define ADPLLM_FREQ_CTRL0_SD_DIV_SHIFT          24U    /* ADPLLJ only */
#define ADPLLM_FREQ_CTRL0_SD_DIV_MASK           (((u32) 0xffU) << 24U)
/*
 * Feedback Multiplier is REGM
 * Can be loaded on the fly if N is not changed and value does not change
 * by more than 3%.
 */
#define ADPLLM_FREQ_CTRL0_M_INT_MULT_SHIFT      8U
#define ADPLLM_FREQ_CTRL0_M_INT_MULT_MASK       (((u32) 0xfffU) << 8U)
/* Input divider */
#define ADPLLM_FREQ_CTRL0_N_DIV_SHIFT           0U
#define ADPLLM_FREQ_CTRL0_N_DIV_MASK            ((u32) 0xffU)

/*
 * FREQ_CTRL1
 * DCO frequency range selector, ADPLLJ only
 */
#define ADPLLM_FREQ_CTRL1_SEL_FREQ_DCO_SHIFT    24U
#define ADPLLM_FREQ_CTRL1_SEL_FREQ_DCO_MASK     (((u32) 7U) << 24U)
#define ADPLLM_FREQ_CTRL1_SEL_FREQ_DCO_1500MHZ  (((u32) 2U) << 24U)
#define ADPLLM_FREQ_CTRL1_SEL_FREQ_DCO_2500MHZ  (((u32) 4U) << 24U)
/*
 * Fractional part of the M divider, 0 disables.
 * Fractional multiplier M.f is not supported for M < 20 and M > 2045 for type
 * ADPLLM.
 * Fractional multiplier M.f is not supported for M > 4093 for type ADPLLJ.
 */
#define ADPLLM_FREQ_CTRL1_M_FRAC_MULT_SHIFT     0U
#define ADPLLM_FREQ_CTRL1_M_FRAC_MULT_BITS      18U
#define ADPLLM_FREQ_CTRL1_M_FRAC_MULT_MASK      0x3ffffU

/* CLKDIV */
/* Additional divider is REGM3 */
#define ADPLLM_CLKDIV_M3_DIV_SHIFT              16U    /* ADPLLM only */
#define ADPLLM_CLKDIV_M3_DIV_MASK               (((u32) 0x1fU) << 16U)
/* Post divider is REGM2 */
#define ADPLLM_CLKDIV_M2_DIV_SHIFT              8U
#define ADPLLM_CLKDIV_M2_DIV_MASK               (((u32) 0x7fU) << 8U)
/* Bypass divider is REGM1+1 */
#define ADPLLM_CLKDIV_M1_DIV_SHIFT              0U
#define ADPLLM_CLKDIV_M1_DIV_MASK               ((u32) 0xfU)

/* PROG */
/* Core RegM3/M2/M1 and HSDIV load enable (L-H). Should return to low. */
#define ADPLLM_PROG_TENABLEDIV                  BIT(8)
/* Core RegM/N/SD/REQDCO load enable (L-H). Sholud return to low. */
#define ADPLLM_PROG_TENABLE                     BIT(1)
/* Core soft reset (lock sequence initialization: HLH). */
#define ADPLLM_PROG_TINIT_Z                     BIT(0)

/* CTRL */
/* Enables clock spreading. */
#define ADPLLM_CTRL_SSC_EN                      BIT(31)
/*
 * '1' enables low frequency spread only
 * '0' enables both side frequency spread about the programmed frequency.
 */
#define ADPLLM_CTRL_DOWNSPREAD                  BIT(30)
/*
 * DCC. Set to '1' when CLKOUT > 1.4GHz required. M2 must be set to 1.
 * Change requires PLL reinitialization.
 */
#define ADPLLM_CTRL_DC_CORRECTOR_EN             BIT(27) /* ADPLLM only */
/* '0' decrease bandwidth, '1' increase bandwidth */
#define ADPLLM_CTRL_BW_INCR_DECRZ               BIT(26) /* ADPPLJ only */
/*
 * Can be used to reduce BW from default (Nominal Fref/50) to reduce jitter in
 * FractionalM usage.
 */
#define ADPLLM_CTRL_BW_CONTROL_SHIFT            24U
#define ADPLLM_CTRL_BW_CONTROL_MASK             (((u32) 0x3U) << 24U)
/* BW=Fref/30 to Fref/70. */
#define ADPLLM_CTRL_BW_CONTROL_FREF30_FREF70    (((u32) 0x0U) << 24U)
/* BW=Fref/60 to Fref/140. */
#define ADPLLM_CTRL_BW_CONTROL_FREF60_FREF140   (((u32) 0x1U) << 24U)
/* BW=Fref/120 to Fref/280. */
#define ADPLLM_CTRL_BW_CONTROL_FREF120_FREF280  (((u32) 0x2U) << 24U)
/* BW=Fref/240 to Fref/560. */
#define ADPLLM_CTRL_BW_CONTROL_FREF240_FREF560  (((u32) 0x3U) << 24U)
/* Can be set with VCO input <= 1MHz and VCO output <= 100MHz */
#define ADPLLM_CTRL_LPMODE                      BIT(23) /* ADPLLM only */
/* FREQLOCK asserted when DC frequency error < 1% on '0', < 2% on '1' */
#define ADPLLM_CTRL_RELAXED_LOCK                BIT(22)
/* When in Idle / Lossclk. '0' Fast relock, '1' Slow relock */
#define ADPLLM_CTRL_LOWCURRSTDBY                BIT(21) /* ADPLLM only */
/*
 * FailSafe enable to trigger re-calibration in case CycleSlip occurs between
 * REFCLK & FBCLK. Cycleslip could be caused if loop is not able to track input
 * clock.
 */
#define ADPLLM_CTRL_CYCLESLIP_EN                BIT(20)
/* Clock ramping happens both during initial lock and relock */
#define ADPLLM_CTRL_RELOCK_RAMP_EN              BIT(19) /* ADPLLM only */
/* Input to the M3 divider is the: '0' internal osc, '1' external CLKINPHIF */
#define ADPLLM_CTRL_CLKINPHIF_SEL               BIT(18) /* ADPLLM only */
/*
 * Enable REGM*4. Can be set to only for CLKOUT*M2 > 850MHz when LPMODE = '0'
 * & CLKOUT*M2 > 80MHz when LPMODE = '1'. This bit should not be changed on
 * the fly in locked condition. INITIALIZATION should follow change of this bit.
 */
#define ADPLLM_CTRL_REGM4X_EN                   BIT(17) /* ADPLLM only */
/* When RECAL status flag is asserted, Enables recalibration */
#define ADPLLM_CTRL_DRIFTGUARD_EN               BIT(16) /* ADPLLM only */
/* In bypass, CLKOUT = CLKINPULOW/(M1+1) if set, CLKOUT = CLKINP otherwise */
#define ADPLLM_CTRL_ULOWCLK_EN                  BIT(15)
/* Controls the ramp sequence. */
#define ADPLLM_CTRL_CLKRAMP_LEVEL_SHIFT         12UL    /* ADPLLM only */
#define ADPLLM_CTRL_CLKRAMP_LEVEL_MASK          (((u32) 0x3U) << 12U)
/* No ramping */
#define ADPLLM_CTRL_CLKRAMP_LEVEL_NONE          (((u32) 0x0U) << 12U)
/* Fout/8, Fout/4, Fout/2, Fout */
#define ADPLLM_CTRL_CLKRAMP_LEVEL_8_4_2_1       (((u32) 0x1U) << 12U)
/* Fout/4, Fout/2, Fout/1.5, Fout */
#define ADPLLM_CTRL_CLKRAMP_LEVEL_4_2_15_1      (((u32) 0x2U) << 12U)
/* Controls the time spent on each ramp step */
#define ADPLLM_CTRL_CLKRAMP_RATE_SHIFT          8U     /* ADPLLM only */
#define ADPLLM_CTRL_CLKRAMP_RATE_MASK           (((u32) 0x7U) << 8U)
#define ADPLLM_CTRL_CLKRAMP_RATE_2_REFCLKS      (((u32) 0x0U) << 8U)
#define ADPLLM_CTRL_CLKRAMP_RATE_4_REFCLKS      (((u32) 0x1U) << 8U)
#define ADPLLM_CTRL_CLKRAMP_RATE_8_REFCLKS      (((u32) 0x2U) << 8U)
#define ADPLLM_CTRL_CLKRAMP_RATE_16_REFCLKS     (((u32) 0x3U) << 8U)
#define ADPLLM_CTRL_CLKRAMP_RATE_32_REFCLKS     (((u32) 0x4U) << 8U)
#define ADPLLM_CTRL_CLKRAMP_RATE_64_REFCLKS     (((u32) 0x5U) << 8U)
#define ADPLLM_CTRL_CLKRAMP_RATE_128_REFCLKS    (((u32) 0x6U) << 8U)
#define ADPLLM_CTRL_CLKRAMP_RATE_512_REFCLKS    (((u32) 0x7U) << 8U)
/* Sets PLL to Idle mode */
#define ADPLLM_CTRL_IDLE                        BIT(7)
/* Synchronously enables/disables CLKOUTLDO */
#define ADPLLM_CTRL_CLKOUTLDO_EN                BIT(5)  /* ADPLLJ only */
/* Synchronously enables/disables CLKDCOLDO */
#define ADPLLM_CTRL_CLKDCOLDO_EN                BIT(4)
/* Synchronously enables/disables CLKOUT */
#define ADPLLM_CTRL_CLKOUT_EN                   BIT(3)
/* Synchronously enables/disables CLKOUTX2 */
#define ADPLLM_CTRL_CLKOUTX2_EN                 BIT(2)  /* ADPLLM only */
/* Synchronously enables/disables CLKOUTHIF */
#define ADPLLM_CTRL_CLKOUTHIF_EN                BIT(1)  /* ADPLLM only */
/* Synchronously enables/disables CLKOUTBYPASS */
#define ADPLLM_CTRL_CLKOUTBYPASS_EN             BIT(0)

/* STAT */
/* Spread-spectrum Clocking is enabled on output clocks */
#define ADPLLM_STAT_SSC_ACK                     BIT(31)
/* Reserved */
#define ADPLLM_STAT_RECAL                       BIT(27)
/* Reference input loss acknowledge */
#define ADPLLM_STAT_LOSSREF                     BIT(26)
/* PLL in frequency lock condition. */
#define ADPLLM_STAT_FREQLOCK                    BIT(25)
/* PLL in phase lock condition. */
#define ADPLLM_STAT_PHASELOCK                   BIT(24)
/*
 * High indicates jitter. After PHASELOCK is asserted high, the HIGHJITTER
 * flag is asserted high if phase error between REFCLK & FBCLK > 24%.
 */
#define ADPLLM_STAT_HIGHJITTER                  BIT(23)
/* ADPLL internal loop lock status. */
#define ADPLLM_STAT_LOCK2                       BIT(22)
/* Goes high after PHASELOCK. Goes low if jitter is high. */
#define ADPLLM_STAT_TVALID                      BIT(20)
/* ? */
#define ADPLLM_STAT_BYPASS_CLKTYPE              BIT(15)
/* Acknowledge flag indicating on the fly change of M3 divider is done */
#define ADPLLM_STAT_M3CHANGE_ACK                BIT(11) /* ADPLLM only */
/* Acknowledge flag indicating on the fly change of M2 divider is done */
#define ADPLLM_STAT_M2CHANGE_ACK                BIT(10)
/* Acknowledge flag indicating on the fly change of M1 divider is done */
#define ADPLLM_STAT_M1CHANGE_ACK                BIT(9)
/* Bypass status signal. '1'  CLKOUT in bypass */
#define ADPLLM_STAT_BYPASS_ACK                  BIT(7)
/* Indicates enable/disable condition of CLKOUTLDO */
#define ADPLLM_STAT_CLKOUTLDO_EN_ACK            BIT(5)  /* ADPLLJ only */
/* Indicates enable/disable condition of CLKDCOLDO */
#define ADPLLM_STAT_CLKDCOLDO_EN_ACK            BIT(4)
/* Indicates enable/disable condition of CLKOUT */
#define ADPLLM_STAT_CLKOUT_EN_ACK               BIT(3)
/* Indicates enable/disable condition of CLKOUTX2 */
#define ADPLLM_STAT_CLKOUTX2_EN_ACK             BIT(2)  /* ADPLLM only */
/* Indicates enable/disable condition of CLKOUTHIF */
#define ADPLLM_STAT_CLKOUTHIF_EN_ACK            BIT(1)  /* ADPLLM only */
/* Indicates enable/disable condition of CLKOUTBYPASS */
#define ADPLLM_STAT_CLKOUTBYPASS_EN_ACK         BIT(0)

/* PWR_CTRL */
/* ON/OFF control of the weak power-switch in digital. */
#define ADPLLM_PWR_CTRL_PONIN                   BIT(31)
/* ON/Off control of the strong power-switch in digital. */
#define ADPLLM_PWR_CTRL_PGOODIN                 BIT(30)
/* Save/Restore control for Retention mode. */
#define ADPLLM_PWR_CTRL_RET                     BIT(23)
/* Save/Restore control for Isolation of the Scanout pins. */
#define ADPLLM_PWR_CTRL_ISO_SCAN                BIT(21)
/* Save/Restore control for Isolation of output pins. */
#define ADPLLM_PWR_CTRL_ISORET                  BIT(20)
/* Clear signal for Isolation latches in design controlled by ISOSCAN i/p */
#define ADPLLM_PWR_CTRL_ISOSCAN_CLR             BIT(17)
/* Clear signal for Isolation latches in design controlled by ISORET i/p */
#define ADPLLM_PWR_CTRL_ISO_CLR                 BIT(16)
/*
 * Indicates VDD is inactive while VDDA is active. Used to switch OFF the logic
 * on VDDA.
 */
#define ADPLLM_PWR_CTRL_OFFMODE                 BIT(8)
/* Active Low Presetz for Isolation latches controlled by ISOSCAN i/p */
#define ADPLLM_PWR_CTRL_ISOSCAN_PRE_Z           BIT(1)
/* Active Low Presetz for Isolation latches controlled by ISORET i/p */
#define ADPLLM_PWR_CTRL_ISO_PRE_Z               BIT(0)

/* PWR_STAT */
/* ON/OFF status of the weak power-switch in digital to SOC. */
#define ADPLLM_PWR_STAT_PONOUT                  BIT(31)
/* ON/OFF status of the strong power-switch in digital to SOC. */
#define ADPLLM_PWR_STAT_PGOODOUT                BIT(30)
/* Core DCO power down status. */
#define ADPLLM_PWR_STAT_TICOPWDN                BIT(15)
/*
 * Indicates DPLL internal LDO is power down. VDDLDOOUT will be un-defined in
 * this condition.
 */
#define ADPLLM_PWR_STAT_LDOPWDN                 BIT(14)

/*
 * SS_SPREAD
 * Integer part of Frequency Spread control.
 */
#define ADPLLM_SS_SPREAD_DELTA_MSTEP_INT_SHIFT  24U
#define ADPLLM_SS_SPREAD_DELTA_MSTEP_INT_MASK   (((u32) 0x7U) << 24U)
/* Fraction part of Frequency Spread control. */
#define ADPLLM_SS_SPREAD_DELTA_MSTEP_FRAC_SHIFT 0U
#define ADPLLM_SS_SPREAD_DELTA_MSTEP_FRAC_MASK  0x3ffffU

/*
 * SS_MODFREQ
 * Exponent of the REFCLK divider to define the modulation frequency.
 */
#define ADPLLM_SS_MODFREQ_DIV_EXP_SHIFT         8U
#define ADPLLM_SS_MODFREQ_DIV_EXP_MASK          (((u32) 0x7U) << 8U)
/* Mantissa of the REFCLK divider to define the modulation frequency.  */
#define ADPLLM_SS_MODFREQ_DIV_MANT_SHIFT        0U
#define ADPLLM_SS_MODFREQ_DIV_MANT_MASK         ((u32) 0x7fU)

/*
 * CLKNIP -		Control clock.
 * CLKINPHIFLDO -	High frequency input clock.
 * CLKINBYPASS -	Bypass input clock.
 * CLKOUTx -		High speed divider output clock.
 */

/* HSDIV_CLKDIV */
/*
 * Divide value = DIVx+1. Max DIVx=62 for DIV1,DIV2, DIV3. For DIV4 value of
 * 63 is mapped to divide by 2.5 (idx 1-4)
 */
#define ADPLLM_HSDIV_CLKDIV_SHIFT(n)            ((n - 1U) * 8U)
#define ADPLLM_HSDIV_CLKDIV_MASK(n)             (((u32) 0x3fU) << ((n - 1U) * 8U))

#define ADPLLM_HSDIV_CTRL_TENABLEDIV            BIT(31)
#define ADPLLM_HSDIV_CTRL_SPAREIN_SHIFT         16U
#define ADPLLM_HSDIV_CTRL_SPAREIN_MASK          (((u32) 0x1fU) << 16U)

/* Synchronously enables/disables CLKOUTx (idx 1-4) */
#define ADPLLM_HSDIV_CTRL_CLKOUT_EN(n)          BIT((n - 1U))
/* HSDIV_STAT */
#define ADPLLM_HSDIV_STAT_SPAREOUT_SHIFT        16U
#define ADPLLM_HSDIV_STAT_SPAREOUT_MASK         (((u32) 0x1fU) << 16U)

/*
 * Indicates all the active output clocks (respective CLKOUTxEN='1') are out
 * of bypass & giving out the divider output clock.
 */
#define ADPLLM_HSDIV_STAT_LOCK                  BIT(15)
/* Acknowledge flag indicating on the fly change of DIVx divider is done */
#define ADPLLM_HSDIV_STAT_DIV_CHANGE_ACK(n)     BIT((n) + 8)
/* Indicates enable/disable condition of CLKOUTx */
#define ADPLLM_HSDIV_STAT_CLKOUT_EN_ACK(n)      BIT((n))

/* HSDIV_PWR_CTRL */
/* Poweron control for weak power switch */
#define ADPLLM_HSDIV_PWR_CTRL_PONIN             BIT(31)
/* Poweron control for strong power switch */
#define ADPLLM_HSDIV_PWR_CTRL_PGOODIN           BIT(30)
/* RET control for the retention cells */
#define ADPLLM_HSDIV_PWR_CTRL_RET               BIT(23)
/* ISO control of the isolation cells */
#define ADPLLM_HSDIV_PWR_CTRL_ISO               BIT(20)
/* ISOCLR control of the isolation cells */
#define ADPLLM_HSDIV_PWR_CTRL_ISO_CLR           BIT(16)
/*
 * Indicates VDD is inactive while VDDA is active. Used to switch OFF the
 * logic on VDDA.
 */
#define ADPLLM_HSDIV_PWR_CTRL_AIPOFF            BIT(8)
/* ISOPREZ control of the isolation cells (active low) */
#define ADPLLM_HSDIV_PWR_CTRL_ISO_PRE_Z         BIT(0)

/* HSDIV_PWR_STAT */
/* Poweron acknowledge from weak power switch */
#define ADPLLM_HSDIV_PWR_STAT_PONOUT            BIT(31)
/* Poweron acknowledge from strong power switch */
#define ADPLLM_HSDIV_PWR_STAT_PGOODOUT          BIT(30)
/* LDO power down acknowledge */
#define ADPLLM_HSDIV_PWR_STAT_LPOPWDN           BIT(14)

static struct clk *clk_adpllm_hsdiv_get_pll_root(struct clk *clkp);

struct adpllm_program_data {
	/** The ADPLLM clock */
	struct clk				*pll_clk;
	/** The output divider associated with this PLL */
	struct clk				*clkout_clk;
	/** Program a frequency closest to this frequency */
	u32					target_hz;
	/** Frequency must be at least min_hz */
	u32					min_hz;
	/** Frequency must be less than max_hz + 1 */
	u32					max_hz;
	/** Set to STRUE if changes were made */
	sbool					*changed;
	/** The const data that describes the ADPLLM */
	const struct clk_data_pll_adpllm	*pll;
	/** The frequency of the parent clock in Hz */
	u32					parent_freq_hz;
	/** The input divider */
	u32					plld;
	/** The VCO/DCO multiplier (not including m4x/dcc multiplier) */
	u32					pllm;
	/** The VCO/DCO fractional multiplier */
	u32					pllfm;
	/** The output divider (not including dcc divider) */
	u32					clkod;
	/** STRUE if M4X mode is enabled */
	sbool					m4x;
	/** STRUE if the DC corrector is enabled */
	sbool					dcc;
	/**
	 * STRUE to verify that such a frequency change is possible, SFALSE
	 * to program the new frequency.
	 */
	sbool					query;
};

/*
 * \brief Check if the ADPLLM pllm value is valid (dcc on)
 *
 * \param clkp The ADPLLM clock.
 * \param pllm The multiplier value
 *
 * \return STRUE if pllm value is valid, SFALSE otherwise
 */
static sbool adpllm_dcc_pllm_valid(struct clk *clkp UNUSED, u32 pllm, sbool is_frac)
{
	sbool ret = STRUE;
	sbool m4x;

	/* For values above 2047, M4X must be on. */
	m4x = pllm > 2047UL;

	/* Fractional multiplier M.f is not supported for M < 20 and M > 2045 */
	if (is_frac) {
		if (m4x) {
			if ((pllm < (20UL * 4UL)) || (pllm > (2045UL * 4UL))) {
				ret = SFALSE;
			}
		} else {
			if ((pllm < 20UL) || (pllm > 2045UL)) {
				ret = SFALSE;
			}
		}
	}

	if (pllm <= 1UL) {
		/* pllm must greater than 1 */
		ret = SFALSE;
	} else if (m4x) {
		/* Only values divisible by 4 are permitted when M4X is on */
		if ((pllm & 3UL) != 0UL) {
			ret = SFALSE;
		}
	}

	return ret;
}

/*
 * \brief Return the stride size for a give pllm (dcc on)
 *
 * \param clkp The ADPLLM clock.
 * \param pllm The multiplier value
 *
 * \return stride size between multipliers
 */
static u32 adpllm_dcc_pllm_stride(struct clk *clkp UNUSED, u32 pllm)
{
	u32 ret;

	if (pllm > 2047UL) {
		ret = 4U;
	} else {
		ret = 1U;
	}

	return ret;
}

static s32 adpllm_bin(struct clk *clkp UNUSED, u32 plld UNUSED,
		      u32 pllm UNUSED, sbool do_frac, u32 clkod UNUSED)
{
	/* Prefer a non-fractional configuration if possible */
	s32 ret;

	if (do_frac) {
		ret = 0;
	} else {
		ret = 1;
	}
	return ret;
}

/* Prefer higher VCO frequencies */
static u32 adpllm_vco_fitness(struct clk *clkp UNUSED, u32 vco, sbool do_frac UNUSED)
{
	return vco;
}

static const struct pll_data adpllm_dcc_data = {
	.plld_max	= 128U,
	.pllm_max	= 8188U,
	.pllfm_bits	= ADPLLM_FREQ_CTRL1_M_FRAC_MULT_BITS,
	.clkod_max	= 1U,
	.pllm_valid	= adpllm_dcc_pllm_valid,
	.pllm_stride	= adpllm_dcc_pllm_stride,
	.bin		= adpllm_bin,
	.vco_fitness	= adpllm_vco_fitness,
};

/*
 * \brief Check if the ADPLLM pllm value is valid (dcc off)
 *
 * \param clkp The ADPLLM clock.
 * \param pllm The multiplier value
 *
 * \return STRUE if pllm value is valid, SFALSE otherwise
 */
static sbool adpllm_pllm_valid(struct clk *clkp UNUSED, u32 pllm, sbool is_frac)
{
	sbool ret = STRUE;
	sbool m4x;

	/* For values above 4094, M4X must be on. */
	m4x = pllm > (2047UL * 2UL);

	/* Fractional multiplier M.f is not supported for M < 20 and M > 2045 */
	if (is_frac) {
		if (m4x) {
			if ((pllm < (20UL * 8UL)) || (pllm > (2045UL * 8UL))) {
				ret = SFALSE;
			}
		} else {
			if ((pllm < (20UL * 2UL)) || (pllm > (2045UL * 2UL))) {
				ret = SFALSE;
			}
		}
	}

	if (pllm <= (1UL * 2UL)) {
		/* pllm must greater than 2 */
		ret = SFALSE;
	} else if (m4x) {
		/* Only values divisible by 8 are permitted when M4X is on */
		if ((pllm & 7UL) != 0UL) {
			ret = SFALSE;
		}
	} else {
		/* Only even numbers are allowed. */
		if ((pllm & 1UL) != 0UL) {
			ret = SFALSE;
		}
	}

	return ret;
}

/*
 * \brief Return the stride size for a give pllm (dcc off)
 *
 * \param clkp The ADPLLM clock.
 * \param pllm The multiplier value
 *
 * \return stride size between multipliers
 */
static u32 adpllm_pllm_stride(struct clk *clkp UNUSED, u32 pllm)
{
	u32 ret;

	if (pllm > (2047UL * 2UL)) {
		ret = 4UL * 2UL;
	} else {
		ret = 1UL * 2UL;
	}

	return ret;
}

/*
 * \brief Check if the ADPLLM clkod value is valid.
 *
 * \param clkp The ADPLLM clock.
 * \param clkod The clock output divider value
 *
 * \return STRUE if clkod value is valid, SFALSE otherwise
 */
static sbool adpllm_clkod_valid(struct clk *clkp UNUSED, u32 clkod)
{
	/* Only even numbers are allowed. */
	return !(clkod & 1UL);
}

static const struct pll_data adpllm_data = {
	.plld_max	= 128U,
	.pllm_max	= 16376U,
	.pllfm_bits	= ADPLLM_FREQ_CTRL1_M_FRAC_MULT_BITS,
	.clkod_max	= 62U,
	.pllm_valid	= adpllm_pllm_valid,
	.pllm_stride	= adpllm_pllm_stride,
	.clkod_valid	= adpllm_clkod_valid,
	.bin		= adpllm_bin,
	.vco_fitness	= adpllm_vco_fitness,
};

/*
 * \brief Check if the ADPLLLJM pllm value is valid.
 *
 * \param clkp The ADPLLLJM clock.
 * \param pllm The multiplier value
 *
 * \return STRUE if pllm value is valid, SFALSE otherwise
 */
static sbool adpllj_pllm_valid(struct clk *clkp UNUSED, u32 pllm, sbool is_frac)
{
	sbool ret = STRUE;

	/* Fractional multiplier M.f is not supported for M > 4093 */
	if (is_frac && (pllm > 4093UL)) {
		ret = SFALSE;
	}

	/* pllm must greater than 1 */
	if (pllm <= 1UL) {
		ret = SFALSE;
	}

	return ret;
}

static const struct pll_data adpllj_data = {
	.plld_max	= 256U,
	.pllm_max	= 4095U,
	.pllfm_bits	= ADPLLM_FREQ_CTRL1_M_FRAC_MULT_BITS,
	.clkod_max	= 127U,
	.pllm_valid	= adpllj_pllm_valid,
	.bin		= adpllm_bin,
	.vco_fitness	= adpllm_vco_fitness,
};

/*
 * \brief Check if the ADPLLM VCO/DCO is locked.
 *
 * \param clkp The ADPLLM clock.
 *
 * \return STRUE if VCO/DCO is locked, SFALSE otherwise
 */
static sbool clk_adpllm_check_lock(struct clk *clkp)
{
	const struct clk_data *clk_datap;
	const struct clk_data_pll_adpllm *pll;
	const struct clk_data_pll *data_pll;
	u32 stat;

	clk_datap = clk_get_data(clkp);
	data_pll = container_of(clk_datap->data, const struct clk_data_pll,
				data);
	pll = container_of(data_pll, const struct clk_data_pll_adpllm,
			   data_pll);

	stat = readl(pll->base + ADPLLM_PLL_STAT(pll->idx));
	return (stat & ADPLLM_STAT_FREQLOCK) != 0UL;
}

/*
 * \brief Wait for ADPLLM VCO/DCO to lock.
 *
 * \param clkp The ADPLLM clock.
 *
 * \return STRUE if VCO/DCO is now locked, SFALSE if there was a timeout.
 */
static sbool clk_adpllm_wait_for_lock(struct clk *clkp)
{
	s32 i;

	/* FIXME: Proper delay */
	for (i = 0; i < 100; i++) {
		if (clk_adpllm_check_lock(clkp)) {
			break;
		}
	}

	/* FIXME: PLL failed to lock */
	return i != 100;
}

/*
 * \brief Query bypass state of ADPLLM
 *
 * \param clkp The ADPLLM clock.
 *
 * \return STRUE if ADPLLM is in IDLE or MN bypass.
 */
static sbool clk_adpllm_is_bypass(struct clk *clkp)
{
	const struct clk_data *clk_datap;
	const struct clk_data_pll_adpllm *pll;
	const struct clk_data_pll *data_pll;
	sbool ret = SFALSE;
	u32 ctrl;

	clk_datap = clk_get_data(clkp);
	data_pll = container_of(clk_datap->data, const struct clk_data_pll,
				data);
	pll = container_of(data_pll, const struct clk_data_pll_adpllm,
			   data_pll);

	/* IDLE Bypass */
	ctrl = readl(pll->base + ADPLLM_PLL_CTRL(pll->idx));
	ret = (ctrl & ADPLLM_CTRL_IDLE) != 0UL;

	/* MN Bypass */
	if (!ret) {
		u32 pllm;
		pllm = readl(pll->base + ADPLLM_PLL_FREQ_CTRL0(pll->idx));
		pllm &= ADPLLM_FREQ_CTRL0_M_INT_MULT_MASK;
		pllm >>= ADPLLM_FREQ_CTRL0_M_INT_MULT_SHIFT;
		ret = pllm <= 1UL;
	}

	return ret;
}

/*
 * \brief Enable/disable ADPLLM bypass mode.
 *
 * Puts the ADPLLM in and out of IDLE bypass mode. Can also take ADPLLM
 * out of MN bypass mode.
 *
 * \param clkp The ADPLLM clock.
 * \param bypass STRUE to enable bypass, SFALSE disable.
 */
static void clk_adpllm_bypass(struct clk *clkp, sbool bypass)
{
	const struct clk_data *clk_datap;
	const struct clk_data_pll_adpllm *pll;
	const struct clk_data_pll *data_pll;
	u32 ctrl;
	u32 freq_ctrl0 = 0U;
	u32 prog;
	sbool load_freq_ctrl0 = SFALSE;

	clk_datap = clk_get_data(clkp);
	data_pll = container_of(clk_datap->data, const struct clk_data_pll,
				data);
	pll = container_of(data_pll, const struct clk_data_pll_adpllm,
			   data_pll);

	ctrl = readl(pll->base + ADPLLM_PLL_CTRL(pll->idx));
	if (bypass) {
		/* Enable IDLE Bypass */
		ctrl |= ADPLLM_CTRL_IDLE;
	} else {
		u32 pllm;

		/* Disable IDLE Bypass */
		ctrl &= ~ADPLLM_CTRL_IDLE;
		writel(ctrl, pll->base + ADPLLM_PLL_CTRL(pll->idx));

		/* Disable MN Bypass, make sure we have a valid pllm value */
		freq_ctrl0 = readl(pll->base + ADPLLM_PLL_FREQ_CTRL0(pll->idx));
		pllm = freq_ctrl0 & ADPLLM_FREQ_CTRL0_M_INT_MULT_MASK;
		pllm >>= ADPLLM_FREQ_CTRL0_M_INT_MULT_SHIFT;
		if (pllm <= 1UL) {
			pllm = 2U;
			freq_ctrl0 &= ADPLLM_FREQ_CTRL0_M_INT_MULT_MASK;
			freq_ctrl0 |= pllm << ADPLLM_FREQ_CTRL0_M_INT_MULT_SHIFT;
			load_freq_ctrl0 = STRUE;
		}
	}
	writel(ctrl, pll->base + ADPLLM_PLL_CTRL(pll->idx));

	/* Toggle TINIT_Z */
	prog = readl(pll->base + ADPLLM_PLL_PROG(pll->idx));
	prog &= ~ADPLLM_PROG_TINIT_Z;
	writel(prog, pll->base + ADPLLM_PLL_PROG(pll->idx));
	prog |= ADPLLM_PROG_TINIT_Z;
	writel(prog, pll->base + ADPLLM_PLL_PROG(pll->idx));

	if (load_freq_ctrl0) {
		/* Program new M value */
		writel(freq_ctrl0, pll->base + ADPLLM_PLL_FREQ_CTRL0(pll->idx));

		/* Load new M value, L-H-L */
		prog &= ~ADPLLM_PROG_TENABLE;
		writel(prog, pll->base + ADPLLM_PLL_PROG(pll->idx));
		prog |= ADPLLM_PROG_TENABLE;
		writel(prog, pll->base + ADPLLM_PLL_PROG(pll->idx));
		prog &= ~ADPLLM_PROG_TENABLE;
		writel(prog, pll->base + ADPLLM_PLL_PROG(pll->idx));
	}
}

/*
 * \brief Get the output frequency of the ADPLLM
 *
 * The ADPLLM has various outputs including clkout and the HSDIV outputs, each
 * with their own divider. Normally we could have a single function used by
 * both that just returns the VCO/DCO frequency, but we also want to include
 * the fractional multiplier which necessitates a 64 bit calculation. Including
 * the output divider in that calculation insures we only have to perform
 * one 64 bit division and that we also don't lose any precision.
 *
 * \param clkp The ADPLLM clock
 * \param multiplier Additional multiplier to be applied to parent frequency
 * \param clkod The output clock divider
 *
 * \return The output frequency in Hz.
 */
static u32 clk_adpllm_get_freq_internal(struct clk *clkp,
					u32 multiplier, u32 clkod)
{
	const struct clk_data *clk_datap = clk_get_data(clkp);
	const struct clk_data_pll_adpllm *pll;
	const struct clk_data_pll *data_pll;
	u32 freq_ctrl0;
	u32 freq_ctrl1;
	u32 ctrl;
	u32 plld;
	u32 pllm;
	u32 pllfm;
	u64 ret;        /* Return frequency */
	u64 rem;        /* Return frequency remainder */
	sbool m4x;      /* M4X enabled */
	sbool dcc;      /* DC corrector enabled */
	u32 clkod_plld; /* clkod * plld */
	u32 parent_freq_hz;

	data_pll = container_of(clk_datap->data, const struct clk_data_pll,
				data);
	pll = container_of(data_pll, const struct clk_data_pll_adpllm,
			   data_pll);

	/* Read reg values */
	if (pll->ljm) {
		/* ADPLLJ */
		m4x = SFALSE;
		dcc = STRUE;
	} else {
		/* ADPLLM */
		ctrl = readl(pll->base + ADPLLM_PLL_CTRL(pll->idx));
		m4x = (ctrl & ADPLLM_CTRL_REGM4X_EN) != 0UL;
		dcc = (ctrl & ADPLLM_CTRL_DC_CORRECTOR_EN) != 0UL;
	}

	freq_ctrl0 = readl(pll->base + ADPLLM_PLL_FREQ_CTRL0(pll->idx));
	freq_ctrl1 = readl(pll->base + ADPLLM_PLL_FREQ_CTRL1(pll->idx));

	pllm = freq_ctrl0 & ADPLLM_FREQ_CTRL0_M_INT_MULT_MASK;
	pllm >>= ADPLLM_FREQ_CTRL0_M_INT_MULT_SHIFT;

	plld = freq_ctrl0 & ADPLLM_FREQ_CTRL0_N_DIV_MASK;
	plld >>= ADPLLM_FREQ_CTRL0_N_DIV_SHIFT;
	plld += 1U;

	pllfm = freq_ctrl1 & ADPLLM_FREQ_CTRL1_M_FRAC_MULT_MASK;
	pllfm >>= ADPLLM_FREQ_CTRL1_M_FRAC_MULT_SHIFT;

	if (m4x) {
		multiplier *= 4U;
	}

	if (!dcc) {
		multiplier *= 2U;
	}

	/* Calculate non-fractional part */
	clkod_plld = clkod * plld;
	parent_freq_hz = clk_get_parent_freq(clkp);
	ret = ((u64) (parent_freq_hz / clkod_plld)) * pllm * multiplier;
	rem = ((u64) (parent_freq_hz % clkod_plld)) * pllm * multiplier;
	if (rem > (u64) ULONG_MAX) {
		/*
		 * Remainder should always fit within 32 bits. We add this
		 * in case of a programming error or unexpected input.
		 *
		 * clkod_plld - 16 bits
		 * pllm -       12 bits
		 * multiplier -	4 bits (up to 8)
		 * total -	32 bits (should not need div64)
		 */
		ret += pm_div64(&rem, clkod_plld);
	} else {
		ret += ((u32) rem) / clkod_plld;
		rem = ((u32) rem) % clkod_plld;
	}


	if (pllfm != 0UL) {
		u64 fret;       /* Fraction return value */
		u64 frem;       /* Fraction remainder */
		const u32 mask = (1UL << ADPLLM_FREQ_CTRL1_M_FRAC_MULT_BITS) -
				 1UL;

		/* Calculate fractional component of frequency */
		fret = ((u64) (parent_freq_hz / clkod_plld)) * pllfm;
		frem = ((u64) (parent_freq_hz % clkod_plld)) * pllfm;
		if (frem > (u64) ULONG_MAX) {
			/*
			 * pllfm - 18 bits
			 * clkod_plld - 16 bits
			 * total - 34 bits
			 *
			 * NB: Overflow, 64 bit div may be required.
			 *
			 * plld max - 127
			 * clkod max - 256
			 * pllfm max - 0x3ffff
			 * max clkod = ULONG_MAX / (0x3ffff * 127) = 129
			 *
			 * Max remainder combined with plld max gives a max
			 * clkod value of 129. Any values over 100 lead to
			 * an invalid combination of CLKIN/REFCLK
			 * frequences. Hitting this div64 is very unlikely,
			 * but possible.
			 */
			fret += pm_div64(&frem, clkod_plld);
		} else {
			fret += ((u32) frem) / clkod_plld;
			frem = ((u32) frem) % clkod_plld;
		}

		/* Fold in multiplier */
		fret *= multiplier;
		frem *= multiplier;
		if (frem > ULONG_MAX) {
			/*
			 * clkod_plld - 16 bits
			 * multiplier - 4 bits
			 * total - 20 bits (should not need div64)
			 */
			fret += pm_div64(&frem, clkod_plld);
		} else {
			fret += ((u32) frem) / clkod_plld;
			frem = ((u32) frem) % clkod_plld;
		}

		/* Fold back into return/remainder */
		frem += (fret & (u64) mask) * clkod_plld;

		ret += fret >> ADPLLM_FREQ_CTRL1_M_FRAC_MULT_BITS;
		rem += frem >> ADPLLM_FREQ_CTRL1_M_FRAC_MULT_BITS;

		ret += ((u32) rem) / clkod_plld;
		rem = ((u32) rem) % clkod_plld;
	}

	if (ret > (u64) ULONG_MAX) {
		/* FIXME: Handle PLL value overflow */
		ret = (u64) ULONG_MAX;
	}
	return (u32) ret;
}

/*
 * \brief Program a new ADPLLM frequency.
 *
 * Note, broken up into two functions. One to calculate the new settings, and
 * one to actually program them. This half programs the new settings.
 *
 * \param data State data for this operation.
 */
static void clk_adpllm_program_freq(struct adpllm_program_data *data)
{
	const struct clk_data *div_clk_data;
	const struct clk_drv_div *drv_div;
	u32 freq_ctrl0;
	u32 freq_ctrl1;
	u32 ctrl;
	u32 prog;

	data->pll_clk->flags &= (u8) ~CLK_FLAG_CACHED;

	if (!clk_adpllm_is_bypass(data->pll_clk)) {
		/* Put the PLL into IDLE bypass */
		clk_adpllm_bypass(data->pll_clk, STRUE);
	}

	/* Program the new rate */
	freq_ctrl0 = readl(data->pll->base + ADPLLM_PLL_FREQ_CTRL0(data->pll->idx));
	freq_ctrl1 = readl(data->pll->base + ADPLLM_PLL_FREQ_CTRL1(data->pll->idx));
	ctrl = readl(data->pll->base + ADPLLM_PLL_CTRL(data->pll->idx));

	freq_ctrl0 &= ~ADPLLM_FREQ_CTRL0_N_DIV_MASK;
	freq_ctrl0 |= (data->plld - 1U) << ADPLLM_FREQ_CTRL0_N_DIV_SHIFT;

	freq_ctrl0 &= ~ADPLLM_FREQ_CTRL0_M_INT_MULT_MASK;
	freq_ctrl0 |= data->pllm << ADPLLM_FREQ_CTRL0_M_INT_MULT_SHIFT;

	if (data->pll->ljm) {
		/* Select which DCO to use for PLLJM */
		u64 dco = (u64) data->parent_freq_hz * data->pllm;
		u64 cutoff = (u64) FREQ_MHZ(1375) * data->plld;
		u32 sdd;

		/* Program the Sigma-Delta Divider */

		/* sdd = ceil(parent_freq_hz * pllm / (plld * FREQ_MHZ(250))) */
		sdd = (u32) ((dco + (u64) (FREQ_MHZ(250) * data->plld) - 1ULL) / FREQ_MHZ(250));
		/*
		 * 250MHz * plld doesn't fit in 32 bits, do two divisions by
		 * a u32 rather than one by a u64.
		 */
		sdd /= data->plld;

		freq_ctrl0 &= ~ADPLLM_FREQ_CTRL0_SD_DIV_MASK;
		freq_ctrl0 |= sdd << ADPLLM_FREQ_CTRL0_SD_DIV_SHIFT;

		freq_ctrl1 &= ~ADPLLM_FREQ_CTRL1_SEL_FREQ_DCO_MASK;
		if (dco < cutoff) {
			freq_ctrl1 |= ADPLLM_FREQ_CTRL1_SEL_FREQ_DCO_1500MHZ;
		} else {
			freq_ctrl1 |= ADPLLM_FREQ_CTRL1_SEL_FREQ_DCO_2500MHZ;
		}
	} else {
		/* ADPLLM */
		if (data->m4x) {
			ctrl |= ADPLLM_CTRL_REGM4X_EN;
		} else {
			ctrl &= ~ADPLLM_CTRL_REGM4X_EN;
		}
		if (data->dcc) {
			ctrl |= ADPLLM_CTRL_DC_CORRECTOR_EN;
		} else {
			ctrl &= ~ADPLLM_CTRL_DC_CORRECTOR_EN;
		}
	}

	/* Program output divider */
	div_clk_data = clk_get_data(data->clkout_clk);
	if (div_clk_data->drv) {
		drv_div = container_of(div_clk_data->drv,
				       const struct clk_drv_div, drv);
	} else {
		drv_div = NULL;
	}
	if (drv_div && drv_div->set_div) {
		drv_div->set_div(data->clkout_clk, data->clkod);
	}

	/* Program fractional multiplier */
	freq_ctrl1 &= ~ADPLLM_FREQ_CTRL1_M_FRAC_MULT_MASK;
	freq_ctrl1 |= data->pllfm << ADPLLM_FREQ_CTRL1_M_FRAC_MULT_SHIFT;

	writel(freq_ctrl0, data->pll->base + ADPLLM_PLL_FREQ_CTRL0(data->pll->idx));
	writel(freq_ctrl1, data->pll->base + ADPLLM_PLL_FREQ_CTRL1(data->pll->idx));
	writel(ctrl, data->pll->base + ADPLLM_PLL_CTRL(data->pll->idx));

	/* Pulse L-H-L to load new multipliers */
	prog = readl(data->pll->base + ADPLLM_PLL_PROG(data->pll->idx));
	prog &= ~ADPLLM_PROG_TENABLE;
	writel(prog, data->pll->base + ADPLLM_PLL_PROG(data->pll->idx));
	prog |= ADPLLM_PROG_TENABLE;
	writel(prog, data->pll->base + ADPLLM_PLL_PROG(data->pll->idx));
	prog &= ~ADPLLM_PROG_TENABLE;
	writel(prog, data->pll->base + ADPLLM_PLL_PROG(data->pll->idx));

	if (data->pll_clk->ref_count != 0U) {
		/* Take the PLL out of IDLE bypass */
		clk_adpllm_bypass(data->pll_clk, SFALSE);
		clk_adpllm_wait_for_lock(data->pll_clk);
	}
}

/*
 * \brief Set a new ADPLLM frequency.
 *
 * Note, broken up into two functions. One to calculate the new settings, and
 * one to actually program them. This half just calculates the new settings.
 *
 * \param data State data for this operation.
 *
 * \return New frequency in Hz
 */
static u32 clk_adpllm_internal_set_freq(struct adpllm_program_data *data)
{
	const struct clk_data *pll_clk_data;
	const struct clk_data_pll *data_pll;
	u32 freq_ctrl0;
	u32 freq_ctrl1;
	u32 clkdiv;
	u32 ctrl;
	sbool prev_m4x;
	sbool prev_dcc;
	u32 prev_pllm;
	u32 prev_pllfm;
	u32 prev_plld;
	u32 prev_clkod;
	u32 freq;
	sbool was_bypass;

	data->plld = 0U;
	data->pllm = 0U;
	data->pllfm = 0U;
	data->clkod = 0U;
	data->m4x = SFALSE;
	data->dcc = SFALSE;

	was_bypass = clk_adpllm_is_bypass(data->pll_clk);

	pll_clk_data = clk_get_data(data->pll_clk);
	data_pll = container_of(pll_clk_data->data, const struct clk_data_pll,
				data);
	data->pll = container_of(data_pll, const struct clk_data_pll_adpllm,
				 data_pll);

	freq_ctrl0 = readl(data->pll->base + ADPLLM_PLL_FREQ_CTRL0(data->pll->idx));
	freq_ctrl1 = readl(data->pll->base + ADPLLM_PLL_FREQ_CTRL1(data->pll->idx));
	clkdiv = readl(data->pll->base + ADPLLM_PLL_CLKDIV(data->pll->idx));
	ctrl = readl(data->pll->base + ADPLLM_PLL_CTRL(data->pll->idx));

	if (data->pll->ljm) {
		/* ADPLLJ */
		prev_m4x = SFALSE;
		prev_dcc = STRUE;
	} else {
		/* ADPLLM */
		prev_m4x = (ctrl & ADPLLM_CTRL_REGM4X_EN) != 0UL;
		prev_dcc = (ctrl & ADPLLM_CTRL_DC_CORRECTOR_EN) != 0UL;
	}

	/* Check current values */
	prev_pllm = freq_ctrl0 & ADPLLM_FREQ_CTRL0_M_INT_MULT_MASK;
	prev_pllm >>= ADPLLM_FREQ_CTRL0_M_INT_MULT_SHIFT;

	prev_pllfm = freq_ctrl1 & ADPLLM_FREQ_CTRL1_M_FRAC_MULT_MASK;
	prev_pllfm >>= ADPLLM_FREQ_CTRL1_M_FRAC_MULT_SHIFT;

	prev_plld = freq_ctrl0 & ADPLLM_FREQ_CTRL0_N_DIV_MASK;
	prev_plld >>= ADPLLM_FREQ_CTRL0_N_DIV_SHIFT;
	prev_plld += 1U;

	prev_clkod = clkdiv & ADPLLM_CLKDIV_M2_DIV_MASK;
	prev_clkod >>= ADPLLM_CLKDIV_M2_DIV_SHIFT;

	/* Compute new values */
	data->parent_freq_hz = clk_get_parent_freq(data->pll_clk);
	if (data->pll->ljm) {
		freq = pll_calc(data->pll_clk, &adpllj_data, data->parent_freq_hz,
				data->target_hz, data->min_hz, data->max_hz,
				&data->plld, &data->pllm, &data->pllfm, &data->clkod);
	} else {
		u32 dcc_freq = 0U;
		u32 dcc_plld = 0U;
		u32 dcc_pllm = 0U;
		u32 dcc_pllfm = 0U;
		u32 dcc_clkod = 0U;
		freq = 0ULL;

		/* Check "DCC off" range */
		if (data->min_hz <= FREQ_GHZ(1.4)) {
			u32 norm_target_hz = data->target_hz;
			u32 norm_max_hz = data->max_hz;

			if (norm_max_hz > FREQ_GHZ(1.4)) {
				norm_max_hz = FREQ_GHZ(1.4);
			}

			if (norm_target_hz > FREQ_GHZ(1.4)) {
				norm_target_hz = FREQ_GHZ(1.4);
			}

			freq = pll_calc(data->pll_clk, &adpllm_data, data->parent_freq_hz,
					norm_target_hz, data->min_hz, norm_max_hz,
					&data->plld, &data->pllm, &data->pllfm, &data->clkod);
		}

		/* Check "DCC on" range */
		if (data->max_hz > FREQ_GHZ(1.4)) {
			u32 dcc_target_hz = data->target_hz;
			u32 dcc_min_hz = data->min_hz;

			if (dcc_min_hz <= FREQ_GHZ(1.4)) {
				dcc_min_hz = FREQ_GHZ(1.4) + 1UL;
			}

			if (dcc_target_hz <= FREQ_GHZ(1.4)) {
				dcc_target_hz = FREQ_GHZ(1.4) + 1UL;
			}

			dcc_freq = pll_calc(data->pll_clk, &adpllm_dcc_data,
					    data->parent_freq_hz, dcc_target_hz,
					    dcc_min_hz, data->max_hz,
					    &dcc_plld, &dcc_pllm, &dcc_pllfm, &dcc_clkod);
		}

		/* Both returned frequencies, see which was closer */
		if ((freq != 0ULL) && (dcc_freq != 0UL)) {
			u32 norm_delta;
			u32 dcc_delta;

			if (freq > data->target_hz) {
				norm_delta = freq - data->target_hz;
			} else {
				norm_delta = data->target_hz - freq;
			}

			if (dcc_freq > data->target_hz) {
				dcc_delta = freq - data->target_hz;
			} else {
				dcc_delta = data->target_hz - freq;
			}

			if (dcc_delta < norm_delta) {
				freq = 0U;
			} else if (norm_delta > dcc_delta) {
				dcc_freq = 0U;
			} else if ((data->pllfm != 0UL) && (dcc_pllfm == 0UL)) {
				/* Prefer to not use fractional */
				freq = 0U;
			} else if ((data->pllfm == 0UL) && (dcc_pllfm != 0UL)) {
				/* Prefer to not use fractional */
				dcc_freq = 0U;
			} else {
				/* Prefer DCC off */
				dcc_freq = 0U;
			}
		}

		if (dcc_freq != 0UL) {
			/* "DCC on" was the better choice */
			data->plld = dcc_plld;
			data->pllm = dcc_pllm;
			data->pllfm = dcc_pllfm;
			data->clkod = dcc_clkod;
			freq = dcc_freq;
			data->dcc = STRUE;
		} else if (freq != 0ULL) {
			/*
			 * pllm value we used integrated the "DCC off" doubling.
			 * break it out so we get the actual register value
			 * instead.
			 */
			data->pllm /= 2U;
			/*
			 * clkod integrated the divide by 2 that gets added
			 * as part of "DCC off" mode, get the actual register
			 * value instead.
			 */
			data->clkod /= 2U;
		} else {
			/* No valid settings frequency */
		}

		if ((freq != 0ULL) && (data->pllm > 4094UL)) {
			/*
			 * pllm value we used integrated m4x, break it
			 * out into a boolean so we get the actual
			 * register value.
			 *
			 * NB: m4x can be set to only for:
			 *	CLKOUT*M2 > 850MHz when LPMODE = '0'
			 *	CLKOUT*M2 > 80MHz when LPMODE = '1'
			 */
			data->pllm /= 4U;
			data->m4x = STRUE;
		}
	}


	*data->changed = STRUE;
	/* No need to do anything if they haven't changed */
	if ((data->plld == prev_plld) && (data->pllm == prev_pllm) && (data->pllfm == prev_pllfm) &&
	    (data->clkod == prev_clkod) && (prev_m4x == data->m4x) && (prev_dcc == data->dcc) &&
	    !was_bypass) {
		*data->changed = SFALSE;
	}

	if ((freq != 0ULL) && !data->query && *data->changed) {
		clk_adpllm_program_freq(data);
	}

	return freq;
}

/*
 * Bit of a glue function, there is no way for the clock tree to call this
 * directly, it is only called by pll_init. We actually want to set the
 * frequency of clkout not the DCO.
 */
static u32 clk_adpllm_set_freq(struct clk *clkp,
			       u32 target_hz,
			       u32 min_hz,
			       u32 max_hz,
			       sbool query, sbool *changed)
{
	struct clk *clkout_clk = NULL;
	u32 ret = 0U;
	u32 i;

	/*
	 * Find our child divider that can modify our frequency, that's the
	 * clkout output.
	 */
	for (i = 0; !clkout_clk && (i < soc_clock_count); i++) {
		const struct clk_data *sub_data = soc_clock_data + i;
		if ((sub_data->drv == &clk_drv_div_hsdiv.drv) &&
		    ((sub_data->flags & CLK_DATA_FLAG_MODIFY_PARENT_FREQ) != 0U)) {
			struct clk *sub_clk = soc_clocks + i;
			if (clk_adpllm_hsdiv_get_pll_root(sub_clk) == clkp) {
				clkout_clk = sub_clk;
			}
		}
	}

	if (clkout_clk) {
		struct adpllm_program_data data = {
			.pll_clk	= clkp,
			.clkout_clk	= clkout_clk,
			.target_hz	= target_hz,
			.min_hz		= min_hz,
			.max_hz		= max_hz,
			.query		= query,
			.changed	= changed,
		};
		ret = clk_adpllm_internal_set_freq(&data);
	}

	return ret;
}

/*
 * \brief Return the ADPLLM DCO frequency.
 *
 * \param clkp The ADPLLM clock.
 *
 * \return The DCO frequency of the ADPLLM in Hz.
 */
static u32 clk_adpllm_get_freq(struct clk *clkp)
{
	const struct clk_data *clk_datap = clk_get_data(clkp);
	u32 ret;

	/*
	 * If disabled, return the frequency we would be running at once
	 * we bring it out of bypass. If enabled and in bypass, return 0.
	 */
	if (clkp->ref_count == 0U) {
		ret = 0U;
	} else if ((clkp->flags & CLK_FLAG_CACHED) != 0U) {
		ret = soc_clock_values[clk_datap->freq_idx];
	} else {
		if (clk_adpllm_is_bypass(clkp)) {
			ret = 0U;
		} else {
			ret = clk_adpllm_get_freq_internal(clkp, 1U, 1U);
		}
		soc_clock_values[clk_datap->freq_idx] = ret;
		clkp->flags |= CLK_FLAG_CACHED;
	}

	return ret;
}

/*
 * \brief Program the state of the ADPLLM.
 *
 * \param clkp The ADPLLM clock.
 * \param enabled STRUE to enable (take out of bypass) SFALSE disable (bypass).
 *
 * \return True on success, SFALSE on failure (always returns STRUE)
 */
static sbool clk_adpllm_set_state(struct clk *clkp, sbool enabled)
{
	clkp->flags &= ~CLK_FLAG_CACHED;

	clk_adpllm_bypass(clkp, !enabled);

	if (enabled) {
		clk_adpllm_wait_for_lock(clkp);
	}

	return STRUE;
}

/*
 * \brief Get the state of the ADPLLM.
 *
 * \param clkp The ADPLLM clock.
 *
 * \return Returns HW state using the CLK_HW_STATE macro.
 */
static u32 clk_adpllm_get_state(struct clk *clkp)
{
	struct clk *clkp_parent = NULL;
	u32 ret = CLK_HW_STATE_ENABLED;

	if (clkp->ref_count == 0U) {
		ret = CLK_HW_STATE_DISABLED;
	}

	/* PLLs can't function without an enabled parent */
	if (ret == CLK_HW_STATE_ENABLED) {
		const struct clk_parent *p;
		p = clk_get_parent(clkp);
		if (p) {
			clkp_parent = clk_lookup((clk_idx_t) p->clk);
		}
		if (clkp_parent) {
			ret = clk_get_state(clkp_parent);
		} else {
			ret = CLK_HW_STATE_DISABLED;
		}
	}

	if (ret == CLK_HW_STATE_ENABLED) {
		if (!clk_adpllm_is_bypass(clkp) && !clk_adpllm_check_lock(clkp)) {
			ret = CLK_HW_STATE_TRANSITION;
		}
	}

	return ret;
}

static s32 clk_adpllm_init_internal(struct clk *clkp)
{
	const struct clk_data *clk_datap = clk_get_data(clkp);
	const struct clk_data_pll_adpllm *pll;
	const struct clk_data_pll *data_pll;
	u32 ctrl;

	clkp->flags &= ~CLK_FLAG_CACHED;

	data_pll = container_of(clk_datap->data, const struct clk_data_pll,
				data);
	pll = container_of(data_pll, const struct clk_data_pll_adpllm,
			   data_pll);

	/* Unlock write access */
	writel(ADPLLM_PLL_KICK0_VALUE, pll->base + ADPLLM_PLL_KICK0(pll->idx));
	writel(ADPLLM_PLL_KICK1_VALUE, pll->base + ADPLLM_PLL_KICK1(pll->idx));

	ctrl = readl(pll->base + ADPLLM_PLL_CTRL(pll->idx));

	/*
	 * We don't currently bother with a separate clock node for these.
	 *
	 * We just set the parent directly to the input clock. Since we don't
	 * track a usage count, just enable always.
	 */
	ctrl |= ADPLLM_CTRL_CLKOUTBYPASS_EN;
	/* Connects to the HSDIVs, gating is handled by each HSDIV output */
	ctrl |= ADPLLM_CTRL_CLKDCOLDO_EN;

	if (!pll->ljm) {
		/* Unused ADPLLM clock outputs */
		ctrl &= ~(ADPLLM_CTRL_CLKOUTX2_EN | ADPLLM_CTRL_CLKOUTHIF_EN);
	}
	writel(ctrl, pll->base + ADPLLM_PLL_CTRL(pll->idx));

	if (pll->hsdiv) {
		/*
		 * Find and program hsdiv defaults.
		 *
		 * HSDIV defaults must be programmed before programming the
		 * PLL since their power on default is /1. Most DCO
		 * frequencies will exceed clock rate maximums of the HSDIV
		 * outputs.
		 *
		 * We walk through the clock tree to find all the clocks
		 * with the hsdiv driver who have this adpllm for a parent.
		 *
		 * Note: This won't find our clkout "hsdiv" if there is a
		 * DCC mux. If we are programming a new frequency, it'll
		 * get set as part of that anyway.
		 */
		u32 i;

		for (i = 0; i < soc_clock_count; i++) {
			const struct clk_data *sub_data = soc_clock_data + i;
			if (((sub_data->drv == &clk_drv_div_hsdiv.drv) ||
			     (sub_data->drv == &clk_drv_div_hsdiv4.drv)) &&
			    sub_data->drv->init) {
				struct clk *sub_clk = soc_clocks + i;
				if (clk_adpllm_hsdiv_get_pll_root(sub_clk) == clkp) {
					sub_data->drv->init(clkp);
				}
			}
		}
	}

	return pll_init(clkp);
}

static s32 clk_adpllm_init(struct clk *clkp)
{
	const struct clk_data *clk_datap = clk_get_data(clkp);
	const struct clk_data_pll *data_pll;
	s32 ret;

	data_pll = container_of(clk_datap->data, const struct clk_data_pll,
				data);

	if (pm_devgroup_is_enabled(data_pll->devgrp)) {
		ret = clk_adpllm_init_internal(clkp);
	} else {
		ret = -EDEFER;
	}

	return ret;
}

const struct clk_drv clk_drv_adpllm = {
	.init		= clk_adpllm_init,
	.get_freq	= clk_adpllm_get_freq,
	.set_freq	= clk_adpllm_set_freq,
	.set_state	= clk_adpllm_set_state,
	.get_state	= clk_adpllm_get_state,
};

static sbool clk_adpllm_hsdiv_set_div(struct clk *clkp, u32 d)
{
	sbool ret;

	ret = clk_div_reg_set_div(clkp, d);
	if (ret) {
		const struct clk_data *clk_datap;
		const struct clk_data_div *data_div;
		const struct clk_data_div_reg *data_reg;
		const struct clk_data_hsdiv *data_hsdiv;
		u32 prog;

		clk_datap = clk_get_data(clkp);
		data_div = container_of(clk_datap->data,
					const struct clk_data_div, data);
		data_reg = container_of(data_div,
					const struct clk_data_div_reg, data_div);
		data_hsdiv = container_of(data_reg,
					  const struct clk_data_hsdiv, data_div_reg);

		/* Pulse L-H-L to load new divider */
		prog = readl(data_hsdiv->go_reg) & ~BIT(data_hsdiv->go_bit);
		writel(prog, data_hsdiv->go_reg);
		writel(prog | BIT(data_hsdiv->go_bit), data_hsdiv->go_reg);
		writel(prog, data_hsdiv->go_reg);
	}
	return ret;
}

/*
 * \brief Get the divider for an HSDIV.
 *
 * \param clkp The HSDIV clock
 *
 * \return HSDIV clock divider
 */
static u32 clk_adpllm_hsdiv_get_div(struct clk *clkp)
{
	u32 ret;

	ret = clk_div_reg_get_div(clkp);
	if (ret == 64UL) {
		/* Only valid for /2.5 HSDIV */
		ret = 1U;
	}

	return ret;
}

static struct clk *clk_adpllm_hsdiv_get_pll_root(struct clk *clkp)
{
	const struct clk_parent *p = NULL;
	struct clk *pll_clk = NULL;

	/* Get PLL root */
	p = clk_get_parent(clkp);
	if (p) {
		pll_clk = clk_lookup((clk_idx_t) p->clk);
	}

	/* Check for DCC block */
	if (pll_clk) {
		const struct clk_data *pll_clk_data;
		pll_clk_data = clk_get_data(pll_clk);
		if (pll_clk_data->type == CLK_TYPE_MUX) {
			/* Skip DCC block to get to actual pll root */
			p = clk_get_parent(pll_clk);
			if (p) {
				pll_clk = clk_lookup((clk_idx_t) p->clk);
			} else {
				pll_clk = NULL;
			}
		}
	}

	return pll_clk;
}

static u32 clk_adpllm_hsdiv_set_freq(struct clk *clkp,
				     u32 target_hz,
				     u32 min_hz,
				     u32 max_hz,
				     sbool query, sbool *changed)
{
	const struct clk_data *clk_datap = clk_get_data(clkp);
	u32 ret = 0U;

	if ((clk_datap->flags & CLK_DATA_FLAG_MODIFY_PARENT_FREQ) != 0U) {
		/* Program the whole PLL */
		struct clk *pll_clk = clk_adpllm_hsdiv_get_pll_root(clkp);

		/* FIXME: Add extra notify to parent */

		if (pll_clk) {
			struct adpllm_program_data data = {
				.pll_clk	= pll_clk,
				.clkout_clk	= clkp,
				.target_hz	= target_hz,
				.min_hz		= min_hz,
				.max_hz		= max_hz,
				.query		= query,
				.changed	= changed,
			};
			ret = clk_adpllm_internal_set_freq(&data);
		}
	} else {
		/*
		 * Just program the output divider.
		 * NB: programming /2.5 for hsdiv4 not currenly supported
		 */
		ret = clk_div_set_freq(clkp, target_hz, min_hz, max_hz,
				       query, changed);
	}

	return ret;
}

const struct clk_drv_div clk_drv_div_hsdiv = {
	.drv			= {
		.init		= clk_div_init,
		.notify_freq	= clk_div_notify_freq,
		.get_freq	= clk_div_get_freq,
		.set_freq	= clk_adpllm_hsdiv_set_freq,
	},
	.set_div		= clk_adpllm_hsdiv_set_div,
	.get_div		= clk_adpllm_hsdiv_get_div,
};

sbool clk_adpllm_hsdiv4_notify_freq(struct clk	*clkp,
				    u32		parent_freq_hz __attribute__((unused)),
				    sbool	query)
{
	const struct clk_data *clk_datap = clk_get_data(clkp);
	const struct clk_data_div *data_div;
	u32 div = clk_get_div(clkp);
	u32 start = div;
	u32 i;
	sbool found = SFALSE;
	sbool raise = SFALSE;
	sbool lower = SFALSE;
	u32 new_freq = 0U;

	data_div = container_of(clk_datap->data, const struct clk_data_div,
				data);
	/* Just find a frequency that works for all children */

	/* Check if current /2.5 works */
	if (div == 64UL) {
		struct clk *pll_clk = clk_adpllm_hsdiv_get_pll_root(clkp);
		if (pll_clk) {
			new_freq = clk_adpllm_get_freq_internal(pll_clk,
								2U, 5U);
			found = clk_notify_children_freq(clkp, new_freq, STRUE);
		}
		if (!found) {
			/* Start at /3 */
			start = 3U;
		}
	}

	/* Try the current divisor and lower frequencies */
	for (i = start; !found && (i < (data_div->n + 1UL)); i++) {
		new_freq = i;
		found = clk_notify_children_freq(clkp, new_freq, STRUE);
		if (found) {
			lower = STRUE;
		}
	}

	/* Try larger frequencies */
	for (i = start - 1U; i > 0UL; i--) {
		new_freq = i;
		found = clk_notify_children_freq(clkp, new_freq, STRUE);
		if (found) {
			raise = STRUE;
		}
	}

	if (!query && found) {
		const struct clk_drv_div *drv_div;
		drv_div = container_of(clk_datap->drv,
				       const struct clk_drv_div, drv);

		if (lower && (i != div)) {
			drv_div->set_div(clkp, i);
		}
		clk_notify_children_freq(clkp, new_freq, SFALSE);
		if (raise && (i != div)) {
			drv_div->set_div(clkp, i);
		}
	}

	return found;
}

/*
 * \brief Return the HSDIV frequency.
 *
 * Override default divider frequency calculation function to handle
 * divide by 2.5.
 *
 * \param clkp The HSDIV clock
 *
 * \return The frequency in Hz
 */
static u32 clk_adpllm_hsdiv4_get_freq(struct clk *clkp)
{
	struct clk *pll_clk;
	u32 ret = 0U;

	pll_clk = clk_adpllm_hsdiv_get_pll_root(clkp);
	if (pll_clk == NULL) {
		/* Invalid clock tree */
		ret = 0U;
	} else {
		u32 div = clk_get_div(clkp);
		u32 multiplier = 1U;

		if (div == 64UL) {
			/* Handle divide by 2.5, 5/2. */
			multiplier *= 2U;
			div = 5U;
		}
		ret = clk_adpllm_get_freq_internal(pll_clk, multiplier, div);
	}

	return ret;
}

const struct clk_drv_div clk_drv_div_hsdiv4 = {
	.drv			= {
		.init		= clk_div_init,
		.notify_freq	= clk_adpllm_hsdiv4_notify_freq,
		.get_freq	= clk_adpllm_hsdiv4_get_freq,
		.set_freq	= clk_adpllm_hsdiv_set_freq,
	},
	.set_div		= clk_adpllm_hsdiv_set_div,
	.get_div		= clk_div_reg_get_div,
};

static const struct clk_parent *clk_adpllm_get_div_parent(struct clk *clkp)
{
	const struct clk_parent *p = NULL;
	const struct clk_data *clk_datap;
	const struct clk_data_mux *mux;

	clk_datap = clk_get_data(clkp);
	mux = container_of(clk_datap->data, const struct clk_data_mux, data);

	/* div (clkout/hsdiv) is parent 0 */
	if (mux->parents[0].div) {
		p = &mux->parents[0];
	}

	return p;
}

static const struct clk_parent *clk_adpllm_get_bypass_parent(struct clk *clkp)
{
	const struct clk_parent *p = NULL;
	const struct clk_data *clk_datap;
	const struct clk_data_mux *mux;

	clk_datap = clk_get_data(clkp);
	mux = container_of(clk_datap->data, const struct clk_data_mux, data);

	/* Bypass clock is parent 1 */
	if (mux->parents[1].div) {
		p = &mux->parents[1];
	}

	return p;
}

static struct clk *clk_adpllm_bypass_get_pll_root(struct clk *clkp)
{
	const struct clk_parent *p = NULL;
	struct clk *div_clk = NULL;
	struct clk *pll_clk = NULL;

	p = clk_adpllm_get_div_parent(clkp);
	if (p) {
		div_clk = clk_lookup((clk_idx_t) p->clk);
	}

	if (div_clk) {
		pll_clk = clk_adpllm_hsdiv_get_pll_root(div_clk);
	}

	return pll_clk;
}

static const struct clk_parent *clk_adpllm_bypass_get_parent(struct clk *clkp)
{
	const struct clk_parent *p = NULL;
	struct clk *pll_clk;

	pll_clk = clk_adpllm_bypass_get_pll_root(clkp);
	if (pll_clk && clk_adpllm_is_bypass(pll_clk)) {
		p = clk_adpllm_get_bypass_parent(clkp);
	} else {
		p = clk_adpllm_get_div_parent(clkp);
	}

	return p;
}

static u32 clk_adpllm_bypass_get_freq(struct clk *clkp)
{
	const struct clk_parent *p = NULL;
	struct clk *div_clk = NULL;
	struct clk *pll_clk = NULL;
	u32 ret = 0U;

	p = clk_adpllm_get_div_parent(clkp);
	if (p) {
		div_clk = clk_lookup((clk_idx_t) p->clk);
	}

	if (div_clk) {
		pll_clk = clk_adpllm_hsdiv_get_pll_root(div_clk);
	}

	if (pll_clk) {
		if ((pll_clk->ref_count != 0U) || !clk_adpllm_is_bypass(pll_clk)) {
			/*
			 * If we are disabled, ignore bypass state and return
			 * frequency we will run at once enabled.
			 */
			ret = clk_get_freq(div_clk) / p->div;
		} else {
			/*
			 * If we are enabled and in bypass, return the bypass
			 * frequency.
			 */
			struct clk *bypass_clk = NULL;
			p = clk_adpllm_get_bypass_parent(clkp);
			if (p) {
				bypass_clk = clk_lookup((clk_idx_t) p->clk);
			}
			if (bypass_clk) {
				ret = clk_get_freq(bypass_clk) / p->div;
			}
		}
	}

	return ret;
}

static u32 clk_adpllm_bypass_set_freq(struct clk *clkp,
				      u32 target_hz,
				      u32 min_hz,
				      u32 max_hz,
				      sbool query, sbool *changed)
{
	const struct clk_parent *p = NULL;
	struct clk *div_clk = NULL;
	u32 ret = 0;

	p = clk_adpllm_get_div_parent(clkp);
	if (p) {
		div_clk = clk_lookup((clk_idx_t) p->clk);
	}

	if (div_clk) {
		ret = clk_set_freq(div_clk, target_hz, min_hz, max_hz,
				   query, changed);
	}

	return ret;
}

const struct clk_drv_mux clk_drv_adpllm_bypass_mux = {
	.drv			= {
		.get_freq	= clk_adpllm_bypass_get_freq,
		.set_freq	= clk_adpllm_bypass_set_freq,
	},
	.get_parent		= clk_adpllm_bypass_get_parent,
};
