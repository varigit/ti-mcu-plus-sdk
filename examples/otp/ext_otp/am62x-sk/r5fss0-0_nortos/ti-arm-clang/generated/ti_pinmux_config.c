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
#include "ti_drivers_config.h"
#include <drivers/pinmux.h>

static Pinmux_PerCfg_t gPinMuxMainDomainCfg[] = {
            /* I2C1 pin config */
    /* I2C1_SCL -> I2C1_SCL (B17) */
    {
        PIN_I2C1_SCL,
        ( PIN_MODE(0) | PIN_INPUT_ENABLE | PIN_PULL_DIRECTION )
    },
    /* I2C1_SDA -> I2C1_SDA (A17) */
    {
        PIN_I2C1_SDA,
        ( PIN_MODE(0) | PIN_INPUT_ENABLE | PIN_PULL_DIRECTION )
    },

            /* USART1 pin config */
    /* UART1_RXD -> MCASP0_AFSR (E19) */
    {
        PIN_MCASP0_AFSR,
        ( PIN_MODE(2) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
    /* UART1_TXD -> MCASP0_ACLKR (A20) */
    {
        PIN_MCASP0_ACLKR,
        ( PIN_MODE(2) | PIN_PULL_DISABLE )
    },
        
    {PINMUX_END, PINMUX_END}
};

static Pinmux_PerCfg_t gPinMuxMcuDomainCfg[] = {
        
                    /* WKUP_USART0 pin config */
    /* WKUP_UART0_RXD -> WKUP_UART0_RXD (B4) */
    {
        PIN_WKUP_UART0_RXD,
        ( PIN_MODE(0) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
    /* WKUP_UART0_TXD -> WKUP_UART0_TXD (C5) */
    {
        PIN_WKUP_UART0_TXD,
        ( PIN_MODE(0) | PIN_PULL_DISABLE )
    },

    {PINMUX_END, PINMUX_END}
};

/*
 * Pinmux
 */
void Pinmux_init(void)
{
    Pinmux_config(gPinMuxMainDomainCfg, PINMUX_DOMAIN_ID_MAIN);
    Pinmux_config(gPinMuxMcuDomainCfg, PINMUX_DOMAIN_ID_MCU);
}

