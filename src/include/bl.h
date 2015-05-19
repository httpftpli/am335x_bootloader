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

#include "pf_bootloader.h"
#include <time.h>


/******************************************************************************
**                    Global Variable Declarations
*******************************************************************************/

extern unsigned int entryPoint;
extern unsigned int DspEntryPoint;


/******************************************************************************
**                    Global Function Declarations
*******************************************************************************/




typedef struct __fileinfo {
    char filename[13];
    time_t modtime;
} MYFILEINFO;


extern void BootAbort(void);
extern void Entry (void);

#endif
