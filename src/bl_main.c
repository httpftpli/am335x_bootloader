#include "bl_copy.h"
#include "bl_platform.h"
#include "bl.h"
#include "type.h"
#include "platform.h"



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



FATFS inandfs ;
BOOL isIDok;

extern BOOL isIDvailable(void);
extern void hmiInit(void);
extern void guiExec(void);
extern void probUdisk(void);
extern void displayUdisk(void);
extern void probIdisk_display(void);
extern void hmishow(void);
extern void shortcuthandler(int keycode);

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
   registKeyHandler(shortcuthandler);
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
   }
}

