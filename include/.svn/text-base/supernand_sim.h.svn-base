/**********************************************************
*
* Proprietary & confidential, All right reserved, 1997
*         Silicon Storage Technology, Inc.
*
*     Module Name : SuperNandSim.h
*     Description : The functions driver internal used
**********************************************************/

/*********************************************************
*     Kaifeng Zhuang @ 2010.09.06
*     Description  : Initial create
**********************************************************/
#ifndef _SUPER_NANDSIM_H
#define _SUPER_NANDSIM_H

/*********************************************************
*  Include section
*  Add all #includes here
*
**********************************************************/
#include <stdint.h>
#include "nandflash_sim.h"

/*********************************************************
 * Defines section
 * Add all #defines here
 *
 *********************************************************/
#define BLOCKVIEW           0 //Enable: use 320k/576k mapview

#define BYTE_PER_UDSECTOR   8
#define BYTE_PER_UDROWPAGE  (BYTE_PER_UDSECTOR*gNandFlashSize.mBytesPerPage/512)
#define BYTE_PER_UDCONTROL  16
#define BYTE_PER_UDPAGE     (BYTE_PER_UDROWPAGE+BYTE_PER_UDCONTROL)
#define BYTE_PER_UDBLOCK    (BYTE_PER_UDPAGE*gNandFlashSize.mPagesPerBlock)
#define BYTE_PER_UDCHIP     (BYTE_PER_UDBLOCK*gNandFlashSize.mBlocksPerChip)

#define BYTE_PER_VIEW       (BYTE_PER_UDPAGE*4*1024*16)
#define PAGE_PER_VIEW       (BYTE_PER_VIEW/BYTE_PER_UDPAGE)
#define PAGE_PER_VIEW_MASK  (PAGE_PER_VIEW-1)

extern uint8_t   ChipFilesInit(void);
extern void      ChipFilesExit(void);
extern void      ChipFilesFlush(void);
extern void      EraseDataFromBlock(uint32_t aChipAddr, uint32_t aRowAddr);
extern uint8_t   IsEraseDataFromPage(uint32_t aChipAddr, uint32_t aRowAddr, uint8_t* apCtrl);
extern uint8_t   WriteDataToPage(uint32_t aChipAddr, uint32_t aRowAddr, uint8_t* pDataBuffer, uint8_t* pCtrlBuffer);
extern uint8_t   ReadDataFromPage(uint32_t aChipAddr, uint32_t aRowAddr, uint8_t* pDataBuffer, uint8_t* pCtrlBuffer);

#endif
