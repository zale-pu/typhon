/*******************************************************************************
 * @Copyright   2013-2016,      Gigadevice .Co., Ltd
 * @file        userdatalist.c
 * @brief       The User Data list.
 * @author      Qiang Pu
 * @version     1.0
 * @date        2013.11
 * @warning
 * @history     version     author      time            record
 *              1.0         Qiang Pu    2013.11.22      create
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "userdatalist.h"

/** The user data's relative location of node.*/
typedef enum USERDATALOC
{
	AT_BEGIN    = 0x1, /** Located at the beginning of the node. */
	IN_MIDDLE	= 0x2, /** Located in the middle of the node. */
}USERDATALOC_E;

/** Write or Compare data. */
typedef enum OPERATION
{
    IS_WRITE    = 0, /** Operation is write data. */
    IS_COMPARE  = 1, /** Operation is compare data. */
}OPERATION_E;

/** Result of compare data. */
typedef enum CHECKRESULT
{
    TRUE    = 0,    /** No error occurred. */
    FALSE   = 1,    /** Error occurred. */
}CHECKRESULT_E;

/** Status of node where the user data will be insert or write in. */
typedef struct DATASTATUS
{
    UserDataList_S*     spPrev;     /** Pointer to the previous node of spBegin node. */
    UserDataList_S*     spBegin;    /** Pointer to the node where the new node should be inserted in. */
    uint8_t             ucStatus;   /** Relative position with the spBegin node. */
}DATASTATUS_S;

/** The total number of lists this partiton should have. */
/** It always related with gPartSize and gMaxMapSize.*/
/** gMapSize = ((aPartSize - 1) / gMaxMapSize) + 1. */
static uint32_t gMapSize = 0;

/** The tatal size of this partition. */
/** It is translated in by aPartSize when init. */
static uint32_t gPartSize = 0;

/** The maximum size of one list. */
static uint32_t gMaxMapSize = 0;

/**
 * @function    void GetHeadStatus(DATASTATUS_S* spStatus, uint32_t ulLba)
 * @brief       Get the address of the node in the list, and the position in this node.
 * @prime[i/o]  spStatus: input the beginning node address; output the result of search.
 * @prime[in]   ulLba: The LBA need to search.
 */
static void GetHeadStatus(DATASTATUS_S* spStatus, uint32_t ulLba)
{
    assert(NULL != spStatus->spBegin);

    while(ulLba >= (spStatus->spBegin->sData.ulLba +
            spStatus->spBegin->sData.ulLen)) /** The ulLba in the back node. */
    {
        spStatus->spPrev = spStatus->spBegin;
        spStatus->spBegin = spStatus->spBegin->spNext;
    }

    if(ulLba == spStatus->spBegin->sData.ulLba)
    {
        spStatus->ucStatus = AT_BEGIN;
    }
    else
    {
        spStatus->ucStatus = IN_MIDDLE;
        spStatus->spPrev = spStatus->spBegin;
    }
}

/**
 * @function    uint32_t CompareInOneList(DATASTATUS_S* spStatus, UserDataItem_S* apMapData, uint32_t aDataBuf[])
 * @brief       Compare Data in one list.
 * @prime[in]   spStatus: Address of the first node to be compared.
 * @prime[in]   apMapData: Information of CMD. The start LBA and Length need to be compared.
 * @prime[in]   aDataBuf[]: Data buffer of data need to be compared.
 * @return      0 is success. others is Error occurring, the value is the first wrong LBA+1.
 */
static uint32_t CompareInOneList(DATASTATUS_S* spStatus, UserDataItem_S* apMapData, uint32_t* apDataBuf)
{
    uint32_t lba = apMapData->ulLba;
    uint32_t len = apMapData->ulLen;
    uint32_t uli = 0;
    uint32_t cnt = 0;
    uint32_t key = 0;
    uint32_t result = 0;
    UserDataList_S* spTempNode = spStatus->spBegin;
    UserDataSector_S* spDataBuf = (UserDataSector_S *)apDataBuf;

    while(len) /** Compare data in every node one by one. */
    {
        cnt = (spTempNode->sData.ulLen + spTempNode->sData.ulLba - lba);
        if(len <= cnt)
        {
            cnt = len;
        }

        key = spTempNode->sData.ulKey;

        for(uli = 0; uli < cnt; uli++) /** Compare data in one node. */
        {
            if((spDataBuf->ulLba != lba) ||
                    (spDataBuf->ulKey != spTempNode->sData.ulKey)) /** Error occurred. */
            {
                printf("Data Compare Error(0x%08x, 0x%x):\n",
                        apMapData->ulLba, apMapData->ulLen);
                printf("Expected: (0x%08x, 0x%x[%d])\n", lba, key, key);
                printf("Actual:   (0x%08x, 0x%x[%d])\n", spDataBuf->ulLba,
                        spDataBuf->ulKey, spDataBuf->ulKey);

                return ++lba;
            }

            lba++;
            spDataBuf++;
        }

        len -= cnt;

        if(len)
        {
            spTempNode = spTempNode->spNext;
        }
    }

    return result;
}

/**
 * @function    void InsertNodeHead(UserDataList_S* spBegin, uint8_t ucStartSta, UserDataItem_S* apMapData)
 * @brief       Insert node into the list, but do not care the tail.
 * @prime[i/o]  spBegin: Input original node addr where the new node will be inserted, and output the new node addr.
 * @prime[in]   ucStartSta: Related position of Inserted node with original node.
 * @prime[in]   apMapData: The LBA need to search.
 */
static void InsertNodeHead(UserDataList_S** spBegin, uint8_t ucStartSta, UserDataItem_S* apMapData)
{
    UserDataList_S* spTempNode = NULL;

    assert(NULL != (*spBegin));

    spTempNode = (UserDataList_S *)malloc(sizeof(UserDataList_S));
    assert(NULL != spTempNode);

    if(IN_MIDDLE == ucStartSta) /** Devided node into two, if the new node's LBA is in middle of original node. */
    {
        memcpy(spTempNode, (*spBegin), sizeof(UserDataList_S));
        (*spBegin)->spNext = spTempNode;
        (*spBegin)->sData.ulLen = (apMapData->ulLba - (*spBegin)->sData.ulLba);

        (*spBegin) = spTempNode;

        spTempNode = NULL;
        spTempNode = (UserDataList_S *)malloc(sizeof(UserDataList_S));
        assert(NULL != spTempNode);
    }

    memcpy(spTempNode, (*spBegin), sizeof(UserDataList_S));
    memcpy(&((*spBegin)->sData), apMapData, sizeof(UserDataItem_S));
    (*spBegin)->spNext = spTempNode;
}

/**
 * @function    void InsertNodeTail(UserDataList_S* spBegin)
 * @brief       Handle the tail of inserted node.
 * @prime[in]   spBegin: Address of new inserted node.
 */
static void InsertNodeTail(UserDataList_S* spBegin)
{
    uint32_t ulEnd = spBegin->sData.ulLba + spBegin->sData.ulLen;
    UserDataList_S* spTempNode = spBegin->spNext;

    while(ulEnd >= spTempNode->sData.ulLba + spTempNode->sData.ulLen)
        /** Search the node cooresponding to the tail of inserted node and free the intermediate node. */
    {
        if(NULL != spTempNode->spNext)
        {
            spBegin->spNext = spTempNode->spNext;
            free(spTempNode);
            spTempNode = spBegin->spNext;
        }
        else /** Reach the end of this list. */
        {
            free(spTempNode);
            spTempNode = NULL;
            spBegin->spNext = NULL;
            break;
        }
    }

    if(NULL != spTempNode) /** Modify node data next to the inserted node. */
    {
        spTempNode->sData.ulLen -= (ulEnd - spTempNode->sData.ulLba);
        spTempNode->sData.ulLba = ulEnd;
    }
}

/**
 * @function    void MergeTrimmedNode(DATASTATUS_S* spStatus)
 * @brief       Merge the node with type of TYPE_TRIM.
 * @prime[in]   spStatus: Information of inserted node.
 */
static void MergeTrimmedNode(DATASTATUS_S* spStatus)
{
    UserDataList_S* spTempNode =  spStatus->spBegin->spNext;

    if((spStatus->spPrev != spStatus->spBegin) &&
        (TYPE_TRIM == spStatus->spPrev->sData.ulType))
        /** Handle Previous node and inserted node. */
    {
        spStatus->spPrev->sData.ulLen += spStatus->spBegin->sData.ulLen;
        spStatus->spPrev->spNext = spStatus->spBegin->spNext;
        free(spStatus->spBegin);
        spStatus->spBegin = spStatus->spPrev;
    }

    if((NULL != spTempNode) && (TYPE_TRIM == spTempNode->sData.ulType))
        /** Handle inserted node and the next node. */
    {
        spStatus->spBegin->spNext = spTempNode->spNext;
        spStatus->spBegin->sData.ulLen += spTempNode->sData.ulLen;
        free(spTempNode);
    }
}

/**
 * @function    InsertNode(DATASTATUS_S* spStatus, UserDataItem_S* apMapData)
 * @brief       Inserted the node into the list.
 * @prime[in]   spStatus: The position where the node will be inserted.
 * @prime[in]   apMapData: Data of inserted node.
 */
static void InsertNode(DATASTATUS_S* spStatus, UserDataItem_S* apMapData)
{
    InsertNodeHead(&(spStatus->spBegin), spStatus->ucStatus, apMapData);

    InsertNodeTail(spStatus->spBegin);

    if(TYPE_TRIM == apMapData->ulType)
    {
        MergeTrimmedNode(spStatus);
    }
}

/**
 * @function    uint32_t CompareOrInsert(UserDataList_S* apMapList, UserDataItem_S* apMapData, uint32_t isCompare, uint32_t aDataBuf[])
 * @brief       Compare or inserte data.
 * @prime[in]   apMapList: Position of list to be inserted in or compared with.
 * @prime[in]   apMapData: Data of inserted node.
 * @prime[in]   isCompare: Flag of compare. non-0 is Compare.
 * @prime[in]   aDataBuf[]: Compared data buffer.
 * @return      result of Compare. 0 is success, others is the LBA where the first error occurred.
 */
uint32_t CompareOrInsert(UserDataList_S* apMapList, UserDataItem_S* apMapData, uint32_t isCompare, uint32_t aDataBuf[])
{
    uint32_t lba = apMapData->ulLba;
    uint32_t len = apMapData->ulLen;
    uint32_t idx = 0;
    uint32_t cnt = 0;
    uint32_t result = 0;
    DATASTATUS_S sStatus = {NULL, NULL, 0};
    UserDataItem_S sOneListData;

    assert(lba < gPartSize);
    if((lba + len) > gPartSize)
    {
        len = gPartSize - lba;
    }

    sOneListData.ulType = apMapData->ulType;
    sOneListData.ulKey = apMapData->ulKey;

    while(len) /** Operate in every list one by one. */
    {
        idx = lba / gMaxMapSize;
        cnt = (gMaxMapSize - lba % gMaxMapSize);

        if(len <= cnt) /** The last list to be operated. */
        {
            cnt = len;
        }

        memset(&sStatus, 0, sizeof(DATASTATUS_S));
        sStatus.spBegin = &apMapList[idx];
        sStatus.spPrev = sStatus.spBegin;

        sOneListData.ulLba = lba;
        sOneListData.ulLen = cnt;

        GetHeadStatus(&sStatus, lba);

        if(IS_COMPARE == isCompare) /** operation is comparing data in the list. */
        {
            result = CompareInOneList(&sStatus, &sOneListData, aDataBuf);

            if(0 != result) /** Error has occurred. */
            {
                return result;
            }
        }
        else /** Operation is write or trim. */
        {
            InsertNode(&sStatus, &sOneListData);
        }

        len -= cnt;
        lba += cnt;
    }

    return result;
}

/**
 * @function    void UserDataList_Init(UserDataList_S** apMapList, uint32_t aPartSize)
 * @brief       Initialize.
 * @prime[i/o]  apMapList:
 * @prime[in]   aPartSize: The total size of this partition.
 */
void UserDataList_Init(UserDataList_S** apMapList, uint32_t aPartSize, uint32_t aMaxMapSize)
{
    uint32_t uli = 0;

    assert(0 != aPartSize);
    gPartSize = aPartSize;

    if(0 == aMaxMapSize)
    {
        gMaxMapSize = MAX_MAP_LENGTH;
    }
    else
    {
        gMaxMapSize = aMaxMapSize;
    }

    gMapSize = ((aPartSize - 1) / gMaxMapSize) + 1;

    *apMapList = malloc(gMapSize*sizeof(UserDataList_S));
    assert(NULL != *apMapList);

    memset(*apMapList, 0, gMapSize*sizeof(UserDataList_S));

    for(uli = 0; uli < gMapSize; uli++) /** Init all the list one by one. */
    {
        (*apMapList)[uli].sData.ulLba = uli * gMaxMapSize;
        (*apMapList)[uli].sData.ulType = TYPE_BLANK;

        if(uli == gMapSize - 1) /** The last list size is the rest. */
        {
            (*apMapList)[uli].sData.ulLen = aPartSize % gMaxMapSize;
        }
        else
        {
            (*apMapList)[uli].sData.ulLen = gMaxMapSize;
        }
    }
}

/**
 * @function    void UserDataListTerm(UserDataList_S** apMapList)
 * @brief       Free all space and terminate these functions.
 * @prime[i/o]  apMapList:
 */
void UserDataList_Term(UserDataList_S** apMapList)
{
    uint32_t uli = 0;
    UserDataList_S* spFirstNode = NULL;
    UserDataList_S* spNextNode = NULL;

    for(uli = 0; uli < gMapSize; uli++) /** Term all the  list one by one. */
    {
        spFirstNode = (*apMapList)[uli].spNext;

        while(spFirstNode) /** Free all the node except the first one in the list. */
        {
            spNextNode = spFirstNode->spNext;
            free(spFirstNode);
            spFirstNode = spNextNode;
        }
    }

    memset(*apMapList, 0, gMapSize*sizeof(UserDataList_S));

    free(*apMapList);
    *apMapList = NULL;
}

/**
 * @function    void UserDataList_Insert(UserDataList_S* apMapList, UserDataItem_S* apMapData)
 * @brief       Inserte data.
 * @prime[in]   apMapList: Address of list to be inserted in.
 * @prime[in]   apMapData: Data of inserted node.
 */
void UserDataList_Insert(UserDataList_S* apMapList, UserDataItem_S* apMapData)
{
    CompareOrInsert(apMapList, apMapData, IS_WRITE, NULL);
}

/**
 * @function    uint32_t UserDataList_Compare(UserDataList_S* apMapList, UserDataItem_S* apMapData, uint32_t aDataBuf[])
 * @brief       Compare data.
 * @prime[in]   apMapList: Position of list to be inserted in or compared with.
 * @prime[in]   apMapData: Data of inserted node.
 * @prime[in]   aDataBuf[]: Compared data buffer.
 * @return      result of Compare. 0 is success, others is the LBA where the first error occurred.
 */
uint32_t UserDataList_Compare(UserDataList_S* apMapList, UserDataItem_S* apMapData, uint32_t aDataBuf[])
{
    return CompareOrInsert(apMapList, apMapData, IS_COMPARE, aDataBuf);
}

/**
 * @function    void UserDataList_Store(FILE* apFile, UserDataList_S* apMapList)
 * @brief       Write all the data into a file.
 * @prime[out]  apFile: The file which the data will be writen into.
 * @prime[in]   apMapList: Address where data need to be writen from.
 */
void UserDataList_Store(FILE* apFile, UserDataList_S* apMapList)
{
    uint32_t uli = 0;
    UserDataList_S* spTempNode = NULL;

    for (uli = 0; uli < gMapSize; ++uli)
    {
        spTempNode = &apMapList[uli];

        while (NULL != spTempNode)
        {
            fwrite(&spTempNode->sData, sizeof(UserDataItem_S), 1, apFile);
            spTempNode = spTempNode->spNext;
        }
    }
}

/**
 * @function    void UserDataList_Load(FILE* apFile, UserDataList_S* apMapList, uint32_t aPartSize)
 * @brief       Read all the data from a file.
 * @prime[in]   apFile: The file which the data will be writen into.
 * @prime[out]  apMapList: Address of which the data need to be read to.
 * @prime[in]   aPartSize: The total size of this partition.
 */
void UserDataList_Load(FILE* apFile, UserDataList_S* apMapList, uint32_t aPartSize)
{
    uint32_t uli = 0;
    uint32_t ulcnt = 0;
    UserDataItem_S spTempData = {0, 0, 0, 0};
    UserDataList_S* spPrevNode = NULL;
    UserDataList_S* spTempNode = NULL;

    assert(NULL != apFile);

    /** Get the size of this file. */
    fseek(apFile, 0L, SEEK_END);
    ulcnt = ftell(apFile);
    ulcnt /= sizeof(UserDataItem_S);
    fseek(apFile, 0L, SEEK_SET);

    assert(0 != ulcnt);

    while(ulcnt)
    {
        fread(&spTempData, sizeof(UserDataItem_S), 1, apFile);

        if(spTempData.ulLba % gMaxMapSize) /** This node is in the middle of list. */
        {
            spTempNode = (UserDataList_S *)malloc(sizeof(UserDataList_S));
            assert(spTempNode);
            memcpy(&spTempNode->sData, &spTempData, sizeof(UserDataItem_S));
            spTempNode->spNext = NULL;

            spPrevNode->spNext = spTempNode;
            spPrevNode = spPrevNode->spNext;
            spTempNode = NULL;
        }
        else /** It is the header of list. */
        {
            memcpy(&apMapList[uli].sData, &spTempData, sizeof(UserDataItem_S));
            spPrevNode = &apMapList[uli];
            uli++;
        }

        ulcnt--;
    }
}

/**
 * @function    uint8_t  UserDataList_CheckNode(UserDataList_S* apMapList, UserDataItem_S apMapData)
 * @brief       Check a node in the list, used to test.
 * @prime[in]   apMapList: Address the data writen from.
 * @prime[in]   apMapData: Expected data of the node.
 * @return      0 is success, 1 is error.
 */
uint8_t UserDataList_CheckNode(UserDataList_S* apMapList, UserDataItem_S* apMapData)
{
    uint32_t uli = 0;
    UserDataList_S* spTempNode = NULL;

    for(uli = 0; uli < gMapSize; uli++)
    {
        spTempNode = &apMapList[uli];

        while(spTempNode) /** search the node. */
        {
            if((spTempNode->sData.ulLba == apMapData->ulLba) &&
                    (spTempNode->sData.ulLen == apMapData->ulLen) &&
                    (spTempNode->sData.ulKey == apMapData->ulKey) &&
                    (spTempNode->sData.ulType == apMapData->ulType))
            {
                return 0;
            }

            spTempNode = spTempNode->spNext;
        }
    }

    return 1;
}

