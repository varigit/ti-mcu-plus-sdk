/*
 * System Firmware
 *
 * PLL Raw driver for direct PLL manipulation
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

#include <config.h>
#include <lib/bitops.h>
#include <types/errno.h>
#include <types/short_types.h>

#include "lpm_io.h"
#include "pll_16fft_raw.h"

/* PLL CTRL Registers */
#define PLLCTRL_RSCTRL_OFFSET                           0x0E8U
#define PLLCTRL_RSCTRL_KEY                              0x15a69U
#define PLLCTRL_RSISO_OFFSET                            0x0F0U
#define PLLCTRL_PLLCTL_OFFSET                           0x100U

/* 16FFT PLL Registers */
#define PLL_16FFT_PID_OFFSET                    ((u32) 0x00UL)
#define PLL_16FFT_CFG_OFFSET                    ((u32) 0x08UL)

#define PLL_16FFT_LOCKKEY0_OFFSET               ((u32) 0x10UL)
#define PLL_16FFT_LOCKKEY0_VALUE                ((u32) 0x68EF3490UL)

#define PLL_16FFT_LOCKKEY1_OFFSET               ((u32) 0x14UL)
#define PLL_16FFT_LOCKKEY1_VALUE                ((u32) 0xD172BC5AUL)

#define PLL_16FFT_CTRL_OFFSET                   ((u32) 0x20UL)
#define PLL_16FFT_CTRL_BYPASS_EN                BIT(31)
#define PLL_16FFT_CTRL_BYP_ON_LOCKLOSS          BIT(16)
#define PLL_16FFT_CTRL_PLL_EN                   BIT(15)
#define PLL_16FFT_CTRL_INTL_BYP_EN              BIT(8)
#define PLL_16FFT_CTRL_CLK_4PH_EN               BIT(5)
#define PLL_16FFT_CTRL_CLK_POSTDIV_EN           BIT(4)
#define PLL_16FFT_CTRL_DSM_EN                   BIT(1)
#define PLL_16FFT_CTRL_DAC_EN                   BIT(0)

#define PLL_16FFT_STAT_OFFSET                   ((u32) 0x24UL)
#define PLL_16FFT_STAT_LOCK                     BIT(0)

#define PLL_16FFT_FREQ_CTRL0_OFFSET             ((u32) 0x30UL)
#define PLL_16FFT_FREQ_CTRL0_FB_DIV_INT_SHIFT   ((u32) 0UL)
#define PLL_16FFT_FREQ_CTRL0_FB_DIV_INT_MASK    ((u32) (0xfffUL << 0UL))

#define PLL_16FFT_FREQ_CTRL1_OFFSET             ((u32) 0x34UL)
#define PLL_16FFT_FREQ_CTRL1_FB_DIV_FRAC_SHIFT  ((u32) 0UL)
#define PLL_16FFT_FREQ_CTRL1_FB_DIV_FRAC_MASK   ((u32) (0xffffffUL << 0UL))
#define PLL_16FFT_FREQ_CTRL1_FB_DIV_FRAC_BITS   ((u32) 24UL)

#define PLL_16FFT_DIV_CTRL_OFFSET               ((u32) 0x38UL)
#define PLL_16FFT_DIV_CTRL_POST_DIV2_SHIFT      ((u32) 24UL)
#define PLL_16FFT_DIV_CTRL_POST_DIV2_MASK       ((u32) (0x7UL << 24UL))
#define PLL_16FFT_DIV_CTRL_POST_DIV1_SHIFT      ((u32) 16UL)
#define PLL_16FFT_DIV_CTRL_POST_DIV1_MASK       ((u32) (0x7UL << 16UL))
#define PLL_16FFT_DIV_CTRL_REF_DIV_SHIFT        ((u32) 0UL)
#define PLL_16FFT_DIV_CTRL_REF_DIV_MASK         ((u32) (0x3fUL << 0UL))

#define PLL_16FFT_SS_CTRL_OFFSET                0x40UL
#define PLL_16FFT_SS_SPREAD_OFFSET              0x44UL

#define PLL_16FFT_HSDIV_CTRL_OFFSET             ((u32) 0x80UL)
#define PLL_16FFT_HSDIV_CTRL_RESET              BIT(31)
#define PLL_16FFT_HSDIV_CTRL_CLKOUT_EN          BIT(15)
#define PLL_16FFT_HSDIV_CTRL_SYNC_DIS           BIT(8)
#define PLL_16FFT_HSDIV_CTRL_HSDIV_SHIFT        ((u32) 0UL)
#define PLL_16FFT_HSDIV_CTRL_HSDIV_MASK         ((u32) (0x7fUL << 0UL))

#define PLL_16FFT_RAW_LOCK_TIMEOUT                              10000

#define pll_readl readl
#define pll_writel writel

s32 pll_restore(struct pll_raw_data *pll)
{
	u8 i;
	u32 ctrl, cfg;
	s32 ret = SUCCESS;
	u32 pll_stat;
	u32 timeout;

	/* Unlock write access */
	pll_writel(PLL_16FFT_LOCKKEY0_VALUE, pll->base + PLL_16FFT_LOCKKEY0_OFFSET);
	pll_writel(PLL_16FFT_LOCKKEY1_VALUE, pll->base + PLL_16FFT_LOCKKEY1_OFFSET);

	ctrl = pll_readl(pll->base + PLL_16FFT_CTRL_OFFSET);

	/* Always bypass if we lose lock */
	ctrl |= PLL_16FFT_CTRL_BYP_ON_LOCKLOSS;

	/* Prefer glitchless bypass */
	if (ctrl & PLL_16FFT_CTRL_INTL_BYP_EN) {
		ctrl |= PLL_16FFT_CTRL_BYPASS_EN;
		ctrl &= ~PLL_16FFT_CTRL_INTL_BYP_EN;
	}

	/* Always enable output if PLL */
	ctrl |= PLL_16FFT_CTRL_CLK_POSTDIV_EN;

	/* Currently unused by all PLLs */
	ctrl &= ~PLL_16FFT_CTRL_CLK_4PH_EN;

	/* Make sure we have fractional support if required */
	if (pll->freq_ctrl1 != 0UL) {
		ctrl |= PLL_16FFT_CTRL_DSM_EN;
	} else {
		ctrl &= ~PLL_16FFT_CTRL_DSM_EN;
	}

	pll_writel(ctrl, pll->base + PLL_16FFT_CTRL_OFFSET);

	/* Program all HSDIV outputs */
	cfg = pll_readl(pll->base + PLL_16FFT_CFG_OFFSET);
	for (i = 0U; i < 16U; i++) {
		/* Program HSDIV output if present */
		if (((1UL << (i + 16UL)) & cfg) != 0UL) {
			pll_writel(pll->hsdiv[i], pll->base + PLL_16FFT_HSDIV_CTRL_OFFSET + i * 0x4);
		}
	}

	pll_writel(pll->freq_ctrl0, pll->base + PLL_16FFT_FREQ_CTRL0_OFFSET);
	pll_writel(pll->freq_ctrl1, pll->base + PLL_16FFT_FREQ_CTRL1_OFFSET);
	pll_writel(pll->div_ctrl, pll->base + PLL_16FFT_DIV_CTRL_OFFSET);

	/* Make sure PLL is enabled */
	ctrl = pll_readl(pll->base + PLL_16FFT_CTRL_OFFSET);
	if ((ctrl & PLL_16FFT_CTRL_PLL_EN) == 0UL) {
		ctrl |= PLL_16FFT_CTRL_PLL_EN;
	}

	if ((ctrl & PLL_16FFT_CTRL_BYPASS_EN) != 0) {
		/*
		 * The PLL_CTRL BYPASS_EN should be cleared to make sure the clocks are running
		 * at the locked PLL frequency.
		 */
		ctrl &= ~PLL_16FFT_CTRL_BYPASS_EN;
		pll_writel(ctrl, pll->base + PLL_16FFT_CTRL_OFFSET);

		timeout = PLL_16FFT_RAW_LOCK_TIMEOUT;
		do {
			timeout--;
			pll_stat = pll_readl(pll->base + PLL_16FFT_STAT_OFFSET) & ((u32) PLL_16FFT_STAT_LOCK);
		} while ((timeout > 0U) && (pll_stat != 1U));
		if (timeout == 0U) {
			ret = -EFAIL;
		}
	}

	return ret;
}

void pll_save(struct pll_raw_data *pll)
{
	u8 i;
	u32 cfg;

	pll->freq_ctrl0 = pll_readl(pll->base + PLL_16FFT_FREQ_CTRL0_OFFSET);
	pll->freq_ctrl1 = pll_readl(pll->base + PLL_16FFT_FREQ_CTRL1_OFFSET);
	pll->div_ctrl = pll_readl(pll->base + PLL_16FFT_DIV_CTRL_OFFSET);

	/* Enable all HSDIV outputs */
	cfg = pll_readl(pll->base + PLL_16FFT_CFG_OFFSET);
	for (i = 0U; i < 16U; i++) {
		/* Read HSDIV output if present */
		if (((1UL << (i + 16UL)) & cfg) != 0UL) {
			pll->hsdiv[i] = pll_readl(pll->base + PLL_16FFT_HSDIV_CTRL_OFFSET + i * 0x4);
		}
	}
}

void pll_disable(struct pll_raw_data *pll, u16 hsdivs_to_disable)
{
	u8 i;
	u32 ctrl, cfg, hsdiv_bit;

	/* disable all hsdivs */
	cfg = pll_readl(pll->base + PLL_16FFT_CFG_OFFSET);
	for (i = 0U; i < 16U; i++) {
		/* Disable HSDIV output if present */
		hsdiv_bit = (1UL << (i + 16UL)) & hsdivs_to_disable;
		if ((hsdiv_bit & cfg) != 0UL) {
			ctrl = pll_readl(pll->base + PLL_16FFT_HSDIV_CTRL_OFFSET + i * 0x4);
			ctrl &= ~(PLL_16FFT_HSDIV_CTRL_HSDIV_MASK);
			pll_writel(ctrl, pll->base + PLL_16FFT_HSDIV_CTRL_OFFSET + i * 0x4);
		}
	}

	/* Bypass actual PLL */
	pll_bypass(pll);
}

void pll_bypass(struct pll_raw_data *pll)
{
	u32 ctrl;

	ctrl = pll_readl(pll->base + PLL_16FFT_CTRL_OFFSET);
	ctrl |= PLL_16FFT_CTRL_BYPASS_EN;
	pll_writel(ctrl, pll->base + PLL_16FFT_CTRL_OFFSET);
}
