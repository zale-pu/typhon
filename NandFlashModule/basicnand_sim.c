/**********************************************************
*     Module Name : basicnand_sim.c
*     Description : The functions driver internal used
**********************************************************/

/*********************************************************
*     Kaifeng Zhuang @ 2013.08.03
*     Description  : Initial create
**********************************************************/

/*********************************************************
*  Include section
*  Add all #includes here
*
**********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#include "file_lib.h"
#include "file_access.h"
#include "memory_file.h"
#include "basicnand_sim.h"
#include "nandflash_sim.h"

/*********************************************************
 * Defines section
 * Add all #defines here
 *
 *********************************************************/
#define NODE_PER_BLOCKLIST (128)
#define BYTE_PER_BASIC_VIEW (64*1024)

typedef struct _BLOCK_FILE {
	uint32_t        mBlock;
	uint32_t        mChipAddr;
    FILE_HANDLE_S*  pFileHandle;
}BLOCK_FILE_S;

typedef struct _BLOCK_NODE {
    uint32_t            mRight;
    BLOCK_FILE_S*       pBlockFile;
    struct _BLOCK_NODE* pPrior;
    struct _BLOCK_NODE* pNext;
}BLOCK_NODE_S, BLOCK_POSITION_S;

typedef struct _BLOCK_LIST{
    uint32_t      mCount;
    BLOCK_NODE_S* pHead;
    BLOCK_NODE_S* pTail;
}BLOCK_LIST_S;

 /*********************************************************
 * Global variables section
 * Add all globalvariables here
 *
 *********************************************************/
static BLOCK_LIST_S gBlockList;

static void GenerateBlockFileName(uint32_t aChipAddr, uint32_t aBlock, char *apFileName)
{
    assert(apFileName);

    sprintf(apFileName, "%06x_%04x.bin", aChipAddr, aBlock);
}

static uint8_t BlockFileOpen(BLOCK_FILE_S* pBlockFile)
{
    uint32_t blockSize;
    char fileName[64];

    GenerateBlockFileName(pBlockFile->mChipAddr, pBlockFile->mBlock, fileName);

    blockSize = gNandFlashSize.mPageSize * gNandFlashSize.mPagesPerBlock;

    pBlockFile->pFileHandle = MemoryFileCreate(fileName, blockSize);
    return FileOpen(pBlockFile->pFileHandle);
}

static void BlockFileClose(BLOCK_FILE_S* pBlockFile, uint32_t iRight)
{
    if (iRight > 0)
    {
        FileClose(pBlockFile->pFileHandle);
    }

    MemoryFileDestroy(pBlockFile->pFileHandle);
    free(pBlockFile);
}

static void BlockFileFlush(BLOCK_FILE_S* pBlockFile, uint32_t iRight)
{
    if (iRight > 0)
    {
        FileFlush(pBlockFile->pFileHandle);
    }
}

static BLOCK_NODE_S* MakeBlockNode(uint32_t aChipAddr, uint32_t aRowAddr)
{
    BLOCK_NODE_S* pBlockNode;

    if(!(pBlockNode =(BLOCK_NODE_S*)malloc(sizeof(BLOCK_NODE_S))))
    {
        return NULL;
    }

    BLOCK_FILE_S* pBlockFile = (BLOCK_FILE_S*)malloc(sizeof(BLOCK_FILE_S));
    if (pBlockFile == NULL)
    {
        return NULL;
    }

    pBlockFile->mChipAddr =  aChipAddr;
    pBlockFile->mBlock = aRowAddr / gNandFlashSize.mPagesPerBlock;

    if (BlockFileOpen(pBlockFile) == FAILURE)
    {
        return NULL;
    }

    pBlockNode->pBlockFile = pBlockFile;
    pBlockNode->pPrior = NULL;
    pBlockNode->pNext = NULL;
    pBlockNode->mRight = 0;

    return pBlockNode;
}

static void FreeBlockNode(BLOCK_NODE_S* pBlockNode)
{
    BlockFileClose(pBlockNode->pBlockFile, pBlockNode->mRight);
    free (pBlockNode->pBlockFile);
    pBlockNode->pBlockFile = NULL;
    free(pBlockNode);
    pBlockNode = NULL;
}

static void FlushBlockNode(BLOCK_NODE_S* pBlockNode)
{
    BlockFileFlush(pBlockNode->pBlockFile, pBlockNode->mRight);
}

uint8_t InitBlockList(void)
{
    BLOCK_LIST_S* pBlockList = &gBlockList;

    pBlockList->pHead = pBlockList->pTail = NULL;
    pBlockList->mCount = 0;

    return SUCCESS;
}

uint8_t ClearBlockList(void)
{
    BLOCK_NODE_S *pNode;
    BLOCK_LIST_S* pBlockList = &gBlockList;

    while(pBlockList->mCount)
    {
        assert(pBlockList->pHead != NULL);

        pNode = pBlockList->pHead;
        pBlockList->pHead = pNode->pNext;
        FreeBlockNode(pNode);
        pBlockList->mCount--;
    }

    pBlockList->pTail = pBlockList->pHead = NULL;
    pBlockList->mCount = 0;

    return SUCCESS;
}

uint8_t FlushBlockList(void)
{
    uint32_t iNodeCount;
    BLOCK_NODE_S *pTemp, *pHead;
    BLOCK_LIST_S* pBlockList = &gBlockList;

    pHead = pBlockList->pHead;
    iNodeCount = pBlockList->mCount;
    while(iNodeCount)
    {
        assert(pBlockList->pHead != NULL);

        pTemp = pHead;
        pHead = pTemp->pNext;
        FlushBlockNode(pTemp);
        iNodeCount--;
    }

    return SUCCESS;
}

static uint32_t DeleteBlockNode(BLOCK_NODE_S* pDeleteNode)
{
    BLOCK_LIST_S* pBlockList = &gBlockList;

    if (pBlockList->mCount == 0)
    {
        return FAILURE;
    }
    else
    {
        if (pBlockList->mCount == 1)
        {
            pBlockList->pHead = NULL;
            pBlockList->pTail = NULL;
        }
        else
        {
            if (pDeleteNode == pBlockList->pHead)
            {
                pDeleteNode->pNext->pPrior = NULL;
                pBlockList->pHead = pDeleteNode->pNext;
            }
            else if (pDeleteNode == pBlockList->pTail)
            {
                pDeleteNode->pPrior->pNext = NULL;
                pBlockList->pTail = pDeleteNode->pPrior;
            }
            else
            {
                pDeleteNode->pPrior->pNext = pDeleteNode->pNext;
                pDeleteNode->pNext->pPrior = pDeleteNode->pPrior;
            }
        }

        FreeBlockNode(pDeleteNode);
        pBlockList->mCount--;
    }

    return SUCCESS;
}

static uint32_t InsertBlockNode(BLOCK_NODE_S* pBlockNode)
{
    BLOCK_LIST_S* pBlockList = &gBlockList;
    //sBlockNode* pDeleteNode;

    if(pBlockNode == NULL)
    {
        return FAILURE;
    }

    if (pBlockList->mCount >= NODE_PER_BLOCKLIST)
    {
        //pDeleteNode = GetDeleteNode();
        DeleteBlockNode(pBlockList->pTail);
    }

    if (pBlockList->mCount == 0)
    {
        pBlockList->pHead = pBlockList->pTail = pBlockNode;
    }
    else
    {
        pBlockNode->pNext = pBlockList->pHead;
        pBlockList->pHead->pPrior = pBlockNode;
        pBlockList->pHead = pBlockNode;
    }

    pBlockList->mCount++;
    return SUCCESS;
}

static BLOCK_NODE_S* GetBlockNode(uint32_t aChipAddr, uint32_t aRowAddr, uint8_t bMake)
{
    BLOCK_LIST_S* pBlockList = &gBlockList;
    BLOCK_NODE_S* pGetNode = pBlockList->pHead;

    while (pGetNode != NULL)
    {
        if ((pGetNode->pBlockFile->mChipAddr == aChipAddr) &&
            (pGetNode->pBlockFile->mBlock == aRowAddr / gNandFlashSize.mPagesPerBlock)
           )
        {
            pGetNode->mRight++;
            return pGetNode;
        }

        pGetNode = pGetNode->pNext;
    }

    if (bMake == true)
    {
        pGetNode = MakeBlockNode(aChipAddr, aRowAddr);
        if (pGetNode != NULL)
        {
            InsertBlockNode(pGetNode);
            pGetNode->mRight++;
            return pGetNode;
        }
    }

    return NULL;
}

uint8_t BasicEraseDataFromBlock(uint32_t aChipAddr, uint32_t aRowAddr, uint32_t aBlkLen)
{
    char filename[64];
    uint32_t blockno;
    BLOCK_NODE_S* pBlockNode = GetBlockNode(aChipAddr, aRowAddr, false);

    blockno = aRowAddr / gNandFlashSize.mPagesPerBlock;
    if (pBlockNode != NULL)
    {
        pBlockNode->mRight = 0;
        DeleteBlockNode(pBlockNode);
    }

    GenerateBlockFileName(aChipAddr, blockno, filename);
    TestDeleteFile(filename);

    return SUCCESS;
}

uint8_t BasicReadDataFromPage(uint32_t aChipAddr, uint32_t aRowAddr, uint8_t *apBuf, uint32_t aLen)
{
    uint32_t pageOffset;
    BLOCK_NODE_S* pBlockNode = GetBlockNode(aChipAddr, aRowAddr, true);

    if (pBlockNode != NULL)
    {
        pageOffset = aRowAddr % gNandFlashSize.mPagesPerBlock;
        FileRead(pBlockNode->pBlockFile->pFileHandle, pageOffset*gNandFlashSize.mPageSize, aLen, apBuf);
        return SUCCESS;
    }

    return FAILURE;
}

uint8_t BasicWriteDataToPage(uint32_t aChipAddr, uint32_t aRowAddr, uint8_t *apBuf, uint32_t aLen)
{
    uint32_t pageOffset;
    BLOCK_NODE_S* pBlockNode = GetBlockNode(aChipAddr, aRowAddr, true);

    if (pBlockNode != NULL)
    {
        pageOffset = aRowAddr % gNandFlashSize.mPagesPerBlock;
        FileWrite(pBlockNode->pBlockFile->pFileHandle, pageOffset*gNandFlashSize.mPageSize, aLen, apBuf);
        return SUCCESS;
    }

    return FAILURE;
}

