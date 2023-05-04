/*
 * IRQ Management
 *
 * IRQ Management SoC data for AM6
 *
 * Copyright (C) 2018-2020 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 */

#include <types/short_types.h>
#include <types/array_size.h>
#include <resasg_types.h>
#include <devices.h>
#include <devices_internal.h>
#include <hosts.h>
#include <hosts_internal.h>

#include "../../include/irq_resasg.h"

/* SoC devices that are sources for system events */
const u16 evt_rt_srcs[] = {
	AM6_DEV_NAVSS0_UDMASS_INTA0,
	AM6_DEV_NAVSS0_TIMER_MGR0,
	AM6_DEV_NAVSS0_TIMER_MGR1,
	AM6_DEV_NAVSS0_PROXY0,
	AM6_DEV_NAVSS0_RINGACC0,
	AM6_DEV_NAVSS0_UDMAP0,
	AM6_DEV_MCU_NAVSS0_INTR_AGGR_0,
	AM6_DEV_MCU_NAVSS0_PROXY0,
	AM6_DEV_MCU_NAVSS0_UDMAP0,
	AM6_DEV_MCU_NAVSS0_RINGACC0,
};

const u32 evt_rt_srcs_count = ARRAY_SIZE(evt_rt_srcs);

const struct irq_global_event_type irq_global_event_types[] = {
	{
		.start = 0U,
		.num = 4608U,
		.utype = RESASG_UTYPE(AM6_DEV_NAVSS0_UDMASS_INTA0, RESASG_SUBTYPE_GLOBAL_EVENT_SEVT),
	},
	{
		.start = 16384U,
		.num = 1536U,
		.utype = RESASG_UTYPE(AM6_DEV_MCU_NAVSS0_INTR_AGGR_0, RESASG_SUBTYPE_GLOBAL_EVENT_SEVT),
	},
	{
		.start = 20480U,
		.num = 1024U,
		.utype = RESASG_UTYPE(AM6_DEV_NAVSS0_MODSS_INTA0, RESASG_SUBTYPE_GLOBAL_EVENT_SEVT),
	},
	{
		.start = 22528U,
		.num = 1024U,
		.utype = RESASG_UTYPE(AM6_DEV_NAVSS0_MODSS_INTA1, RESASG_SUBTYPE_GLOBAL_EVENT_SEVT),
	},
	{
		.start = 32768U,
		.num = 512U,
		.utype = RESASG_UTYPE(AM6_DEV_NAVSS0_UDMASS_INTA0, RESASG_SUBTYPE_GLOBAL_EVENT_MEVT),
	},
	{
		.start = 34816U,
		.num = 128U,
		.utype = RESASG_UTYPE(AM6_DEV_MCU_NAVSS0_INTR_AGGR_0, RESASG_SUBTYPE_GLOBAL_EVENT_MEVT),
	},
	{
		.start = 36864U,
		.num = 512U,
		.utype = RESASG_UTYPE(AM6_DEV_NAVSS0_UDMASS_INTA0, RESASG_SUBTYPE_GLOBAL_EVENT_GEVT),
	},
	{
		.start = 39936U,
		.num = 256U,
		.utype = RESASG_UTYPE(AM6_DEV_MCU_NAVSS0_INTR_AGGR_0, RESASG_SUBTYPE_GLOBAL_EVENT_GEVT),
	},
	{
		.start = 49152U,
		.num = 1024U,
		.utype = RESASG_UTYPE(AM6_DEV_NAVSS0_UDMAP0, RESASG_SUBTYPE_GLOBAL_EVENT_TRIGGER),
	},
	{
		.start = 56320U,
		.num = 256U,
		.utype = RESASG_UTYPE(AM6_DEV_MCU_NAVSS0_UDMAP0, RESASG_SUBTYPE_GLOBAL_EVENT_TRIGGER),
	},
};

const u16 irq_global_event_types_count = ARRAY_SIZE(irq_global_event_types);
