

#include "bl.h"
#include "uartStdio.h"
#include "hw_types.h"
#include "bl_platform.h"
#include "bl_copy.h"
#include "mmcsd_proto.h"
#include "soc_AM335X.h"
#include "type.h"
#include "pf_usbmsc.h"
#include "ff.h"
#include "string.h"
#include "module.h"

extern void BootAbort(void);

extern int HSMMCSDInit(void);
extern unsigned int HSMMCSDImageCopy(void);

  
extern mmcsdCtrlInfo mmcsdctr[2];
extern mmcsdCardInfo card;

void copyInit(void){
}


int bootCopy() {
   char bockbuf[512] ;
   int reval;
   reval = MMCSDP_Read(&mmcsdctr[0], bockbuf, APP_HEAD_SECTOR, 1);
   APPHEADER *header = (APPHEADER *)bockbuf;
   if(FALSE==reval){
      return APP_COPY_ERROR;
   }
   if (header->magic != APP_MAGIC_OK) {
      return APP_UNAVAILABLE;
   }
   if (header->imageSize > APP_MAX_SIZE) {
      return   APP_SIZE_ERROR;
   }
   if (header->loadAddr != APP_ENTRY) {
      return  APP_ENTRY_ERROR;
   }
   if(SUCCESS==MMCSDP_Read(&mmcsdctr[0], (void *)(header->loadAddr),APP_BEGIN_SECTOR,
               (header->imageSize + 511) / 512)){
      reval =  0;
    }else{
      reval =  APP_COPY_ERROR;
    }
                 
   return reval;
}



void jumptoApp(){
   UARTPuts("Jumping to application...\r\n\n", -1);
   ((void (*)())APP_ENTRY)();
}




