/**********************************************************
*     Module Name : memory_file.h
*     Description : memory file declare
**********************************************************/

/*********************************************************
*     Kaifeng Zhuang @ 2013.08.01
*     Description  : Initial create
**********************************************************/
#ifndef _MEMORY_FILE_H
#define _MEMORY_FILE_H

/*********************************************************
*  Include section
*  Add all #includes here
*
**********************************************************/
#include "file_access.h"

extern uint8_t MemoryFileDestroy(FILE_HANDLE_S* pFileHandle);
extern FILE_HANDLE_S* MemoryFileCreate(char* pFileName, uint32_t aFileSize);

#endif
