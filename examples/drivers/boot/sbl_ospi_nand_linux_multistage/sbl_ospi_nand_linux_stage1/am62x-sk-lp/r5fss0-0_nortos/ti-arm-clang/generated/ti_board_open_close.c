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

#include "ti_board_open_close.h"

int32_t Board_driversOpen(void)
{
    int32_t status = SystemP_SUCCESS;
    if(status==SystemP_SUCCESS)
    {
        status = Board_flashOpen();
    }
    return status;
}

void Board_driversClose(void)
{
    Board_flashClose();
}

/*
 * FLASH
 */
/* FLASH Object - initialized during Flash_open() */
Flash_NandOspiObject gFlashObject_W35N01JWTBAG;

/* FLASH Driver handles - opened during Board_flashOpen() */
Flash_Handle gFlashHandle[CONFIG_FLASH_NUM_INSTANCES];

/* FLASH Attrs */
Flash_Attrs gFlashAttrs_W35N01JWTBAG =
{
    .flashName = "W35N01JWTBAG",
    .deviceId = 0xDC21,
    .manufacturerId = 0xEF,
    .flashSize = 134217728,
    .blockCount = 512,
    .blockSize = 262144,
    .pageCount = 64,
    .pageSize = 4096,
};

/* FLASH DevConfig */
Flash_DevConfig gFlashDevCfg_W35N01JWTBAG =
{
    .cmdExtType = OSPI_CMD_EXT_TYPE_REPEAT,
    .cmdWren = 0x06,
    .cmdPageLoad = 0x13,
    .cmdPageProg = 0x10,
    .eraseCfg = {
        .blockSize = 262144,
        .cmdBlockErase = 0xD8,
    },
    .idCfg = {
        .cmd = 0x9F, /* Constant */
        .numBytes = 3,
        .dummy4 = 0,
        .dummy8 = 0,
    },
    .protocolCfg = {
        .protocol = FLASH_CFG_PROTO_8D_8D_8D,
        .isDtr = TRUE,
        .cmdRd = 0x9D,
        .cmdWr = 0x84,
        .dummyClksCmd = 8,
        .dummyClksRd = 8,
        .protoCfg = {
            .isAddrReg = TRUE,
            .cmdRegRd = 0x85,
            .cmdRegWr = 0x81,
            .cfgReg = 0x0,
            .shift = 0,
            .mask = 0xFF,
            .cfgRegBitP = 231,
        },
        .dummyCfg  = {
            .isAddrReg = TRUE,
            .cmdRegRd = 0x85,
            .cmdRegWr = 0x81,
            .cfgReg = 0x01,
            .shift = 0,
            .mask = 0xFF,
            .cfgRegBitP = 8,
        },
    },
    .cmdRdsr = 0x0F,
    .cmdWrsr = 0x1F,
    .srWip = (1 << 0),
    .srWipReg = 0xC0,
    .xspiWipRdCmd = 0x0F,
    .xspiWipReg = 0x0000C000,
    .xspiRdsrDummy = 7,
    .progStatusReg = 0xC0,
    .xspiProgStatusReg = 0x0000C000,
    .eraseStatusReg = 0xC0,
    .xspiEraseStatusReg = 0x0000C000,
    .srProgStatus = (1 << 3),
    .srEraseStatus = (1 << 2),
    .srWriteProtectReg = 0xA0,
    .srWriteProtectMask = 255,
};

/* FLASH Config */
Flash_Config gFlashConfig[CONFIG_FLASH_NUM_INSTANCES] =
{
    {
        .attrs = &gFlashAttrs_W35N01JWTBAG,
        .fxns = &gFlashNandOspiFxns,
        .devConfig = &gFlashDevCfg_W35N01JWTBAG,
        .object = (void *)&gFlashObject_W35N01JWTBAG,
    },
};

/* Externs of quirk functions and custom protocol functions */

/* FLASH Open Params - populated from SysConfig options */
Flash_Params gFlashParams[CONFIG_FLASH_NUM_INSTANCES] =
{
    {
        .quirksFxn = NULL,
        .custProtoFxn = NULL,
    },
};

uint32_t gFlashConfigNum = CONFIG_FLASH_NUM_INSTANCES;

int32_t Board_flashOpen()
{
    uint32_t instCnt;
    int32_t  status = SystemP_SUCCESS;

    /* Set the peripheral instance to use in the under lying flash implementation */
    gFlashConfig[CONFIG_FLASH0].attrs->driverInstance = CONFIG_OSPI0;

    for(instCnt = 0U; instCnt < CONFIG_FLASH_NUM_INSTANCES; instCnt++)
    {
        gFlashHandle[instCnt] = NULL;   /* Init to NULL so that we can exit gracefully */
    }

    /* Set the underlying driver instance to the FLASH config */
    gFlashAttrs_W35N01JWTBAG.driverInstance = CONFIG_OSPI0;

    /* Open all instances */
    for(instCnt = 0U; instCnt < CONFIG_FLASH_NUM_INSTANCES; instCnt++)
    {

        gFlashHandle[instCnt] = Flash_open(instCnt, &gFlashParams[instCnt]);
        if(NULL == gFlashHandle[instCnt])
        {
            DebugP_logError("FLASH open failed for instance %d !!!\r\n", instCnt);
            status = SystemP_FAILURE;
            break;
        }
    }

    if(SystemP_FAILURE == status)
    {
        Board_flashClose();   /* Exit gracefully */
    }
    return status;
}

void Board_flashClose(void)
{
    uint32_t instCnt;

    /* Close all instances that are open */
    for(instCnt = 0U; instCnt < CONFIG_FLASH_NUM_INSTANCES; instCnt++)
    {
        if(gFlashHandle[instCnt] != NULL)
        {
            Flash_close(gFlashHandle[instCnt]);
            gFlashHandle[instCnt] = NULL;
        }
    }
    return;
}

