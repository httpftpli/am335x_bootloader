
#ifndef __BL_COPY_H__
#define __BL_COPY_H__
#include "type.h"
#include "ff.h"


#define APP_UNAVAILABLE    -1
#define APP_COPY_ERROR     -2
#define APP_SIZE_ERROR     -3
#define APP_ENTRY_ERROR    -4


extern int bootCopy();
extern BOOL burn(const TCHAR *filePath);
extern void jumptoApp();


#endif
