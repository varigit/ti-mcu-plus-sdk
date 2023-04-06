/*
 * System Firmware
 *
 * Minimal driver for UART access
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

/* Register Definitions */

#define UART_16550_THR                  0x00
#define UART_16550_RHR                  0x00
#define UART_16550_DLL                  0x00

#define UART_16550_IER                  0x04
#define UART_16550_DLH                  0x04

#define UART_16550_IIR                  0x08
#define UART_16550_FCR                  0x08

#define UART_16550_LCR                  0x0C

#define UART_16550_MCR                  0x10

#define UART_16550_LSR                  0x14
#define UART_16550_MSR_TCR              0x18
#define UART_16550_SPR_TLR              0x1C
#define UART_16550_MDR1                 0x20
#define UART_16550_MDR2                 0x24
#define UART_16550_BLR_UASR             0x38
#define UART_16550_ACREG                0x3C
#define UART_16550_SCR                  0x40
#define UART_16550_SSR                  0x44
#define UART_16550_EBLR                 0x48
#define UART_16550_MVR                  0x50
#define UART_16550_SYSC                 0x54
#define UART_16550_SYSS                 0x58
#define UART_16550_WER                  0x5C
#define UART_16550_CFPS                 0x60
#define UART_16550_RXFIFO_LVL           0x64
#define UART_16550_TXFIFO_LVL           0x68
#define UART_16550_IER2                 0x6C
#define UART_16550_ISR2                 0x70
#define UART_16550_MDR3                 0x80
#define UART_16550_TDT                  0x84
#define UART_16550_MDR4                 0x88
#define UART_16550_EFR2                 0x8C
#define UART_16550_ECR                  0x90
#define UART_16550_SCCR                 0xA0
#define UART_16550_ERHR_ETHR            0xA4
#define UART_16550_MAR                  0xA8
#define UART_16550_MMR                  0xAC
#define UART_16550_MBR                  0xB0

/* Line Control Register bits */
#define UART_16550_LCR_DLAB             (1 << 7)        /* Divisor Latch Access */
#define UART_16550_LCR_SETB             (1 << 6)        /* Set BREAK Condition */
#define UART_16550_LCR_SETP             (1 << 5)        /* Set Parity to LCR[4] */
#define UART_16550_LCR_EVEN             (1 << 4)        /* Even Parity Format */
#define UART_16550_LCR_PAR              (1 << 3)        /* Parity */
#define UART_16550_LCR_STOP             (1 << 2)        /* Stop Bit */
#define UART_16550_LCR_WORDSZ_5         0               /* Word Length of 5 */
#define UART_16550_LCR_WORDSZ_6         1               /* Word Length of 6 */
#define UART_16550_LCR_WORDSZ_7         2               /* Word Length of 7 */
#define UART_16550_LCR_WORDSZ_8         3               /* Word Length of 8 */

/* FIFO Control Register bits */
#define UART_16550_FCR_RXTRIG_8         (0 << 6)
#define UART_16550_FCR_RXTRIG_16        (1 << 6)
#define UART_16550_FCR_RXTRIG_56        (2 << 6)
#define UART_16550_FCR_RXTRIG_60        (3 << 6)
#define UART_16550_FCR_TXTRIG_8         (0 << 4)
#define UART_16550_FCR_TXTRIG_16        (1 << 4)
#define UART_16550_FCR_TXTRIG_56        (2 << 4)
#define UART_16550_FCR_TXTRIG_60        (3 << 4)
#define UART_16550_FCR_DMAEN            (1 << 3)        /* Enable DMA mode */
#define UART_16550_FCR_TXCLR            (1 << 2)        /* Clear contents of Tx FIFO */
#define UART_16550_FCR_RXCLR            (1 << 1)        /* Clear contents of Rx FIFO */
#define UART_16550_FCR_FIFOEN           (1 << 0)        /* Enable the Tx/Rx FIFO */

/* Line Status Register bits */
#define UART_16550_LSR_SR_E             (1 << 6)        /* Shift register empty */
#define UART_16550_LSR_TX_FIFO_E        (1 << 5)        /* TX FIFO empty */

/* Modem Control Register bits */
#define UART_16550_MCR_RTS              (1 << 0)        /* Request to Send */
#define UART_16550_MCR_DTR              (1 << 1)        /* Data Terminal Ready */

struct uart_16550_config {
	u32	base_addr;
	u32	baud_rate;
	u32	uart_clk;
};

/* SoC Specific Configs */
/* Boot time UART configuration */
extern const struct uart_16550_config soc_uart_16550_boot_config;

/** Post PLL setup UART configuration */
extern const struct uart_16550_config soc_uart_16550_reinit_config;

void lpm_serial_8250_init(const struct uart_16550_config *cfg);
int lpm_console_tx(int data);
int lpm_puts(const char *str);

void lpm_console_init(void);
void lpm_console_bypass_init(void);
