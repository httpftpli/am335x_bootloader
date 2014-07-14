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
