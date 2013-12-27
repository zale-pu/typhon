/**********************************************************
*
* Proprietary & confidential, All right reserved, 1997
*         Silicon Storage Technology, Inc.
*
*     Module Name : SuperNandSim.c
*     Description : The functions driver internal used
**********************************************************/

/*********************************************************
*     Kaifeng Zhuang @ 2010.09.06
*     Description  : Initial create
**********************************************************/

/*********************************************************
*  Include section
*  Add all #includes here
*
**********************************************************/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include "file_lib.h"
#include "file_access.h"
#include "memory_file.h"
#include "supernand_sim.h"

/*********************************************************
 * Defines section
 * Add all #defines here
 *
 *********************************************************/
typedef struct _SUPERNAND_FILE {
	uint32_t        mChipAddr;
    FILE_HANDLE_S*  pFileHandle;
}SUPERNAND_FILE_S;

 /*********************************************************
 * Global variables section
 * Add all globalvariables here
 *
 *********************************************************/
static uint8_t           gUDPageCmpBuffer[256];
static uint8_t           gUDPageDataBuffer[256];
static SUPERNAND_FILE_S* g_SuperNandFiles = NULL;

static uint32_t ConvertPageAddr(uint32_t aRowAddr)
{
    uint32_t pageNo, blkNo, planeNo;

    pageNo = aRowAddr % gNandFlashSize.mPagesPerBlock;
    blkNo  = aRowAddr / gNandFlashSize.mPagesPerBlock;

    planeNo  = blkNo / gNandFlashSize.mBlocksPerPlane;
    blkNo  = blkNo % gNandFlashSize.mBlocksPerPlane;

    blkNo += planeNo * (gNandFlashSize.mBlocksPerChip / gNandFlashSize.mPlanesPerChip);
    pageNo += blkNo * gNandFlashSize.mPagesPerBlock;

    return pageNo;
}

static void GenerateChipFileName(uint32_t aChipAddr, char *apFileName)
{
    assert(apFileName);

    sprintf(apFileName, "%06x_data.bin", aChipAddr);
}

static uint8_t ChipFileOpen(SUPERNAND_FILE_S* pChipFile)
{
    char filename[64];

    GenerateChipFileName(pChipFile->mChipAddr, filename);
    pChipFile->pFileHandle = MemoryFileCreate(filename, BYTE_PER_UDCHIP);
    return FileOpen(pChipFile->pFileHandle);
}

static void ChipFileFlush(SUPERNAND_FILE_S* pChipFile, uint32_t bSave)
{
    if (bSave)
    {
        FileFlush(pChipFile->pFileHandle);
    }
}

static void ChipFileClose(SUPERNAND_FILE_S* pChipFile, uint32_t bSave)
{
    if (bSave)
    {
        FileFlush(pChipFile->pFileHandle);
    }

    FileClose(pChipFile->pFileHandle);
}

static SUPERNAND_FILE_S* GetChipFile(uint32_t aChipAddr, uint32_t aRowAddr)
{
    uint32_t iChip;

    for (iChip=0; iChip<gNandFlashChipCnt; iChip++)
    {
        SUPERNAND_FILE_S* pChipFile = &g_SuperNandFiles[iChip];
        assert(pChipFile);
        if (pChipFile->mChipAddr == aChipAddr)
        {
            return pChipFile;
        }
    }

     return NULL;
}

static uint32_t NandReadUDPage(uint32_t aChipAddr, uint32_t aRowAddr, uint8_t* pDataBuffer, uint32_t aDataLen)
{
    SUPERNAND_FILE_S* pChipFile;

    pChipFile = GetChipFile(aChipAddr, aRowAddr);
    assert(pChipFile);

    if (pChipFile != NULL)
    {
        FileRead(pChipFile->pFileHandle, BYTE_PER_UDPAGE*aRowAddr, aDataLen, pDataBuffer);
    }

    return SUCCESS;
}

static uint32_t NandWriteUDPage(uint32_t aChipAddr, uint32_t aRowAddr, uint8_t* pDataBuffer, uint32_t aDataLen)
{
    SUPERNAND_FILE_S* pChipFile;

    pChipFile = GetChipFile(aChipAddr, aRowAddr);
    assert(pChipFile);

    if (pChipFile != NULL)
    {
        FileWrite(pChipFile->pFileHandle, BYTE_PER_UDPAGE*aRowAddr, aDataLen, pDataBuffer);
    }

    return SUCCESS;
}

static void NandEraseFromBlock(uint32_t aChipAddr, uint32_t aRowAddr)
{
    uint8_t* pUDBlockDataBuffer;
    SUPERNAND_FILE_S* pChipFile;

    pChipFile = GetChipFile(aChipAddr, aRowAddr);
    assert(pChipFile);

    if (pChipFile != NULL)
    {
        pUDBlockDataBuffer = (uint8_t*)malloc(BYTE_PER_UDBLOCK);
        memset(pUDBlockDataBuffer, 0x00, BYTE_PER_UDBLOCK);
        FileWrite(pChipFile->pFileHandle, BYTE_PER_UDPAGE*aRowAddr, BYTE_PER_UDBLOCK, pUDBlockDataBuffer);
        free(pUDBlockDataBuffer);
    }
}

static uint8_t NandIsEraseUDPage(uint32_t aChipAddr, uint32_t aRowAddr, uint8_t *apCtrl)
{
    SUPERNAND_FILE_S* pChipFile;

    pChipFile = GetChipFile(aChipAddr, aRowAddr);
    assert(pChipFile);

    if (pChipFile != NULL)
    {
        memset(gUDPageCmpBuffer, 0x00, BYTE_PER_UDPAGE);

        FileRead(pChipFile->pFileHandle, BYTE_PER_UDPAGE*aRowAddr, BYTE_PER_UDPAGE, gUDPageDataBuffer);
        if (memcmp(gUDPageDataBuffer, gUDPageCmpBuffer, BYTE_PER_UDPAGE) == 0)
        {
            return SUCCESS;
        }
        else
        {
            if (apCtrl)
            {
                memcpy(apCtrl, gUDPageDataBuffer+BYTE_PER_UDROWPAGE, BYTE_PER_UDCONTROL);
            }
            return FAILURE;
        }
    }

    return FAILURE;
}

uint8_t ReadDataFromPage( uint32_t aChipAddr, uint32_t aRowAddr, uint8_t* pDataBuffer, uint8_t* pCtrlBuffer)
{
    aRowAddr = ConvertPageAddr(aRowAddr);

#if CHECK_OVERWRITE
	if (CheckNandPageBlank(aChipAddr, aRowAddr, 0))
	{
        if (pDataBuffer != NULL)
        {
            memset(pDataBuffer, 0x00, BYTE_PER_UDPAGE);
        }
		return FAILURE;
	}
#endif

    memset(gUDPageDataBuffer, 0x00, BYTE_PER_UDPAGE);
    NandReadUDPage(aChipAddr, aRowAddr, gUDPageDataBuffer, BYTE_PER_UDPAGE);

    if (pDataBuffer != NULL)
    {
        memcpy(pDataBuffer, gUDPageDataBuffer, BYTE_PER_UDROWPAGE);
    }

    if (pCtrlBuffer != NULL)
    {
        memcpy(pCtrlBuffer, &gUDPageDataBuffer[BYTE_PER_UDROWPAGE], BYTE_PER_UDCONTROL);
    }

    return SUCCESS;
}

uint8_t WriteDataToPage(uint32_t aChipAddr, uint32_t aRowAddr, uint8_t* pDataBuffer, uint8_t* pCtrlBuffer)
{
    aRowAddr = ConvertPageAddr(aRowAddr);

#if CHECK_OVERWRITE
    CheckNandPageBlank(aChipAddr, aRowAddr, 1);
#endif

    memset(gUDPageDataBuffer, 0, BYTE_PER_UDPAGE);
    if (pDataBuffer != NULL)
    {
        memcpy(&gUDPageDataBuffer[0], pDataBuffer, BYTE_PER_UDROWPAGE);
    }

    if (pCtrlBuffer != NULL)
    {
        memcpy(&gUDPageDataBuffer[BYTE_PER_UDROWPAGE], pCtrlBuffer, BYTE_PER_UDCONTROL);
    }

    NandWriteUDPage(aChipAddr, aRowAddr, gUDPageDataBuffer, BYTE_PER_UDPAGE);

    return SUCCESS;
}

void EraseDataFromBlock(uint32_t aChipAddr, uint32_t aRowAddr)
{
    uint32_t cvtRow = ConvertPageAddr(aRowAddr);

    NandEraseBlock(aChipAddr, aRowAddr);
    NandEraseFromBlock(aChipAddr, cvtRow);
}

uint8_t IsEraseDataFromPage(uint32_t aChipAddr, uint32_t aRowAddr, uint8_t* apCtrl)
{
    uint32_t cvtRow = ConvertPageAddr(aRowAddr);

    if (NandIsEraseUDPage(aChipAddr, cvtRow, apCtrl) == SUCCESS)
    {
        return SUCCESS;//NandIsPageErased(aChipAddr, aRow, apCtrl);
    }
    else
    {
        return FAILURE;
    }
}

uint8_t ChipFilesInit(void)
{
    uint32_t iChip;

    g_SuperNandFiles = (SUPERNAND_FILE_S*)malloc(sizeof(SUPERNAND_FILE_S)*gNandFlashChipCnt);

    for (iChip=0; iChip<gNandFlashChipCnt; iChip++)
    {
        g_SuperNandFiles[iChip].mChipAddr = iChip;
        if (ChipFileOpen(&g_SuperNandFiles[iChip]) == FAILURE)
        {
            return FAILURE;
        }
    }

    return SUCCESS;
}

void ChipFilesFlash(void)
{
    uint32_t iChip;

    if (g_SuperNandFiles != NULL)
    {
        for (iChip=0; iChip<gNandFlashChipCnt; iChip++)
        {
            ChipFileFlush(&g_SuperNandFiles[iChip], 1);
        }
    }
}

void ChipFilesExit(void)
{
    uint32_t iChip;

    if (g_SuperNandFiles != NULL)
    {
        for (iChip=0; iChip<gNandFlashChipCnt; iChip++)
        {
            ChipFileClose(&g_SuperNandFiles[iChip], 0);
        }
        free(g_SuperNandFiles);
        g_SuperNandFiles = NULL;
    }
}

