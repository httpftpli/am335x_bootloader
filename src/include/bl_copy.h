
#ifndef __BL_COPY_H__
#define __BL_COPY_H__
#include "type.h"
#include "ff.h"


#define APP_UNAVAILABLE    -1
#define APP_COPY_ERROR     -2
#define APP_SIZE_ERROR     -3
#define APP_ENTRY_ERROR    -4


#define APP_HEAD_SECTOR         770
#define APP_BEGIN_SECTOR      771
#define APP_MAX_SIZE          20*1024*1024
#define APP_ENTRY             0x80000000
#define APP_MAGIC_OK          0x5555aaaa
#define APP_MAGIC_NO          0xaaaa5555




extern int bootCopy();
extern BOOL burn(const TCHAR *filePath);
extern void jumptoApp();


#endif
