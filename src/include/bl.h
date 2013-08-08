/**
 * \file  bl.h
 *
 * \brief This file defines boot macros and objects
 */


/******************************************************************************
**                        Macro Definitions 
*******************************************************************************/
#ifndef __BL_H__
#define __BL_H__

#define BURNAPP_READERROR       -1
#define BURNAPP_FILE_ERROR      -2
#define BURNAPP_WRITEERROR      -3

typedef struct __appsection{
   unsigned int imageaddr;
   unsigned int imageSize;
   unsigned int imageCheck;
   char imageRevPrefix;
   char dumy;
   unsigned short imageMainRev;
   unsigned short imageMidRev;
   unsigned short imageMinRev;
}APPSETCTION;

typedef struct __apppackhead{
   unsigned int nsection;
   APPSETCTION appsec1;
   APPSETCTION appsec2;
}APPPACKHEAD;


typedef struct app_header_
{
   unsigned int magic;
   unsigned int nsection;
   APPSETCTION appsec1;
   APPSETCTION appsec2;
}APPHEADER;



/******************************************************************************
**                    Global Variable Declarations
*******************************************************************************/

extern unsigned int entryPoint;
extern unsigned int DspEntryPoint;


/******************************************************************************
**                    Global Function Declarations
*******************************************************************************/

extern void BootAbort(void);
extern void Entry (void);

#endif
