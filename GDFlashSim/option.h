/**********************************************************
 *
 * Proprietary & confidential, All right reserved, 2010
 *         GLS.
 *
 *     Module Name : .option.h
 *     Description : Version Info
***********************************************************/
#ifndef OPTION_H_INCLUDED
#define OPTION_H_INCLUDED

/***********************************************************
 *     Kaifeng Zhuang@2013.7.19
 *         Description : Create
************************************************************/

/****************************************************
 *  Include section
 * Add all #includes here
 *
****************************************************/

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

/***************************************************
 *  Variables section
 * Add all variable decleartion here
 *
****************************************************/
extern const char *test_config_file;
extern const char *test_param_file;

/***************************************************
 *  Function Prototype Section
 * Add prototypes for all functions called by this
 * module, with the exception of runtime routines.
 *
****************************************************/
int option_init(int argc, const char * argv[]);

#endif // OPTION_H_INCLUDED
