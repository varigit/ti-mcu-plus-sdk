/*
 * System Firmware
 *
 * am62x soc baseaddress.h
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

#ifndef LPM_SOC_BASEADDRESS_H_
#define LPM_SOC_BASEADDRESS_H_

#ifdef __cplusplus
extern "C" {
#endif

#define PLLMMR1_CFG_BASE                (0x00680000U)
#define MAIN_PLL_MMR_BASE               PLLMMR1_CFG_BASE
#define PLLMMR0_CFG_BASE                (0x04040000U)
#define MCU_PLL_MMR_BASE                PLLMMR0_CFG_BASE
#define MMR1_CFG_BASE                   (0x04500000U)
#define MCU_CTRL_MMR_BASE               MMR1_CFG_BASE
#define MMR0_CFG_BASE                   (0x43000000U)
#define WKUP_CTRL_MMR_BASE              MMR0_CFG_BASE

#define MCU_PSC_BASE                    (0x04000000U)
#define PD_GP_CORE_CTL_MCU              0
#define PD_MCU_M4F                      1
#define LPSC_MCU_ALWAYSON               0
#define LPSC_MAIN2MCU_ISO               1
#define LPSC_MCU2MAIN_ISO               2
#define LPSC_DM2SAFE_ISO                3
#define LPSC_MCU2DM_ISO                 4
#define LPSC_MCU_TEST                   5
#define LPSC_MCU_M4F                    6
#define LPSC_MCU_MCANSS0                7
#define LPSC_MCU_MCANSS1                8
#define LPSC_MCU_COMMON                 9

#define MAIN_PSC_BASE                   (0x00400000U)
#define PD_GP_CORE_CTL                  0
#define PD_SMS                          1
#define PD_ICSSM                        2
#define PD_CPSW                         3
#define PD_A53_CLUSTER_0                4
#define PD_A53_0                        5
#define PD_GPU                          9
#define PD_DSS                          10
#define PD_DEBUG                        11
#define PD_MAIN_IP                      12
#define LPSC_MAIN_DM                    1       /* PD_GP_CORE_CTL */
#define LPSC_MAIN2DM_ISO                3       /* PD_GP_CORE_CTL */
#define LPSC_DM2MAIN_ISO                4       /* PD_GP_CORE_CTL */
#define LPSC_CENTRAL2DM_INFRA_ISO       5       /* PD_GP_CORE_CTL */
#define LPSC_DM2CENTRAL_ISO             6       /* PD_GP_CORE_CTL */
#define LPSC_CENTRAL2DM_ISO             7       /* PD_GP_CORE_CTL */
#define LPSC_EMIF_LOCAL                 9       /* PD_GP_CORE_CTL */
#define LPSC_EMIF_CFG_ISO               10      /* PD_GP_CORE_CTL */
#define LPSC_EMIF_DATA_ISO              11      /* PD_GP_CORE_CTL */
#define LPSC_MAIN_TEST                  14      /* PD_GP_CORE_CTL */
#define LPSC_SMS_COMMON                 27      /* PD_SMS */
#define LPSC_TIFS                       29      /* PD_SMS */
#define LPSC_HSM                        30      /* PD_SMS */
#define LPSC_SA3UL                      31      /* PD_SMS */
#define LPSC_HSM_ISO                    32      /* PD_SMS */
#define LPSC_A53_CLUSTER_0              35      /* PD_A53_CLUSTER_0 */
#define LPSC_A53_0                      38      /* PD_A53_0 */
#define LPSC_DEBUGSS                    45      /* PD_DEBUG */
#define LPSC_MAIN_IP                    46      /* PD_MAIN_IP */

#define DDR_CTRL_BASE                   (0x0f308000U)
#define VIM_BASE                        (0x2fff0000U)
#define ROM_SEC_PROXY_RT_ADDRESS        (0x44880000U)
#define ROM_SEC_PROXY_TARGET_ADDRESS    (0x43600000U)
#define TIFS_SEC_PROXY_RT_ADDRESS       (0x4a600000U)
#define TIFS_SEC_PROXY_TARGET_ADDRESS   (0x4d000000U)
#define SEC_PROXY_MSG_RX_TID            0
#define SEC_PROXY_MSG_TX_TID            1
#define TIFS_SEC_PROXY_MSG_RX_TID               69
#define TIFS_SEC_PROXY_MSG_TX_TID               61
#define DM2DMSC_SEC_PROXY_MSG_RX_TID            22
#define DM2DMSC_SEC_PROXY_MSG_TX_TID            23

#ifdef __cplusplus
}
#endif
#endif /* LPM_SOC_BASEADDRESS_H_ */
