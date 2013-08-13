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
   unsigned int imageaddr;         //地址，相对于打包后的文件头
   unsigned int imageSize;         //大小，单位：字节
   unsigned int imageCheck;        //暂时不用
   char imageRevPrefix;            //版本号前缀
   char dumy;                      //填充字符，无意义，为使结构体4字节对齐
   unsigned short imageMainRev;    //主版本号
   unsigned short imageMidRev;     //中间版本号
   unsigned short imageMinRev;     //小版本号
}APPSETCTION;

typedef struct __apppackhead{
   //被打包程序标记，位标记，第0为表示主程序，第1为表示背包，如0x03表示被打包的有主程序和背包
   unsigned int secflag;                            
   APPSETCTION appsec1; //主程序APPSETCTION
   APPSETCTION appsec2; //背包APPSETCTION
}APPPACKHEAD;


typedef struct app_header_
{
   unsigned int magic;
   unsigned int secflag;
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
