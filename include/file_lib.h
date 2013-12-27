/**********************************************************
 *     Module Name : file_lib.h
 *     Description : basic file function declare.
 *
 **********************************************************/

/**********************************************************
 *     Kaifeng Zhuang@2013.07.27
 *     Description :  Initial create
 **********************************************************/
#ifndef FILE_LIB_H
#define FILE_LIB_H

#define   SUCCESS    1
#define   FAILURE    0

#if defined(linux) || defined(__linux__)
void   init_keyboard(void);
void   close_keyboard(void);
int    kbhit(void);
int    readch(void);
#else

#include <conio.h>

#define init_keyboard()
#define close_keyboard()
#define kbhit  _kbhit
#define readch _getch
#endif


#if defined(linux) || defined(__linux__)

#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>

#define mkdir(x) mkdir(x, S_IRWXU|S_IRWXG|S_IRWXO)
#define RdFlag "r"
#define WrFlag "w"

#define isFileExist(f)  (access(f, 0) == 0)
#define isDirExist(d)   (access(d, 0) == 0)

#define getch getchar
#define strupr STRUPR

#define COPY  "cp"
#define XCOPY  "cp -r"
#define RMDIR "rm -rf"
#define RM    "rm -f"

#else
#include <io.h>

#define RdFlag "rb"
#define WrFlag "wb"

#define isFileExist(f)  (_access(f, 0) == 0)
#define isDirExist(d)   (_access(d, 0) == 0)

#define COPY  "copy"
#define XCOPY  "xcopy"
#define RMDIR "rmdir /s /q"
#define RM    "del /q"

#endif

extern char* ConvertPath(char *path);
extern int TestDeleteDir(const char *dir);
extern int TestDeleteFile(const char *file);
extern int TestCopyDir(const char *dest, const char *src);
extern int TestCopyFile(const char *dest, const char *src);

#endif
