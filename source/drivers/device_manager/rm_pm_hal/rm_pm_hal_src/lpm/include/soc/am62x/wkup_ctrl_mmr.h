/*
 * System Firmware
 *
 * am62x lpm wkup_ctrl_mmr.h
 *
 * Copyright (C) 2021 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 */

#ifndef LPM_WKUP_CTRL_MMR_H_
#define LPM_WKUP_CTRL_MMR_H_

#ifdef __cplusplus
extern "C" {
#endif

#define WKUP_CLKSEL                     (0x00008010U)
#define DDR16SS_PMCTRL                  (0x000080d0U)
#define WKUP_WWD0_CTRL                  (0x00008340U)
#define PMCTRL_IO_0                     (0x00018084U)
#define PMCTRL_MOSC                     (0x00018090U)
#define RST_CTRL                        (0x00018170U)
#define SLEEP_STATUS                    (0x00018410U)
#define DS_MAGIC_WORD                   (0x00018418U)
#define DS_MAIN                         (0x00018420U)
#define DS_DM_RESET                     (0x00018440U)
#define DS_DDR0_RESET                   (0x00018450U)
#define DS_USB0_RESET                   (0x00018460U)
#define DS_USB1_RESET                   (0x00018464U)

/* WKUP0_EN */
#define WKUP0_EN                        (0x00018180U)

#define WKUP0_EN_WKUP_I2C0              BIT(0)
#define WKUP0_EN_WKUP_USART0            BIT(1)
#define WKUP0_EN_MCU_GPIO0              BIT(2)
#define WKUP0_EN_DEBUG                  BIT(3)
#define WKUP0_EN_MCU_M4F_INT            BIT(4)
#define WKUP0_EN_WKUP_DMTIMER0          BIT(5)
#define WKUP0_EN_WKUP_DMTIMER1          BIT(6)
#define WKUP0_EN_WKUP_RTC               BIT(7)
#define WKUP0_EN_MAIN_RESET_REQZ        BIT(8)
#define WKUP0_EN_USB0                   BIT(9)
#define WKUP0_EN_USB1                   BIT(10)
#define WKUP0_EN_MAIN_IO_DAISY_CHAIN    BIT(16)
#define WKUP0_EN_MCU_IO_DAISY_CHAIN     BIT(17)
#define WKUP0_EN_CANUART_IO_DAISY_CHAIN BIT(18)

/* WKUP_CLKSEL */
#define WKUP_CLKSEL_MAIN                0
#define WKUP_CLKSEL_MCU                 1

/* DDR16SS_PMCTRL */
#define DDR16SS_RETENTION_DIS           0
#define DDR16SS_RETENTION_EN            0x6

/* WKUP_WWD0_CTRL */
#define WWD_RUN                         0
#define WWD_STOP                        0xA

/* PMCTRL_IO_0 */
#define PMCTRL_IO_0_IO_ISO_STATUS       BIT(25)
#define PMCTRL_IO_0_IO_ISO_CTRL         BIT(24)
#define PMCTRL_IO_0_IO_ON_STATUS        BIT(5)

/* PMCTRL_MOSC */
#define PMCTRL_MOSC_OSC_CG_ON_WFI       BIT(31)

/* RST_CTRL */
#define RST_CTRL_MAIN_RST_ISO_DONE_Z    BIT(18)

/* SLEEP_STATUS */
#define SLEEP_STATUS_MAIN_RESETSTATZ	BIT(8)

/* DS_MAGIC_WORD */
#define DS_MAGIC_WORD_RESUME_TIFS	0xdee51ee5
#define DS_MAGIC_WORD_RESUME_ROM	0x00d5d02e

/* DS_MAIN */
#define DS_MAIN_OFF                     0x6
#define DS_MAIN_ON                      0xF

/* DS_DM_RESET */
#define DS_DM_RESET_MASK                0x6
#define DS_DM_RESET_UNMASK              0xF

/* DS_DDDR0_RESET */
#define DS_DDR0_RESET_MASK              0x6
#define DS_DDR0_RESET_UNMASK            0xF

#ifdef __cplusplus
}
#endif
#endif /* LPM_WKUP_CTRL_MMR_H_ */
