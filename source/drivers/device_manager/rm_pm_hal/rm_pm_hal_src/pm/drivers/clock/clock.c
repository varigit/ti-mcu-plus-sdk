/*
 * System Firmware
 *
 * Cortex-M3 (CM3) firmware for power management
 *
 * Copyright (C) 2020, Texas Instruments Incorporated
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
#include <clk_div.h>
#include <device.h>
#include <device_clk.h>
#include <types/errno.h>
#include <osal/osal_core.h>
#include <tisci/tisci_protocol.h>

#include <tisci/pm/tisci_pm_clock.h>
#include <limits.h>
#include <lib/trace.h>
#include <lib/mmr_lock.h>
#include <device_prepare.h>

s32 set_clock_handler(u32 *msg_recv)
{
	struct tisci_msg_set_clock_req *req =
		(struct tisci_msg_set_clock_req *) msg_recv;
	struct tisci_msg_set_clock_resp *resp =
		(struct tisci_msg_set_clock_resp *) msg_recv;
	struct device *dev = NULL;
	sbool gated = SFALSE;
	u32 id = req->device;
	dev_clk_idx_t clkidx;
	u8 state = req->state;
	s32 ret = SUCCESS;
	u32 flags = req->hdr.flags;

	mmr_unlock_all();

	/* Handle higher clock IDs */
	if (req->clk == 255UL) {
		clkidx = (dev_clk_idx_t) req->clk32;
	} else {
		clkidx = (dev_clk_idx_t) req->clk;
	}

	pm_trace(TRACE_PM_ACTION_MSG_RECEIVED, TISCI_MSG_SET_CLOCK);
	pm_trace(TRACE_PM_ACTION_MSG_PARAM_DEV_CLK_ID,
		 id | ((u32) clkidx << TRACE_PM_MSG_CLK_ID_SHIFT));
	pm_trace(TRACE_PM_ACTION_MSG_PARAM_VAL, state);

	resp->hdr.flags = 0U;

	ret = device_prepare_exclusive(req->hdr.host, id, NULL, &dev);
	if (ret == SUCCESS) {
		switch (state) {
		case TISCI_MSG_VALUE_CLOCK_SW_STATE_UNREQ:
			gated = STRUE;
			break;
		case TISCI_MSG_VALUE_CLOCK_SW_STATE_AUTO:
		case TISCI_MSG_VALUE_CLOCK_SW_STATE_REQ:
			gated = SFALSE;
			break;
		default:
			pm_trace(TRACE_PM_ACTION_INVALID_STATE, state);
			ret = -EINVAL;
			break;
		}
	}

	if (ret == SUCCESS) {
		if ((flags & TISCI_MSG_FLAG_CLOCK_ALLOW_SSC) != 0UL) {
			device_clk_set_ssc(dev, clkidx, STRUE);
		} else {
			device_clk_set_ssc(dev, clkidx, SFALSE);
		}

		if ((flags & TISCI_MSG_FLAG_CLOCK_ALLOW_FREQ_CHANGE) != 0UL) {
			device_clk_set_freq_change(dev, clkidx, STRUE);
		} else {
			device_clk_set_freq_change(dev, clkidx, SFALSE);
		}

		if ((flags & TISCI_MSG_FLAG_CLOCK_INPUT_TERM) != 0UL) {
			device_clk_set_input_term(dev, clkidx, STRUE);
		} else {
			device_clk_set_input_term(dev, clkidx, SFALSE);
		}

		if (!device_clk_set_gated(dev, clkidx, gated)) {
			ret = -EINVAL;
		}
	}

	mmr_lock_all();

	return ret;
}

s32 get_clock_handler(u32 *msg_recv)
{
	struct tisci_msg_get_clock_req *req =
		(struct tisci_msg_get_clock_req *) msg_recv;
	struct tisci_msg_get_clock_resp *resp =
		(struct tisci_msg_get_clock_resp *) msg_recv;
	struct device *dev = NULL;
	u32 id = req->device;
	dev_clk_idx_t clkidx;
	s32 ret = SUCCESS;

	mmr_unlock_all();

	/* Handle higher clock IDs */
	if (req->clk == 255UL) {
		clkidx = (dev_clk_idx_t) req->clk32;
	} else {
		clkidx = (dev_clk_idx_t) req->clk;
	}

	pm_trace(TRACE_PM_ACTION_MSG_RECEIVED, TISCI_MSG_GET_CLOCK);
	pm_trace(TRACE_PM_ACTION_MSG_PARAM_DEV_CLK_ID,
		 id | ((u32) clkidx << TRACE_PM_MSG_CLK_ID_SHIFT));

	resp->hdr.flags = 0U;

	ret = device_prepare_nonexclusive(req->hdr.host, id, NULL, &dev);
	if (ret == SUCCESS) {
		u8 prog;
		u8 state;

		prog = (u8) (device_clk_get_sw_gated(dev, clkidx) ?
			     TISCI_MSG_VALUE_CLOCK_SW_STATE_UNREQ :
			     TISCI_MSG_VALUE_CLOCK_SW_STATE_AUTO);

		state = (u8) (device_clk_get_hw_ready(dev, clkidx) ?
			      TISCI_MSG_VALUE_CLOCK_HW_STATE_READY :
			      TISCI_MSG_VALUE_CLOCK_HW_STATE_NOT_READY);

		if (device_clk_get_ssc(dev, clkidx)) {
			resp->hdr.flags |= TISCI_MSG_FLAG_CLOCK_ALLOW_SSC;
		}

		if (device_clk_get_hw_ssc(dev, clkidx)) {
			resp->hdr.flags |= TISCI_MSG_FLAG_CLOCK_SSC_ACTIVE;
		}

		if (device_clk_get_freq_change(dev, clkidx)) {
			resp->hdr.flags |= TISCI_MSG_FLAG_CLOCK_ALLOW_FREQ_CHANGE;
		}

		if (device_clk_get_input_term(dev, clkidx)) {
			resp->hdr.flags |= TISCI_MSG_FLAG_CLOCK_INPUT_TERM;
		}

		resp->programmed_state  = prog;
		resp->current_state     = state;
	}

	mmr_lock_all();

	return ret;
}

s32 set_clock_parent_handler(u32 *msg_recv)
{
	struct tisci_msg_set_clock_parent_req *req =
		(struct tisci_msg_set_clock_parent_req *) msg_recv;
	struct tisci_msg_set_clock_parent_resp *resp =
		(struct tisci_msg_set_clock_parent_resp *) msg_recv;
	struct device *dev = NULL;
	u32 id = req->device;
	dev_clk_idx_t clkidx;
	dev_clk_idx_t parent;
	s32 ret = SUCCESS;

	mmr_unlock_all();

	/* Handle higher clock IDs */
	if (req->clk == 255UL) {
		clkidx = (dev_clk_idx_t) req->clk32;
	} else {
		clkidx = (dev_clk_idx_t) req->clk;
	}

	if (req->parent == 255UL) {
		parent = (dev_clk_idx_t) req->parent32;
	} else {
		parent = (dev_clk_idx_t) req->parent;
	}

	pm_trace(TRACE_PM_ACTION_MSG_RECEIVED, TISCI_MSG_SET_CLOCK_PARENT);
	pm_trace(TRACE_PM_ACTION_MSG_PARAM_DEV_CLK_ID,
		 id | ((u32) clkidx << TRACE_PM_MSG_CLK_ID_SHIFT));
	pm_trace(TRACE_PM_ACTION_MSG_PARAM_VAL, parent);

	resp->hdr.flags = 0U;

	ret = device_prepare_exclusive(req->hdr.host, id, NULL, &dev);
	if (ret == SUCCESS) {
		if (!device_clk_set_parent(dev, clkidx, parent)) {
			ret = -EINVAL;
		}
	}

	mmr_lock_all();

	return ret;
}

s32 get_clock_parent_handler(u32 *msg_recv)
{
	struct tisci_msg_get_clock_parent_req *req =
		(struct tisci_msg_get_clock_parent_req *) msg_recv;
	struct tisci_msg_get_clock_parent_resp *resp =
		(struct tisci_msg_get_clock_parent_resp *) msg_recv;
	struct device *dev = NULL;
	u32 id = req->device;
	dev_clk_idx_t clkidx;
	s32 ret = SUCCESS;

	mmr_unlock_all();

	/* Handle higher clock IDs */
	if (req->clk == 255UL) {
		clkidx = (dev_clk_idx_t) req->clk32;
	} else {
		clkidx = (dev_clk_idx_t) req->clk;
	}

	pm_trace(TRACE_PM_ACTION_MSG_RECEIVED, TISCI_MSG_GET_CLOCK_PARENT);
	pm_trace(TRACE_PM_ACTION_MSG_PARAM_DEV_CLK_ID,
		 id | ((u32) clkidx << TRACE_PM_MSG_CLK_ID_SHIFT));

	resp->hdr.flags = 0U;

	ret = device_prepare_nonexclusive(req->hdr.host, id, NULL, &dev);
	if (ret == SUCCESS) {
		dev_clk_idx_t parent;

		parent = device_clk_get_parent(dev, clkidx);

		if (parent == DEV_CLK_ID_NONE) {
			ret = -EINVAL;
		} else {
			/* Use extended API for clock indexes >= 255 */
			if (parent >= 255UL) {
				resp->parent = 255U;
				resp->parent32 = parent;
			} else {
				resp->parent = (u8) parent;
				resp->parent32 = 0xffffffffU;
			}
		}
	}

	mmr_lock_all();

	return ret;
}

s32 get_num_clock_parents_handler(u32 *msg_recv)
{
	struct tisci_msg_get_num_clock_parents_req *req =
		(struct tisci_msg_get_num_clock_parents_req *) msg_recv;
	struct tisci_msg_get_num_clock_parents_resp *resp =
		(struct tisci_msg_get_num_clock_parents_resp *) msg_recv;
	struct device *dev = NULL;
	u32 id = req->device;
	dev_clk_idx_t clkidx;
	s32 ret = SUCCESS;

	mmr_unlock_all();

	/* Handle higher clock IDs */
	if (req->clk == 255UL) {
		clkidx = (dev_clk_idx_t) req->clk32;
	} else {
		clkidx = (dev_clk_idx_t) req->clk;
	}

	pm_trace(TRACE_PM_ACTION_MSG_RECEIVED,
		 TISCI_MSG_GET_NUM_CLOCK_PARENTS);
	pm_trace(TRACE_PM_ACTION_MSG_PARAM_DEV_CLK_ID,
		 id | ((u32) clkidx << TRACE_PM_MSG_CLK_ID_SHIFT));

	resp->hdr.flags = 0U;

	ret = device_prepare_nonexclusive(req->hdr.host, id, NULL, &dev);
	if (ret == SUCCESS) {
		dev_clk_idx_t num_parents;

		num_parents = device_clk_get_num_parents(dev, clkidx);

		if (num_parents == DEV_CLK_ID_NONE) {
			ret = -EINVAL;
		} else {
			/* Use extended API for clock indexes >= 255 */
			if (num_parents >= 255UL) {
				resp->num_parents = 255U;
				resp->num_parents32 = num_parents;
			} else {
				resp->num_parents = (u8) num_parents;
				resp->num_parents32 = 0xffffffffU;
			}
		}
	}

	mmr_lock_all();

	return ret;
}

s32 set_freq_handler(u32 *msg_recv)
{
	struct tisci_msg_set_freq_req *req =
		(struct tisci_msg_set_freq_req *) msg_recv;
	struct tisci_msg_set_freq_resp *resp =
		(struct tisci_msg_set_freq_resp *) msg_recv;
	struct device *dev = NULL;
	u32 id = req->device;
	u64 min_freq_hz = req->min_freq_hz;
	u64 target_freq_hz = req->target_freq_hz;
	u64 max_freq_hz = req->max_freq_hz;
	dev_clk_idx_t clkidx;
	s32 ret = SUCCESS;

	mmr_unlock_all();

	/* Handle higher clock IDs */
	if (req->clk == 255UL) {
		clkidx = (dev_clk_idx_t) req->clk32;
	} else {
		clkidx = (dev_clk_idx_t) req->clk;
	}

	pm_trace(TRACE_PM_ACTION_MSG_RECEIVED, TISCI_MSG_SET_FREQ);
	pm_trace(TRACE_PM_ACTION_MSG_PARAM_DEV_CLK_ID,
		 id | ((u32) clkidx << TRACE_PM_MSG_CLK_ID_SHIFT));

	resp->hdr.flags = 0U;

	ret = device_prepare_exclusive(req->hdr.host, id, NULL, &dev);
	if (ret == SUCCESS) {
		if ((min_freq_hz > ULONG_MAX) || (min_freq_hz > target_freq_hz) ||
		    (target_freq_hz > max_freq_hz)) {
			ret = -EINVAL;
		}
	}

	if (ret == SUCCESS) {
		if (max_freq_hz > ULONG_MAX) {
			max_freq_hz = ULONG_MAX;
		}

		if (target_freq_hz > ULONG_MAX) {
			target_freq_hz = ULONG_MAX;
		}

		if (!device_clk_set_freq(dev, clkidx, (u32) min_freq_hz,
					 (u32) target_freq_hz, (u32) max_freq_hz)) {
			ret = -EINVAL;
		}
	}

	mmr_lock_all();

	return ret;
}

s32 query_freq_handler(u32 *msg_recv)
{
	struct tisci_msg_query_freq_req *req =
		(struct tisci_msg_query_freq_req *) msg_recv;
	struct tisci_msg_query_freq_resp *resp =
		(struct tisci_msg_query_freq_resp *) msg_recv;
	struct device *dev = NULL;
	u32 id = req->device;
	u64 min_freq_hz = req->min_freq_hz;
	u64 target_freq_hz = req->target_freq_hz;
	u64 max_freq_hz = req->max_freq_hz;
	dev_clk_idx_t clkidx;
	u64 freq_hz;
	s32 ret = SUCCESS;

	mmr_unlock_all();

	/* Handle higher clock IDs */
	if (req->clk == 255UL) {
		clkidx = (dev_clk_idx_t) req->clk32;
	} else {
		clkidx = (dev_clk_idx_t) req->clk;
	}

	pm_trace(TRACE_PM_ACTION_MSG_RECEIVED, TISCI_MSG_QUERY_FREQ);
	pm_trace(TRACE_PM_ACTION_MSG_PARAM_DEV_CLK_ID,
		 id | ((u32) clkidx << TRACE_PM_MSG_CLK_ID_SHIFT));

	resp->hdr.flags = 0U;

	ret = device_prepare_exclusive(req->hdr.host, id, NULL, &dev);
	if (ret == SUCCESS) {
		if ((min_freq_hz > ULONG_MAX) || (min_freq_hz > target_freq_hz) ||
		    (target_freq_hz > max_freq_hz)) {
			ret = -EINVAL;
		}
	}

	if (ret == SUCCESS) {
		if (max_freq_hz > ULONG_MAX) {
			max_freq_hz = ULONG_MAX;
		}

		if (target_freq_hz > ULONG_MAX) {
			target_freq_hz = ULONG_MAX;
		}

		freq_hz = device_clk_query_freq(dev, clkidx, (u32) min_freq_hz,
						(u32) target_freq_hz,
						(u32) max_freq_hz);

		resp->freq_hz = freq_hz;
	}

	mmr_lock_all();

	return ret;
}

s32 get_freq_handler(u32 *msg_recv)
{
	struct tisci_msg_get_freq_req *req =
		(struct tisci_msg_get_freq_req *) msg_recv;
	struct tisci_msg_get_freq_resp *resp =
		(struct tisci_msg_get_freq_resp *) msg_recv;
	struct device *dev = NULL;
	u32 id = req->device;
	dev_clk_idx_t clkidx;
	s32 ret = SUCCESS;

	mmr_unlock_all();

	/* Handle higher clock IDs */
	if (req->clk == 255UL) {
		clkidx = (dev_clk_idx_t) req->clk32;
	} else {
		clkidx = (dev_clk_idx_t) req->clk;
	}

	pm_trace(TRACE_PM_ACTION_MSG_RECEIVED, TISCI_MSG_GET_FREQ);
	pm_trace(TRACE_PM_ACTION_MSG_PARAM_DEV_CLK_ID,
		 id | ((u32) clkidx << TRACE_PM_MSG_CLK_ID_SHIFT));

	resp->hdr.flags = 0U;

	ret = device_prepare_nonexclusive(req->hdr.host, id, NULL, &dev);
	if (ret == SUCCESS) {
		resp->freq_hz = device_clk_get_freq(dev, clkidx);
	}

	mmr_lock_all();

	return ret;
}
