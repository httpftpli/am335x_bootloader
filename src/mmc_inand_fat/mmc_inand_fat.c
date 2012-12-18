#include "mmcsd_proto.h"
#include "platform.h"
#include "edma_event.h"
#include "soc_AM335x.h"
#include "interrupt.h"
#include "uartStdio.h"
#include "hs_mmcsd.h"
#include "string.h"
#include "delay.h"
#include "cache.h"
#include "edma.h"
#include "mmu.h"
#include "debug.h"
#include "ff.h"
//#include "fatfs.h"
#include "ff_ext.h"
#ifdef MMCSD_PERF
#include "perf.h"
#endif

/******************************************************************************
**                      INTERNAL MACRO DEFINITIONS
*******************************************************************************/


/* Global data pointers */
#define HSMMCSD_DATA_SIZE              512


#define SIZEOFARRYA                     2*1024

/* SD card info structure */
mmcsdCardInfo card0, card1;

/* SD Controller info structure */
mmcsdCtrlInfo mmcsdctr[2];

FATFS fs[2];


/******************************************************************************
**                      VARIABLE DEFINITIONS
*******************************************************************************/

#ifdef __IAR_SYSTEMS_ICC__
#pragma data_alignment=SOC_CACHELINE_SIZE
unsigned char data[HSMMCSD_DATA_SIZE];

#elif defined(__TMS470__)
#pragma DATA_ALIGN(data, SOC_CACHELINE_SIZE);
unsigned char data[HSMMCSD_DATA_SIZE];

#elif defined(gcc)
unsigned char data[HSMMCSD_DATA_SIZE]
                    __attribute__ ((aligned (SOC_CACHELINE_SIZE)))= {0};
#else
#error "Unsupported Compiler. \r\n"
#endif


/******************************************************************************
**                          FUNCTION DEFINITIONS
*******************************************************************************/

volatile unsigned  int mmcreadfinish = 0,mmcwritefinish = 0;


static void HSMMCSDXferSetup(mmcsdCtrlInfo *ctrl, unsigned char rwFlag, void *ptr, 
                             unsigned int blkSize, unsigned int nBlks)
{
   unsigned int index = HSMMCSDBaseAddrToIndex(ctrl->memBase);
   unsigned int dmachannelr,dmachannelw;
   
   if (0 == index) {
      dmachannelr = EDMA3_CHA_MMCSD0_RX;
      dmachannelw = EDMA3_CHA_MMCSD0_TX;
   }else{
      dmachannelr = EDMA3_CHA_MMCSD1_RX;
      dmachannelw = EDMA3_CHA_MMCSD1_TX;
   }
   //g_Mmcsd[index].fgXferComp = 0;

   if (rwFlag == 1)
    {
        mmcreadfinish = 0;
        EDMARegisterHandler(dmachannelr,NULL);
        EDMARequestXferWithBufferEntry(EDMA3_TRIG_MODE_EVENT,dmachannelr,
                                       ctrl->memBase + MMCHS_DATA,
                                       (unsigned int)ptr,1,32,blkSize,
                                       nBlks,dmachannelr);
    }
    else
    {
        mmcwritefinish = 0;
        EDMARegisterHandler(dmachannelw,NULL);
        EDMARequestXferWithBufferEntry(EDMA3_TRIG_MODE_EVENT,dmachannelw,
                                       ctrl->memBase + MMCHS_DATA,(unsigned int)ptr,
                                       0,32,blkSize,nBlks,dmachannelw);
    }
    ctrl->dmaEnable = 1;                                           
}


/*
 * Check command status.will block
 */
static unsigned int HSMMCSDCmdStatusGet(mmcsdCtrlInfo *ctrl) {

   unsigned int index = HSMMCSDBaseAddrToIndex(ctrl->memBase);
   
   while ((g_Mmcsd[index].fgCmdComp == 0) && (g_Mmcsd[index].fgCmdTimeout == 0));
   if (g_Mmcsd[index].fgCmdComp == 1) {
      g_Mmcsd[index].fgCmdComp = 0;
      return 1;
   }
   if (g_Mmcsd[index].fgCmdTimeout == 1) {
      g_Mmcsd[index].fgCmdTimeout = 0;
      return 0;
   }
   return 0;
}


/*
 * Check data transfer finish .will block
 */

static unsigned int HSMMCSDXferStatusGet(mmcsdCtrlInfo *ctrl)
{
    unsigned int index = HSMMCSDBaseAddrToIndex(ctrl->memBase);
    while ((g_Mmcsd[index].fgXferComp == 0)&&(g_Mmcsd[index].fgDataTimeout == 0));
    if (g_Mmcsd[index].fgXferComp == 1) {
       g_Mmcsd[index].fgXferComp = 0; 
       return 1;
    } 
    if (g_Mmcsd[index].fgDataTimeout ==1) {
       g_Mmcsd[index].fgDataTimeout = 0;
       return 0;
    }     
    return 0;
}


int main(void)
{
    volatile unsigned int i = 0;
    volatile unsigned int initFlg = 1;
    unsigned int status;
    platformInit(); 
    DelayTimerSetup();
    
   MMCSDP_CtrlInfoInit(&mmcsdctr[0],SOC_MMCHS_0_REGS,96000000,24000000,MMCSD_BUSWIDTH_4BIT,0,
                      &card0, HSMMCSDXferSetup,
                      HSMMCSDCmdStatusGet,HSMMCSDXferStatusGet);
   MMCSDP_CtrlInfoInit(&mmcsdctr[1],SOC_MMCHS_1_REGS,96000000,24000000,MMCSD_BUSWIDTH_4BIT,0,
                      &card1, HSMMCSDXferSetup,
                      HSMMCSDCmdStatusGet,HSMMCSDXferStatusGet);
   MMCSDP_CtrlInit(&mmcsdctr[0]);
   MMCSDP_CtrlInit(&mmcsdctr[1]);
   return 0 ;

 

   // FATFS fs1,fs2;
   // f_mount(0,&fs1);
   // f_mount(3,&fs2);
   
   // f_copy("3:/MLO","0:/MLO",buf,sizeof buf);
   // return  0; 
   
}



