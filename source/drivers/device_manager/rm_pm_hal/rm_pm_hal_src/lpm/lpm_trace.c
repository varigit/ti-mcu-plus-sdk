/*
 * DM Stub Firmware
 *
 * DM Stub Minimal Trace Debug Layer
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

#include "lpm_trace.h"
#include "lpm_serial_8250.h"

void lpm_trace_output(const char *str)
{
	lpm_puts(str);
}

static void lpm_trace_output_char(u8 ch)
{
	lpm_console_tx(ch);
}

void lpm_trace_debug(u32 value)
{
	u32 idx = 0;
	u32 val;
	u32 i;
	u8 str[9];

	if (value == 0U) {
		str[0] = (u8) '0';
		idx+=2;
	}

	while (value > 0U) {
		val = value % 16;
		if (val < 10) {
			str[idx] = (u8) (val + '0');
		} else {
			str[idx] = (u8) ((val - 10U) + 'A');
		}

		idx++;
		value /= 16;
	}

	str[idx] = (u8) '\0';
	/* Output "0x" at start of hex */
	lpm_trace_output_char('0');
	lpm_trace_output_char('x');

	if (idx > 0U) {
		/* Get length of string - NULL terminator*/
		idx--;

		/*
		 * Output string backwards as we converted from low
		 * digit to high
		 */
		for (i = 0U; i <= idx; i++) {
			lpm_trace_output_char(str[idx - i]);
		}
		lpm_trace_output_char('\n');
	}
}
