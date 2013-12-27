/**********************************************************
 *     Module Name : test_plan.h
 *     Description : defination of all the test cases
***********************************************************/

/***********************************************************
 *     Kaifeng Zhuang@2013.7.30
 *     Description : Initial Create
 ***********************************************************/
#ifndef _TEST_PLAN_H
#define _TEST_PLAN_H

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
#define MAKE_SUITE_ID(m,s)     (m*1000 + s)

/***************************************************
 * Enum section
 * Add all enums and unions here
 *
****************************************************/

enum {
	MODULE_FILEACCESS 	    = 1,
	MODULE_DATAMAP          = 2,
};

enum {
	SUITE_FILEACCESS_NORMAL = MAKE_SUITE_ID(MODULE_FILEACCESS, 1),
};

enum {
	SUITE_DATAMAP_NORMAL = MAKE_SUITE_ID(MODULE_DATAMAP, 1),
};

/***************************************************
 * Structure and Union section
 * Add all structures and unions here
 *
****************************************************/


/***************************************************
 *  Variables section
 * Add all variable decleartion here
 *
****************************************************/

/***************************************************
 *  Function Prototype Section
 * Add prototypes for all functions called by this
 * module, with the exception of runtime routines.
 *
****************************************************/
extern uint32_t TestPlan(const char *cfg_file, const char *param_file);

#endif


