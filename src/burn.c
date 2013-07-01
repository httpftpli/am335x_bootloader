#include "ff.h"
#include "ff_ext.h"
#include "type.h"
#include "pf_usbmsc.h"
#include "mmath.h"
#include "string.h"
#include "mmcsd_proto.h"
#include "bl_copy.h"
#include "mem.h"
#include "stdio.h"


#define BOOTLOADER_BEGIN_SECTOR   256
#define BOOTLOADER_ENTRY          0x402f0400
#define FS_BEGIN_SECTOR           128*1024
#define FIRST_PARTITION_SIZE      256*1024 //in sector

typedef struct __fileinfo{
  char filename[13];
} MYFILEINFO;

FRESULT scan_files(TCHAR *path, MYFILEINFO *fileinfolist, unsigned int *nfiles)
{
 
#if _USE_LFN!=0
#error Cannot use LFN feature without valid code page.
#endif
   FRESULT res;
   FILINFO fno;
   DIR dir;
   res = f_opendir(&dir, path);                       /* Open the directory */
   if (res != FR_OK){
      *nfiles = 0;
      return res;
   }
   int i = 0;
   res = f_readdir(&dir, NULL); //rewind reading index of the directory object 
   for (; ; ){     
      res = f_readdir(&dir, &fno);
      if (res != FR_OK || fno.fname[0] == 0 || *nfiles==i){
         break;
      }
      if (fno.fattrib & AM_DIR)continue;
      memcpy(fileinfolist[i].filename,fno.fname,sizeof(fno.fname));
      i++;
   }
   *nfiles = i;
   return res;
}

extern mmcsdCtrlInfo mmcsdctr[2];
BOOL idiskFormat(){
   int r;
   DWORD plist[] = {FIRST_PARTITION_SIZE, -1UL, 0, 0};  /* Divide drive into two partitions */
   BYTE work[_MAX_SS];
   unsigned int beginLBA = FS_BEGIN_SECTOR;  //64M BYTE BEGIN
   r =  f_fdiskEx(0,beginLBA, plist, work);  /* Divide physical drive 0 */ 
   if (FR_OK!=r) {
      return FALSE;
   }
   FATFS fs1;
   f_mount(0, &fs1);
   r = f_mkfs(0,0,0);
   if (FR_OK!=r) {
      return FALSE;
   }
   return TRUE;
}


BOOL burnAPP(TCHAR *path){
   unsigned char *buf = (unsigned char *)0x80000000;
   unsigned int rdlen;
   APPHEADER header;
   FIL file;
   FRESULT fret;
   int ret;
   char disbuf[32];
   unsigned int  percent = 0,percentold =0;
   unsigned int  filesizekb;
   if (!strendwith(path,".BIN")) {
      return FALSE;
   }
   fret = f_open(&file, path, FA_READ);
   if (fret != FR_OK) {
      return FALSE;
   }
   if (file.fsize > APP_MAX_SIZE) {
      goto ERROR;
   }
   header.imageSize = file.fsize;
   header.loadAddr = APP_ENTRY;
   header.magic = APP_MAGIC_NO;
   memcpy(buf,&header,sizeof header);
   ret = MMCSDP_Write(mmcsdctr, buf, APP_HEAD_SECTOR, 1);
   if (FALSE == ret) {
      goto ERROR;
   }
   sprintf(disbuf, "%d%%", percent);
   statBarPrint(disbuf);
   filesizekb = BOUND(file.fsize,4096);
   statBarPrint("reading");
   for (int i = 0;; i++) {
      fret = f_read(&file, buf+4096*i, 4096, &rdlen);
      if (fret != FR_OK) {
         goto ERROR;
      }
      percent = i * 4096* 100 / filesizekb;
      if (percent / 5 != percentold / 5) {
         sprintf(disbuf, "%d%%", percent + 10);
         statBarPrint(disbuf);
         percentold = percent;
      }
      if (rdlen != 4096) {
         break;
      }
   }
   f_close(&file);
   statBarPrint("writing");
   ret = MMCSDP_Write(mmcsdctr, buf, APP_BEGIN_SECTOR, BOUND(file.fsize,512)/512);
      if (FALSE == ret) {
         goto ERROR;
   }
   header.magic = APP_MAGIC_OK;
   memcpy(buf,&header,sizeof header);
   MMCSDP_Write(mmcsdctr, buf, APP_HEAD_SECTOR, 1);
   return TRUE;
ERROR:
   f_close(&file); 
   return FALSE;
}



BOOL f_copy_disp(const TCHAR *scrpath,const TCHAR * despath){
   FRESULT res;
   FIL scrfile,desfile;
   char *workbuf = (char *)0x80000000;
   char disbuf[20];
   int percent = 0;
   res = f_open(&scrfile,scrpath,FA_OPEN_EXISTING|FA_READ);
   if (res!=0) {
      return res;
   }
   res = f_open(&desfile,despath,FA_CREATE_ALWAYS | FA_WRITE);
   if (res!=0) {
      f_close(&scrfile);
      return res;
   }
   int copysize = (scrfile.fsize+1023)/1024*1024/20;
   unsigned int rb,wb;
   while (1) {
      res = f_read(&scrfile, workbuf, copysize, &rb);
      if (res != 0) {
         goto ERROR;
      }
      res = f_write(&desfile, workbuf, rb, &wb);
      if (res != 0) {
         goto ERROR;
      }
      if (rb!=wb) {
         goto ERROR;
      }
      if (rb < copysize) {
         f_close(&scrfile);
         f_close(&desfile);
         sprintf(disbuf,"%d%%",100);
         statBarPrint(disbuf);
         return  FR_OK;
      }else{
         percent+=5;
         sprintf(disbuf,"%d%%",percent);
         statBarPrint(disbuf);
      }
   }  
ERROR:
   f_close(&scrfile);
   f_close(&desfile);
   return res; 
}


extern mmcsdCtrlInfo mmcsdctr[2];
extern unsigned char emmcheader[80];

BOOL burnBootloader(const TCHAR *path){
   unsigned char *buf = (unsigned char *)0x80000000;
   unsigned int rdlen;
   FIL file;
   FRESULT fret;
   int ret;
   if (!strendwith(path,".MBT")) {
      return FALSE;
   }
   fret = f_open(&file, path, FA_READ);
   if (fret != FR_OK) {
      return FALSE;
   }
   if (file.fsize >109*1024) { //109KB
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
   ret = MMCSDP_Write(mmcsdctr, buf, BOOTLOADER_BEGIN_SECTOR, (file.fsize + 8 + 511) / 512 + 1);
   if (FALSE == ret) {
      goto ERROR;
   }
   f_close(&file);
   return TRUE;
ERROR:
   f_close(&file); 
   return FALSE;
}


BOOL burnFont(const TCHAR *path ){
  static_assert(sizeof(TCHAR)==1,"file name should not multibyte charactor");
  if (!strendwith(path,"FNT")) {
      return FALSE;
  }
  TCHAR *desname = strrchr(path,'/')+1;
  TCHAR despath[30] = "0:/";      
  BOOL r = f_copy_disp(path,strcat(despath,desname));
  
  if (FR_OK!=r) {
     return  FALSE;
  }
  return TRUE;
}

