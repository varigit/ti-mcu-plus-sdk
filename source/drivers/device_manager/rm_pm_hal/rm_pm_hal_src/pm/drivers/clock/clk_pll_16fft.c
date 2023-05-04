/*
 * DMSC firmware
 *
 * Copyright (C) 2018-2023, Texas Instruments Incorporated
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

#include <clk_pll_16fft.h>
#include <device.h>
#include <lib/container_of.h>
#include <lib/ioremap.h>
#include <types/array_size.h>
#include <clk_pll.h>
#include <clk_mux.h>
#include <clk_div.h>
#include <div64.h>
#include <compiler.h>
#include <lib/trace.h>
#include <delay.h>
#include <osal/osal_clock_user.h>

#define PLL_16FFT_PID   (idx)                   (0x1000UL * (idx) + 0x00UL)
#define PLL_16FFT_CFG(idx)                      (0x1000UL * (idx) + 0x08UL)
#define PLL_16FFT_CFG_HSDIV_PRSNC(n)            BIT((n) + 16UL)
#define PLL_16FFT_CFG_SSM                       BIT(11)
#define PLL_16FFT_CFG_SSM_WVTBL                 BIT(8)
#define PLL_16FFT_CFG_PLL_TYPE_SHIFT            (0UL)
#define PLL_16FFT_CFG_PLL_TYPE_MASK             (0x3UL << 0UL)
#define PLL_16FFT_CFG_PLL_TYPE_FRAC2            0UL
#define PLL_16FFT_CFG_PLL_TYPE_FRACF            1UL
#define PLL_16FFT_CFG_PLL_TYPE_DESKEW           2UL

#define PLL_16FFT_LOCKKEY0(idx)                 (0x1000UL * (idx) + 0x10UL)
#define PLL_16FFT_LOCKKEY0_VALUE                0x68EF3490UL

#define PLL_16FFT_LOCKKEY1(idx)                 (0x1000UL * (idx) + 0x14UL)
#define PLL_16FFT_LOCKKEY1_VALUE                0xD172BC5AUL

#define PLL_16FFT_CTRL(idx)                     (0x1000UL * (idx) + 0x20UL)
#define PLL_16FFT_CTRL_BYPASS_EN                BIT(31)
#define PLL_16FFT_CTRL_BYP_ON_LOCKLOSS          BIT(16)
#define PLL_16FFT_CTRL_PLL_EN                   BIT(15)
#define PLL_16FFT_CTRL_INTL_BYP_EN              BIT(8)
#define PLL_16FFT_CTRL_CLK_4PH_EN               BIT(5)
#define PLL_16FFT_CTRL_CLK_POSTDIV_EN           BIT(4)
#define PLL_16FFT_CTRL_DSM_EN                   BIT(1)
#define PLL_16FFT_CTRL_DAC_EN                   BIT(0)

#define PLL_16FFT_STAT(idx)                     (0x1000UL * (idx) + 0x24UL)
#define PLL_16FFT_STAT_LOCK                     BIT(0)

#define PLL_16FFT_FREQ_CTRL0(idx)               (0x1000UL * (idx) + 0x30UL)
#define PLL_16FFT_FREQ_CTRL0_FB_DIV_INT_SHIFT   0UL
#define PLL_16FFT_FREQ_CTRL0_FB_DIV_INT_MASK    (0xfffUL << 0UL)

#define PLL_16FFT_FREQ_CTRL1(idx)               (0x1000UL * (idx) + 0x34UL)
#define PLL_16FFT_FREQ_CTRL1_FB_DIV_FRAC_SHIFT  0UL
#define PLL_16FFT_FREQ_CTRL1_FB_DIV_FRAC_MASK   (0xffffffUL << 0UL)
#define PLL_16FFT_FREQ_CTRL1_FB_DIV_FRAC_BITS   24UL

#define PLL_16FFT_DIV_CTRL(idx)                 (0x1000UL * (idx) + 0x38UL)
#define PLL_16FFT_DIV_CTRL_POST_DIV2_SHIFT      24UL
#define PLL_16FFT_DIV_CTRL_POST_DIV2_MASK       (0x7UL << 24UL)
#define PLL_16FFT_DIV_CTRL_POST_DIV1_SHIFT      16UL
#define PLL_16FFT_DIV_CTRL_POST_DIV1_MASK       (0x7UL << 16UL)
#define PLL_16FFT_DIV_CTRL_REF_DIV_SHIFT        0UL
#define PLL_16FFT_DIV_CTRL_REF_DIV_MASK         (0x3fUL << 0UL)

#define PLL_16FFT_SS_CTRL(idx)                  (0x1000UL * (idx) + 0x40UL)
#define PLL_16FFT_SS_SPREAD(idx)                (0x1000UL * (idx) + 0x44UL)

#define PLL_16FFT_CAL_CTRL(idx)                 (0x1000UL * (idx) + 0x60UL)
#define PLL_16FFT_CAL_CTRL_CAL_EN               BIT(31)
#define PLL_16FFT_CAL_CTRL_FAST_CAL             BIT(20)
#define PLL_16FFT_CAL_CTRL_CAL_CNT_SHIFT        16UL
#define PLL_16FFT_CAL_CTRL_CAL_CNT_MASK         (0x7UL << 16UL)
#define PLL_16FFT_CAL_CTRL_CAL_BYP              BIT(15)
#define PLL_16FFT_CAL_CTRL_CAL_IN_SHIFT         0U
#define PLL_16FFT_CAL_CTRL_CAL_IN_MASK          (0xFFFU << 0U)

#define PLL_16FFT_CAL_STAT(idx)                 (0x1000U * (idx) + 0x64U)
#define PLL_16FFT_CAL_STAT_CAL_LOCK             BIT(31)
#define PLL_16FFT_CAL_STAT_LOCK_CNT_SHIFT       16U
#define PLL_16FFT_CAL_STAT_LOCK_CNT_MASK        (0xFU << 16U)
#define PLL_16FFT_CAL_STAT_CAL_OUT_SHIFT        0U
#define PLL_16FFT_CAL_STAT_CAL_OUT_MASK         (0xFFFU)

#define PLL_16FFT_HSDIV_CTRL(idx, n)            (0x1000UL * (idx) + 0x80UL + ((n) * 4UL))
#define PLL_16FFT_HSDIV_CTRL_RESET              BIT(31)
#define PLL_16FFT_HSDIV_CTRL_CLKOUT_EN          BIT(15)
#define PLL_16FFT_HSDIV_CTRL_SYNC_DIS           BIT(8)
#define PLL_16FFT_HSDIV_CTRL_HSDIV_SHIFT        0UL
#define PLL_16FFT_HSDIV_CTRL_HSDIV_MASK         (0x7fUL << 0UL)

static sbool pll_16fft_pllm_valid(struct clk *clk UNUSED, u32 pllm, sbool is_frac);

static s32 pll_16fft_bin(struct clk *clk UNUSED, u32 plld UNUSED, u32 pllm UNUSED, sbool is_frac, u32 clkod UNUSED);

static u32 pll_16fft_vco_fitness(struct clk *clk UNUSED, u32 vco, sbool is_frac UNUSED);

static sbool pll_16fft_clkod_valid(struct clk *clk UNUSED, u32 clkod);

static const struct pll_data pll_16fft_raw_data = {
	.plld_max	= 1U,
	.pllm_max	= 640U,
	.pllfm_bits	= PLL_16FFT_FREQ_CTRL1_FB_DIV_FRAC_BITS,
	.clkod_max	= 1U,
	.pllm_valid	= pll_16fft_pllm_valid,
	.bin		= pll_16fft_bin,
	.vco_fitness	= pll_16fft_vco_fitness,
};


static const struct pll_data pll_16fft_postdiv_data = {
	.plld_max	= 1U,
	.pllm_max	= 640U,
	.pllfm_bits	= PLL_16FFT_FREQ_CTRL1_FB_DIV_FRAC_BITS,
	.clkod_max	= 49U,
	.pllm_valid	= pll_16fft_pllm_valid,
	.clkod_valid	= pll_16fft_clkod_valid,
	.bin		= pll_16fft_bin,
	.vco_fitness	= pll_16fft_vco_fitness,
};


static const struct pll_data pll_16fft_hsdiv_data = {
	.plld_max	= 1U,
	.pllm_max	= 640U,
	.pllfm_bits	= PLL_16FFT_FREQ_CTRL1_FB_DIV_FRAC_BITS,
	.clkod_max	= 128U,
	.pllm_valid	= pll_16fft_pllm_valid,
	.bin		= pll_16fft_bin,
	.vco_fitness	= pll_16fft_vco_fitness,
};

#if defined (CONFIG_CLK_PLL_16FFT_FRACF_CALIBRATION)
/*
 * \brief Implement the option 3 PLL calibration method.
 *
 * This calibration method calibrates the PLL and allows a calibration value
 * to be obtained when calibration is complete. It utilizes FASTCAL mode
 * to obtain a calibration value.
 *
 * \param pll The PLL data associated with this FRACF PLL.
 */
static void clk_pll_16fft_cal_option3(const struct clk_data_pll_16fft *pll)
{
	u32 cal;

	cal = readl(pll->base + PLL_16FFT_CAL_CTRL(pll->idx));

	/* Enable calibration for FRACF */
	cal |= PLL_16FFT_CAL_CTRL_CAL_EN;

	/* Enable fast cal mode */
	cal |= PLL_16FFT_CAL_CTRL_FAST_CAL;

	/* Disable calibration bypass */
	cal &= ~PLL_16FFT_CAL_CTRL_CAL_BYP;

	/* Set CALCNT to 2 */
	cal &= ~PLL_16FFT_CAL_CTRL_CAL_CNT_MASK;
	cal |= 2U << PLL_16FFT_CAL_CTRL_CAL_CNT_SHIFT;

	/* Note this register does not readback the written value. */
	writel(cal, pll->base + PLL_16FFT_CAL_CTRL(pll->idx));
}

/*
 * \brief Implement the option 4 PLL calibration method.
 *
 * This calibration method relies on an existing calibration value and allows
 * continual background calibration.
 *
 * \param pll The PLL data associated with this FRACF PLL.
 */
static void clk_pll_16fft_cal_option4(const struct clk_data_pll_16fft *pll)
{
	u32 calout;
	u32 cal;
	u32 stat;

	cal = readl(pll->base + PLL_16FFT_CAL_CTRL(pll->idx));
	stat = readl(pll->base + PLL_16FFT_CAL_STAT(pll->idx));

	/* Read generated calibration value */
	calout = stat & PLL_16FFT_CAL_STAT_CAL_OUT_MASK;
	calout >>= PLL_16FFT_CAL_STAT_CAL_OUT_SHIFT;

	/* Program stored calibration value */
	cal &= ~PLL_16FFT_CAL_CTRL_CAL_IN_MASK;
	cal |= calout << PLL_16FFT_CAL_CTRL_CAL_IN_SHIFT;

	/* Disable calibration bypass */
	cal &= ~PLL_16FFT_CAL_CTRL_CAL_BYP;

	/* Disable fast cal mode */
	cal &= ~PLL_16FFT_CAL_CTRL_FAST_CAL;

	/* Set CALCNT to 7 */
	cal &= ~PLL_16FFT_CAL_CTRL_CAL_CNT_MASK;
	cal |= 7U << PLL_16FFT_CAL_CTRL_CAL_CNT_SHIFT;

	/* Note this register does not readback the written value. */
	writel(cal, pll->base + PLL_16FFT_CAL_CTRL(pll->idx));
}
static void clk_pll_16fft_disable_cal(const struct clk_data_pll_16fft *pll)
{
	u32 cal, stat;

	cal = readl(pll->base + PLL_16FFT_CAL_CTRL(pll->idx));
	cal &= ~PLL_16FFT_CAL_CTRL_CAL_EN;
	/* Note this register does not readback the written value. */
	writel(cal, pll->base + PLL_16FFT_CAL_CTRL(pll->idx));
	do {
		stat = readl(pll->base + PLL_16FFT_CAL_STAT(pll->idx));
	} while ((stat & PLL_16FFT_CAL_STAT_CAL_LOCK) != 0U);
	return;
}
#else
static void clk_pll_16fft_cal_option3(const struct clk_data_pll_16fft *pll __attribute__((unused)))
{
	return;
}
static void clk_pll_16fft_cal_option4(const struct clk_data_pll_16fft *pll __attribute__((unused)))
{
	return;
}
static void clk_pll_16fft_disable_cal(const struct clk_data_pll_16fft *pll __attribute__((unused)))
{
	return;
}
#endif

/*
 * \brief Check if the pllm value is valid
 *
 * \param clk The PLL clock.
 * \param pllm The multiplier value
 *
 * \return true if pllm value is valid, false otherwise
 */
static sbool pll_16fft_pllm_valid(struct clk *clk UNUSED, u32 pllm, sbool is_frac)
{
	sbool ret = STRUE;

	if (is_frac) {
		if (pllm < 20UL || pllm > 320UL) {
			ret = SFALSE;
		}
	} else {
		if (pllm < 16UL || pllm > 640UL) {
			ret = SFALSE;
		}
	}
	return ret;
}

/* Prefer non-fractional configuration if possible */
static s32 pll_16fft_bin(struct clk *clk UNUSED, u32 plld UNUSED,
			 u32 pllm UNUSED, sbool is_frac, u32 clkod UNUSED)
{
	s32 ret;

	if (is_frac) {
		ret = 0L;
	} else {
		ret = 1L;
	}

	return ret;
}

/* Prefer higher VCO frequencies */
static u32 pll_16fft_vco_fitness(struct clk *clk UNUSED, u32 vco, sbool is_frac UNUSED)
{
	return vco;
}

/*
 * \brief Check if the PLL VCO/DCO is locked.
 *
 * \param clk The PLL clock.
 *
 * \return true if VCO/DCO is locked, false otherwise
 */
static sbool clk_pll_16fft_check_lock(const struct clk_data_pll_16fft *pll)
{
	u32 stat;

	stat = readl(pll->base + PLL_16FFT_STAT(pll->idx));
	return (stat & PLL_16FFT_STAT_LOCK) != 0U;
}

#if defined (CONFIG_CLK_PLL_16FFT_FRACF_CALIBRATION)
/*
 * \brief Check if the PLL deskew calibration is complete.
 *
 * \param clk The PLL clock.
 *
 * \return true if deskew calibration is complete, false otherwise
 */
static sbool clk_pll_16fft_check_cal_lock(const struct clk_data_pll_16fft *pll)
{
	u32 stat;

	stat = readl(pll->base + PLL_16FFT_CAL_STAT(pll->idx));
	return (stat & PLL_16FFT_CAL_STAT_CAL_LOCK) != 0U;
}
#endif

static sbool clk_pll_16fft_wait_for_lock(struct clk *clk)
{
	const struct clk_data *clk_data;
	const struct clk_data_pll_16fft *pll;
	const struct clk_data_pll *data_pll;
	u32 i;
	sbool success;
	u32 freq_ctrl1;
	u32 pllfm;

	clk_data = clk_get_data(clk);
	data_pll = container_of(clk_data->data, const struct clk_data_pll,
				data);
	pll = container_of(data_pll, const struct clk_data_pll_16fft,
			   data_pll);


	/*
	 * Minimum VCO input freq is 5MHz, and the longest a lock should
	 * be consider to be timed out after 120 cycles. That's 24us. Be
	 * conservative and assume each loop takes 10 cycles and we run at a
	 * max of 1GHz. That gives 2400 loop cycles. We may end up waiting
	 * longer than neccessary for timeout, but that should be ok.
	 */
	success = SFALSE;
	for (i = 0U; i < 24U * 100U; i++) {
		if (clk_pll_16fft_check_lock(pll)) {
			success = STRUE;
			break;
		}
	}

	/* Disable calibration in the fractional mode of the FRACF PLL based on data
	 * from silicon and simulation data.
	 */
	freq_ctrl1 = readl(pll->base + PLL_16FFT_FREQ_CTRL1(pll->idx));
	pllfm = freq_ctrl1 & PLL_16FFT_FREQ_CTRL1_FB_DIV_FRAC_MASK;
	pllfm >>= PLL_16FFT_FREQ_CTRL1_FB_DIV_FRAC_SHIFT;

#if defined (CONFIG_CLK_PLL_16FFT_FRACF_CALIBRATION)
	{
		u32 pll_type;
		u32 cfg;
		cfg = readl(pll->base + PLL_16FFT_CFG(pll->idx));
		pll_type = (cfg & PLL_16FFT_CFG_PLL_TYPE_MASK) >> PLL_16FFT_CFG_PLL_TYPE_SHIFT;
		if (success &&
		    (pll_type == PLL_16FFT_CFG_PLL_TYPE_FRACF) && (pllfm == 0UL)) {
			/*
			 * Wait for calibration lock.
			 *
			 * Lock should occur within:
			 *
			 *	32 * 2^(4+CALCNT) / PFD
			 *       2048 / PFD
			 *
			 * CALCNT = 2, PFD = 5-50MHz. This gives a range of 41uS to
			 * 410uS depending on PFD frequency. Using the above logic
			 * we calculate a maximum expected 41000 loop cycles.
			 *
			 * The recommend timeout for CALLOCK to go high is 2.2 ms
			 */
			success = SFALSE;
			for (i = 0U; i < 2200U * 100U; i++) {
				if (clk_pll_16fft_check_cal_lock(pll)) {
					success = STRUE;
					break;
				}
			}
		}
		/* Disable calibration in the fractional mode of the FRACF PLL based on data
		 * from silicon and simulation data.
		 */
		if (success && (pll_type == PLL_16FFT_CFG_PLL_TYPE_FRACF)
		    && (pllfm == 0UL)) {
			u32 cal;
			cal = readl(pll->base + PLL_16FFT_CAL_CTRL(pll->idx));
			if ((cal & PLL_16FFT_CAL_CTRL_FAST_CAL) != 0U) {
				/*
				 * Fast cal enabled indicates we were performing
				 * option 3. Now that we have a calibration value,
				 * switch to option 4.
				 */
				clk_pll_16fft_cal_option4(pll);
			}
		}
	}
#endif
	return success;
}

/*
 * \brief Query bypass state of PLL
 *
 * \param clk The PLL clock.
 *
 * \return true if PLL is in bypass.
 */
static sbool clk_pll_16fft_is_bypass(const struct clk_data_pll_16fft *pll)
{
	sbool ret = SFALSE;
	u32 ctrl;

	/* IDLE Bypass */
	ctrl = readl(pll->base + PLL_16FFT_CTRL(pll->idx));
	ret = (ctrl & PLL_16FFT_CTRL_BYPASS_EN) != 0U;

	return ret;
}

static s32 clk_pll_16fft_bypass(struct clk *clk, sbool bypass)
{
	const struct clk_data *clk_data;
	const struct clk_data_pll_16fft *pll;
	const struct clk_data_pll *data_pll;
	u32 ctrl;

	clk_data = clk_get_data(clk);
	data_pll = container_of(clk_data->data, const struct clk_data_pll,
				data);
	pll = container_of(data_pll, const struct clk_data_pll_16fft,
			   data_pll);

	ctrl = readl(pll->base + PLL_16FFT_CTRL(pll->idx));
	if (bypass) {
		/* Enable bypass */
		ctrl |= PLL_16FFT_CTRL_BYPASS_EN;
	} else {
		/* Disable bypass */
		ctrl &= ~PLL_16FFT_CTRL_BYPASS_EN;
	}
	return pm_writel_verified(ctrl, pll->base + PLL_16FFT_CTRL(pll->idx));
}

static u32 clk_pll_16fft_get_freq_internal(struct clk *clk, u32 clkod)
{
	const struct clk_data *clk_data = clk_get_data(clk);
	const struct clk_data_pll_16fft *pll;
	const struct clk_data_pll *data_pll;
	u32 ret = 0UL;
	u32 freq_ctrl0;
	u32 freq_ctrl1;
	u32 div_ctrl;
	u32 pllm;
	u32 m_frac_mult;
	u32 plld;
	u32 clkod_plld;

	data_pll = container_of(clk_data->data, const struct clk_data_pll,
				data);
	pll = container_of(data_pll, const struct clk_data_pll_16fft,
			   data_pll);
	freq_ctrl0 = readl(pll->base + PLL_16FFT_FREQ_CTRL0(pll->idx));
	freq_ctrl1 = readl(pll->base + PLL_16FFT_FREQ_CTRL1(pll->idx));
	div_ctrl = readl(pll->base + PLL_16FFT_DIV_CTRL(pll->idx));

	pllm = freq_ctrl0 & PLL_16FFT_FREQ_CTRL0_FB_DIV_INT_MASK;
	pllm >>= PLL_16FFT_FREQ_CTRL0_FB_DIV_INT_SHIFT;

	m_frac_mult = freq_ctrl1 & PLL_16FFT_FREQ_CTRL1_FB_DIV_FRAC_MASK;
	m_frac_mult >>= PLL_16FFT_FREQ_CTRL1_FB_DIV_FRAC_SHIFT;

	plld = div_ctrl & PLL_16FFT_DIV_CTRL_REF_DIV_MASK;
	plld >>= PLL_16FFT_DIV_CTRL_REF_DIV_SHIFT;

	clkod_plld = clkod * plld;

	if (clkod_plld != 0UL) {
		u32 parent_freq_hz;
		u64 ret64;
		u64 rem;

		/* Calculate non-fractional part */
		parent_freq_hz = clk_get_parent_freq(clk);
		ret64 = ((u64) (parent_freq_hz / clkod_plld)) * pllm;
		rem = ((u64) (parent_freq_hz % clkod_plld)) * pllm;
		if (rem > (u64) ULONG_MAX) {
			/*
			 * Remainder should always fit within 32 bits.
			 * We add this in case of a programming error
			 * or unexpected input.
			 *
			 * clkod_plld - 12 bits
			 * pllm -       12 bits
			 * total -	24 bits (should not need div64)
			 */
			ret64 += pm_div64(&rem, clkod_plld);
		} else {
			ret64 += ((u32) rem) / clkod_plld;
			rem = ((u32) rem) % clkod_plld;
		}

		if (m_frac_mult != 0UL) {
			u64 fret;       /* Fraction return value */
			u64 frem;       /* Fraction remainder */
			const u32 mask = (1UL << PLL_16FFT_FREQ_CTRL1_FB_DIV_FRAC_BITS) -
					 1UL;

			/* Calculate fractional component of frequency */
			fret = ((u64) (parent_freq_hz / clkod_plld)) * m_frac_mult;
			frem = ((u64) (parent_freq_hz % clkod_plld)) * m_frac_mult;
			if (frem > (u64) ULONG_MAX) {
				/*
				 * m_frac_mult - 24 bits
				 * clkod_plld - 12 bits
				 * total - 36 bits
				 *
				 * NB: Overflow, 64 bit div may be required.
				 *
				 * plld max - 63
				 * clkod max - 7*7 = 49
				 * m_frac_mult max - 0xffffff
				 * max clkod = ULONG_MAX / 0xffffff = 255
				 *
				 * Any combined plld/clkod value over 255 may
				 * lead to pm_div64 being required.
				 */
				fret += pm_div64(&frem, clkod_plld);
			} else {
				fret += ((u32) frem) / clkod_plld;
				frem = ((u32) frem) % clkod_plld;
			}

			/* Fold back into return/remainder */
			frem += (fret & (u64) mask) * clkod_plld;

			/* Round the fractional component. Above mid point is rounded up*/
			fret += 1 << (PLL_16FFT_FREQ_CTRL1_FB_DIV_FRAC_BITS - 1U);

			ret64 += fret >> PLL_16FFT_FREQ_CTRL1_FB_DIV_FRAC_BITS;
			rem += frem >> PLL_16FFT_FREQ_CTRL1_FB_DIV_FRAC_BITS;

			ret64 += ((u32) rem) / clkod_plld;
			rem = ((u32) rem) % clkod_plld;
		}

		if (ret64 > (u64) ULONG_MAX) {
			/* FIXME: Handle PLL value overflow */
			ret = ULONG_MAX;
		} else {
			ret = (u32) ret64;
		}
	}

	return ret;
}


static sbool clk_pll_16fft_program_freq(struct clk			*pll_clk,
					struct clk			*div_clk,
					const struct clk_data_pll_16fft *pll,
					u32				plld,
					u32				pllm,
					u32				pllfm,
					u32				clkod)
{
	const struct clk_data *div_clk_data;
	const struct clk_drv_div *drv_div;
	u32 freq_ctrl0;
	u32 freq_ctrl1;
	u32 div_ctrl;
	u32 ctrl;
	sbool ret = STRUE;
	s32 err;
	u32 cfg;
	u32 pll_type;

	pll_clk->flags &= ~CLK_FLAG_CACHED;

	if (!clk_pll_16fft_is_bypass(pll)) {
		/* Put the PLL into bypass */
		err = clk_pll_16fft_bypass(pll_clk, STRUE);
		if (err != SUCCESS) {
			ret = SFALSE;
		}
	}

	cfg = readl(pll->base + PLL_16FFT_CFG(pll->idx));
	pll_type = (cfg & PLL_16FFT_CFG_PLL_TYPE_MASK) >> PLL_16FFT_CFG_PLL_TYPE_SHIFT;
	if ((pll_type == PLL_16FFT_CFG_PLL_TYPE_FRACF) && (pllfm != 0UL)) {
		clk_pll_16fft_disable_cal(pll);
	}

	if ((pll_type == PLL_16FFT_CFG_PLL_TYPE_FRACF) && (pllfm == 0UL)) {
		u32 cal;
		u32 stat;

		cal = readl(pll->base + PLL_16FFT_CAL_CTRL(pll->idx));
		stat = readl(pll->base + PLL_16FFT_CAL_STAT(pll->idx));

		/* Check if calibration is already enabled and locked */
		if (((cal & PLL_16FFT_CAL_CTRL_CAL_EN) != 0U) &&
		    ((stat & PLL_16FFT_CAL_STAT_CAL_LOCK) != 0U)) {
			/* Yes, go straight to option 4 */
			clk_pll_16fft_cal_option4(pll);
		} else {
			/* No, get an initial calibration via option 3 */
			clk_pll_16fft_cal_option3(pll);
		}
	}

	/* Program the new rate */
	freq_ctrl0 = readl(pll->base + PLL_16FFT_FREQ_CTRL0(pll->idx));
	freq_ctrl1 = readl(pll->base + PLL_16FFT_FREQ_CTRL1(pll->idx));
	div_ctrl = readl(pll->base + PLL_16FFT_DIV_CTRL(pll->idx));

	freq_ctrl0 &= ~PLL_16FFT_FREQ_CTRL0_FB_DIV_INT_MASK;
	freq_ctrl0 |= pllm << PLL_16FFT_FREQ_CTRL0_FB_DIV_INT_SHIFT;

	freq_ctrl1 &= ~PLL_16FFT_FREQ_CTRL1_FB_DIV_FRAC_MASK;
	freq_ctrl1 |= pllfm << PLL_16FFT_FREQ_CTRL1_FB_DIV_FRAC_SHIFT;

	div_ctrl &= ~PLL_16FFT_DIV_CTRL_REF_DIV_MASK;
	div_ctrl |= plld << PLL_16FFT_DIV_CTRL_REF_DIV_SHIFT;

	/* Make sure we have fractional support if required */
	ctrl = readl(pll->base + PLL_16FFT_CTRL(pll->idx));
	if (pllfm != 0UL) {
		ctrl |= PLL_16FFT_CTRL_DSM_EN;
	} else {
		ctrl &= ~PLL_16FFT_CTRL_DSM_EN;
	}

	if (ret) {
		err = pm_writel_verified(freq_ctrl0, pll->base + PLL_16FFT_FREQ_CTRL0(pll->idx));
		if (err != SUCCESS) {
			ret = SFALSE;
		}
	}
	if (ret) {
		err = pm_writel_verified(freq_ctrl1, pll->base + PLL_16FFT_FREQ_CTRL1(pll->idx));
		if (err != SUCCESS) {
			ret = SFALSE;
		}
	}
	if (ret) {
		err = pm_writel_verified(div_ctrl, pll->base + PLL_16FFT_DIV_CTRL(pll->idx));
		if (err != SUCCESS) {
			ret = SFALSE;
		}
	}
	if (ret) {
		err = pm_writel_verified(ctrl, pll->base + PLL_16FFT_CTRL(pll->idx));
		if (err != SUCCESS) {
			ret = SFALSE;
		}
	}

	/* Program output divider */
	if (div_clk && ret) {
		div_clk_data = clk_get_data(div_clk);
		if (div_clk_data->drv) {
			drv_div = container_of(div_clk_data->drv,
					       const struct clk_drv_div, drv);
		} else {
			drv_div = NULL;
		}
		if (drv_div && drv_div->set_div) {
			ret = drv_div->set_div(div_clk, clkod);
		}
	}

	if (pll_clk->ref_count != 0U && ret) {
		/* Take the PLL out of bypass */
		ret = clk_pll_16fft_wait_for_lock(pll_clk);
		if (ret) {
			err = clk_pll_16fft_bypass(pll_clk, SFALSE);
			if (err != SUCCESS) {
				ret = SFALSE;
			}
		}
	}

	return ret;
}

static u32 clk_pll_16fft_internal_set_freq(struct clk *pll_clk,
					   struct clk *div_clk,
					   const struct pll_data *pll_data,
					   u32 target_hz,
					   u32 min_hz,
					   u32 max_hz,
					   sbool query, sbool *changed)
{
	const struct clk_data *pll_clk_data;
	const struct clk_data_pll_16fft *pll;
	const struct clk_data_pll *data_pll;
	u32 freq_ctrl0;
	u32 freq_ctrl1;
	u32 div_ctrl;
	u32 prev_pllm;
	u32 prev_pllfm;
	u32 prev_plld;
	u32 prev_clkod;
	u32 pllm = 0UL;
	u32 pllfm = 0UL;
	u32 plld = 0UL;
	u32 clkod = 0UL;
	u32 freq;
	u32 parent_freq_hz;
	sbool was_bypass;

	pll_clk_data = clk_get_data(pll_clk);
	data_pll = container_of(pll_clk_data->data, const struct clk_data_pll,
				data);
	pll = container_of(data_pll, const struct clk_data_pll_16fft,
			   data_pll);

	was_bypass = clk_pll_16fft_is_bypass(pll);

	freq_ctrl0 = readl(pll->base + PLL_16FFT_FREQ_CTRL0(pll->idx));
	freq_ctrl1 = readl(pll->base + PLL_16FFT_FREQ_CTRL1(pll->idx));
	div_ctrl = readl(pll->base + PLL_16FFT_DIV_CTRL(pll->idx));

	/* Check current values */
	prev_pllm = freq_ctrl0 & PLL_16FFT_FREQ_CTRL0_FB_DIV_INT_MASK;
	prev_pllm >>= PLL_16FFT_FREQ_CTRL0_FB_DIV_INT_SHIFT;

	prev_pllfm = freq_ctrl1 & PLL_16FFT_FREQ_CTRL1_FB_DIV_FRAC_MASK;
	prev_pllfm >>= PLL_16FFT_FREQ_CTRL1_FB_DIV_FRAC_SHIFT;

	prev_plld = div_ctrl & PLL_16FFT_DIV_CTRL_REF_DIV_MASK;
	prev_plld >>= PLL_16FFT_DIV_CTRL_REF_DIV_SHIFT;

	if (div_clk) {
		prev_clkod = clk_get_div(div_clk);
	} else {
		prev_clkod = 1U;
	}

	/* Compute new values */
	parent_freq_hz = clk_get_parent_freq(pll_clk);
	freq = pll_calc(pll_clk, pll_data, parent_freq_hz,
			target_hz, min_hz, max_hz,
			&plld, &pllm, &pllfm, &clkod);

	*changed = STRUE;
	/* No need to do anything if they haven't changed */
	if (plld == prev_plld && pllm == prev_pllm && clkod == prev_clkod &&
	    pllfm == prev_pllfm && !was_bypass) {
		*changed = SFALSE;
	}

	if (freq != 0UL && !query && *changed) {
		if (!clk_pll_16fft_program_freq(pll_clk, div_clk, pll,
						plld, pllm, pllfm, clkod)) {
			freq = 0UL;
		}
	}

	return freq;
}

/*
 * Bit of a glue function, there is no way for the clock tree to call this
 * directly, it is only called by pll_init. We actually want to set the
 * frequency of foutpostdiv_clk not the DCO.
 */
static u32 clk_pll_16fft_set_freq(struct clk *clk,
				  u32 target_hz,
				  u32 min_hz,
				  u32 max_hz,
				  sbool query, sbool *changed)
{
	const struct pll_data *pll_data;
	struct clk *div_clk = NULL;
	u32 ret = 0U;
	u32 i;

	/* Find our associated postdiv */
	for (i = 0U; (div_clk == NULL) && (i < soc_clock_count); i++) {
		const struct clk_data *sub_data = &soc_clock_data[i];
		if (sub_data->drv == &clk_drv_div_pll_16fft_postdiv.drv) {
			struct clk *sub_clk = &soc_clocks[i];
			const struct clk_parent *p;

			p = clk_get_parent(sub_clk);
			if (p != NULL) {
				struct clk *pll_clk;
				pll_clk = clk_lookup((clk_idx_t) p->clk);
				if (pll_clk == clk) {
					div_clk = sub_clk;
				}
			}
		}
	}

	/*
	 * Since this function is only called by pll_init, the frequency
	 * passed in will always be from autogen. Because this PLL type
	 * can have many output dividers, including FOUTPOSTDIV and
	 * the various HSDIV options, the frequency listed by autogen
	 * is a bit amiguous.
	 *
	 * The current rule to resolve abiguity is that if a PLL has a
	 * connected FOUTPOSTDIV, the default frequency given is for this
	 * output. If there is no FOUTPOSTDIV, the default frequency given
	 * is for FOUTVCO.
	 */

	if (div_clk != NULL) {
		/*
		 * This is a clock with FOUTPOSTDIV. Utilize the postdiv
		 * output divider when determining a new frequency.
		 */
		pll_data = &pll_16fft_postdiv_data;
	} else {
		/*
		 * This is a clock without FOUTPOSTDIV. The clock
		 * output divider must be 1.
		 */
		pll_data = &pll_16fft_raw_data;
	}

	ret = clk_pll_16fft_internal_set_freq(clk, div_clk, pll_data,
					      target_hz, min_hz, max_hz,
					      query, changed);

	return ret;
}

/*
 * \brief Return the 16FFT PLL DCO frequency.
 *
 * \param clk The 16FFT PLL clock.
 *
 * \return The DCO frequency of the 16FFT in Hz.
 */
static u32 clk_pll_16fft_get_freq(struct clk *clk)
{
	const struct clk_data *clk_data = clk_get_data(clk);
	u32 ret;

	/*
	 * If disabled, return the frequency we would be running at once
	 * we bring it out of bypass. If enabled and in bypass, return 0.
	 */
	if (clk->ref_count == 0U) {
		ret = 0U;
	} else if ((clk->flags & CLK_FLAG_CACHED) != 0U) {
		ret = soc_clock_values[clk_data->freq_idx];
	} else {
		const struct clk_data_pll_16fft *pll;
		const struct clk_data_pll *data_pll;
		data_pll = container_of(clk_data->data, const struct clk_data_pll,
					data);
		pll = container_of(data_pll, const struct clk_data_pll_16fft,
				   data_pll);

		if (!clk_pll_16fft_is_bypass(pll)) {
			ret = clk_pll_16fft_get_freq_internal(clk, 1UL);
		} else {
			ret = 0U;
		}
		soc_clock_values[clk_data->freq_idx] = ret;
		clk->flags |= CLK_FLAG_CACHED;
	}

	return ret;
}

static sbool clk_pll_16fft_set_state(struct clk *clk, sbool enabled)
{
	sbool ret = STRUE;

	if (enabled) {
		ret = clk_pll_16fft_wait_for_lock(clk);
	}

	if (ret) {
		s32 err;
		clk->flags &= ~CLK_FLAG_CACHED;
		err = clk_pll_16fft_bypass(clk, !enabled);
		if (err != SUCCESS) {
			ret = SFALSE;
		}
	}

	return ret;
}

/*
 * \brief Get the state of the 16FFT PLL.
 *
 * \param clk The 16FFT PLL clock.
 *
 * \return Returns HW state using the CLK_HW_STATE macro.
 */
static u32 clk_pll_16fft_get_state(struct clk *clk)
{
	struct clk *clk_parent = NULL;
	u32 ret = CLK_HW_STATE_ENABLED;

	if (clk->ref_count == 0U) {
		ret = CLK_HW_STATE_DISABLED;
	}

	/* PLLs can't function without an enabled parent */
	if (ret == CLK_HW_STATE_ENABLED) {
		const struct clk_parent *p;
		p = clk_get_parent(clk);
		if (p) {
			clk_parent = clk_lookup(p->clk);
		}
		if (clk_parent) {
			ret = clk_get_state(clk_parent);
		} else {
			ret = CLK_HW_STATE_DISABLED;
		}
	}

	if (ret == CLK_HW_STATE_ENABLED) {
		const struct clk_data *clk_data;
		const struct clk_data_pll_16fft *pll;
		const struct clk_data_pll *data_pll;
		clk_data = clk_get_data(clk);
		data_pll = container_of(clk_data->data, const struct clk_data_pll,
					data);
		pll = container_of(data_pll, const struct clk_data_pll_16fft,
				   data_pll);

		if (!clk_pll_16fft_is_bypass(pll) && !clk_pll_16fft_check_lock(pll)) {
			ret = CLK_HW_STATE_TRANSITION;
		}
	}

	return ret;
}

static struct clk *clk_pll_16fft_hsdiv_get_pll_root(struct clk *clk)
{
	const struct clk_parent *p = NULL;
	struct clk *pll_clk = NULL;

	/* Get PLL root */
	p = clk_get_parent(clk);
	if (p) {
		pll_clk = clk_lookup((clk_idx_t) p->clk);
	}

	return pll_clk;
}

static struct clk *clk_pll_16fft_postdiv_get_pll_root(struct clk *clk)
{
	const struct clk_parent *p = NULL;
	struct clk *pll_clk = NULL;
	const struct clk_data *pll_clk_data;

	/* Get PLL post divider */
	p = clk_get_parent(clk);
	if (p) {
		pll_clk = clk_lookup((clk_idx_t) p->clk);
	}

	/* Verify correct clock tree */
	pll_clk_data = clk_get_data(clk);
	if (pll_clk && pll_clk_data->drv == &clk_drv_div_pll_16fft_postdiv.drv) {
		/* Get PLL root */
		p = clk_get_parent(pll_clk);
		if (p) {
			pll_clk = clk_lookup((clk_idx_t) p->clk);
		} else {
			pll_clk = NULL;
		}
	}

	return pll_clk;
}

static s32 clk_pll_16fft_init_internal(struct clk *clk)
{
	const struct clk_data *clk_data = clk_get_data(clk);
	const struct clk_data_pll_16fft *pll;
	const struct clk_data_pll *data_pll;
	u32 freq_ctrl1;
	u32 ctrl;
	u32 cfg;
	u32 pll_type;
	u32 i;
	u32 pllfm;
	s32 ret = SUCCESS;
	sbool skip_hw_init = SFALSE;

	clk->flags &= ~CLK_FLAG_CACHED;

	data_pll = container_of(clk_data->data, const struct clk_data_pll,
				data);
	pll = container_of(data_pll, const struct clk_data_pll_16fft,
			   data_pll);
	/*
	 * In order to honor the CLK_DATA_FLAG_NO_HW_REINIT flag when set,
	 * we must check if the clk is enabled, and if so, skip re-setting
	 * default frequency if it is available.
	 */
	if (((clk_data->flags & CLK_DATA_FLAG_NO_HW_REINIT) != 0) &&
	    (clk_pll_16fft_check_lock(pll) == STRUE)) {
		skip_hw_init = STRUE;
	}

	if (skip_hw_init == SFALSE) {
		/*
		* Unlock write access. Note this register does not readback the
		* written value.
		*/
		writel(PLL_16FFT_LOCKKEY0_VALUE, pll->base + PLL_16FFT_LOCKKEY0(pll->idx));
		writel(PLL_16FFT_LOCKKEY1_VALUE, pll->base + PLL_16FFT_LOCKKEY1(pll->idx));

		cfg = readl(pll->base + PLL_16FFT_CFG(pll->idx));
		ctrl = readl(pll->base + PLL_16FFT_CTRL(pll->idx));

		pll_type = (cfg & PLL_16FFT_CFG_PLL_TYPE_MASK) >> PLL_16FFT_CFG_PLL_TYPE_SHIFT;
		freq_ctrl1 = readl(pll->base + PLL_16FFT_FREQ_CTRL1(pll->idx));
		pllfm = freq_ctrl1 & PLL_16FFT_FREQ_CTRL1_FB_DIV_FRAC_MASK;
		pllfm >>= PLL_16FFT_FREQ_CTRL1_FB_DIV_FRAC_SHIFT;
		/* Disable calibration in the fractional mode of the FRACF PLL based on
		 * data from silicon and simulation data.
		 */
		if ((pll_type == PLL_16FFT_CFG_PLL_TYPE_FRACF) && (pllfm == 0UL)) {
			u32 cal;
			u32 stat;

			cal = readl(pll->base + PLL_16FFT_CAL_CTRL(pll->idx));
			stat = readl(pll->base + PLL_16FFT_CAL_STAT(pll->idx));

			/* Check if calibration is already enabled and locked */
			if (((cal & PLL_16FFT_CAL_CTRL_CAL_EN) != 0U) &&
			    ((stat & PLL_16FFT_CAL_STAT_CAL_LOCK) != 0U)) {
				/* Yes, go straight to option 4 */
				clk_pll_16fft_cal_option4(pll);
			} else {
				/* No, get an initial calibration via option 3 */
				clk_pll_16fft_cal_option3(pll);
			}
		}

		/* Make sure PLL is enabled */
		if ((ctrl & PLL_16FFT_CTRL_PLL_EN) == 0U) {
			ctrl |= PLL_16FFT_CTRL_PLL_EN;
			ret = pm_writel_verified(ctrl, pll->base + PLL_16FFT_CTRL(pll->idx));
			if (ret == SUCCESS) {
				osal_delay(1UL); /* Wait 1us */
			}
		}

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
		if (pllfm != 0UL) {
			ctrl |= PLL_16FFT_CTRL_DSM_EN;
		} else {
			ctrl &= ~PLL_16FFT_CTRL_DSM_EN;
		}

		if (ret == SUCCESS) {
			ret = pm_writel_verified(ctrl, pll->base + PLL_16FFT_CTRL(pll->idx));
		}

		/* Enable all HSDIV outputs */
		for (i = 0U; (i < 16U) && (ret == SUCCESS); i++) {
			/* Enable HSDIV output if present */
			if ((PLL_16FFT_CFG_HSDIV_PRSNC(i) & cfg) != 0UL) {
				ctrl = readl(pll->base + PLL_16FFT_HSDIV_CTRL(pll->idx, i));
				ctrl |= PLL_16FFT_HSDIV_CTRL_CLKOUT_EN;
				ret = pm_writel_verified(ctrl, pll->base + PLL_16FFT_HSDIV_CTRL(pll->idx, i));
			}
		}

		/*
		* Find and program hsdiv defaults.
		*
		* HSDIV defaults must be programmed before programming the
		* PLL since their power on default is /1. Most DCO
		* frequencies will exceed clock rate maximums of the HSDIV
		* outputs.
		*
		* We walk through the clock tree to find all the clocks
		* with the hsdiv driver who have this PLL for a parent.
		*/
		for (i = 0; (i < soc_clock_count) && (ret == SUCCESS); i++) {
			const struct clk_data *sub_data = soc_clock_data + i;
			struct clk *sub_clk = soc_clocks + i;
			if (sub_data->drv == &clk_drv_div_reg.drv &&
			    sub_data->drv->init) {
				if (clk_pll_16fft_postdiv_get_pll_root(sub_clk) == clk) {
					sub_data->drv->init(sub_clk);
				}
			} else if (sub_data->drv == &clk_drv_div_pll_16fft_hsdiv.drv &&
				   sub_data->drv->init) {
				if (clk_pll_16fft_hsdiv_get_pll_root(sub_clk) == clk) {
					sub_data->drv->init(sub_clk);
				}
			}
		}

		if (ret == SUCCESS) {
			ret = pll_init(clk);
		}
	}

	/*
	 * We must always assume we are enabled as we could be operating
	 * clocks in bypass.
	 */
	if (ret == SUCCESS) {
		clk->flags |= CLK_FLAG_PWR_UP_EN;
	}

	return ret;
}

static s32 clk_pll_16fft_init(struct clk *clk)
{
	const struct clk_data *clk_data = clk_get_data(clk);
	const struct clk_data_pll *data_pll;
	s32 ret;

	data_pll = container_of(clk_data->data, const struct clk_data_pll,
				data);

	if (pm_devgroup_is_enabled(data_pll->devgrp)) {
		ret = clk_pll_16fft_init_internal(clk);
	} else {
		ret = -EDEFER;
	}

	return ret;
}

const struct clk_drv clk_drv_pll_16fft = {
	.init		= clk_pll_16fft_init,
	.get_freq	= clk_pll_16fft_get_freq,
	.set_freq	= clk_pll_16fft_set_freq,
	.get_state	= clk_pll_16fft_get_state,
	.set_state	= clk_pll_16fft_set_state,
};

/*
 * There are two post dividers. Total post divide is postdiv1 * postdiv2.
 * postdiv1 must always be greater than or equal to postdiv2. Rather than
 * factor the divider, just keep a small 50 byte lookup table.
 *
 * Note that this means that even though that there are 6 bits used for
 * divider values, there are only 25 divider values (rather than 64).
 */
#define PDV(pd1, pd2)           ((pd1) | ((pd2) << 4UL))
#define PDV_ENTRY(div, pd1)     [(div)] = PDV((pd1), (div) / (pd1))
static const u8 postdiv_mapping[7U * 7U + 1U] = {
	PDV_ENTRY(1U,  1U),
	PDV_ENTRY(2U,  2U),
	PDV_ENTRY(3U,  3U),
	PDV_ENTRY(4U,  4U),
	PDV_ENTRY(5U,  5U),
	PDV_ENTRY(6U,  6U),
	PDV_ENTRY(7U,  7U),
	PDV_ENTRY(8U,  4U),
	PDV_ENTRY(9U,  3U),
	PDV_ENTRY(10U, 5U),
	PDV_ENTRY(12U, 4U),
	PDV_ENTRY(14U, 7U),
	PDV_ENTRY(15U, 5U),
	PDV_ENTRY(16U, 4U),
	PDV_ENTRY(18U, 6U),
	PDV_ENTRY(20U, 5U),
	PDV_ENTRY(21U, 7U),
	PDV_ENTRY(24U, 6U),
	PDV_ENTRY(25U, 5U),
	PDV_ENTRY(28U, 7U),
	PDV_ENTRY(30U, 6U),
	PDV_ENTRY(35U, 7U),
	PDV_ENTRY(36U, 6U),
	PDV_ENTRY(42U, 7U),
	PDV_ENTRY(49U, 7U),
};

/*
 * \brief Check if the clkod value is valid
 *
 * \param clk The PLL clock.
 * \param clkod The output divider value
 *
 * \return true if clkod value is valid, false otherwise
 */
static sbool pll_16fft_clkod_valid(struct clk *clk UNUSED, u32 clkod)
{
	return clkod < ARRAY_SIZE(postdiv_mapping) && postdiv_mapping[clkod] != 0U;
}

static u32 clk_pll_16fft_postdiv_set_freq(struct clk *clk,
					  u32 target_hz,
					  u32 min_hz,
					  u32 max_hz,
					  sbool query, sbool *changed)
{
	const struct clk_data *clk_data = clk_get_data(clk);
	u32 ret = 0UL;

	if ((clk_data->flags & CLK_DATA_FLAG_MODIFY_PARENT_FREQ) != 0U) {
		/* Program the whole PLL */
		const struct clk_parent *p;
		struct clk *pll_clk = NULL;

		p = clk_get_parent(clk);
		if (p) {
			pll_clk = clk_lookup((clk_idx_t) p->clk);
		}

		if (pll_clk) {
			ret = clk_pll_16fft_internal_set_freq(pll_clk, clk,
							      &pll_16fft_postdiv_data,
							      target_hz, min_hz, max_hz,
							      query, changed);
		}
	} else {
		/* Just program the output divider. */
		ret = clk_div_set_freq(clk, target_hz, min_hz, max_hz,
				       query, changed);
	}

	return ret;
}

static sbool clk_pll_16fft_postdiv_valid_div(struct clk *clk UNUSED, u32 d)
{
	return d < ARRAY_SIZE(postdiv_mapping) && postdiv_mapping[d] != 0U;
}

static sbool clk_pll_16fft_postdiv_set_div(struct clk *clk, u32 d)
{
	const struct clk_parent *p;
	struct clk *pll_clk = NULL;
	sbool ret = SFALSE;
	u32 err;

	p = clk_get_parent(clk);
	if (p) {
		pll_clk = clk_lookup((clk_idx_t) p->clk);
	}

	if (pll_clk) {
		const struct clk_data *pll_clk_data = clk_get_data(pll_clk);
		const struct clk_data_pll *data_pll;
		const struct clk_data_pll_16fft *pll;
		u32 div_ctrl;
		u32 post_div1;
		u32 post_div2;
		u8 v;

		data_pll = container_of(pll_clk_data->data,
					const struct clk_data_pll, data);
		pll = container_of(data_pll, const struct clk_data_pll_16fft,
				   data_pll);

		div_ctrl = readl(pll->base + PLL_16FFT_DIV_CTRL(pll->idx));
		div_ctrl &= ~PLL_16FFT_DIV_CTRL_POST_DIV2_MASK;
		div_ctrl &= ~PLL_16FFT_DIV_CTRL_POST_DIV1_MASK;

		v = postdiv_mapping[d];
		post_div1 = v & 0xfUL;
		post_div2 = v >> 4UL;

		div_ctrl |= post_div1 << PLL_16FFT_DIV_CTRL_POST_DIV1_SHIFT;
		div_ctrl |= post_div2 << PLL_16FFT_DIV_CTRL_POST_DIV2_SHIFT;

		err = pm_writel_verified(div_ctrl, pll->base + PLL_16FFT_DIV_CTRL(pll->idx));
		if (err == SUCCESS) {
			ret = STRUE;
		}
	}

	return ret;
}

static u32 clk_pll_16fft_postdiv_get_div(struct clk *clk)
{
	const struct clk_parent *p;
	struct clk *pll_clk = NULL;
	u32 ret = 0UL;

	p = clk_get_parent(clk);
	if (p) {
		pll_clk = clk_lookup((clk_idx_t) p->clk);
	}

	if (pll_clk) {
		const struct clk_data *pll_clk_data = clk_get_data(pll_clk);
		const struct clk_data_pll *data_pll;
		const struct clk_data_pll_16fft *pll;
		u32 div_ctrl;
		u32 post_div1;
		u32 post_div2;

		data_pll = container_of(pll_clk_data->data,
					const struct clk_data_pll, data);
		pll = container_of(data_pll, const struct clk_data_pll_16fft,
				   data_pll);

		div_ctrl = readl(pll->base + PLL_16FFT_DIV_CTRL(pll->idx));
		post_div1 = div_ctrl & PLL_16FFT_DIV_CTRL_POST_DIV1_MASK;
		post_div1 >>= PLL_16FFT_DIV_CTRL_POST_DIV1_SHIFT;
		post_div2 = div_ctrl & PLL_16FFT_DIV_CTRL_POST_DIV2_MASK;
		post_div2 >>= PLL_16FFT_DIV_CTRL_POST_DIV2_SHIFT;

		ret = post_div1 * post_div2;
	}
	return ret;
}

const struct clk_drv_div clk_drv_div_pll_16fft_postdiv = {
	.drv			= {
		.init		= clk_div_init,
		.notify_freq	= clk_div_notify_freq,
		.get_freq	= clk_div_get_freq,
		.set_freq	= clk_pll_16fft_postdiv_set_freq,
	},
	.valid_div		= clk_pll_16fft_postdiv_valid_div,
	.set_div		= clk_pll_16fft_postdiv_set_div,
	.get_div		= clk_pll_16fft_postdiv_get_div,
};

static u32 clk_pll_16fft_hsdiv_set_freq(struct clk *clk,
					u32 target_hz,
					u32 min_hz,
					u32 max_hz,
					sbool query, sbool *changed)
{
	const struct clk_data *clk_data = clk_get_data(clk);
	u32 ret = 0UL;

	if ((clk_data->flags & CLK_DATA_FLAG_MODIFY_PARENT_FREQ) != 0U) {
		/* Program the whole PLL */
		const struct clk_parent *p;
		struct clk *pll_clk = NULL;

		p = clk_get_parent(clk);
		if (p) {
			pll_clk = clk_lookup((clk_idx_t) p->clk);
		}

		if (pll_clk) {
			ret = clk_pll_16fft_internal_set_freq(pll_clk, clk,
							      &pll_16fft_hsdiv_data,
							      target_hz, min_hz, max_hz,
							      query, changed);
		}
	} else {
		/* Just program the output divider. */
		ret = clk_div_set_freq(clk, target_hz, min_hz, max_hz,
				       query, changed);
	}

	return ret;
}

const struct clk_drv_div clk_drv_div_pll_16fft_hsdiv = {
	.drv			= {
		.init		= clk_div_init,
		.notify_freq	= clk_div_notify_freq,
		.get_freq	= clk_div_get_freq,
		.set_freq	= clk_pll_16fft_hsdiv_set_freq,
	},
	.set_div		= clk_div_reg_set_div,
	.get_div		= clk_div_reg_get_div,
};
