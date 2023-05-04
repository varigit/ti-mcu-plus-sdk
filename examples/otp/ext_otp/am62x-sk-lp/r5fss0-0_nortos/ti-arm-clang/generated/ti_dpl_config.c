/*
 *  Copyright (C) 2021 Texas Instruments Incorporated
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*
 * Auto generated file 
 */
#include <stdio.h>
#include <drivers/soc.h>
#include <kernel/dpl/AddrTranslateP.h>
#include "ti_dpl_config.h"
#include "ti_drivers_config.h"

/* ----------- HwiP ----------- */
HwiP_Config gHwiConfig = {
    .intcBaseAddr = 0x2FFF0000u,
};

/* ----------- ClockP ----------- */
#define TIMER0_CLOCK_SRC_MUX_ADDR (0x430081B0u)
#define TIMER0_CLOCK_SRC_MCU_HFOSC0 (0x0u)
#define TIMER0_BASE_ADDR     (0x2B100000u)

ClockP_Config gClockConfig = {
    .timerBaseAddr = TIMER0_BASE_ADDR,
    .timerHwiIntNum = 138,
    .timerInputClkHz = 25000000,
    .timerInputPreScaler = 1,
    .usecPerTick = 1000,
};

/* ----------- DebugP ----------- */
void putchar_(char character)
{
    /* Output to UART console */
    DebugP_uartLogWriterPutChar(character);
}


void Dpl_init(void)
{
    /* initialize Hwi but keep interrupts disabled */
    HwiP_init();

    /* init debug log zones early */
    /* Debug log init */
    DebugP_logZoneEnable(DebugP_LOG_ZONE_ERROR);
    /* UART console to use for reading input */
    DebugP_uartSetDrvIndex(CONFIG_UART0);


    /* set timer clock source */
    SOC_controlModuleUnlockMMR(SOC_DOMAIN_ID_WKUP, 2);
    *(volatile uint32_t*)(TIMER0_CLOCK_SRC_MUX_ADDR) = TIMER0_CLOCK_SRC_MCU_HFOSC0;
    SOC_controlModuleLockMMR(SOC_DOMAIN_ID_WKUP, 2);
    /* initialize Clock */
    ClockP_init();


    /* Enable interrupt handling */
    HwiP_enable();
}

void Dpl_deinit(void)
{
}
