/*
 * DMSC RM
 *
 * Resource Manager core infrastructure
 *
 * Copyright (C) 2017-2022, Texas Instruments Incorporated
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
#include <types/errno.h>
#include <lib/trace.h>

#include <config.h>

#include <rm.h>
#include <rm_core.h>

#ifdef CONFIG_RM_IRQ
#include <rm_irq.h>
#endif
#ifdef CONFIG_RM_RA
#include <rm_ra.h>
#endif
#ifdef CONFIG_RM_UDMAP
#include <rm_udmap.h>
#endif
#ifdef CONFIG_RM_PROXY
#include <rm_proxy.h>
#endif
#ifdef CONFIG_RM_LOCAL_SUBSYSTEM_REQUESTS
#ifdef CONFIG_RM_PSIL
#include <rm_psil.h>
#endif
#endif

s32 rm_init(void)
{
	s32 r = SUCCESS;

	r = rm_core_init();

#ifdef CONFIG_RM_IRQ
	if (r == SUCCESS) {
		r = rm_irq_init();
	}
#endif
#ifdef CONFIG_RM_RA
	if (r == SUCCESS) {
		r = rm_ra_init();
	}
#endif
#ifdef CONFIG_RM_UDMAP
	if (r == SUCCESS) {
		r = rm_udmap_init();
	}
#endif
#ifdef CONFIG_RM_PROXY
	if (r == SUCCESS) {
		r = rm_proxy_init();
	}
#endif

	if (r != SUCCESS) {
		rm_trace_sub(TRACE_RM_ACTION_RM_INIT |
			     TRACE_RM_ACTION_FAIL, 0U, 0U);
	}

	return r;
}

s32 rm_deinit(devgrp_t devgrp)
{
	s32 r = SUCCESS;

#ifdef CONFIG_RM_IRQ
	if (r == SUCCESS) {
		r = rm_irq_deinit(devgrp);
	}
#endif
#ifdef CONFIG_RM_RA
	if (r == SUCCESS) {
		r = rm_ra_deinit(devgrp);
	}
#endif
#ifdef CONFIG_RM_UDMAP
	if (r == SUCCESS) {
		r = rm_udmap_deinit(devgrp);
	}
#endif
#ifdef CONFIG_RM_PROXY
	if (r == SUCCESS) {
		r = rm_proxy_deinit(devgrp);
	}
#endif

	return r;
}
