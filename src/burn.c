#include "string.h"
#include "bl_copy.h"
#include "stdio.h"
#include "bl.h"
#include "platform.h"
#include "pf_eeprom.h"
#include "pf_bootloader.h"


typedef struct __fileinfo {
   char filename[13];
} MYFILEINFO;


FRESULT scan_files(TCHAR *path, MYFILEINFO *fileinfolist, unsigned int *nfiles) {

#if _USE_LFN!=0
#error Cannot use LFN feature without valid code page.
#endif
   FRESULT res;
   FILINFO fno;
   DIR dir;
   res = f_opendir(&dir, path);                       /* Open the directory */
   if (res != FR_OK) {
      *nfiles = 0;
      return res;
   }
   int i = 0;
   res = f_readdir(&dir, NULL); //rewind reading index of the directory object
   for (;;) {
      res = f_readdir(&dir, &fno);
      if (res != FR_OK || fno.fname[0] == 0 || *nfiles == i) {
         break;
      }
      if (fno.fattrib & AM_DIR) continue;
      memcpy(fileinfolist[i].filename, fno.fname, sizeof(fno.fname));
      i++;
   }
   *nfiles = i;
   return res;
}

extern mmcsdCtrlInfo mmcsdctr[2];
//extern FATFS inandfs;
BOOL idiskFormat() {
   int r;
   DWORD plist[] = { FIRST_PARTITION_SIZE, -1UL, 0, 0 };  /* Divide drive into two partitions */
   BYTE work[_MAX_SS];
   unsigned int beginLBA = FS_BEGIN_SECTOR;  //64M BYTE BEGIN
   r =  f_fdiskEx(0, beginLBA, plist, work);  /* Divide physical drive 0 */
   if (FR_OK != r) {
      return FALSE;
   }
   r = f_mkfs(0, 0, 0);
   if (FR_OK != r) {
      return FALSE;
   }
   return TRUE;
}


extern void statBarPrint(unsigned int error, TEXTCHAR *buf);
BURN_RET burnAPP(TCHAR *path) {
   unsigned char *buf = (unsigned char *)0x80000000;
   unsigned int rdlen;
   FIL file;
   FRESULT fret;
   BURN_RET ret;
   char disbuf[32];
   unsigned int  percent = 0, percentold = 0;
   unsigned int  filesize, count;
   fret = f_open(&file, path, FA_READ);
   if (fret != FR_OK) {
      ret = BURN_SRC_ERROR;
      return ret;
   }
   filesize = file.fsize;
   if ((filesize > MAX_APP_FILE_SIZE) || (filesize <= 1024)||(filesize%512!=0)) {
      ret = BURN_SRC_ERROR;
      goto ERROR;
    }
    count = DIVUP(filesize, 512);
    statBarPrint(0, "reading");
    delay(300);
    for (int i = 0; i < count; i++, buf += 512) {
        fret = f_read(&file, buf, 512, &rdlen);
        if (fret != FR_OK){
            ret = BURN_SRC_ERROR;
            goto ERROR;
        }
        percent = i * 80 / count;
        if (percent / 5 != percentold / 5) {
            sprintf(disbuf, "%d%%", percent);
            statBarPrint(0, disbuf);
            percentold = percent;
        }
    }
    statBarPrint(0, "90%%");
    ret = burnAppFormBuf(buf, filesize, 0x02);
    if(ret==BURN_OK) statBarPrint(0, "100%%");
ERROR:
    f_close(&file);
    return ret;
}


BOOL f_copy_disp(const TCHAR *scrpath, const TCHAR *despath) {
   FRESULT res;
   FIL scrfile, desfile;
   char *workbuf = (char *)0x80000000;
   char disbuf[20];
   int percent = 0;
   res = f_open(&scrfile, scrpath, FA_OPEN_EXISTING | FA_READ);
   if (res != 0) {
      return res;
   }
   res = f_open(&desfile, despath, FA_CREATE_ALWAYS | FA_WRITE);
   if (res != 0) {
      f_close(&scrfile);
      return res;
   }
   int copysize = (scrfile.fsize + 1023) / 1024 * 1024 / 20;
   unsigned int rb, wb;
   while (1) {
      res = f_read(&scrfile, workbuf, copysize, &rb);
      if (res != 0) {
         goto ERROR;
      }
      res = f_write(&desfile, workbuf, rb, &wb);
      if (res != 0) {
         goto ERROR;
      }
      if (rb != wb) {
         goto ERROR;
      }
      if (rb < copysize) {
         f_close(&scrfile);
         f_close(&desfile);
         sprintf(disbuf, "%d%%", 100);
         statBarPrint(0, disbuf);
         return  FR_OK;
      } else {
         percent += 5;
         sprintf(disbuf, "%d%%", percent);
         statBarPrint(0, disbuf);
      }
   }
ERROR:
   f_close(&scrfile);
   f_close(&desfile);
   return res;
}


extern mmcsdCtrlInfo mmcsdctr[2];
extern unsigned char emmcheader[80];

BOOL burnBootloader(const TCHAR *path) {
   unsigned char *buf = (unsigned char *)0x80000000;
   unsigned int rdlen;
   FIL file;
   FRESULT fret;
   int ret;
   if (!strendwith(path, ".MBT")) {
      return FALSE;
   }
   fret = f_open(&file, path, FA_READ);
   if (fret != FR_OK) {
      return FALSE;
   }
   if (file.fsize > 109 * 1024) { //109KB
      goto ERROR;
   }
  
   memset(buf, 0, 512);
   memcpy(buf, emmcheader, sizeof emmcheader);
   *(unsigned int *)(buf + 512) = file.fsize;
   *(unsigned int *)(buf + 512 + 4) = BOOTLOADER_ENTRY;
   fret = f_read(&file, buf + 512 + 8, file.fsize, &rdlen);
   if (fret != FR_OK) {
      goto ERROR;
   }
   if (rdlen != file.fsize) {
      goto ERROR;
   }
   //memset(buf,0,file.fsize+8+512);
   ret = MMCSDP_Write(mmcsdctr, buf, BOOTLOADER_BEGIN_SECTOR, DIVUP(file.fsize + 8 + 512,512));
   if (FALSE == ret) {
      goto ERROR;
   }
   
   long long flashid;
   spiFlashReadId(&flashid);
   if ((flashid!=0)&&(flashid!=-1L)){
      unsigned char flashstatus = spiFlashReadStatus();
      if(!(flashstatus & 0x01)){
         spiFlashSwitch256PageSize();
         flashstatus = spiFlashReadStatus();
         if(!(flashstatus & 0x01)){
           goto ERROR;
         }
      }
   }
   unsigned int flashwcont = DIVUP(file.fsize+8,256);
   unsigned int byteswapcont = DIVUP(file.fsize+8,4);
   if ((flashid!=0)&&(flashid!=-1L)) {
      statBarPrint(0, "found dataflash chip ,burn to dataflash");
      for(int i=0;i<byteswapcont;i++){
         *(unsigned int *)(buf+512+4*i) = htonl(*(unsigned int *)(buf+512+4*i));
      }
      delay(500);
      unsigned char percent1 = 0,percent2 = 0;
      char checkbuf[256];
      for (int i=0;i<flashwcont;i++) {
         ret = spiFlashPageWrite(256*i,(void *)(buf + 512+256*i),256 );
         percent1 = (i+1)*100/flashwcont;
         if(percent1/5 != percent2/5){
            percent2 = percent1;
            char  printbuf[200]; 
            sprintf(printbuf,"dataflash write percent %d%%",percent2);
            statBarPrint(0, printbuf);
         }
         if (FALSE==ret) {
            statBarPrint(1, "data flash write  error");
            delay(1000);
            goto ERROR;
         }
         delay(45);
         spiFlashRead(256*i,checkbuf,256);
         if(memcmp(checkbuf,(void *)(buf + 512+256*i),256)!=0){
           statBarPrint(1, "data flash write check error");
           delay(500);
           goto ERROR;
         }
      } 
   } 
   f_close(&file);
   return TRUE;
ERROR:
   f_close(&file);
   return FALSE;
}


BOOL burnFont(const TCHAR *path) {
   static_assert(sizeof(TCHAR) == 1, "file name should not multibyte charactor");
   if (!strendwith(path, "FNT")) {
      return FALSE;
   }
   TCHAR *desname = strrchr(path, '/') + 1;
   TCHAR despath[30] = "0:/";
   BOOL r = f_copy_disp(path, strcat(despath, desname));

   if (FR_OK != r) {
      return  FALSE;
   }
   return TRUE; 
}

