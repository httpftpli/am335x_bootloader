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
#include "algorithm.h"
#include "delay.h"


#define BOOTLOADER_BEGIN_SECTOR   256
#define BOOTLOADER_ENTRY          0x402f0400
#define FS_BEGIN_SECTOR           128*1024
#define FIRST_PARTITION_SIZE      256*1024 //in sector

typedef struct __fileinfo{
  char filename[13];
} MYFILEINFO;


const unsigned char ProgramTable[256]=
{
	0xD5,0xFD,0xC3,0xB6,0xBE,0xD9,0x55,0x53,0x42,0xB4,0xC5,0xC5,0xCC,0x2E,0x2E,0x2E,
	0x55,0x53,0x42,0x20,0x45,0x6E,0x75,0x6D,0x65,0x72,0x61,0x74,0x65,0x64,0x2E,0x3E,
	0xC3,0xB6,0xBE,0xD9,0xC9,0xE8,0xB1,0xB8,0xCA,0xA7,0xB0,0xDC,0x21,0x21,0x21,0xc0,
	0x45,0x6E,0x75,0x6D,0x20,0x66,0x61,0x69,0x6C,0x75,0x72,0x65,0x20,0x21,0x01,0x27,
	0xD5,0xFD,0xD4,0xDA,0xD0,0xB4,0xC8,0xEB,0xCE,0xC4,0xBC,0xFE,0x21,0x21,0x21,0x70,
	0x50,0x6C,0x65,0x61,0x73,0x65,0x20,0x57,0x61,0x69,0x74,0x69,0x6E,0x67,0x2E,0x2E,
	0x43,0x68,0x69,0x70,0x20,0x72,0x65,0x73,0x65,0x74,0x20,0x4F,0x4B,0x21,0x20,0x00,
	0x52,0x65,0x73,0x65,0x74,0x20,0x63,0x68,0x69,0x70,0x20,0x45,0x72,0x72,0x6F,0x72,
	0x52,0xd3,0x44,0xD0,0xBE,0xC6,0xAC,0xB8,0xB4,0xCE,0xB3,0xB3,0xC9,0xB9,0xA6,0xf1,
	0x54,0xc3,0x46,0xDb,0xBE,0xC3,0xA2,0xB2,0xB4,0xC3,0xBB,0xca,0xa7,0xB0,0xdc,0x31,
	0x35,0xb3,0x48,0xDc,0xBE,0xC5,0xA3,0xC9,0xE8,0xD6,0xC3,0xCA,0xA7,0xB0,0xDC,0x71,
	0x85,0xa3,0x42,0xB4,0xC5,0xC5,0xCC,0xC9,0xE8,0xD6,0xC3,0xCA,0xA7,0xB0,0xDC,0xc1,
	0xD6,0xB8,0xC1,0xEE,0xC3,0xBB,0xD3,0xD0,0xD5,0xFD,0xB3,0xA3,0xCA,0xE4,0xB3,0xF6,
	0x55,0x79,0xab,0xDb,0x3E,0xC7,0x89,0x34,0x34,0x11,0x35,0x90,0xfd,0xf3,0xf3,0xf9,
	0x35,0xb3,0x48,0xDc,0xBE,0xC5,0xA3,0xbb,0xc3,0x43,0xC3,0xCA,0xA7,0x34,0x1C,0x28,
	0x16,0x55,0x11,0xcE,0xC3,0xB7,0xD3,0xD0,0xD5,0xFD,0xB3,0xA3,0x33,0x45,0x34,0x36,
};

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
//extern FATFS inandfs;
BOOL idiskFormat(){
   int r;
   DWORD plist[] = {FIRST_PARTITION_SIZE, -1UL, 0, 0};  /* Divide drive into two partitions */
   BYTE work[_MAX_SS];
   unsigned int beginLBA = FS_BEGIN_SECTOR;  //64M BYTE BEGIN
   r =  f_fdiskEx(0,beginLBA, plist, work);  /* Divide physical drive 0 */ 
   if (FR_OK!=r) {
      return FALSE;
   }
   r = f_mkfs(0,0,0);
   if (FR_OK!=r) {
      return FALSE;
   }
   return TRUE;
}

extern void statBarPrint(unsigned int error,TEXTCHAR *buf);
BOOL burnAPP(TCHAR *path){
   unsigned char *buf = (unsigned char *)0x80000000;
   unsigned char headbuf[512];
   unsigned int rdlen;
   APPHEADER header;
   FIL file;
   FRESULT fret;
   int ret;
   char disbuf[32];
   unsigned short crc16;
   unsigned int  percent = 0,percentold =0;
   unsigned int  filesize, count;
   fret = f_open(&file, path, FA_READ);
   if (fret != FR_OK) {
      return FALSE;
   }
   if (file.fsize > APP_MAX_SIZE) {
      goto ERROR;
   }
   filesize = file.fsize;
   if (filesize%512) {
      goto ERROR; 
   }
   count = filesize/512;
   if (count<2) goto ERROR;
   header.imageSize = (count-1)*510;
   header.loadAddr = APP_ENTRY;
   header.magic = APP_MAGIC_NO;
   memset(headbuf,0,sizeof headbuf);
   memcpy(headbuf,&header,sizeof header);
   ret = MMCSDP_Write(mmcsdctr, headbuf, APP_HEAD_SECTOR, 1);
   if (FALSE == ret) {
      goto ERROR;
   }
   statBarPrint(0,"reading");
   delay(300);
   for (int i = 0;i<count; i++) {
      fret = f_read(&file, headbuf, 512, &rdlen);
      if (fret != FR_OK)  goto ERROR;
      if (0 == rdlen) {
         if ((count - 1) != i) goto ERROR;
         break;
      }
      for (int j = 0; j < 256; j++) {
         headbuf[j] ^= ProgramTable[j];
         headbuf[j + 256] ^= ProgramTable[j];
      }
      crc16 = crc16_MD(headbuf, 510);
      if (crc16 != *(unsigned short *)&headbuf[510]) goto ERROR;
      if (i == 0) {
         if ((headbuf[0] != 'T') || (headbuf[1] != 'H') || (headbuf[2] != 'J')) goto ERROR;
         if ((headbuf[10] != 'A') || (headbuf[11] != 'R') || (headbuf[12] != 'A')) goto ERROR;
      } else {
         memcpy(buf + 510 * (i - 1), headbuf, 510);
      }
      percent = i * 100 / count;
      if (percent / 5 != percentold / 5) {
         sprintf(disbuf, "%d%%", percent);
         statBarPrint(0,disbuf);
         percentold = percent;
      }
   }
   statBarPrint(0,"writing");
   ret = MMCSDP_Write(mmcsdctr, buf, APP_BEGIN_SECTOR, DIVUP(header.imageSize,512));
      if (FALSE == ret) {
         goto ERROR;
   }
   header.magic = APP_MAGIC_OK;
   memset(headbuf,0,sizeof headbuf);
   memcpy(headbuf,&header,sizeof header);
   MMCSDP_Write(mmcsdctr, headbuf, APP_HEAD_SECTOR, 1);
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
         statBarPrint(0,disbuf);
         return  FR_OK;
      }else{
         percent+=5;
         sprintf(disbuf,"%d%%",percent);
         statBarPrint(0,disbuf);
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

