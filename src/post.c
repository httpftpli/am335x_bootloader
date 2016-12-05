#include "pf_platform.h"
#include "pf_bootloader.h"
#include "string.h"
#include <stdio.h>



void post(void){
   char dispbuf[100];
   unsigned char buf[512];
   drawRectFillEx(0,0,lcdCtrl.panel->width,lcdCtrl.panel->height,C_BLACK);
   drawStringEx("power on self test (post) begin,touch screen to skip",10,10,FONT_ASCII_16,C_WHITE,C_BLACK);
   
   //nand flash
   drawStringEx("nand flash test",30,30,FONT_ASCII_16,C_WHITE,C_BLACK);
   memset(buf,0x55,sizeof buf);
   MMCSDP_Write(mmcsdctr,buf,INAND_TEST_SECTOR,1);
   memset(buf,0,sizeof buf);
   MMCSDP_Read(mmcsdctr,buf,INAND_TEST_SECTOR,1);
   if(memis(buf,0x55,sizeof buf)==FALSE){
      drawStringAlignEx("FAIL",ALIGN_RIGHT_MIDDLE,250,30,80,20,FONT_ASCII_16,C_WHITE,C_BLACK);
      while (1);
   }
   memset(buf,0xaa,sizeof buf);
   MMCSDP_Write(mmcsdctr,buf,INAND_TEST_SECTOR,1);
   memset(buf,0,sizeof buf);
   MMCSDP_Read(mmcsdctr,buf,INAND_TEST_SECTOR,1);
   if(memis(buf,0xaa,sizeof buf)==FALSE){
      drawStringAlignEx("FAIL",ALIGN_RIGHT_MIDDLE,250,30,80,20,FONT_ASCII_16,C_WHITE,C_BLACK);
      while (1);
   }
   drawStringAlignEx("SUCCESS",ALIGN_RIGHT_MIDDLE,250,30,80,20,FONT_ASCII_16,C_WHITE,C_BLACK);

   //memery check
   drawStringEx("scan ddr memery",30,50,FONT_ASCII_16,C_WHITE,C_BLACK);
   char *ddr = (char *)0x80000000;
   atomicClear(&g_touched);
   for (int i=0;i<17*1024;i++) {
      memset32(ddr+i*4096,0x55555555,4096);
      if (!memis_32(ddr+i*4096,0x55555555,4096)){
        drawStringAlignEx("ERROR",ALIGN_RIGHT_MIDDLE,250,50,80,20,FONT_ASCII_16,C_WHITE,C_BLACK);
        while (1);
      }
      memset32(ddr+i*4096,0xaaaaaaaa,4096);
      if (!memis_32(ddr+i*4096,0xaaaaaaaa,4096)){
         drawStringAlignEx("ERROR",ALIGN_RIGHT_MIDDLE,250,50,80,20,FONT_ASCII_16,C_WHITE,C_BLACK);
         while (1);
      } else{
         if (i%50==0) {
            sprintf(dispbuf, "%d%s", i*4, " KB");
            drawStringAlignEx(dispbuf,ALIGN_RIGHT_MIDDLE,250,50,80,20,FONT_ASCII_16,C_WHITE,C_BLACK);
         }
      }     
      if(isTouched() || atomicTestClear(&g_keyPushed)){
         break;
      }
   }
   //lcd check

   for (int i=0;i<LCD_XSize;i++) {
       drawVLineEx(i,0,LCD_YSize/4,RGB(0xff*i/LCD_XSize,0,0));
       drawVLineEx(i,LCD_YSize/4,LCD_YSize/4,RGB(0,0xff*i/LCD_XSize,0));
       drawVLineEx(i,LCD_YSize/4*2,LCD_YSize/4,RGB(0,0,0xff*i/LCD_XSize));
       drawVLineEx(i,LCD_YSize/4*3,LCD_YSize/4,RGB(0xff*i/LCD_XSize,0xff*i/LCD_XSize,0xff*i/LCD_XSize));
   }
   unsigned int timerindex = StartTimer(10000);
   atomicClear(&g_touched);   
   while(!isTouched() && !atomicTestClear(&g_keyPushed)
         && !IsTimerElapsed(timerindex));

}





void lcdTest(){
  for(int i=1;i<=800;i++)   {
    drawVLineEx(i-1, 0,             LCD_YSize/4, RGB(0xff*i/800,0,0));
    drawVLineEx(i-1, LCD_YSize/4*1, LCD_YSize/4, RGB(0,0xff*i/800,0));
    drawVLineEx(i-1, LCD_YSize/4*2, LCD_YSize/4, RGB(0,0,0xff*i/800));
    drawVLineEx(i-1, LCD_YSize/4*3, LCD_YSize/4, RGB(0xff*i/800,0xff*i/800,0xff*i/800)) ;
  }
}