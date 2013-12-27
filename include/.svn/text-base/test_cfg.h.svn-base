/**********************************************************
 *     Module Name : test_cfg.h
 *     Description : test config parse
***********************************************************/
#ifndef _TEST_CFG_H
#define _TEST_CFG_H

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

#define MAKE_TEST_ID(module, suite, case) (((module)*1000 + (suite))*10000 + (case))

extern int test_plan_init(const char *cfg_file);
extern char* test_plan_get_suite(uint32_t index);
extern uint32_t test_plan_get_suite_cnt(void);

extern uint32_t test_plan_get_case_cnt(void);
extern uint32_t* test_plan_get_case_list(void);

#endif



