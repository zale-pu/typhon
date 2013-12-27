#include <stddef.h>
#include <string.h>

#include "cmockery.h"
#include "test_plan.h"
#include "map_file.h"
#include "memory_file.h"
#include "file_access.h"

#define TEST_BUFFER_LEN  256

static void Test_MemFileAccess(void **state)
{
    FILE_HANDLE_S* pMemFile = NULL;
    uint8_t   readBuf[TEST_BUFFER_LEN];
    uint8_t   writeBuf[TEST_BUFFER_LEN];

    pMemFile = MemoryFileCreate("test_mem_file.txt", 1024);

    memset(readBuf, 0, TEST_BUFFER_LEN);
    memset(writeBuf, 0x1, TEST_BUFFER_LEN);

    FileOpen(pMemFile);
    FileWrite(pMemFile, 0, TEST_BUFFER_LEN, writeBuf);

    FileFlush(pMemFile);
    FileRead(pMemFile, 0, TEST_BUFFER_LEN, readBuf);
    FileClose(pMemFile);

    MemoryFileDestroy(pMemFile);

    assert_int_equal(memcmp(writeBuf, readBuf, TEST_BUFFER_LEN), 0);
}

static void Test_MapFileAccess(void **state)
{
    FILE_HANDLE_S* pMapFile = NULL;
    uint8_t   readBuf[TEST_BUFFER_LEN];
    uint8_t   writeBuf[TEST_BUFFER_LEN];
    uint64_t  fileSize = (uint64_t)8*(uint64_t)(1024*1024*1024);

    pMapFile = MapFileCreate("test_map_file.txt", fileSize);

    memset(readBuf, 0, TEST_BUFFER_LEN);
    memset(writeBuf, 1, TEST_BUFFER_LEN);

    FileOpen(pMapFile);
    FileWrite(pMapFile, 0, TEST_BUFFER_LEN, writeBuf);
    FileClose(pMapFile);

    FileOpen(pMapFile);
    FileRead(pMapFile, 0, TEST_BUFFER_LEN, readBuf);
    FileClose(pMapFile);

    MapFileDestroy(pMapFile);

    assert_int_equal(memcmp(readBuf, writeBuf, TEST_BUFFER_LEN), 0);
}

static void setup(void **state)
{
}

static void term (void **state)
{
}

int TestSuite_FileAccessModule_Test(void)
{
    const UnitTest tests[] =
    {
        unit_test_setup_teardown_id(Test_MemFileAccess, setup, term, 1, NULL),
        unit_test_setup_teardown_id(Test_MapFileAccess, setup, term, 2, NULL),
    };

    return run_tests_id(tests, SUITE_FILEACCESS_NORMAL);
}
