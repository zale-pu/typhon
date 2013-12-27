
/*******************************************************************************
 * @Copyright   2013-2016,      Gigadevice .Co., Ltd
 * @file        UserMapModuleTest.c
 * @brief       Data Map Module test file.
 * @author      Qiang Pu
 * @version     1.0
 * @date        2013.12
 * @warning
 * @history     version     author      time            record
 *              1.0         Qiang Pu    2013.12.10      create
 ******************************************************************************/

#include <stddef.h>
#include <string.h>

#include "cmockery.h"
#include "test_plan.h"
#include "map_file.h"
#include "memory_file.h"
#include "file_access.h"

#include "userdata.h"
#include "userdatalist.h"

typedef enum _RUN_OPERATION
{
    _END     = 0,
    _WRITE   = 1,
    _COMPARE = 2,
    _CHECK   = 3,
    _DEL     = 4
}_RUN_OPERATION_E;

static uint32_t gDataBuf[32];
static uint32_t gResult[3];
static uint32_t gPartIndex;

static void RunCmdList(const uint32_t aCmdTable[])
{
    uint32_t aMapData[4];

    while(_END != aCmdTable[0])
    {
        aMapData[0] = aCmdTable[1];
        aMapData[1] = aCmdTable[2];
        aMapData[2] = aCmdTable[3];
        aMapData[3] = aCmdTable[4];

        switch(aCmdTable[0])
        {
            case _WRITE:
                UserData_Insert(gPartIndex, aMapData);
//                printf("Insert Node:%d, %d, %d, %d\n",aMapData[0],aMapData[1],aMapData[2],aMapData[3]);

                break;

            case _COMPARE:
                gResult[0] = UserData_Compare(gPartIndex, aMapData, gDataBuf);
                if(0 != gResult[0])
                {
                    return;
                }
                break;

            case _CHECK:
                gResult[1] = UserData_CheckNode(gPartIndex, aMapData);
                if(0 != gResult[1])
                {
                    printf("Check Node error:%d, %d, %d, %d\n",aMapData[0],aMapData[1],aMapData[2],aMapData[3]);
//                    return;
                }
                break;

            case _DEL:
                gResult[2] = UserData_CheckNode(gPartIndex, aMapData);
                if(0 == gResult[2])
                {
                    printf("Del Node error:%d, %d, %d, %d\n",aMapData[0],aMapData[1],aMapData[2],aMapData[3]);
//                    return;
                }
                break;

            default:
                break;
        }

        aCmdTable += 5;
    }
}

static void Test_Init(void **state)
{
    const uint32_t table[] = {
        _CHECK, 0, 10, TYPE_BLANK, 0,
        _CHECK, 10, 6, TYPE_BLANK, 0,
        _END
    };
    RunCmdList(table);
//    assert_int_equal(0, gResult[0]);
    assert_int_equal(0, gResult[1]);
//    assert_int_not_equal(0, gResult[2]);
}

static void Test_MapStart_Mid_One(void **state)
{
    const uint32_t table[] =
    {
        _WRITE, 0, 2, TYPE_WRITE, 1,
        _CHECK, 0, 2, TYPE_WRITE, 1,
        _CHECK, 2, 8, TYPE_BLANK, 0,
        _DEL,   0, 10, TYPE_BLANK, 0,
        _END
    };
    RunCmdList(table);

    assert_int_equal(0, gResult[0]);     /** read */
    assert_int_equal(0, gResult[1]);     /** check*/
    assert_int_not_equal(0, gResult[2]); /** del  */
}

static void Test_MapStart_ListEnd_One(void **state)
{
    const uint32_t table[] =
    {
        _WRITE, 0, 10, TYPE_WRITE, 1,
        _CHECK, 0, 10, TYPE_WRITE, 1,
        _DEL,   0, 10, TYPE_BLANK, 0,
        _END
    };
    RunCmdList(table);

    assert_int_equal(0, gResult[0]);
    assert_int_equal(0, gResult[1]);
    assert_int_not_equal(0, gResult[2]);
}

static void Test_Mid_ListEnd_One(void **state)
{
    const uint32_t table[] =
    {
        _WRITE, 8, 2, TYPE_WRITE, 1,
        _CHECK, 0, 8, TYPE_BLANK, 0,
        _CHECK, 8, 2, TYPE_WRITE, 1,
        _DEL,   0, 10, TYPE_BLANK, 0,
        _END
    };
    RunCmdList(table);

    assert_int_equal(0, gResult[0]);
    assert_int_equal(0, gResult[1]);
    assert_int_not_equal(0, gResult[2]);
}

static void Test_Mid_Mid_One(void **state)
{
    const uint32_t table[] =
    {
        _WRITE, 2, 2, TYPE_WRITE, 1,
        _CHECK, 0, 2, TYPE_BLANK, 0,
        _CHECK, 2, 2, TYPE_WRITE, 1,
        _CHECK, 4, 6, TYPE_BLANK, 0,
        _DEL,   0, 10, TYPE_BLANK, 0,
        _END
    };
    RunCmdList(table);

    assert_int_equal(0, gResult[0]);
    assert_int_equal(0, gResult[1]);
    assert_int_not_equal(0, gResult[2]);
}

static void Test_Mid_Mid_Cross(void **state)
{
    const uint32_t table[] =
    {
        _WRITE, 8, 4, TYPE_WRITE, 1,
        _CHECK, 0, 8, TYPE_BLANK, 0,
        _CHECK, 8, 2, TYPE_WRITE, 1,
        _CHECK, 10, 2, TYPE_WRITE, 1,
        _DEL,   0, 10, TYPE_BLANK, 0,
        _DEL,   10, 6, TYPE_BLANK, 0,
        _END
    };
    RunCmdList(table);

    assert_int_equal(0, gResult[0]);
    assert_int_equal(0, gResult[1]);
    assert_int_not_equal(0, gResult[2]);
}

static void Test_ListStart_Mid_NextList(void **state)
{
    const uint32_t table[] =
    {
        _WRITE, 10, 2, TYPE_WRITE, 1,
        _CHECK, 10, 2, TYPE_WRITE, 1,
        _CHECK, 12, 4, TYPE_BLANK, 0,
        _DEL,   10, 6, TYPE_BLANK, 0,
        _END
    };
    RunCmdList(table);

    assert_int_equal(0, gResult[0]);
    assert_int_equal(0, gResult[1]);
    assert_int_not_equal(0, gResult[2]);
}

static void Test_Mid_End_One(void **state)
{
    const uint32_t table[] =
    {
        _WRITE, 0, 4, TYPE_WRITE, 1,
        _WRITE, 2, 2, TYPE_WRITE, 2,
        _CHECK, 0, 2, TYPE_WRITE, 1,
        _CHECK, 2, 2, TYPE_WRITE, 2,
        _CHECK, 4, 6, TYPE_BLANK, 0,
        _DEL,   0, 4, TYPE_BLANK, 0,
        _END
    };
    RunCmdList(table);

    assert_int_equal(0, gResult[0]);
    assert_int_equal(0, gResult[1]);
    assert_int_not_equal(0, gResult[2]);
}

static void Test_Start_Mid_One(void **state)
{
    const uint32_t table[] =
    {
        _WRITE, 2, 4, TYPE_WRITE, 1,
        _WRITE, 2, 2, TYPE_WRITE, 2,
        _CHECK, 0, 2, TYPE_BLANK, 0,
        _CHECK, 2, 2, TYPE_WRITE, 2,
        _CHECK, 4, 2, TYPE_WRITE, 1,
        _CHECK, 6, 4, TYPE_BLANK, 0,
        _DEL,   2, 4, TYPE_WRITE, 1,
        _END
    };
    RunCmdList(table);

    assert_int_equal(0, gResult[0]);
    assert_int_equal(0, gResult[1]);
    assert_int_not_equal(0, gResult[2]);
}

static void Test_Start_End_One(void **state)
{
    const uint32_t table[] =
    {
        _WRITE, 2, 2, TYPE_WRITE, 1,
        _WRITE, 2, 2, TYPE_WRITE, 2,
        _CHECK, 0, 2, TYPE_BLANK, 0,
        _CHECK, 2, 2, TYPE_WRITE, 2,
        _CHECK, 4, 6, TYPE_BLANK, 0,
        _DEL,   2, 2, TYPE_WRITE, 1,
        _END
    };
    RunCmdList(table);

    assert_int_equal(0, gResult[0]);
    assert_int_equal(0, gResult[1]);
    assert_int_not_equal(0, gResult[2]);
}

static void Test_Start_Mid_Two(void **state)
{
    const uint32_t table[] =
    {
        _WRITE, 2, 2, TYPE_WRITE, 1,
        _WRITE, 2, 4, TYPE_WRITE, 2,
        _CHECK, 0, 2, TYPE_BLANK, 0,
        _CHECK, 2, 4, TYPE_WRITE, 2,
        _CHECK, 6, 4, TYPE_BLANK, 0,
        _DEL,   2, 2, TYPE_WRITE, 1,
        _DEL,   4, 6, TYPE_BLANK, 0,
        _END
    };
    RunCmdList(table);

    assert_int_equal(0, gResult[0]);
    assert_int_equal(0, gResult[1]);
    assert_int_not_equal(0, gResult[2]);
}

static void Test_Start_End_Two(void **state)
{
    const uint32_t table[] =
    {
        _WRITE, 2, 2, TYPE_WRITE, 1,
        _WRITE, 4, 2, TYPE_WRITE, 2,
        _WRITE, 2, 4, TYPE_WRITE, 3,
        _CHECK, 0, 2, TYPE_BLANK, 0,
        _CHECK, 2, 4, TYPE_WRITE, 3,
        _CHECK, 6, 4, TYPE_BLANK, 0,
        _DEL,   2, 2, TYPE_WRITE, 1,
        _DEL,   4, 2, TYPE_WRITE, 2,
        _END
    };
    RunCmdList(table);

    assert_int_equal(0, gResult[0]);
    assert_int_equal(0, gResult[1]);
    assert_int_not_equal(0, gResult[2]);
}

static void Test_Mid_Mid_Two(void **state)
{
    const uint32_t table[] =
    {
        _WRITE, 4, 4, TYPE_WRITE, 1,
        _WRITE, 2, 4, TYPE_WRITE, 2,
        _CHECK, 0, 2, TYPE_BLANK, 0,
        _CHECK, 2, 4, TYPE_WRITE, 2,
        _CHECK, 6, 2, TYPE_WRITE, 1,
        _DEL,   0, 4, TYPE_BLANK, 0,
        _DEL,   4, 4, TYPE_WRITE, 1,
        _END
    };
    RunCmdList(table);

    assert_int_equal(0, gResult[0]);
    assert_int_equal(0, gResult[1]);
    assert_int_not_equal(0, gResult[2]);
}

static void Test_Mid_End_Two(void **state)
{
    const uint32_t table[] =
    {
        _WRITE, 4, 2, TYPE_WRITE, 1,
        _WRITE, 2, 4, TYPE_WRITE, 2,
        _CHECK, 0, 2, TYPE_BLANK, 0,
        _CHECK, 2, 4, TYPE_WRITE, 2,
        _CHECK, 6, 4, TYPE_BLANK, 0,
        _DEL,   0, 4, TYPE_BLANK, 0,
        _DEL,   4, 2, TYPE_WRITE, 1,
        _END
    };
    RunCmdList(table);

    assert_int_equal(0, gResult[0]);
    assert_int_equal(0, gResult[1]);
    assert_int_not_equal(0, gResult[2]);
}

static void Test_Start_Mid_More(void **state)
{
    const uint32_t table[] =
    {
        _WRITE, 2, 2, TYPE_WRITE, 1,
        _WRITE, 4, 2, TYPE_WRITE, 2,
        _WRITE, 2, 6, TYPE_WRITE, 3,
        _CHECK, 0, 2, TYPE_BLANK, 0,
        _CHECK, 2, 6, TYPE_WRITE, 3,
        _CHECK, 8, 2, TYPE_BLANK, 0,
        _DEL,   2, 2, TYPE_WRITE, 1,
        _DEL,   4, 2, TYPE_WRITE, 2,
        _DEL,   6, 4, TYPE_WRITE, 0,
        _END
    };
    RunCmdList(table);

    assert_int_equal(0, gResult[0]);
    assert_int_equal(0, gResult[1]);
    assert_int_not_equal(0, gResult[2]);
}

static void Test_Start_End_More(void **state)
{
    const uint32_t table[] =
    {
        _WRITE, 2, 2, TYPE_WRITE, 1,
        _WRITE, 6, 2, TYPE_WRITE, 2,
        _WRITE, 2, 6, TYPE_WRITE, 3,
        _CHECK, 0, 2, TYPE_BLANK, 0,
        _CHECK, 2, 6, TYPE_WRITE, 3,
        _CHECK, 8, 2, TYPE_BLANK, 0,
        _DEL,   2, 2, TYPE_WRITE, 1,
        _DEL,   4, 2, TYPE_BLANK, 0,
        _DEL,   6, 2, TYPE_WRITE, 2,
        _END
    };
    RunCmdList(table);

    assert_int_equal(0, gResult[0]);
    assert_int_equal(0, gResult[1]);
    assert_int_not_equal(0, gResult[2]);
}

static void Test_Mid_Mid_More(void **state)
{
    const uint32_t table[] =
    {
        _WRITE, 4, 2, TYPE_WRITE, 1,
        _WRITE, 2, 6, TYPE_WRITE, 2,
        _CHECK, 0, 2, TYPE_BLANK, 0,
        _CHECK, 2, 6, TYPE_WRITE, 2,
        _CHECK, 8, 2, TYPE_BLANK, 0,
        _DEL,   0, 4, TYPE_BLANK, 0,
        _DEL,   4, 2, TYPE_WRITE, 1,
        _DEL,   6, 4, TYPE_BLANK, 0,
        _END
    };
    RunCmdList(table);

    assert_int_equal(0, gResult[0]);
    assert_int_equal(0, gResult[1]);
    assert_int_not_equal(0, gResult[2]);
}

static void Test_Mid_End_More(void **state)
{
    const uint32_t table[] =
    {
        _WRITE, 4, 2, TYPE_WRITE, 1,
        _WRITE, 6, 2, TYPE_WRITE, 2,
        _WRITE, 2, 6, TYPE_WRITE, 3,
        _CHECK, 0, 2, TYPE_BLANK, 0,
        _CHECK, 2, 6, TYPE_WRITE, 3,
        _CHECK, 8, 2, TYPE_BLANK, 0,
        _DEL,   0, 4, TYPE_BLANK, 0,
        _DEL,   4, 2, TYPE_WRITE, 1,
        _DEL,   6, 2, TYPE_WRITE, 2,
        _END
    };
    RunCmdList(table);

    assert_int_equal(0, gResult[0]);
    assert_int_equal(0, gResult[1]);
    assert_int_not_equal(0, gResult[2]);
}

static void Test_Mid_Overflow(void **state)
{
    const uint32_t table[] =
    {
        _WRITE, 12, 6, TYPE_WRITE, 1,
        _CHECK, 10, 2, TYPE_BLANK, 0,
        _CHECK, 12, 4, TYPE_WRITE, 1,
        _DEL,   10, 6, TYPE_BLANK, 0,
        _DEL,   12, 6, TYPE_WRITE, 1,
        _DEL,   16, 2, TYPE_WRITE, 1,
        _END
    };
    RunCmdList(table);

    assert_int_equal(0, gResult[0]);
    assert_int_equal(0, gResult[1]);
    assert_int_not_equal(0, gResult[2]);
}

static void Test_Store_One_Part(void **state)
{
    uint32_t accessMode = 001;
    char* pDirName = "DataMapModuleTest";
    const uint32_t table[] =
    {
        _WRITE, 2, 2, TYPE_WRITE, 1,
        _WRITE, 6, 2, TYPE_WRITE, 2,
        _WRITE, 12,2, TYPE_WRITE, 3,
        _END
    };
    RunCmdList(table);

    UserData_Store(0, accessMode, pDirName);
}

static void Test_Load_one_Part(void **state)
{
    uint32_t accessMode = 001;
    char* pDirName = "DataMapModuleTest";

    UserData_Load(0, &accessMode, pDirName);

    const uint32_t table[] =
    {
        _CHECK, 0, 2, TYPE_BLANK, 0,
        _CHECK, 2, 2, TYPE_WRITE, 1,
        _CHECK, 4, 2, TYPE_BLANK, 0,
        _CHECK, 6, 2, TYPE_WRITE, 2,
        _CHECK, 8, 2, TYPE_BLANK, 0,
        _CHECK, 10,2, TYPE_BLANK, 0,
        _CHECK, 12,2, TYPE_WRITE, 3,
        _CHECK, 14,2, TYPE_BLANK, 0,
        _END
    };
    RunCmdList(table);

    assert_int_equal(0, gResult[1]);
}

static void Test_Store_And_Load(void **state)
{
    uint32_t index = 0;
    const uint32_t table[] =
    {
        _WRITE, 2, 2, TYPE_WRITE, 1,
        _WRITE, 6, 2, TYPE_WRITE, 2,
        _WRITE, 12,2, TYPE_WRITE, 3,
        _END
    };
    RunCmdList(table);

    uint32_t accessMode = 001;
    char* pDirName = "Data Map Module Test";

    UserData_Store(index, accessMode, pDirName);
    UserData_Term();

    uint32_t aPartSize[MAX_PARTITION_CNT] = {16, };
    uint32_t aMaxMapSize[MAX_PARTITION_CNT] = {10, };
    UserData_Init(aPartSize, aMaxMapSize);
    UserData_Load(&index, &accessMode, pDirName);

    const uint32_t tableCheck[] =
    {
        _CHECK, 0, 2, TYPE_BLANK, 0,
        _CHECK, 2, 2, TYPE_WRITE, 1,
        _CHECK, 4, 2, TYPE_BLANK, 0,
        _CHECK, 6, 2, TYPE_WRITE, 2,
        _CHECK, 8, 2, TYPE_BLANK, 0,
        _CHECK, 10,2, TYPE_BLANK, 0,
        _CHECK, 12,2, TYPE_WRITE, 3,
        _CHECK, 14,2, TYPE_BLANK, 0,
        _END
    };
    RunCmdList(tableCheck);

    assert_int_equal(0, gResult[1]);
}

static void Test_Setup(void **state)
{
    uint32_t aPartSize[MAX_PARTITION_CNT] = {16, };
    uint32_t aMaxMapSize[MAX_PARTITION_CNT] = {10, };
    UserData_Init(aPartSize, aMaxMapSize);

    memset(gDataBuf, 0, 32*sizeof(uint32_t));
    memset(gResult, 0, 3*sizeof(uint32_t));
    gPartIndex = 0;
}

static void Test_Term (void **state)
{
    UserData_Term();
}

int TestSuite_DataMapModule_Test(void)
{
    const UnitTest tests[] =
    {
        unit_test_setup_teardown_id(Test_Init,                      Test_Setup, Test_Term, 1,   NULL),
        unit_test_setup_teardown_id(Test_MapStart_Mid_One,          Test_Setup, Test_Term, 2,   NULL),
        unit_test_setup_teardown_id(Test_MapStart_ListEnd_One,      Test_Setup, Test_Term, 3,   NULL),
        unit_test_setup_teardown_id(Test_Mid_ListEnd_One,           Test_Setup, Test_Term, 4,   NULL),
        unit_test_setup_teardown_id(Test_Mid_Mid_One,               Test_Setup, Test_Term, 5,   NULL),
        unit_test_setup_teardown_id(Test_Mid_Mid_Cross,             Test_Setup, Test_Term, 6,   NULL),
        unit_test_setup_teardown_id(Test_ListStart_Mid_NextList,    Test_Setup, Test_Term, 7,   NULL),
        unit_test_setup_teardown_id(Test_Mid_End_One,               Test_Setup, Test_Term, 8,   NULL),
        unit_test_setup_teardown_id(Test_Start_Mid_One,             Test_Setup, Test_Term, 9,   NULL),
        unit_test_setup_teardown_id(Test_Start_End_One,             Test_Setup, Test_Term, 10,  NULL),
        unit_test_setup_teardown_id(Test_Start_Mid_Two,             Test_Setup, Test_Term, 11,  NULL),
        unit_test_setup_teardown_id(Test_Start_End_Two,             Test_Setup, Test_Term, 12,  NULL),
        unit_test_setup_teardown_id(Test_Mid_Mid_Two,               Test_Setup, Test_Term, 13,  NULL),
        unit_test_setup_teardown_id(Test_Mid_End_Two,               Test_Setup, Test_Term, 14,  NULL),
        unit_test_setup_teardown_id(Test_Start_Mid_More,            Test_Setup, Test_Term, 15,  NULL),
        unit_test_setup_teardown_id(Test_Start_End_More,            Test_Setup, Test_Term, 16,  NULL),
        unit_test_setup_teardown_id(Test_Mid_Mid_More,              Test_Setup, Test_Term, 17,  NULL),
        unit_test_setup_teardown_id(Test_Mid_End_More,              Test_Setup, Test_Term, 18,  NULL),
        unit_test_setup_teardown_id(Test_Mid_Overflow,              Test_Setup, Test_Term, 19,  NULL),
        unit_test_setup_teardown_id(Test_Store_One_Part,            Test_Setup, Test_Term, 20,  NULL),
        unit_test_setup_teardown_id(Test_Load_one_Part,             Test_Setup, Test_Term, 21,  NULL),
        unit_test_setup_teardown_id(Test_Store_And_Load,            Test_Setup, Test_Term, 22,  NULL)
    };

    return run_tests_id(tests, SUITE_DATAMAP_NORMAL);
}
