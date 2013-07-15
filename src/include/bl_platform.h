/**
 * \file bl_platform.h
 *
 * \brief This file exports the APIs used for configuring devices
 *        required during boot
 *  
 */



#ifndef _BL_PLATFORM_H__
#define _BL_PLATFORM_H__

#include "soc_AM335x.h"

#include "platform.h"
 
/******************************************************************************
**                        Macro Definitions 
*******************************************************************************/

#define IMAGE_SIZE                         50 * 1024 /* Max size */
#define DDR_START_ADDR                     0x80000000
#define UART_BASE                          SOC_UART_0_REGS
#define  MPUPLL_M_720                      720
#define  MPUPLL_M_600                      600


/* Set of config parameters */

#define  OPP OPP_SR_TURBO
/* 
**Setting the CORE PLL values at OPP100:
** OSCIN = 24MHz, Fdpll = 2GHz
** HSDM4 = 200MHz, HSDM5 = 250MHz
** HSDM6 = 500MHz
*/
#define COREPLL_M                          1000
#define COREPLL_N                          23
#define COREPLL_HSD_M4                     10
#define COREPLL_HSD_M5                     8
#define COREPLL_HSD_M6                     4

/* Setting the  PER PLL values at OPP100:
** OSCIN = 24MHz, Fdpll = 960MHz
** CLKLDO = 960MHz, CLKOUT = 192MHz
*/
#define PERPLL_M                           960
#define PERPLL_N                           23
#define PERPLL_M2                          5

#if  (OPP==OPP_SR_TURBO)
     /* Setting the MPU (Cortex) clock at 720MHz */
     #define MPUPLL_M                      MPUPLL_M_720
#elif  (OPP==OPP_120)
    /* Setting the MPU (Cortex) clock at 600MHz */
     #define MPUPLL_M                      MPUPLL_M_600
#endif

#define MPUPLL_N                           23
#define MPUPLL_M2                          1

 /* Setting the Display CLKOUT at 300MHz independently
 ** This is required for full clock 150MHz for LCD
 ** OSCIN = 24MHz, Fdpll = 300MHz
 ** CLKOUT = 150MHz
 */
#define DISPLL_M                           25
#define DISPLL_N                           3
#define DISPLL_M2                          1

/* 
**Setting the DDR2 frequency to 266MHz
*/
#define DDRPLL_M		           266
#define DDRPLL_N		           23
#define DDRPLL_M2		           1  //post-divider

/******************************************************************************
**                    External Function Declararions 
*******************************************************************************/

extern void BlPlatformConfigPostBoot(void);
extern void BlPlatformConfig(void);
extern void BlPlatformMMCSDSetup(void);
extern void BL_PLATFORM_MMCSDSetup(void);
extern unsigned int BlPlatformMMCSDImageCopy(); 



#endif /* _BL_PLATFORM_H__ */

