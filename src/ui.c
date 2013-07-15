#include "mmath.h"
#include "pf_timertick.h"
#include "pf_tsc.h"
#include "pf_usbmsc.h"
#include "string.h"
#include "pf_key_touchpad.h"
#include <stdio.h>
#include "pf_lcd.h"
#include "bl_copy.h"
#include "soc_am335x.h"
#include "hw_control_AM335x.h"
#include "hw_types.h"
#include "mmcsd_proto.h"
#include "mem.h"
#include "lib_gui.h"


#define MAX_BUTTON_CAPTION   30
#define MAX_LABEL_CAPTION    100

#define BTR_VERSION  "V1.0"


#define ID_SET        1
#define ID_BURN       2
#define ID_UFDISK     3
#define ID_IFDISK     4
#define ID_UFORMAT    5
#define ID_IFORMAT    6
#define ID_BURNPRO    7
#define ID_BURNFONT   8
#define ID_UPPRO      9
#define ID_DOWNPRO    10
#define ID_UPFONT     11
#define ID_DOWNFONT   12
#define ID_TC         13


extern GUI_FONT GUI_Fontascii_16,GUI_Fontascii_20;

extern BOOL idiskFdisk();
extern BOOL idiskFormat();


typedef struct __fileinfo{
  char filename[13];
} MYFILEINFO;


typedef void BUTTON_CLICK_HANDLER(void *button,unsigned int stat);

typedef struct __button{
   unsigned short x,y,width,height;
   unsigned short pushed:1;
   unsigned short statChanged:1;
   unsigned short checkable:1;
   unsigned short haveFrame:1;
   unsigned short enable:1;
   unsigned short winId;
   unsigned int   group; 
   TEXTCHAR *caption; 
   BUTTON_CLICK_HANDLER *handler;  
}BUTTON;


typedef struct __label{
   unsigned short x,y,width,height;
   unsigned int haveFrame:1;
   unsigned int statChanged:1;
   TEXTCHAR *caption;
}LABEL;




BUTTON *buttonList[50];
unsigned int buttonindext = 0;
LABEL *labelList[20];
unsigned int labelindex = 0;

BUTTON *buttonGroup1[50];
BUTTON *buttonGroup2[50];


void registButton(BUTTON *button){
   buttonList[buttonindext++] = button;
}

void buttonRegistHandler(BUTTON *button,BUTTON_CLICK_HANDLER handler){
   button->handler = handler;
}


void registLabel(LABEL *label){
   labelList[labelindex++] = label;
}


const LABEL titleLabel = {
   .x = 0,.y = 0,.width=800,.height=20,.haveFrame=1,.caption = "mingde   bootloader  " BTR_VERSION,
};

const LABEL pageLabel = {
   .x = 0,.y = 20,.width=800,.height=560,.haveFrame=1,
};

const LABEL statLabel = {
  .x = 0,.y = 580,.width=60,.height=20,.haveFrame=1,.haveFrame=1,.caption = "status"
};

LABEL statLabel1 = {
   .x = 60,.y = 580,.width=740,.height=20,.haveFrame=1,
};
 

LABEL frametitles[] = {
   [0]={.x = 240,.y = 35,.width=220,.height=20,.haveFrame=1,.haveFrame=1,.caption = NULL},
   [1]={.x = 460,.y = 35,.width=220,.height=20,.haveFrame=1,.haveFrame=1,.caption = "udisk"},  
};

const LABEL frames[] = {
   [0]={.x = 240,.y = 55,.width=220,.height=480,.haveFrame=1,},
   [1]={.x = 460,.y = 55,.width=220,.height=480,.haveFrame=1,},
};

BUTTON burnpagebuttons[] = {
   [0]={.x = 690,.y = 140,.width=100,.height=40,.enable=1,.winId=ID_UFDISK,.caption = "ts calibrat",},
   [1]={.x = 690,.y = 240,.width=100,.height=40,.enable=1,.winId=ID_UFORMAT,.caption = "idisk format",},
   [2]={.x = 690,.y = 290,.width=100,.height=40,.enable=1,.winId=ID_IFORMAT,.caption = "burn program",},
   [3]={.x = 690,.y = 340,.width=100,.height=40,.enable=1,.winId=ID_BURNPRO,.caption = "burn font",},
   [4]={.x = 690,.y = 440,.width=100,.height=40,.enable=1,.winId=ID_BURNFONT,.caption = "run app",},
#ifdef INNERBOOT
   [9]={.x = 690,.y = 390,.width=100,.height=40,.enable=1,.winId=ID_BURNFONT,.caption = "burn boot",},
#endif
   [5]={.x = 250,.y = 540,.width=80,.height=35,.enable=1,.winId=ID_UPPRO,.caption = "up",},
   [6]={.x = 360,.y = 540,.width=80,.height=35,.enable=1,.winId=ID_DOWNPRO,.caption = "down",},
   [7]={.x = 480,.y = 540,.width=80,.height=35,.enable=1,.winId=ID_UPFONT,.caption = "up",},
   [8]={.x = 590,.y = 540,.width=80,.height=35,.enable=1,.winId=ID_DOWNFONT,.caption = "down",},
};

BUTTON inandButtons[] = {
   [0]={.x = 240,.y = 55,.width=220,.height=40,.winId=ID_UFDISK,.checkable=1},
   [1]={.x = 240,.y = 95,.width=220,.height=40,.winId=ID_UFORMAT,.checkable=1},
   [2]={.x = 240,.y = 135,.width=220,.height=40,.winId=ID_IFORMAT,.checkable=1},
   [3]={.x = 240,.y = 175,.width=220,.height=40,.winId=ID_BURNPRO,.checkable=1},
   [4]={.x = 240,.y = 215,.width=220,.height=40,.winId=ID_BURNFONT,.checkable=1},
   [5]={.x = 240,.y = 255,.width=220,.height=40,.winId=ID_UPPRO,.checkable=1},
   [6]={.x = 240,.y = 295,.width=220,.height=40,.winId=ID_DOWNPRO,.checkable=1},
   [7]={.x = 240,.y = 335,.width=220,.height=40,.winId=ID_UPFONT,.checkable=1},
   [8]={.x = 240,.y = 375,.width=220,.height=40,.winId=ID_DOWNFONT,.checkable=1},
   [9]={.x = 240,.y = 415,.width=220,.height=40,.winId=ID_DOWNFONT,.checkable=1},
   [10]={.x = 240,.y = 455,.width=220,.height=40,.winId=ID_DOWNFONT,.checkable=1},
   [11]={.x = 240,.y = 495,.width=220,.height=40,.winId=ID_DOWNFONT,.checkable=1},
};
BUTTON udiskButtons[] = {
   [0]={.x = 460,.y = 55,.width=220,.height=40,.winId=ID_UFDISK,.checkable=1},
   [1]={.x = 460,.y = 95,.width=220,.height=40,.winId=ID_UFORMAT,.checkable=1},
   [2]={.x = 460,.y = 135,.width=220,.height=40,.winId=ID_IFORMAT,.checkable=1},
   [3]={.x = 460,.y = 175,.width=220,.height=40,.winId=ID_BURNPRO,.checkable=1},
   [4]={.x = 460,.y = 215,.width=220,.height=40,.winId=ID_BURNFONT,.checkable=1},
   [5]={.x = 460,.y = 255,.width=220,.height=40,.winId=ID_UPPRO,.checkable=1},
   [6]={.x = 460,.y = 295,.width=220,.height=40,.winId=ID_DOWNPRO,.checkable=1},
   [7]={.x = 460,.y = 335,.width=220,.height=40,.winId=ID_UPFONT,.checkable=1},
   [8]={.x = 460,.y = 375,.width=220,.height=40,.winId=ID_DOWNFONT,.checkable=1},
   [9]={.x = 460,.y = 415,.width=220,.height=40,.winId=ID_DOWNFONT,.checkable=1},
   [10]={.x = 460,.y = 455,.width=220,.height=40,.winId=ID_DOWNFONT,.checkable=1},
   [11]={.x = 460,.y = 495,.width=220,.height=40,.winId=ID_DOWNFONT,.checkable=1},
};

const BUTTON setbuttons[] = {
   [0]={.x = 100,.y = 600,.width=50,.height=20,.caption = "touchpad calibrat",},
};


void buttonRedraw(const BUTTON *button, unsigned int force) {
   unsigned int forcetemp = force || button->statChanged;
   if (forcetemp) {
      if (!button->pushed) {
         if (button->haveFrame == 1) {
            drawHLineEx(button->x, button->y, button->width, C_WHITE);
            drawHLineEx(button->x, button->y + button->height - 1, button->width, C_WHITE);
            drawVLineEx(button->x, button->y, button->height, C_WHITE);
            drawVLineEx(button->x + button->width - 1, button->y, button->height, C_WHITE);
         }
         drawRectFillEx(button->x + 1, button->y + 1, button->width - 2, button->height - 2, C_BLUE);
         if (button->caption != NULL) {
            drawStringAligenEx(button->caption, ALIGEN_MIDDLE, button->x, button->y, button->width, button->height,&GUI_Fontascii_16 , C_WHITE, C_TRANSPARENT);
         }
      } else {
         if (button->haveFrame == 1) {
            drawHLineEx(button->x, button->y, button->width, C_BLUE);
            drawHLineEx(button->x, button->y + button->height - 1, button->width, C_BLUE);
            drawVLineEx(button->x, button->y, button->height, C_BLUE);
            drawVLineEx(button->x + button->width - 1, button->y, button->height, C_BLUE);
         }
         drawRectFillEx(button->x + 1, button->y + 1, button->width - 2, button->height - 2, C_WHITE);
         if (button->caption != NULL) {
            drawStringAligenEx(button->caption, ALIGEN_MIDDLE, button->x, button->y, button->width, button->height, &GUI_Fontascii_16, C_BLUE, C_TRANSPARENT);
         }
      }
   }
}




void buttonShow(const BUTTON *button){
   buttonRedraw(button,1);
}

void buttonEnable(BUTTON *button){
   button->enable = 1;
}


void buttonDisable(BUTTON *button){
   button->enable = 0;
}



void buttonSetCaption(BUTTON *button,TEXTCHAR *caption){
   button->caption = caption;
   button->statChanged = 1;
}

void buttonSetStat(BUTTON *button,int stat){
   button->pushed = stat;
   button->statChanged = 1;
}


void labelSetCaption(LABEL *label,TEXTCHAR *caption){
   label->caption = caption;
   label->statChanged = 1;
}

void labelRedraw(const LABEL *label, unsigned int force);
void statBarPrint(TEXTCHAR *buf){
   statLabel1.caption = buf;
   labelRedraw(&statLabel1,1);
}

void labelRedraw(const LABEL *label, unsigned int force) {
   unsigned int forcetemp = force || label->statChanged;
   if (forcetemp) {
      drawRectFillEx(label->x, label->y, label->width, label->height, C_BLUE);
      if (label->haveFrame) {
         drawHLineEx(label->x, label->y, label->width, C_WHITE);
         drawHLineEx(label->x, label->y + label->height - 1, label->width, C_WHITE);
         drawVLineEx(label->x, label->y, label->height, C_WHITE);
         drawVLineEx(label->x + label->width - 1, label->y, label->height, C_WHITE);
      }
      if (label->caption != NULL) {
         drawStringAligenEx(label->caption, ALIGEN_MIDDLE, label->x, label->y, label->width, label->height, &GUI_Fontascii_16, C_WHITE, C_TRANSPARENT);
      }
   }
}

void guiRedraw(void){
   for (int i=0;(i<NARRAY(labelList))&&(labelList[i]!=NULL);i++) {
      labelRedraw(labelList[i],0);
      labelList[i]->statChanged = 0;
   }
   for (int i=0;(i<50)&&(buttonList[i]!=NULL);i++) {
      buttonRedraw(buttonList[i],0);
      buttonList[i]->statChanged = 0;
   }
}

static unsigned int touch2buttonindex() {
   for (int i = 0; (i < NARRAY(buttonList)) && (buttonList[i] != NULL); i++) {
      if ((g_ts.x > buttonList[i]->x) && (g_ts.x < buttonList[i]->x + buttonList[i]->width) &&
          (g_ts.y > buttonList[i]->y) && (g_ts.y < buttonList[i]->y + buttonList[i]->height)
            &&(buttonList[i]->enable==1)) {
         return i;
      }
   }
   return -1;
}

unsigned short guiExec(void) {
#define STAT_NO         0
#define STAT_PUSHDOWN   1
#define STAT_FROZE      2
   static int stat = STAT_NO;
   static int timemark;
   static unsigned int button;
   unsigned int buttonnow;
   switch (stat) {
   case STAT_NO:
      if (atomicTestClear(&g_touched)) {
         buttonnow = touch2buttonindex();
         if (buttonnow != -1) {
            stat = STAT_PUSHDOWN;
            timemark = TimerTickGet();
            if (0 == buttonList[buttonnow]->checkable) {              
               buttonList[buttonnow]->pushed = 1;
               buttonList[buttonnow]->statChanged = 1;
               button = buttonnow;
            } else {
               buttonList[buttonnow]->pushed = ! buttonList[buttonnow]->pushed;
               buttonList[buttonnow]->statChanged = 1;
               button = buttonnow;
            }
            guiRedraw();
            if (buttonList[buttonnow]->handler != NULL) buttonList[buttonnow]->handler(buttonList[buttonnow],buttonList[buttonnow]->pushed);
            return buttonList[buttonnow]->winId;
         }
      }
      break;
   case STAT_PUSHDOWN:
      if ((!atomicTestClear(&g_touched)) && (TimerTickGet() - timemark > 150)) {
         if (0 == buttonList[button]->checkable) {
            buttonList[button]->pushed = 0;
            buttonList[button]->statChanged = 1;
         }
         stat = STAT_FROZE;
         timemark = TimerTickGet();
      }
      break;  
case STAT_FROZE:
      if (TimerTickGet() - timemark > 50) {
         stat = STAT_NO; 
      }
      break;
   default:
      break;
   }
   guiRedraw();
   return 0;
}


void labelShow(const LABEL *label) {
   labelRedraw(label,1);
};


TEXTCHAR buff[100];


void hmishow(){  
   labelShow(&titleLabel);
   labelShow(&pageLabel);
   labelShow(&statLabel);
   labelShow(&statLabel1);
   for (int i=0;i<NARRAY(frametitles);i++) {
      labelShow(frametitles+i);
   }   
   for (int i=0;i<NARRAY(frames);i++) {
      labelShow(frames+i);
   } 
   for (int i=0;i<NARRAY(burnpagebuttons);i++) {     
      buttonShow(burnpagebuttons+i);
   }
   for (int i=0;i<NARRAY(inandButtons);i++) {
      buttonShow(inandButtons+i);
   }
   for (int i=0;i<NARRAY(udiskButtons);i++) {
      buttonShow(udiskButtons+i);
   }
}

void tscalhandler(void *button,unsigned int stat){
   if (1==stat) {
      while (!TouchCalibrate(1)) ;
   } 
   hmishow();
}

MYFILEINFO udiskfileinfolist[300],idiskfileinfolist[20];
unsigned int udiskFileCount,idiskFileCount;
unsigned int ufileindex=-1,ufilegroupindex=0;
unsigned int ifileindex=-1,ifilegroupindex=0;
unsigned int ufilestatchagned = 0,ifilestatchagned=1;

static void idiskbuttonhandler(void *button, unsigned int stat) {
   BUTTON *b = (BUTTON *)button;
   BUTTON **grop;
   unsigned int index;
   if (stat == 1) {
      grop = (BUTTON **)(b->group);
      for (int i = 0;; i++) {
         if (grop[i] == NULL) {
            break;
         }
         if (grop[i] != b){
           if(grop[i]->pushed==1){
              grop[i]->pushed = 0;
              grop[i]->statChanged = 1;
           }
         }else{
            index=i;
         }
      }
     ifileindex = index;
   }else{
     ifileindex = -1;
   }
   if(ifileindex!=-1){
        statBarPrint(idiskfileinfolist[ifilegroupindex*12+ifileindex].filename);
   }else{
        statBarPrint(NULL);    
   } 
}


void probUdisk(){
   static int probed = 0;
   if ((g_usbMscState == USBMSC_DEVICE_READY)&&(probed==0)) {
      udiskFileCount = NARRAY(udiskfileinfolist);
      scan_files("2:/",udiskfileinfolist,&udiskFileCount);
      probed = 1; 
      ufilegroupindex = 0 ;
      ufileindex  = -1;
      ufilestatchagned = 1;       
   } else if ((g_usbMscState == USBMSC_NO_DEVICE)&&(probed==1)) {
      probed = 0;
      ufilegroupindex = 0 ;
      if (ufileindex!=-1) {
         buttonSetStat(&udiskButtons[ufileindex],0);
      }
      ufileindex = -1;
      memset(udiskfileinfolist,0,sizeof udiskfileinfolist ); 
      udiskFileCount=0;
      ufilestatchagned = 1;
   }
}

void displayUdisk() {
   if (ufilestatchagned == 1) {
      ufilestatchagned = 0;
      for (unsigned int i = 0; i < 12; i++) {
         if ((ufilegroupindex * 12 + i) > udiskFileCount) {
            buttonSetCaption(&udiskButtons[i], NULL);
            buttonDisable(&udiskButtons[i]);
         } else {
           buttonSetCaption(&udiskButtons[i],udiskfileinfolist[ufilegroupindex * 12 + i].filename);
           buttonEnable(&udiskButtons[i]);
         }
      }
   }
}



void probIdisk_display(){
   static int probed = 0;
   static TEXTCHAR labelchar[48];
   if (ifilestatchagned == 1) {
      idiskFileCount = NARRAY(idiskfileinfolist);
      scan_files("0:/",idiskfileinfolist,&idiskFileCount);
      ifilestatchagned = 0 ; 
      for (unsigned int i = 0; i < 12; i++) {
         if (((ifilegroupindex * 12 + i) > idiskFileCount) || (idiskFileCount==0)) {
            buttonSetCaption(&inandButtons[i], NULL);
            buttonDisable(&inandButtons[i]);
         } else {
           buttonSetCaption(&inandButtons[i],idiskfileinfolist[ifilegroupindex * 12 + i].filename);
           buttonEnable(&inandButtons[i]);
         }
      }
      unsigned int free =  getpartitionfree("0:/")/1024;
      sprintf(labelchar,"idisk(%dKB free)",free);
      labelSetCaption(&frametitles[0],labelchar);
   } 
}


static void idiskformathandler(void *button,unsigned int stat){
   BOOL r;
   if (1==stat) {
      r = idiskFormat();
      if (r) {
         statBarPrint("idisk format success");
      }else{
         statBarPrint("idisk format fail");
      }
   }
   ifilestatchagned = 1; 
}

static void udiskbuttonhandler(void *button, unsigned int stat) {
   BUTTON *b = (BUTTON *)button;
   BUTTON **grop;
   unsigned int index;
   if (stat == 1) {
      grop = (BUTTON **)(b->group);
      for (int i = 0;; i++) {
         if (grop[i] == NULL) {
            break;
         }
         if (grop[i] != b){
           if(grop[i]->pushed==1){
              grop[i]->pushed = 0;
              grop[i]->statChanged = 1;
           }
         }else{
            index=i;
         }
      }
      ufileindex = index;
   }else{
      ufileindex = -1;
   }
   if(ufileindex!=-1){
        statBarPrint(udiskfileinfolist[ufilegroupindex*12+ufileindex].filename);
   }else{
        statBarPrint(NULL);    
   }  
}


extern mmcsdCtrlInfo mmcsdctr[2];

#ifdef INNERBOOT
static void burnboothandler(void *button, unsigned int stat) {
   int ret;
   unsigned int buf[512 / 4];
   buf[0] = HWREG(SOC_CONTROL_REGS + CONTROL_MAC_ID_LO(0));
   buf[1] = HWREG(SOC_CONTROL_REGS + CONTROL_MAC_ID_HI(0));
   buf[2] = HWREG(SOC_CONTROL_REGS + CONTROL_MAC_ID_LO(1));
   buf[3] = HWREG(SOC_CONTROL_REGS + CONTROL_MAC_ID_HI(1));
   if (-1==ufileindex) {
      statBarPrint("please select a file");
      return;
   }
   ret = MMCSDP_Write(mmcsdctr, buf, 641, 1);
   if (ret == FALSE) {
      goto ERROR;
   }
   char pathbuf[20];
   memcpy(pathbuf, "2:/", 4);
   ret = burnBootloader(strcat(pathbuf, udiskfileinfolist[ufilegroupindex*12+ufileindex].filename));
   if (FALSE == ret) {
      goto ERROR;
   }
   statBarPrint("burn bootloader success");
   return;
ERROR:
   statBarPrint("burn bootloader fail");
}
#endif

static void burnapphandler(void *button, unsigned int stat){
   if (-1==ufileindex) {
      statBarPrint("please select a file");
      return;
   }
   char pathbuf[20] = "2:/";
   int ret = burnAPP(strcat(pathbuf, udiskfileinfolist[ufilegroupindex*12+ufileindex].filename));
   if (FALSE == ret) {
      statBarPrint("burn bootloader fail");
      return ;
   }
   statBarPrint("burn bootloader success");
}

static void burnfonthandler(void *button,unsigned int stat){
   if (-1==ufileindex) {
      statBarPrint("please select a file");
      return;
   }
   unsigned int index = ufilegroupindex*12+ufileindex;
   if (!strendwith(udiskfileinfolist[index].filename,".FNT")){
      statBarPrint("unknow font file");
      return;
   }
   char pathbuf[20] = "2:/";
   unsigned int ret = burnFont(strcat(pathbuf, udiskfileinfolist[index].filename));
   if (FALSE==ret) {
      statBarPrint("burn font fail");
      return ;
   }
   statBarPrint("burn font success");
   ifilestatchagned = 1;
}

static void runapphandler(void *button,unsigned int stat){
  int ret =  bootCopy();
  if(APP_UNAVAILABLE==ret){
    statBarPrint("app unavailable");
  }else if(APP_COPY_ERROR==ret){
    statBarPrint("app copy error");
  }else if(APP_SIZE_ERROR ==ret){
    statBarPrint("app size error");
  }else if(APP_ENTRY_ERROR ==ret){
    statBarPrint("app entry error");
  }
  if(0==ret){
    LCDBackLightOFF();
    LCDRasterEnd();
    registKeyHandler(NULL);
    jumptoApp();
  }
}


static void ufileuphandler(void *button,unsigned int stat){
   if (ufilegroupindex!=0) {
       ufilegroupindex--;
       if(ufileindex != -1){
          buttonSetStat(&udiskButtons[ufileindex],0);
          ufileindex = -1;
       }       
       ufilestatchagned = 1;
   }
}


static void ufiledownhandler(void *button ,unsigned int stat){
   if ((udiskFileCount+11)/12-1 > ufilegroupindex) {
       ufilegroupindex++;
       if(ufileindex!=-1){
         buttonSetStat(&udiskButtons[ufileindex],0);
         ufileindex = -1;
       }
       ufilestatchagned = 1;
   }
}

void hmiInit(){
   GUI_SetBkColor(C_BLUE);
   GUI_SetColor(C_WHITE);
   buttonRegistHandler(burnpagebuttons,tscalhandler);
   buttonRegistHandler(burnpagebuttons+1,idiskformathandler);
   buttonRegistHandler(burnpagebuttons+8,ufiledownhandler);
   buttonRegistHandler(burnpagebuttons+7,ufileuphandler);  
   buttonRegistHandler(burnpagebuttons+4,runapphandler);
#ifdef INNERBOOT
   buttonRegistHandler(burnpagebuttons+9,burnboothandler);
#endif
   buttonRegistHandler(burnpagebuttons+2,burnapphandler);
   buttonRegistHandler(burnpagebuttons+3,burnfonthandler); 
   registLabel(&statLabel1);
   registLabel(&frametitles[0]);
   for (int i=0;i<NARRAY(burnpagebuttons);i++) {
      burnpagebuttons[i].haveFrame = 1;
      registButton(burnpagebuttons+i);
   }
   for (int i=0;i<NARRAY(inandButtons);i++) {
      buttonRegistHandler(inandButtons+i,idiskbuttonhandler);
      registButton(inandButtons+i);
      inandButtons[i].group = (unsigned int)buttonGroup1;
      buttonGroup1[i] = inandButtons+i;
   }
   for (int i=0;i<NARRAY(udiskButtons);i++) {
      buttonRegistHandler(udiskButtons+i,udiskbuttonhandler);
      registButton(udiskButtons+i);
      udiskButtons[i].group = (unsigned int)buttonGroup2;
      buttonGroup2[i] = udiskButtons+i;
   }
   hmishow();
}



