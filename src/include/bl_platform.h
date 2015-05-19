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

#include "pf_platform.h"

/******************************************************************************
**                        Macro Definitions
*******************************************************************************/

#define IMAGE_SIZE                         50 * 1024 /* Max size */
#define DDR_START_ADDR                     0x80000000
#define UART_BASE                          SOC_UART_0_REGS
#define  MPUPLL_M_720                      720
#define  MPUPLL_M_600                      600


/* Set of config parameters */

#define     OPP_SR_TURBO                     (0x0)
#define     OPP_120                          (0x1)
#define     OPP_100                          (0x2)
#define     OPP_50                           (0x3)

#define  OPP OPP_120    //OPP_SR_TURBO
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




//define keycode

#define KEY_0        	0
#define KEY_1        	1
#define KEY_2        	2
#define KEY_3        	3
#define KEY_4       	4
#define KEY_5        	5
#define KEY_6        	6
#define KEY_7        	7
#define KEY_8        	8
#define KEY_9        	9
#define KEY_A		10
#define KEY_B		11
#define KEY_C		12
#define KEY_D		13
#define KEY_E		14
#define KEY_F		15

#define KEY_UP      16
#define KEY_DOWN    17
#define KEY_LEFT    18
#define KEY_RIGHT   19
#define KEY_PU		20
#define KEY_PD		21

#define KEY_OK      22
#define KEY_ESC     23
#define KEY_F1		24
#define KEY_F2		25
#define KEY_F3		26
#define KEY_F4		27
#define KEY_F5		28
#define KEY_F6		29
#define KEY_CE		30
#define KEY_POP		31

#define KEY_USB		32
#define KEY_MEM		33
#define KEY_QUK		34
#define KEY_ZF		35
#define KEY_POINT	36

#define KEY_RE1		37
#define KEY_RE2		38
#define KEY_RE3		39

#define KEY_Hand    53
#define KEY_Oil	    54
#define KEY_FUN		55
#define KEY_SAdd   	56
#define KEY_SDec 	57
#define KEY_Zero	58
#define KEY_LAM1  	59
#define KEY_LAM2	60
#define KEY_Moto	61
#define KEY_Stop	62
#define KEY_Start	63
#define KEY_Power	64
#define KEY_OIL220	65
#define KEY_BAK		66

#define KEY_F7      67
#define KEY_F8      68
#define KEY_F9      69

#define KEY_NO		0xff

/******************************************************************************
**                    External Function Declararions
*******************************************************************************/

extern void BlPlatformConfigPostBoot(void);
extern void BlPlatformConfig(void);
extern void BlPlatformMMCSDSetup(void);
extern void BL_PLATFORM_MMCSDSetup(void);
extern unsigned int BlPlatformMMCSDImageCopy();



#endif /* _BL_PLATFORM_H__ */

