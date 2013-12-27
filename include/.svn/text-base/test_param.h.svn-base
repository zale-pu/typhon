/**********************************************************
 *     Module Name : test_param.h
 *     Description : test param parse
***********************************************************/
#ifndef _TEST_PARAM_H
#define _TEST_PARAM_H

/***********************************************************
 *     Kaifeng Zhuang@2013.8.01
 *     Description : Initail Create
************************************************************/

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
typedef struct _FLASH_PARAM
{
    uint16_t mBytesPerPage;
    uint16_t mBytesPerSpare;
    uint16_t mPagesPerBlock;
    uint16_t mBlocksPerPlane;
    uint16_t mPlanesPerChip;
} FLASH_PARAM_S;

typedef struct _CONNECT_PARAM
{
    uint16_t mChipsPerChannel;
    uint16_t mChannelsPerArray;
} CONNECT_PARAM_S;

typedef struct _TEST_PARAM
{
    uint16_t mStressTime;
    uint16_t mStressLevel;
    uint8_t  mReplayLogFile[128];
    uint32_t mReplayDumpPoint;
} TEST_PARAM_S;

/***************************************************
 *  Variables section
 * Add all variable decleartion here
 *
****************************************************/
extern TEST_PARAM_S     gTestParam;
extern FLASH_PARAM_S    gFlashParam;
extern CONNECT_PARAM_S  gConnectParam;

/***************************************************
 *  Function Prototype Section
 * Add prototypes for all functions called by this
 * module, with the exception of runtime routines.
 *
****************************************************/
int test_param_init(const char *param_file);

#endif
