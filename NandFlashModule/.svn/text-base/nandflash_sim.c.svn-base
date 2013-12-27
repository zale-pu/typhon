/**********************************************************
 *     Module Name : nandflash_sim.c
 *     Description : nand flash sim implement
***********************************************************/

/***********************************************************
 *     Kaifeng Zhuang@2013.08.03
 *     Description : Initial Create
************************************************************/

/****************************************************
 *  Include section
 * Add all #includes here
 *
****************************************************/
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>

#include "bitmap.h"
#include "file_lib.h"
#include "nandflash_sim.h"
#include "basicnand_sim.h"
#include "supernand_sim.h"

/***************************************************
 * Enum section
 * Add all enums and unions here
 *
****************************************************/

/***************************************************
 * Structure and Union section
 * Add all structures and unions here
 *
****************************************************/

/***************************************************
 *  Variables section
 * Add all variable decleartion here
 *
****************************************************/
static uint8_t  gNandSizeInitDone = 0;
static uint32_t gNandOverWriteStack[16];
static uint32_t gNandOverWriteStackPtr = 0;

void (*gpfFailureCallBack)(void);

uint16_t  gNandFlashChipCnt;
NANDFLASH_SIZE_S gNandFlashSize;
NANDFLASH_CHIP_S gNandFlashChips[MAX_NANDFLASH_CHIP_CNT];

NANDFLASH_CHIP_S* GetNandFlashChip(uint32_t aChipAddr)
{
    uint16_t i;

    for (i=0; i<MAX_NANDFLASH_CHIP_CNT; i++)
    {
        if (aChipAddr == gNandFlashChips[i].mChipAddr)
        {
            return &gNandFlashChips[i];
        }
    }

    return NULL;
}

uint8_t NandReadID(NANDFLASH_CHIP_S* pNandFlashChip, uint8_t* pID)
{
	memset(pID, 0xFF, MAX_NAND_ID_LENGTH);
	if (!pNandFlashChip)
	{
	    return false;
	}

	memcpy(pID, pNandFlashChip->mID, MAX_NAND_ID_LENGTH);

	return true;
}

uint8_t NandIsValidBlock(uint32_t aBlockAddr)
{
    uint8_t planeIdx;

    assert(gNandSizeInitDone);

    planeIdx = aBlockAddr / gNandFlashSize.mBlocksPerPlane;
    aBlockAddr = aBlockAddr % gNandFlashSize.mBlocksPerPlane;

    return ((aBlockAddr < gNandFlashSize.mBlocksPerPlane) && (planeIdx < gNandFlashSize.mPlanesPerChip));
}

uint32_t NandCheckPageBlank(uint32_t aChipAddr, uint32_t aRowAddr, uint8_t aIsWr)
{
    uint32_t          blockNo, pageNo;
	NANDFLASH_CHIP_S* pNandFlashChip;

    pNandFlashChip = GetNandFlashChip(aChipAddr);
    if (!pNandFlashChip)
    {
        printf("\nERROR, Invalid ChipAddr(%x)!\n", aChipAddr);
        if (gpfFailureCallBack)
        {
            gpfFailureCallBack();
        }
    }

    pageNo = aRowAddr % gNandFlashSize.mPagesPerBlock;
    blockNo = aRowAddr / gNandFlashSize.mPagesPerBlock;

    if (NandIsValidBlock(blockNo) == false)
    {
        printf("\nERROR, Invalid Block(0x%x)!\n", blockNo);
        if (gpfFailureCallBack)
        {
            gpfFailureCallBack();
        }
    }

    if (!aIsWr)
    {
        if (BitmapTstBit(pNandFlashChip->mWrPageImg[blockNo], pageNo) != 0)
        {
            return false;
        }
    }
    else
    {
        if ((BitmapTstBit(pNandFlashChip->mWrPageImg[blockNo], pageNo) == 0) && (!NandOverWriteIsEnabled()))
        {
            printf("\nERROR, Chip:%x, Blk:%x, Page:%x Overwrite!\n", aChipAddr, blockNo, pageNo);
            if (gpfFailureCallBack)
            {
                gpfFailureCallBack();
            }
        }

        BitmapClrBit(pNandFlashChip->mWrPageImg[blockNo], pageNo);
        pNandFlashChip->mWrPageImgDirty = 1;
    }

    return true;
}

uint8_t NandReadPage(uint32_t aChipAddr, uint32_t aRowAddr, uint32_t aColAddr, uint8_t* pBuffer, uint32_t aLength)
{
    #if CHECK_OVERWRITE
    if (CheckNandPageBlank(aChipAddr, aRowAddr, 0) == false)
    {
        memset(pBuffer, 0x00, aLength);
        return false;
    }
    #endif

    BasicReadDataFromPage(aChipAddr, aRowAddr, pBuffer, aLength);

    return true;
}

uint8_t  NandWritePage(uint32_t aChipAddr, uint32_t aRowAddr, uint32_t aColAddr, uint8_t* pBuffer, uint32_t aLength)
{
    #if CHECK_OVERWRITE
    CheckNandPageBlank(aChipAddr, aRowAddr, 1);
    #endif

    BasicWriteDataToPage(aChipAddr, aRowAddr, pBuffer, aLength);

    return true;
}

uint8_t NandEraseBlock(uint32_t aChipAddr, uint32_t aRowAddr)
{
	uint32_t          blockNo;
    NANDFLASH_CHIP_S* pNandFlashChip;

    pNandFlashChip = GetNandFlashChip(aChipAddr);
    assert(pNandFlashChip);

    blockNo = aRowAddr / gNandFlashSize.mPagesPerBlock;
    if (NandIsValidBlock(blockNo) == false)
	{
	    printf("\nInvalid block!\n");
        if (gpfFailureCallBack)
        {
            gpfFailureCallBack();
        }

        return false;
	}

	memset(pNandFlashChip->mWrPageImg[blockNo], 0xff, sizeof(pNandFlashChip->mWrPageImg[blockNo]));
    pNandFlashChip->mWrPageImgDirty = 0;

    BasicEraseDataFromBlock(aChipAddr, aRowAddr, gNandFlashSize.mPageSize * gNandFlashSize.mPagesPerBlock);

	return true;
}

uint8_t  NandIsPageErased(uint32_t aChpAddr, uint32_t aRowAddr)
{
	uint8_t pageBuffer[gNandFlashSize.mPageSize];

    return NandReadPage(aChpAddr, aRowAddr, 0, pageBuffer, gNandFlashSize.mPageSize);
}

uint8_t  NandIsBadBlock(uint32_t aChipAddr, uint32_t aRowAddr)
{
	uint32_t          blockNo;
    NANDFLASH_CHIP_S* pNandFlashChip;

    pNandFlashChip = GetNandFlashChip(aChipAddr);
    assert(pNandFlashChip);

    blockNo = aRowAddr / gNandFlashSize.mPagesPerBlock;
    if (NandIsValidBlock(blockNo) == false)
    {
        printf("\nbad block!\n");
        if (gpfFailureCallBack)
        {
            gpfFailureCallBack();
        }

    }

	if(pNandFlashChip->mBadBlkImg[blockNo/8] & (1<<(blockNo%8)))
	{
		return false;
	}

	return true;
}

uint8_t NandMarkBadBlock(uint32_t aChipAddr, uint32_t aRowAddr)
{
    uint32_t          blockNo;
    NANDFLASH_CHIP_S* pNandFlashChip;

    pNandFlashChip = GetNandFlashChip(aChipAddr);
    assert(pNandFlashChip);

    blockNo = aRowAddr / gNandFlashSize.mPagesPerBlock;
    if (NandIsValidBlock(blockNo) == false)
    {
        printf("\nInvalid block!\n");
        if (gpfFailureCallBack)
        {
            gpfFailureCallBack();
        }
    }

	pNandFlashChip->mBadBlkImg[blockNo/8] &= ~(1<<(blockNo%8));

	return true;
}

static void NandSetupSize(void)
{
    if (gNandSizeInitDone == 0)
    {
        gNandSizeInitDone = 1;
    }
}

static uint32_t LoadFromFile(const char *apFileName, uint8_t *apBuf, uint32_t aLen)
{
    FILE *fp;
    uint32_t length;

    assert(apBuf);
    assert(apFileName);

    fp = fopen(apFileName, RdFlag);
    if (fp == NULL)
    {
        return false;
    }

    length = fread(apBuf, 1, aLen, fp);

    fclose(fp);

    return length;
}

static uint32_t SaveToFile(const char *apFileName, const uint8_t *apBuf, uint32_t aLen)
{
    FILE *fp;
    uint32_t length;

    assert(apBuf);
    assert(apFileName);

    fp = fopen(apFileName, WrFlag);
    if (fp == NULL)
    {
        return 0;
    }

    length = fwrite(apBuf, 1, aLen, fp);

    fclose(fp);

    return length;
}

static void GenerateIDFileName(uint32_t aChipAddr, char *apFileName)
{
    assert(apFileName);

    sprintf(apFileName, "%06x-ID.bin", aChipAddr);
}

static void GenerateBBIMGFileName(uint32_t aChipAddr, char *apFileName)
{
    assert(apFileName);

    sprintf(apFileName, "%06x-BadBlkImg.bin", aChipAddr);
}

static void GenerateWRIMGFileName(uint32_t aChipAddr, char *apFileName)
{
    assert(apFileName);

    sprintf(apFileName, "%06x-WrPageIm.bin", aChipAddr);
}

static int NandLoadID(uint32_t aChipAddr, uint8_t *apBuf, uint32_t aLen)
{
    char filename[64];

    assert(apBuf);
    assert(aLen > 0);

    GenerateIDFileName(aChipAddr, filename);

    return LoadFromFile(filename, apBuf, aLen);
}

static int NandSaveID(uint32_t aChipAddr, const uint8_t *apBuf, uint32_t aLen)
{
    char filename[64];

    assert(apBuf);
    assert(aLen > 0);

    GenerateIDFileName(aChipAddr, filename);

    return SaveToFile(filename, apBuf, aLen);
}

static int NandLoadBBIMG(uint32_t aChipAddr, uint8_t *apBuf, uint32_t aLen)
{
    char filename[64];

    assert(apBuf);
    assert(aLen > 0);

    GenerateBBIMGFileName(aChipAddr, filename);

    return LoadFromFile(filename, apBuf, aLen);
}

static int NandSaveBBIMG(uint32_t aChipAddr, const uint8_t *apBuf, uint32_t aLen)
{
    char filename[64];

    assert(apBuf);
    assert(aLen > 0);

    GenerateBBIMGFileName(aChipAddr, filename);

    return SaveToFile(filename, apBuf, aLen);
}

static int NandLoadWRIMG(uint32_t aChipAddr, uint8_t *apBuf, uint32_t aLen)
{
    char filename[64];

    assert(apBuf);
    assert(aLen > 0);

    GenerateWRIMGFileName(aChipAddr, filename);

    return LoadFromFile(filename, apBuf, aLen);
}

static int NandSaveWRIMG(uint32_t aChipAddr, const uint8_t *apBuf, uint32_t aLen)
{
    char filename[64];

    assert(apBuf);
    assert(aLen > 0);

    GenerateWRIMGFileName(aChipAddr, filename);

    return SaveToFile(filename, apBuf, aLen);
}

uint8_t NandLoadChip(uint32_t aChipAddr, NANDFLASH_CHIP_S* pNandFlashChip)
{
    uint8_t id[MAX_NAND_ID_LENGTH];

    assert(pNandFlashChip);

    memset(pNandFlashChip->mBadBlkImg, 0xFF, sizeof(pNandFlashChip->mBadBlkImg));
    memset(pNandFlashChip->mWrPageImg, 0xFF, sizeof(pNandFlashChip->mWrPageImg));

    //ID
    if (NandLoadID(aChipAddr, id, sizeof(id)) != sizeof(id))
    {
        return false;
    }

    if (memcmp(id, pNandFlashChip->mID, sizeof(id)))
    {
        return false;
    }

    //BBIMG
    NandLoadBBIMG(aChipAddr, pNandFlashChip->mBadBlkImg, sizeof(pNandFlashChip->mBadBlkImg));
    //WRIMG
    NandLoadWRIMG(aChipAddr, (uint8_t*)pNandFlashChip->mWrPageImg, sizeof(pNandFlashChip->mWrPageImg));

    return true;
}

uint8_t NandSaveChip(uint32_t aChipAddr, NANDFLASH_CHIP_S* pNandFlashChip)
{
    assert(pNandFlashChip);

    //ID
    if( NandSaveID(aChipAddr, pNandFlashChip->mID, sizeof(pNandFlashChip->mID)) != sizeof(pNandFlashChip->mID) )
    {
        return false;
    }

    //BBIMG
    if( NandSaveBBIMG(aChipAddr, pNandFlashChip->mBadBlkImg, sizeof(pNandFlashChip->mBadBlkImg)) != sizeof(pNandFlashChip->mBadBlkImg) )
    {
        return false;
    }

    //WRIMG
    if( NandSaveWRIMG(aChipAddr, (uint8_t*)pNandFlashChip->mWrPageImg, sizeof(pNandFlashChip->mWrPageImg)) != sizeof(pNandFlashChip->mWrPageImg) )
    {
        return false;
    }

    return true;
}

void NandOverWriteEnable(void)
{
    assert(gNandOverWriteStackPtr < 16 - 1);
    gNandOverWriteStack[++gNandOverWriteStackPtr] = true;
}

void NandOverWriteDisable(void)
{
    assert(gNandOverWriteStackPtr < 16 - 1);
    gNandOverWriteStack[++gNandOverWriteStackPtr] = false;
}

void NandOverWriteRestore(void)
{
    assert(gNandOverWriteStackPtr);
    gNandOverWriteStackPtr--;
}

uint32_t NandOverWriteIsEnabled(void)
{
    assert(gNandOverWriteStackPtr < 16);
    return gNandOverWriteStack[gNandOverWriteStackPtr];
}

void NandOverWriteSet(uint32_t isEna)
{
    assert(gNandOverWriteStackPtr < 16);
    gNandOverWriteStack[++gNandOverWriteStackPtr] = isEna;
}

void NandOverWriteInit(void)
{
    gNandOverWriteStackPtr = 0;
    gNandOverWriteStack[0] = false;
}

void NandSimSetCheckFailureHandle(void (*handle)(void))
{
    gpfFailureCallBack = handle;
}

void NandInit(void)
{
    gNandSizeInitDone = 0;
    NandSetupSize();
    InitBlockList();
}

void NandTerm(void)
{
    ClearBlockList();
}

void NandFlash(void)
{
    FlushBlockList();
}
