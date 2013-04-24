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
#include "dcan.h"
#include "ff_ext.h"
#ifdef MMCSD_PERF
#include "perf.h"
#endif
#include "gpio_v2.h"
#include "utf8.h"
#include "gui.h"
#include "pf_lcd.h"
#include "pf_usbmsc.h"
#include "pf_platform_cfg.h"
#include "font.h"
#include "../include/cpld.h"

/******************************************************************************
**                      INTERNAL MACRO DEFINITIONS
*******************************************************************************/


/* Global data pointers */
#define HSMMCSD_DATA_SIZE              512


#define SIZEOFARRYA                     2*1024

#define DCAN_IN_CLK                       (24000000u)

#define RX8025_ADDR                     0x32
#define EEPROM_ADDR                     0x50
 
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



volatile unsigned int ack = 0;

static void canrcvhandler(unsigned int index,CAN_FRAME *frame){
      ack = 1;
}


unsigned int bufmmc1[1024*1024],bufmmc2[1024*1024];

char buffer[1024];
unsigned int fontloaded = 0;

#define FONT_LOAD_ADDR        0x80000000+128*1024*1024


void udiskread(){
   static unsigned int error=0;
   int r;
   if((g_usbMscState == USBMSC_DEVICE_READY)&&(fontloaded == 0)&&(error == 0)){
     
     TCHAR path[100];
     UTF8toUCS2_string("2:/simkai_U16.bin",path,0);
     r = loadFont(path,  FONT_LOAD_ADDR);
     if (r!=0) {
        error = 1;
        return;
     }
     fontloaded = 1;   
   }
}


void drawText(){
   static unsigned int textdrawed = 0;
   if ((textdrawed == 0)&&(fontloaded == 1)) {
      textdrawed = 1;
      Dis_DrawText("杭州欢迎你",30,30,C_White,C_TRANSPARENT);
      Dis_DrawText("杭州はあなたを歓迎する",30,60,C_White,C_TRANSPARENT);
      Dis_DrawText("Welcome to Hangzhou",30,90,C_White,C_TRANSPARENT);
      Dis_DrawText("Hangzhou Hoşgeldiniz",30,120,C_White,C_TRANSPARENT);
      Dis_DrawText("wiwiwiwiwi",30,150,C_White,C_TRANSPARENT);
   }
}

volatile unsigned short cpldbuffer[100];

int main(void)
{

   volatile unsigned int initFlg = 1;
   unsigned int status;
 
   platformInit();
   

      
      SF_PWM = 100;
      SF_EN = 0;
      while(1);
     



   while(1){
   for(int i=0;i<16;i++){
      ((unsigned short *)(1<<24))[i] =  i;
   }
   for(int i=0;i<16;i++){    
      cpldbuffer[i] =  ((unsigned short *)(1<<24))[i]; 
   }
   }
   
   
     
   LCDRasterStart();	//lcd 
    

   MMCSDP_CtrlInfoInit(& mmcsdctr[0], SOC_MMCHS_0_REGS, 96000000, 24000000, MMCSD_BUSWIDTH_4BIT, 0,
                       &card0, NULL,NULL, NULL);
   MMCSDP_CtrlInit(& mmcsdctr[0]);
   MMCSDP_CardInit(mmcsdctr,MMCSD_CARD_AUTO);
   

   for (int i=0;i<sizeof(bufmmc1)/4;i++) {
      bufmmc1[i]=0x55555555;
   }
   for (int i=0;i<sizeof(bufmmc2)/4;i++) {
      bufmmc2[i]=0;
   }
   
   MMCSDP_Write(mmcsdctr,bufmmc1,0,1024);
   MMCSDP_Read(mmcsdctr,bufmmc2,0,1024);
   for(int i=0;i<512*1024/4;i++){
     if(bufmmc2[i]!=bufmmc1[1])
        while(1);
   }
   
   while(1);
   while (1) {
      usbMscProcess();
      udiskread();
      drawText();
   }
     
}








/*void mainLoop(0){
      LCDSwapContex();  
      LcdClear(C_Red);
      LCDSwapFb(); 
      
      LCDSwapContex();
      LcdClear(C_Green);
      LCDSwapFb();  
}*/

