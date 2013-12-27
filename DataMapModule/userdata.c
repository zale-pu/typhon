/*******************************************************************************
 * @Copyright   2013-2016,      Gigadevice .Co., Ltd
 * @file        userdata.c
 * @brief       Manage User Data list.
 * @author      Qiang Pu
 * @version     1.0
 * @date        2013.11
 * @warning
 * @history     version     author      time            record
 *              1.0         Qiang Pu    2013.11.22      create
 ******************************************************************************/
#include <assert.h>
#include <string.h>
#include <stdint.h>

#include "thread.h"
#include "file_lib.h"
#include "userdatalist.h"
#include "userdata.h"

/** Node structure of one partiton. */
typedef struct UserDataPartition
{
    UserDataList_S* spMapList;  /** Base address of one partiton. */
    uint32_t        ulPartSize; /** Partition size. */
}UserDataPartition_S;

/** Semaphore counter for resources shared between thread.*/
/** It is never less than 0.*/
/** The calling thread will be suspends untill it is non-zero.*/
static struct s_thread_sem_s* gspSemPartition = NULL;

/** Global Array of all partitons.*/
/** The number of partitions is MAX_PARTITION_CNT.*/
/** Whether use or not use one of these partitions, it will exist.*/
static UserDataPartition_S gPartitionArray[MAX_PARTITION_CNT];

/**
 * @function    void UserData_Init(void)
 * @brief       Initialize all partitions at once.
 */
void UserData_Init(uint32_t aPartSize[], uint32_t aMaxMapSize[])
{
    uint32_t index = 0;

    memset(&gPartitionArray, 0, MAX_PARTITION_CNT*sizeof(UserDataPartition_S));

    if(gspSemPartition)
    {
        s_thread_sem_destroy(gspSemPartition);
        gspSemPartition = NULL;
    }

    if(s_thread_sem_create(&gspSemPartition, 1))
    {
        printf("Fatal: User Data Semaphore Create Failure\n");
        while(1);
    }

    for(index = 0; index < MAX_PARTITION_CNT; index++)
    {
        if(0 != aPartSize[index])
        {
            UserData_InitPart(index, aPartSize[index], aMaxMapSize[index]);
        }
    }
}

/**
 * @function    void UserData_Term(void)
 * @brief       Term all partitions at once.
 */
void UserData_Term(void)
{
    uint32_t index = 0;

    for(index = 0; index < MAX_PARTITION_CNT; index++)
    {
        UserData_TermPart(index);
    }

    if(gspSemPartition)
    {
        s_thread_sem_destroy(gspSemPartition);
        gspSemPartition = NULL;
    }
}

/**
 * @function    void UserData_InitPart(uint32_t aPartIndex, uint32_t aPartSize)
 * @brief       Initialize  partition specified by aPartIndex.
 * @param[in]   aPartIndex: Index which partition will be init.
 * @param[in]   aPartSize: The size of this partition will be init.
 */
void UserData_InitPart(uint32_t aPartIndex, uint32_t aPartSize, uint32_t aMaxMapSize)
{
    assert(aPartIndex < MAX_PARTITION_CNT);
    assert(0 != aPartSize);

//    assert(NULL != gPartitionArray[aPartIndex].spMapList); /** It has been used. */

    gPartitionArray[aPartIndex].ulPartSize = aPartSize;
    UserDataList_Init(&gPartitionArray[aPartIndex].spMapList, aPartSize, aMaxMapSize);
}

/**
 * @function    void UserData_TermPart(uint32_t aPartIndex)
 * @brief       Terminate partition specified by aPartIndex.
 * @param[in]   aPartIndex: Index which partition will be term.
 */
void UserData_TermPart(uint32_t aPartIndex)
{
    assert(aPartIndex < MAX_PARTITION_CNT);

    if(gPartitionArray[aPartIndex].spMapList)
    {
        UserDataList_Term(&gPartitionArray[aPartIndex].spMapList);
    }

    gPartitionArray[aPartIndex].ulPartSize = 0;
}

/**
 * @function    void UserData_Insert(uint32_t aPartIndex, UserDataItem_S* apMapData)
 * @brief       Insert data will one partition.
 * @param[in]   aPartIndex: Index which partition will be inserted.
 * @param[in]   apMapData: The data will be inserted.
 */
void UserData_Insert(uint32_t aPartIndex, uint32_t aMapData[])
{
    UserDataItem_S* apMapData = (UserDataItem_S *)aMapData;

    assert(aPartIndex < MAX_PARTITION_CNT);
    assert(NULL != gPartitionArray[aPartIndex].spMapList);
    assert(0 < gPartitionArray[aPartIndex].ulPartSize);

    s_thread_sem_wait(gspSemPartition);
    UserDataList_Insert(gPartitionArray[aPartIndex].spMapList, apMapData);
    s_thread_sem_post(gspSemPartition);
}

/**
 * @function    uint32_t UserData_Compare(uint32_t aPartIndex, UserDataItem_S* apMapData, uint32_t* apDataBuf)
 * @brief       Compare data.
 * @param[in]   aPartIndex: Index which partition's data will be compared.
 * @param[in]   apMapData: The data need to be compared.
 * @param[in]   apMapData: The data need to be compared.
 */
uint32_t UserData_Compare(uint32_t aPartIndex, uint32_t aMapData[], uint32_t* apDataBuf)
{
    uint32_t result = 0;
    UserDataItem_S* apMapData = (UserDataItem_S *)aMapData;

    assert(aPartIndex < MAX_PARTITION_CNT);
    assert(NULL != gPartitionArray[aPartIndex].spMapList);
    assert(0 < gPartitionArray[aPartIndex].ulPartSize);
    assert(NULL != apMapData);
    assert(NULL != apDataBuf);

    s_thread_sem_wait(gspSemPartition);
    result = UserDataList_Compare(
            gPartitionArray[aPartIndex].spMapList, apMapData, apDataBuf);
    s_thread_sem_post(gspSemPartition);

    return result;
}

/**
 * @function    uint8_t UserData_CheckNode(uint32_t aPartIndex, UserDataItem_S* apMapData)
 * @brief       Check a node in one partition, only used to test.
 * @param[in]   aPartIndex: Index which partition's data will be compared.
 * @param[in]   apMapData: The data need to be compared.
 */
uint8_t UserData_CheckNode(uint32_t aPartIndex, uint32_t aMapData[])
{
    uint8_t result;
    UserDataItem_S* apMapData = (UserDataItem_S *)aMapData;

    assert(aPartIndex < MAX_PARTITION_CNT);
    assert(NULL != gPartitionArray[aPartIndex].spMapList);
    assert(0 < gPartitionArray[aPartIndex].ulPartSize);
    assert(NULL != apMapData);

    s_thread_sem_wait(gspSemPartition);
    result = UserDataList_CheckNode(gPartitionArray[aPartIndex].spMapList, apMapData);
    s_thread_sem_post(gspSemPartition);

    return result;
}

/**
 * @function    static void StoreOnePartition(uint32_t aPartIndex, char* apDirName)
 * @brief       Write data of one partition into file.
 * @param[in]   aPartIndex: Partition index.
 * @param[in]   apMapData: Direct name.
 */
static void StoreOnePartition(uint32_t aPartIndex, char* apDirName)
{
    FILE* pFile;
    char filename[128];

    if((NULL == gPartitionArray[aPartIndex].spMapList) &&
            (0 == gPartitionArray[aPartIndex].ulPartSize))
    {
        return;
    }

    sprintf(filename, "%s_UserDataList_%d.bin", apDirName, aPartIndex);
    pFile = fopen(filename, WrFlag);
    assert(pFile);

    UserDataList_Store(pFile, gPartitionArray[aPartIndex].spMapList);
    fclose(pFile);
}

/**
 * @function    void UserData_Store(uint32_t aPartIndex, uint32_t aAccessMode, char* apDirName)
 * @brief       Write data to files.
 * @param[in]   aPartIndex: Partition index.
 * @param[in]   aAccessMode:
 * @param[in]   apMapData: Direct name.
 */
void UserData_Store(uint32_t aPartIndex, uint32_t aAccessMode, char* apDirName)
{
    s_thread_sem_wait(gspSemPartition);

    if(aPartIndex < MAX_PARTITION_CNT) /** Just need to store one partition. */
    {
        StoreOnePartition(aPartIndex, apDirName);
    }
    else /** Store all partitions. */
    {
        FILE* pFile;
        uint32_t index = 0;
        char filename[128];

        sprintf(filename, "%s_UserData.cfg", apDirName);
        pFile = fopen(filename, WrFlag);
        assert(pFile);
        fprintf(pFile, "%d, %d", aPartIndex, aAccessMode);
        fflush(pFile);
        fclose(pFile);

        for(index = 0; index < MAX_PARTITION_CNT; index++)
        {
            StoreOnePartition(index, apDirName);
        }
    }

    s_thread_sem_post(gspSemPartition);
}

/**
 * @function    static void LoadOnePartition(uint32_t aPartIndex, char* apDirName)
 * @brief       Load one partition data from a file.
 * @param[in]   aPartIndex: Partition index.
 * @param[in]   apMapData: Direct name.
 */
static void LoadOnePartition(uint32_t aPartIndex, char* apDirName)
{
    FILE* pFile;
    char filename[128];

    if((NULL == gPartitionArray[aPartIndex].spMapList) &&
            (0 == gPartitionArray[aPartIndex].ulPartSize))
    {
        return;
    }

    sprintf(filename, "%s_UserDataList_%d.bin", apDirName, aPartIndex);
    pFile = fopen(filename, RdFlag);
    assert(pFile);

    UserDataList_Load(pFile, gPartitionArray[aPartIndex].spMapList,
            gPartitionArray[aPartIndex].ulPartSize);
    fclose(pFile);
}

/**
 * @function    void UserData_Load(uint32_t* apPartIndex, uint32_t* apAccessMode, char* apDirName)
 * @brief       Write data to files.
 * @param[in]   aPartIndex: Partition index.
 * @param[in]   aAccessMode:
 * @param[in]   apMapData: Direct name.
 */
void UserData_Load(uint32_t* apPartIndex, uint32_t* apAccessMode, char* apDirName)
{
    s_thread_sem_wait(gspSemPartition);

    uint32_t index = *apPartIndex;

    if(index < MAX_PARTITION_CNT) /** Just load one partition. */
    {
        LoadOnePartition(index, apDirName);
    }
    else /** Load all partitions. */
    {
        FILE* pFile;

        char filename[128];
        char commandLine[128];

        sprintf(filename, "%s_UserData.cfg", apDirName);
        pFile = fopen(filename,RdFlag);
        assert(pFile);

        fgets(commandLine, 128, pFile);
        sscanf(commandLine, "%d, %d", apPartIndex, apAccessMode);
        fclose(pFile);

        for(index = 0; index < MAX_PARTITION_CNT; index++)
        {
            LoadOnePartition(index, apDirName);
        }
    }

    s_thread_sem_post(gspSemPartition);
}
