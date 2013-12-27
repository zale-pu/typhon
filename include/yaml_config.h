/**********************************************************
 *
 * Proprietary & confidential, All right reserved, 2011
 *         GLS.
 *
 *     Module Name : yaml_cfg.h
 *     Description : %a short description for the content%
***********************************************************/


/***********************************************************
 *     Jason Ma @2012.11.28
 *         Description : Create
************************************************************/


#ifndef __YAML_CFG_H
#define __YAML_CFG_H

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
#define SETUP_TABLE(t, i)  				        do {	(t).count = sizeof(i)/sizeof((i)[0]);	(t).items = i; } while(0)
#define SETUP_TABLE_ITEM(type, name, value) 	{type, name, value}

/***************************************************
 * Enum section
 * Add all enums and unions here
 *
****************************************************/
enum CfgItemType {
	TYPE_INT,
	TYPE_BOOL,
	TYPE_STRING,
	TYPE_TABLE,
};

/***************************************************
 * Structure and Union section
 * Add all structures and unions here
 *
****************************************************/

/**
 * struct CfgParser - Cfg File Parser
**/
struct CfgParser;

/**
 * struct CfgItem - Cfg File Item
**/

struct CfgItem
{
	enum CfgItemType type;
	const char *name;
	void *value;
};

/**
 * struct CfgParserTable - Cfg File Parser Table
**/

struct CfgTable
{
	uint32_t count;
	struct CfgItem *items;
};

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
extern struct CfgParser *YamlCfgParser_Create(void);
extern void YamlCfgParser_Destroy(struct CfgParser* apParser);
extern int YamlCfgParser_ParseFile(struct CfgParser* apParser, const char* apFileName);
extern int YamlCfgParser_ParseString(struct CfgParser* apParser, const char* apString);
extern void YamlCfgParser_SetTable(struct CfgParser* apParser, struct CfgTable* apTable);

#endif


