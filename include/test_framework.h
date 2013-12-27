/**********************************************************
 *     Module Name : test_framework.h
 *     Description : test framework modules
 *
 **********************************************************/

/**********************************************************
  *     Kaifeng Zhuang @ 2013.07.27
  *     Description :  Initial create
  *
 **********************************************************/

#ifndef __TEST_FRAMEWORK_H
#define __TEST_FRAMEWORK_H

/***************************************************
 *  Include section
 * Add all #includes here
 *
 ***************************************************/

/***************************************************
 * Structure and Union section
 * Add all structures and unions here
 *
 ***************************************************/

/***************************************************
 *  Defines section
 * Add all #defines here
 *
 ***************************************************/
#if !defined(TESTFRAMEWORK_INTERFACE_MODULE)
#define TEST_FRAMEWORK_INTERFACE __declspec(dllimport)
#else
#define TEST_FRAMEWORK_INTERFACE __declspec(dllexport)
#endif
/***************************************************
* Global variables section
* Add all globalvariables here
*
****************************************************/

/***************************************************
*  Function Prototype Section
* Add prototypes for all functions called by this
* module, with the exception of runtime routines.
*
***************************************************/
#endif

