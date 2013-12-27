/**********************************************************
 *     Module Name : nandflash_sim.h
 *     Description : nand flash sim declare
***********************************************************/

/***********************************************************
 *     Kaifeng Zhuang@2013.08.03
 *     Description : Initial Create
************************************************************/
#ifndef _NANDFLASH_SIM_H_
#define _NANDFLASH_SIM_H_

/****************************************************
 *  Include section
 * Add all #includes here
 *
****************************************************/
#include <stdint.h>

/***************************************************
 *  Defines section
 * Add all #defines here
 *
****************************************************/
#define MAX_NAND_ID_LENGTH      (6)
#define MAX_BYTES_PER_PAGE      (8192+32)
#define MAX_BLOCKS_PER_CHIP     (4096)
#define MAX_NANDFLASH_CHIP_CNT  (256)

typedef struct _NANDFLASH_CHIP{
    uint32_t  mChipAddr;
    uint8_t   mID[MAX_NAND_ID_LENGTH];
	uint8_t   mBadBlkImgDirty;
	uint8_t   mWrPageImgDirty;
	uint8_t   mBadBlkImg[MAX_BLOCKS_PER_CHIP/8];
	uint8_t   mWrPageImg[MAX_BLOCKS_PER_CHIP][256/8];
}NANDFLASH_CHIP_S;

typedef struct _NANDFLASH_SIZE{
	uint32_t mBytesPerPage;
	uint32_t mBytesPerSpare;
	uint32_t mPagesPerBlock;
	uint32_t mBlocksPerChip;
	uint32_t mPageSize;
	uint32_t mBlocksPerPlane;
    uint32_t mPlanesPerChip;
}NANDFLASH_SIZE_S;

extern uint16_t         gNandFlashChipCnt;
extern NANDFLASH_SIZE_S gNandFlashSize;
extern NANDFLASH_CHIP_S gNandFlashChips[MAX_NANDFLASH_CHIP_CNT];

extern void     NandInit(void);
extern void     NandTerm(void);
extern void     NandFlush(void);

extern void     NandOverWriteInit(void);
extern void     NandOverWriteEnable(void);
extern void     NandOverWriteDisable(void);
extern void     NandOverWriteRestore(void);
extern uint32_t  NandOverWriteIsEnabled(void);
extern void     NandOverWriteSetup(uint8_t aIsEnable);

extern uint8_t  NandIsValidBlock(uint32_t aBlockAddr);
extern uint8_t  NandIsBadBlock(uint32_t aChipAddr, uint32_t aRowAddr);
extern uint8_t  NandEraseBlock(uint32_t aChipAddr, uint32_t aRowAddr);
extern uint8_t  NandIsPageErased(uint32_t aChpAddr, uint32_t aRowAddr);
extern uint8_t  NandMarkBadBlock(uint32_t aChipAddr, uint32_t aRowAddr);

extern uint8_t NandLoadChip(uint32_t aChipAddr, NANDFLASH_CHIP_S* pNandFlashChip);
extern uint8_t NandSaveChip(uint32_t aChipAddr, NANDFLASH_CHIP_S* pNandFlashChip);

extern NANDFLASH_CHIP_S* GetNandFlashChip(uint32_t aChipAddr);
extern uint8_t  NandReadID(NANDFLASH_CHIP_S* pNandFlashChip, uint8_t* pID);
extern uint8_t  NandReadPage(uint32_t aChipAddr, uint32_t aRowAddr, uint32_t aColAddr, uint8_t* pBuffer, uint32_t aLength);
extern uint8_t  NandWritePage(uint32_t aChipAddr, uint32_t aRowAddr, uint32_t aColAddr, uint8_t* pBuffer, uint32_t aLength);
#endif
