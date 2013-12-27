/*******************************************************************************
 * @Copyright   2013-2016,      Gigadevice .Co., Ltd
 * @file        userdatalist.h
 * @brief       Header file of the User Data list.
 * @author      Qiang Pu
 * @version     1.0
 * @date        2013.11
 * @warning 
 * @history     version     author      time            record
 *              1.0         Qiang Pu    2013.11.22      create
 ******************************************************************************/
#ifndef _USERDATALIST_H
#define _USERDATALIST_H

#include <stdio.h>
#include <stdint.h>

/** The maximum length of every list. */
#define MAX_MAP_LENGTH 0x10000

/** Type of user data. */
typedef enum USERDATATYPE
{
    TYPE_BLANK    = 0, /** It is blank, and no data writen here. */
    TYPE_WRITE    = 1, /** It is a writen data. */
    TYPE_TRIM     = 2, /** It is a trimmed data. */
}USERDATATYPE_E;

/** Structure of data writen to or read from the node of list. */
typedef struct UserDataItem
{
    uint32_t ulLba;     /** The beginning LBA of this data. */
    uint32_t ulLen;     /** The length of this data. */
    uint32_t ulType;    /** The type of this data. */
    uint32_t ulKey;     /** The key of this data. */
}UserDataItem_S;

/** Structure of data the data buffer should be. */
typedef struct UserDataSector
{
    uint32_t ulLba;     /** The beginning LBA of this data. */
    uint32_t ulKey;     /** The length of this data. */
}UserDataSector_S;

/** Node structure of the user data list. */
typedef struct UserDataList
{
    UserDataItem_S          sData;  /** Data of this node. */
    struct UserDataList*    spNext; /** Pointer to next node. */
}UserDataList_S;

/**
 * @function    void UserDataList_Init(UserDataList_S** apMapList, uint32_t aPartSize)
 * @brief       Initialize.
 * @prime[i/o]  apMapList: 
 * @prime[in]   aPartSize: The total size of this partition.
 */
extern void     UserDataList_Init(UserDataList_S** apMapList, uint32_t aPartSize, uint32_t aMaxMapSize);

/**
 * @function    void UserDataList_Term(UserDataList_S** apMapList)
 * @brief       Free all space and terminate these functions.
 * @prime[i/o]  apMapList: 
 */
extern void     UserDataList_Term(UserDataList_S** apMapList);

/**
 * @function    void UserDataList_Insert(UserDataList_S* apMapList, UserDataItem_S* apMapData)
 * @brief       Inserte data.
 * @prime[in]   apMapList: Address of list to be inserted in.
 * @prime[in]   apMapData: Data of inserted node.
 */
extern void     UserDataList_Insert(UserDataList_S* apMapList, UserDataItem_S* apMapData);

/**
 * @function    uint32_t UserDataList_Compare(UserDataList_S* apMapList, UserDataItem_S* apMapData, uint32_t aDataBuf[])
 * @brief       Compare data.
 * @prime[in]   apMapList: Position of list to be inserted in or compared with.
 * @prime[in]   apMapData: Data of inserted node.
 * @prime[in]   aDataBuf[]: Compared data buffer.
 * @return      result of Compare. 0 is success, others is the LBA where the first error occurred.
 */
extern uint32_t UserDataList_Compare(UserDataList_S* apMapList, UserDataItem_S* apMapData, uint32_t aDataBuf[]);

/**
 * @function    void UserDataList_Store(FILE* apFile, UserDataList_S* apMapList)
 * @brief       Write all the data into a file.
 * @prime[out]  apFile: The file which the data will be writen into.
 * @prime[in]   apMapList: Address where data need to be writen from.
 */
extern void     UserDataList_Store(FILE* apFile, UserDataList_S* apMapList);

/**
 * @function    void UserDataList_Load(FILE* apFile, UserDataList_S* apMapList, uint32_t aPartSize)
 * @brief       Read all the data from a file.
 * @prime[in]   apFile: The file which the data will be writen into.
 * @prime[out]  apMapList: Address of which the data need to be read to.
 * @prime[in]   aPartSize: The total size of this partition.
 */
extern void     UserDataList_Load(FILE* apFile, UserDataList_S* apMapList, uint32_t aPartSize);

/**
 * @function    uint8_t  UserDataList_CheckNode(UserDataList_S* apMapList, UserDataItem_S apMapData)
 * @brief       Check a node in the list, used to test.
 * @prime[in]   apMapList: Address the data writen from.
 * @prime[in]   apMapData: Expected data of the node.
 * @return      0 is success, 1 is error.
 */
extern uint8_t  UserDataList_CheckNode(UserDataList_S* apMapList, UserDataItem_S* apMapData);

#endif
