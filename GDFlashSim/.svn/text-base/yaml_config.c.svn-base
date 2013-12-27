/**********************************************************
 *     Module Name : yaml_cfg.c
 *     Description : %a short description for the content%
***********************************************************/

/***********************************************************
 *     Kaifeng Zhuang @2013.07.27
 *     Description : Init Create
 *
************************************************************/

/****************************************************
 *  Include section
 * Add all #includes here
 *
****************************************************/
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>

#include "yaml.h"
#include "yaml_config.h"

/***************************************************
 *  Defines section
 * Add all #defines here
 *
****************************************************/
//#define DEBUG_YAML
//#define TEST_YAML

#define MAX_DEEPTH            16

#ifdef DEBUG_YAML
#define TRACE                 printf
#else
#define TRACE(...)
#endif

#define GetCurTable(p)       (p->stack[p->deepth-1])
#define PushTableStack(p, t)  do { assert(p->deepth<MAX_DEEPTH); p->stack[p->deepth++] = t; p->cur_item = NULL; }while(0)
#define PopTableStack(p)      do { assert(p->deepth); p->deepth--; p->cur_item = NULL; }while(0)
#define InitTableStack(p)     do { p->deepth = 0;  p->cur_item = NULL;}while(0)

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
struct CfgParser
{
	struct CfgTable *table;
	yaml_parser_t parser;

	struct CfgTable *stack[16];
	int deepth;
	struct CfgItem *cur_item;
};

/***************************************************
 *  Variables section
 * Add all variable decleartion here
 *
****************************************************/
#ifdef DEBUG_YAML
static const char * gEventName[] =
{
    /** An empty event. */
    "YAML_NO_EVENT",

    /** A STREAM-START event. */
    "YAML_STREAM_START_EVENT",
    /** A STREAM-END event. */
    "YAML_STREAM_END_EVENT",

    /** A DOCUMENT-START event. */
    "YAML_DOCUMENT_START_EVENT",
    /** A DOCUMENT-END event. */
    "YAML_DOCUMENT_END_EVENT",

    /** An ALIAS event. */
    "YAML_ALIAS_EVENT",
    /** A SCALAR event. */
    "YAML_SCALAR_EVENT",

    /** A SEQUENCE-START event. */
    "YAML_SEQUENCE_START_EVENT",
    /** A SEQUENCE-END event. */
    "YAML_SEQUENCE_END_EVENT",

    /** A MAPPING-START event. */
    "YAML_MAPPING_START_EVENT",
    /** A MAPPING-END event. */
    "YAML_MAPPING_END_EVENT",
};
#endif
/***************************************************
 *  Function Prototype Section
 * Add prototypes for all functions called by this
 * module, with the exception of runtime routines.
 *
****************************************************/




/*****************************************************************************
 *  Function Implementation Section
 * Add prototypes for all functions called by this
 * module, with the exception of runtime routines.
 *
*****************************************************************************/

/**
 * TestExample() - short function description of foobar
 * @state:	Describe the first argument to foobar.
 *		One can provide multiple line descriptions
 *		for arguments.
 *
 * A longer description, with more discussion of the function foobar()
 * that might be useful to those using or modifying it.  Begins with
 * empty comment line, and may include additional embedded empty
 * comment lines.
 *
 * The longer description can have multiple paragraphs.
**/

struct CfgParser *YamlCfgParser_Create(void)
{
	struct CfgParser * p = malloc(sizeof(*p));

	p->table = NULL;
    yaml_parser_initialize(&p->parser);

	p->deepth = 0;
	memset(p->stack, 0, sizeof(p->stack));

	p->cur_item = NULL;
	return p;
}

void YamlCfgParser_Destroy(struct CfgParser * apParser)
{
	assert(apParser);
	yaml_parser_delete(&apParser->parser);

	free(apParser);
}

void YamlCfgParser_SetTable(struct CfgParser * apParser, struct CfgTable *apTable)
{
	assert(apParser);
	assert(apTable);

	apParser->table = apTable;

	apParser->deepth = 1;
	apParser->stack[0] = apTable;
}

static void process_yaml_parser(struct CfgParser * apParser)
{
    yaml_event_t event;
	yaml_parser_t *parser = &apParser->parser;

    while (1)
	{

		yaml_parser_parse(parser, &event);

#ifdef DEBUG_YAML
		{
			char *data;
			if( event.type == YAML_SCALAR_EVENT )
				data = (char *)event.data.scalar.value;
			else
				data = "null";

			TRACE("\nEvent :%2d(%-40s), Data:%s", event.type, gEventName[event.type], data);
		}
#endif
		if(event.type == YAML_STREAM_END_EVENT || event.type == YAML_NO_EVENT)
			break;

		switch (event.type)
		{
			case YAML_SCALAR_EVENT:
			{
				const char *str = (char *)event.data.scalar.value;
				int i;
				struct CfgTable *tbl = GetCurTable(apParser);

				if (apParser->cur_item)
				{
					switch (apParser->cur_item->type)
					{
						case TYPE_INT:
							*(int *)apParser->cur_item->value = atoi(str);
							break;
						case TYPE_STRING:
							//*(char **)apParser->cur_item->value = strdup(str);
							strcpy(apParser->cur_item->value, str);
							break;
						case TYPE_BOOL:
							if (strcmp(str, "true") == 0 || strcmp(str, "yes") == 0)
								*(bool *)apParser->cur_item->value = true;
							else if (strcmp(str, "false") == 0 || strcmp(str, "no") == 0)
								*(bool *)apParser->cur_item->value = false;
							else
								assert(0);
							break;
						default:
							assert(0);
							break;
					}
					TRACE("Set Item %s to:%s\n", apParser->cur_item->name, str);
					apParser->cur_item = NULL;
				}
				else
				{
					for (i = 0; i < tbl->count; i++)
					{
						if (!strcmp(tbl->items[i].name, str))
						{
							if (tbl->items[i].type == TYPE_TABLE)
							{
								PushTableStack(apParser, tbl->items[i].value);
								TRACE("Switch to Table: name:%s, value:%p\n", tbl->items[i].name, tbl->items[i].value);
							}
							else
							{
								apParser->cur_item = &tbl->items[i];
								TRACE("Switch to Item: type:%d, name:%s\n", apParser->cur_item->type, apParser->cur_item->name);
							}
							break;
						}
					}
					if (i == tbl->count)
					{
						TRACE("Unknown Item:%s\n", str);
					}
				}
				break;
			}
			case YAML_SEQUENCE_END_EVENT:
			{
				PopTableStack(apParser);
				TRACE("Pop to Table: value:%p\n", GetCurTable(apParser));
				break;
			}
			default:
				break;
		}

        yaml_event_delete(&event);
    }
}


int YamlCfgParser_ParseFile(struct CfgParser * apParser, const char *apFileName)
{
    FILE *source = fopen(apFileName, "r");

	if (source == NULL)
		return -1;

	assert(apParser != NULL);
    yaml_parser_set_input_file(&apParser->parser, source);
	process_yaml_parser(apParser);

    fclose(source);
	return 0;
}

int YamlCfgParser_ParseString(struct CfgParser * apParser, const char *apString)
{
	assert(apString != NULL);
	assert(apParser != NULL);

    yaml_parser_set_input_string(&apParser->parser, (const unsigned char*)apString, strlen(apString));
	process_yaml_parser(apParser);

	return 0;
}


#ifdef TEST_YAML

#define PASS()     printf("Test:%-40s passed\n", __FUNCTION__);
#define FAIL(msg)  printf("Test:%-40s failed at line:%d, assertion:%s\n", __FUNCTION__, __LINE__, msg);
#define FAIL_IF(e) do { if (e) {FAIL(#e); return;} }while(0)


void Test_Create(void)
{
	struct CfgParser * parser;

	parser = YamlCfgParser_Create();

	FAIL_IF (parser == NULL);
	FAIL_IF (parser->deepth != 0);
	FAIL_IF (parser->stack[0] != NULL);

	PASS();
}

void Test_SetTable(void)
{
	struct CfgParser * parser;
	struct CfgTable table;
	struct CfgItem	items[2] =
	{
		{TYPE_INT, "TestValue0", 0},
		{TYPE_INT, "TestValue1", 0},
	};

	table.count = 2;
	table.items = items;

	parser = YamlCfgParser_Create();

	YamlCfgParser_SetTable(parser, &table);

	FAIL_IF (parser->deepth != 1);
	FAIL_IF (parser->stack[0] != &table);

	PASS();
}

void Test_ParseStringSimpleInt(void)
{
	struct CfgParser * parser;
	struct CfgTable table;
	int v0 = -1, v1 = -1;
	struct CfgItem	items[2] =
	{
		{TYPE_INT, "TestValue0", &v0},
		{TYPE_INT, "TestValue1", &v1},
	};

	table.count = 2;
	table.items = items;

	parser = YamlCfgParser_Create();
	YamlCfgParser_SetTable(parser, &table);
	YamlCfgParser_ParseString(parser, "TestValue0:  0\nTestValue1:  1\n");

	FAIL_IF (v0 != 0);
	FAIL_IF (v1 != 1);

	PASS();
}

void Test_ParseStringSimpleString(void)
{
	struct CfgParser * parser;
	struct CfgTable table;
	char s0[100], s1[100];
	struct CfgItem	items[2] =
	{
		{TYPE_STRING, "TestValue0", &s0},
		{TYPE_STRING, "TestValue1", &s1},
	};

	table.count = 2;
	table.items = items;

	parser = YamlCfgParser_Create();
	YamlCfgParser_SetTable(parser, &table);
	YamlCfgParser_ParseString(parser, "TestValue0:  012\nTestValue1:  abc\n");

	FAIL_IF (strcmp(s0, "012"));
	FAIL_IF (strcmp(s1, "abc"));

	PASS();
}

void Test_ParseStringSimpleBoolTrueFalse(void)
{
	struct CfgParser * parser;
	struct CfgTable table;
	bool b0 =false, b1 = true;
	struct CfgItem	items[2] =
	{
		{TYPE_BOOL, "TestValue0", &b0},
		{TYPE_BOOL, "TestValue1", &b1},
	};

	table.count = 2;
	table.items = items;

	parser = YamlCfgParser_Create();
	YamlCfgParser_SetTable(parser, &table);
	YamlCfgParser_ParseString(parser, "TestValue0:  true\nTestValue1:  false\n");

	FAIL_IF (b0 != true);
	FAIL_IF (b1 != false);

	PASS();
}

void Test_ParseStringSimpleBoolYesNo(void)
{
	struct CfgParser * parser;
	struct CfgTable table;
	bool b0 =false, b1 = true;
	struct CfgItem	items[2] =
	{
		{TYPE_BOOL, "TestValue0", &b0},
		{TYPE_BOOL, "TestValue1", &b1},
	};

	table.count = 2;
	table.items = items;

	parser = YamlCfgParser_Create();
	YamlCfgParser_SetTable(parser, &table);
	YamlCfgParser_ParseString(parser, "TestValue0:  yes\nTestValue1:  no\n");

	FAIL_IF (b0 != true);
	FAIL_IF (b1 != false);

	PASS();
}

void Test_ParseStringSimpleTable(void)
{
	struct CfgParser * parser;
	struct CfgTable table, table0;
	bool b0 = false;

	struct CfgItem	items0[1] =
	{
		{TYPE_TABLE, "TestTable0", &table0},
	};
	struct CfgItem	items1[1] =
	{
		{TYPE_BOOL, "TestValue0", &b0},
	};

	table0.count = 1;
	table0.items = items1;

	table.count = 1;
	table.items = items0;

	parser = YamlCfgParser_Create();
	YamlCfgParser_SetTable(parser, &table0);
	YamlCfgParser_ParseString(parser, "TestTable0:\n    - TestValue0:  yes\n");

	FAIL_IF (b0 != true);

	PASS();
}

void Test_ParseStringTwoTables(void)
{
	struct CfgParser * parser;
	char * string = \
		"TestTable0:\n"  \
		"    - TestValue0:  yes\n" \
		"TestTable1:\n"  \
		"    - TestValue1:  no\n";

	struct CfgTable table, table0, table1;
	bool b0 = false, b1 = true;

	struct CfgItem	items[2] =
	{
		{TYPE_TABLE, "TestTable0", &table0},
		{TYPE_TABLE, "TestTable1", &table1},
	};
	struct CfgItem	items0[1] =
	{
		{TYPE_BOOL, "TestValue0", &b0},
	};
	struct CfgItem	items1[1] =
	{
		{TYPE_BOOL, "TestValue1", &b1},
	};

	table1.count = 1;
	table1.items = items1;

	table0.count = 1;
	table0.items = items0;

	table.count = 2;
	table.items = items;

	parser = YamlCfgParser_Create();
	YamlCfgParser_SetTable(parser, &table);
	YamlCfgParser_ParseString(parser, string);

	FAIL_IF (b0 != true);
	FAIL_IF (b1 != false);

	PASS();
}

void Test_ParseStringTestParams(void)
{
	char * string = \
		"--- \n"\
		"FixtureParam: \n"\
		"   - NAND:       MT29F64G08CBAAA \n"\
		"     gSLC:       false \n"\
		"     Connection: Internal_Dual_1_1 \n"\
		"     Feature:    Large_Multi \n"\
		"     LiveDebug:  false \n"\
		"--- \n"
		"TestParam: \n"
		"    - StressTime:  1       # in second \n"\
		"    - RPStressTime: 2 \n"\
		"    - StressLevel: 3 \n"\
		"    - RPStressLevel: 4 \n"\
		"    - SnapShotCmdCnt: 5      # -1 means no snapshot \n"\
		"    - StressCtrlBOMode: 6    # 0 means device control, otherwise means host gControlBuffer \n"\
		"    - StressCtrlBOInterval: 7   # how many command per host maintain \n";

	bool Simu_gSLC                     = true;
	char default_nand_name[128]        = "MT29F64G08CBABA";
	char default_connection[128]       = "Internal_Dual_1_2";
	char default_feature[128]          = "Small_Multi";
	bool default_live_debug_mode       = true;

	int  default_stress_time           = 0;
	int  default_rp_stress_time        = 0;

	int  default_stress_level          = 0;
	int  default_rp_stress_level       = 0;
	int  default_snapshot_cmd_cnt      = 0;
	int  default_stress_ctrl_BO_mode   = 0;
	int  default_stress_ctrl_BO_interval = 0;

	struct CfgParser * parser;
	struct CfgTable table, table0, table1;

	struct CfgItem	RootTableItems[] =
	{
		SETUP_TABLE_ITEM(TYPE_TABLE, "FixtureParam", &table0),
		SETUP_TABLE_ITEM(TYPE_TABLE, "TestParam",    &table1),
	};
	struct CfgItem	FixtureParamItems[] =
	{
		SETUP_TABLE_ITEM(TYPE_STRING, "NAND", 			&default_nand_name),
		SETUP_TABLE_ITEM(TYPE_STRING, "Connection", 	&default_connection),
		SETUP_TABLE_ITEM(TYPE_STRING, "Feature", 		&default_feature),

		SETUP_TABLE_ITEM(TYPE_BOOL,   "gSLC", 			&Simu_gSLC),
		SETUP_TABLE_ITEM(TYPE_BOOL,   "LiveDebug",		&default_live_debug_mode),
	};
	struct CfgItem	TestParamItems[] =
	{
		SETUP_TABLE_ITEM(TYPE_INT, 		"StressTime", 			&default_stress_time),
		SETUP_TABLE_ITEM(TYPE_INT, 		"RPStressTime", 		&default_rp_stress_time),
		SETUP_TABLE_ITEM(TYPE_INT, 		"StressLevel", 			&default_stress_level),
		SETUP_TABLE_ITEM(TYPE_INT, 		"RPStressLevel", 		&default_rp_stress_level),
		SETUP_TABLE_ITEM(TYPE_INT, 		"SnapShotCmdCnt", 		&default_snapshot_cmd_cnt),
		SETUP_TABLE_ITEM(TYPE_INT, 		"StressCtrlBOMode", 	&default_stress_ctrl_BO_mode),
		SETUP_TABLE_ITEM(TYPE_INT, 		"StressCtrlBOInterval", &default_stress_ctrl_BO_interval),
	};

	SETUP_TABLE(table, RootTableItems);
	SETUP_TABLE(table0, FixtureParamItems);
	SETUP_TABLE(table1, TestParamItems);


	parser = YamlCfgParser_Create();
	YamlCfgParser_SetTable(parser, &table);
	YamlCfgParser_ParseString(parser, string);

	FAIL_IF (strcmp(default_nand_name, "MT29F64G08CBAAA"));
	FAIL_IF (strcmp(default_connection, "Internal_Dual_1_1"));
	FAIL_IF (strcmp(default_feature, "Large_Multi"));
	FAIL_IF (Simu_gSLC != false);
	FAIL_IF (default_live_debug_mode != false);

	FAIL_IF (default_stress_time != 1);
	FAIL_IF (default_rp_stress_time != 2);
	FAIL_IF (default_stress_level != 3);
	FAIL_IF (default_rp_stress_level != 4);
	FAIL_IF (default_snapshot_cmd_cnt != 5);
	FAIL_IF (default_stress_ctrl_BO_mode != 6);
	FAIL_IF (default_stress_ctrl_BO_interval != 7);

	PASS();
}



void main(void)
{
	Test_Create();
	Test_SetTable();
	Test_ParseStringSimpleInt();
	Test_ParseStringSimpleString();
	Test_ParseStringSimpleBoolTrueFalse();
	Test_ParseStringSimpleBoolYesNo();
	Test_ParseStringSimpleTable();
	Test_ParseStringTwoTables();
	Test_ParseStringTestParams();
}

#endif
