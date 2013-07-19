#include "uartStdio.h"
#include "bl_copy.h"
#include "bl_platform.h"
#include "bl.h"
#include "type.h"
#include "pf_usbmsc.h"
#include "soc_AM335X.h"
#include "interrupt.h"
#include "pf_int.h"
#include "pf_tsc.h"
#include "pf_key_touchpad.h"
#include "pf_lcd.h"
#include "debug.h"



unsigned int entryPoint = 0;
unsigned int DspEntryPoint = 0;


static BOOL forceEnterBoot(){
  atomicClear(&g_keyPushed);
  unsigned int val = 0;
  unsigned int timemark = TimerTickGet();
  while (1) {
     if(atomicTestClear(&g_keyPushed)&& (g_keycode==KEY_QUK)){
        val++;
     }
     if ((TimerTickGet()-timemark)>400) {
        break;
     }
  }
  if (val>0) {
     return TRUE;
  }
  return FALSE;
}


BOOL enterTouchCal = 0;
static void bootkeyhandler(int keycode){
  static int cnt = 0;
  if(KEY_OK==keycode){
    cnt++;
    if(cnt == 4){
      enterTouchCal = 1;
      cnt = 0;
    }
  }else{
    cnt = 0;
  }
}

FATFS inandfs ;
BOOL isIDok;

extern BOOL isIDvailable();
extern void hmiInit();
extern void guiExec(void);
extern void probUdisk();
extern void displayUdisk();
extern void probIdisk_display();
extern void hmishow();

int main(void) {
   BlPlatformConfig();
   UARTPuts("Minde bootloader \n\r ", -1);
   int val = 0;
   isIDok = isIDvailable();
   if(!isIDok){
      UARTPuts("ID error...\r\n\n", -1);
      goto BOOTLOADER;
   }
   if (forceEnterBoot()){
      goto BOOTLOADER;
   }
#ifndef INNERBOOT
   val = bootCopy();
   if(0 == val){
      jumptoApp();
   }
#endif
   if (APP_COPY_ERROR == val) {
      UARTPuts("Application copy error...\r\n\n", -1);
      goto BOOTLOADER;
   }
   if (APP_SIZE_ERROR == val) {
      UARTPuts("Application size error...\r\n\n", -1);
      goto BOOTLOADER;
   }
   if (APP_ENTRY_ERROR==val) {
      UARTPuts("Application entry address error...\r\n\n", -1);
      goto BOOTLOADER;
   }
   if (APP_UNAVAILABLE == val) {
      UARTPuts("Application unavailable . Enter to bootloader...\r\n\n", -1);
      goto BOOTLOADER;
   }
   
BOOTLOADER:
  registKeyHandler(bootkeyhandler);
  LCDRasterStart();
  LCDBackLightON();
  TouchCalibrate(0);
  hmiInit();
  f_mount(0, &inandfs);
  while (1) {
         guiExec();
         usbMscProcess();
         probUdisk();
         displayUdisk();
         probIdisk_display();
         if(enterTouchCal==1){
            while (!TouchCalibrate(1)) ;  
            hmishow();
            enterTouchCal = 0;
         }
  }
}

