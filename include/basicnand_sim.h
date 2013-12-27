/**********************************************************
*     Module Name : basicnand_sim.h
*     Description : basic nand sim declare
**********************************************************/

/*********************************************************
*     Kaifeng Zhuang @ 2010.05.18
*     Description  : Initial create
**********************************************************/
#ifndef _BASIC_NANDSIM_H
#define _BASIC_NANDSIM_H

/*********************************************************
*  Include section
*  Add all #includes here
*
**********************************************************/
#include <stdint.h>

/*********************************************************
 * Defines section
 * Add all #defines here
 *
 *********************************************************/

extern uint8_t InitBlockList(void);
extern uint8_t ClearBlockList(void);
extern uint8_t FlushBlockList(void);

extern uint8_t BasicEraseDataFromBlock(uint32_t aChipAddr, uint32_t aRowAddr, uint32_t aBlkLen);
extern uint8_t BasicWriteDataToPage(uint32_t aChipAddr, uint32_t aRowAddr, uint8_t* apBuf, uint32_t aLen);
extern uint8_t BasicReadDataFromPage(uint32_t aChipAddr, uint32_t aRowAddr, uint8_t* apBuf, uint32_t aLen);
 #endif
