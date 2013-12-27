/*******************************************************************************
 * @Copyright   2013-2016,      Gigadevice .Co., Ltd
 * @file        userdata.h
 * @brief       Header file of the User Data.
 * @author      Qiang Pu
 * @version     1.0
 * @date        2013.12
 * @warning 
 * @history     version     author      time            record
 *              1.0         Qiang Pu    2013.11.22      create
 ******************************************************************************/
#ifndef _USERDATA_H 
#define _USERDATA_H

#include <stdint.h>

#define MAX_PARTITION_CNT   8

/**
 * @function    void UserData_Init(void)
 * @brief       Initialize all partitions at once.
 */
extern void     UserData_Init(uint32_t aPartSize[], uint32_t aMaxMapSize[]);

/**
 * @function    void UserData_Term(void)
 * @brief       Term all partitions at once.
 */
extern void     UserData_Term(void);

/**
 * @function    void UserData_InitPart(uint32_t aPartIndex, uint32_t aPartSize)
 * @brief       Initialize  partition specified by aPartIndex.
 * @param[in]   aPartIndex: Index which partition will be init.
 * @param[in]   aPartSize: The size of this partition will be init.
 */
extern void     UserData_InitPart(uint32_t aPartIndex, uint32_t aPartSize, uint32_t aMaxMapSize);

/**
 * @function    void UserData_TermPart(uint32_t aPartIndex)
 * @brief       Terminate partition specified by aPartIndex.
 * @param[in]   aPartIndex: Index which partition will be term.
 */
extern void     UserData_TermPart(uint32_t aPartIndex);

/**
 * @function    void UserData_Insert(uint32_t aPartIndex, uint32_t aMapData[])
 * @brief       Insert data will one partition.
 * @param[in]   aPartIndex: Index which partition will be inserted.
 * @param[in]   aMapData[]: The data will be inserted.
 */
extern void     UserData_Insert(uint32_t aPartIndex, uint32_t aMapData[]);

/**
 * @function    uint32_t UserData_Compare(uint32_t aPartIndex, uint32_t aMapData[], uint32_t* apDataBuf)
 * @brief       Compare data.
 * @param[in]   aPartIndex: Index which partition's data will be compared.
 * @param[in]   aMapData[]: 
 * @param[in]   aDataBuf[]: The data need to be compared.
 */
extern uint32_t UserData_Compare(uint32_t aPartIndex, uint32_t aMapData[], uint32_t aDataBuf[]);

/**
 * @function    uint8_t UserData_CheckNode(uint32_t aPartIndex, uint32_t aMapData[])
 * @brief       Check a node in one partition, only used to test.
 * @param[in]   aPartIndex: Index which partition's data will be compared.
 * @param[in]   aMapData[]: The data need to be compared.
 */
extern uint8_t  UserData_CheckNode(uint32_t aPartIndex, uint32_t aMapData[]);

/**
 * @function    static void StoreOnePartition(uint32_t aPartIndex, char* apDirName)
 * @brief       Write data of one partition into file.
 * @param[in]   aPartIndex: Partition index.
 * @param[in]   apDirName: Direct name.
 */
extern void     UserData_Store(uint32_t aPartIndex, uint32_t aAccessMode, char* apDirName);

/**
 * @function    void UserData_Store(uint32_t aPartIndex, uint32_t aAccessMode, char* apDirName)
 * @brief       Write data to files.
 * @param[in]   aPartIndex: Partition index.
 * @param[in]   aAccessMode: 
 * @param[in]   apDirName: Direct name.
 */
extern void     UserData_Load(uint32_t* aPartIndex, uint32_t* apAccessMode, char* apDirName);

#endif
