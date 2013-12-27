/**********************************************************
*     Module Name : file_lib.c
*     Description : basic file function implement
**********************************************************/

/*********************************************************
*     Kaifeng Zhuang @ 2013.07.27
*     Description  : Initial create
**********************************************************/

/*********************************************************
*  Include section
*  Add all #includes here
*
**********************************************************/
#include <stdio.h>
#include <stdlib.h>

#include "file_lib.h"
/*********************************************************
 * Defines section
 * Add all #defines here
 *
 *********************************************************/
 /*********************************************************
 * Global variables section
 * Add all globalvariables here
 *
 *********************************************************/

/*********************************************************
 * Function Prototype Section
 * Add prototypes for all functions called by this
 * module, with the exception of runtime routines.
 *
 *********************************************************/

char* ConvertPath(char *path)
{
#if defined(linux) || defined(__linux__)
    char c, *p = path, *n = path;
    while(c = *path)
    {
        if (c == '\\')
        {
            *p = '/';
        }
        else
        {
            *p = *path;
        }
        p++;
        path++;
    }
    *p = '\0';
    return n;
#else
    return path;
#endif
}

int TestCopyFile(const char *dest, const char *src)
{
    char cmd[1024];

    sprintf(cmd, "%s %s %s 1>nul 2>nul", COPY, src, dest);
    ConvertPath(cmd);

    return system(cmd);
}

int TestCopyDir(const char *dest, const char *src)
{
    char cmd[1024];

    sprintf(cmd, "%s %s %s 1>nul 2>nul", XCOPY, src, dest);
    ConvertPath(cmd);

    return system(cmd);
}

int TestDeleteDir(const char *dir)
{
#if 1
    char cmd[1024];

    sprintf(cmd, "%s %s 1>nul 2>nul", RMDIR, dir);

    return system(cmd);
#else
    return _rmdir(dir);
#endif
}

int TestDeleteFile(const char *file)
{
#if 0
    char cmd[1024];

    sprintf(cmd, "%s %s 1>nul 2>nul", RM, file);

    return system(cmd);
#else
    return remove(file);
#endif
}


#if defined(linux) || defined(__linux__)

#include "file_lib.h"
#include <termios.h>
#include <unistd.h>   // for read()

static struct termios initial_settings, new_settings;
static int peek_character = -1;

void init_keyboard(void)
{
    tcgetattr(0,&initial_settings);
    new_settings = initial_settings;
    new_settings.c_lflag &= ~ICANON;
    new_settings.c_lflag &= ~ECHO;
    new_settings.c_lflag &= ~ISIG;
    new_settings.c_cc[VMIN] = 1;
    new_settings.c_cc[VTIME] = 0;
    tcsetattr(0, TCSANOW, &new_settings);
}

void close_keyboard(void)
{
    tcsetattr(0, TCSANOW, &initial_settings);
}

int kbhit(void)
{
    unsigned char ch;
    int nread;

    if (peek_character != -1) return 1;
    new_settings.c_cc[VMIN]=0;
    tcsetattr(0, TCSANOW, &new_settings);
    nread = read(0,&ch,1);
    new_settings.c_cc[VMIN]=1;
    tcsetattr(0, TCSANOW, &new_settings);
    if(nread == 1)
    {
        peek_character = ch;
        return 1;
    }
    return 0;
}

int readch(void)
{
    char ch;

    if(peek_character != -1)
    {
        ch = peek_character;
        peek_character = -1;
        return ch;
    }
    read(0,&ch,1);
    return ch;
}

#endif




