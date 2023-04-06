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

#include <types/short_types.h>

struct pll_raw_data {
	u32	base;
	u32	freq_ctrl0;
	u32	freq_ctrl1;
	u32	div_ctrl;
	u16	hsdiv[16];
};

/**
 * \brief Restore a saved pll configuration from pll_save
 * \param pll Pointer to location to restore saved PLL context
 */
s32 pll_restore(struct pll_raw_data *pll);

/**
 * \brief Save a pll configuration from pll_save
 * \param pll Pointer to location to save PLL context, with base set
 *            to address for PLL to save.
 */
void pll_save(struct pll_raw_data *pll);

/**
 * \brief Bypass a PLL and disable all hsdivs with bit set in hsdivs_to_disable
 * \param pll Pointer to PLL context, with base set to address for pll to
 *            manipulate
 * \param hsdivs_to_disable Bit-field indicating which PLLs should be
 *			    disabled, which LSB indicating HSDIV0. A value
 *			    of 1 means that the PLL will be disabled, and
 *			    0xFFFF will disable all HSDIVs for a PLL. A Value
 *			    of 0xFFFE will disable all but HSDIV0.
 */
void pll_disable(struct pll_raw_data *pll, u16 hsdivs_to_disable);

/**
 * \brief Bypass a PLL and disable all hsdivs with bit set in hsdivs_to_disable
 * \param pll Pointer to PLL context, with base set to address for pll to
 *            manipulate
 */
void pll_bypass(struct pll_raw_data *pll);
