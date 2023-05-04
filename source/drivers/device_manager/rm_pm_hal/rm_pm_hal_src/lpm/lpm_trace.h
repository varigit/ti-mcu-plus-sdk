/*
 * DM Stub Firmware
 *
 * DM Stub Minimal Trace Debug Layer
 *
 * Copyright (C) 2021, Texas Instruments Incorporated
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

#include <lib/trace_protocol.h>

void lpm_trace_debug(u32 value);

#ifdef CONFIG_LPM_DM_TRACE
#define lpm_seq_trace(step) lpm_trace_debug((TRACE_DEBUG_CHANNEL_PM << TRACE_DEBUG_DOMAIN_SHIFT) \
					    | (((u32) TRACE_PM_ACTION_LPM_SEQUENCE) << TRACE_DEBUG_ACTION_SHIFT) \
					    | (((u32) step) << TRACE_PM_ACTION_LPM_SEQ_SHIFT)			\
					    | (0U))


#define lpm_seq_trace_fail(step) lpm_trace_debug((TRACE_DEBUG_CHANNEL_PM << TRACE_DEBUG_DOMAIN_SHIFT) \
						 | (((u32) TRACE_PM_ACTION_LPM_SEQUENCE | TRACE_PM_ACTION_FAIL) << TRACE_DEBUG_ACTION_SHIFT)  \
						 | (((u32) step) << TRACE_PM_ACTION_LPM_SEQ_SHIFT)		     \
						 | (0U))

#define lpm_seq_trace_val(step, val) lpm_trace_debug((TRACE_DEBUG_CHANNEL_PM << TRACE_DEBUG_DOMAIN_SHIFT) \
						     | (((u32) TRACE_PM_ACTION_LPM_SEQUENCE) << TRACE_DEBUG_ACTION_SHIFT) \
						     | (((u32) step) << TRACE_PM_ACTION_LPM_SEQ_SHIFT)			 \
						     | (val))
#else
#define lpm_seq_trace(step)
#define lpm_seq_trace_fail(step)
#define lpm_seq_trace_val(step, val)
#endif
