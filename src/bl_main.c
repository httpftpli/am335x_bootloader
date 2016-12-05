#include "bl_copy.h"
#include "bl_platform.h"
#include "bl.h"
#include "type.h"
#include "pf_platform.h"
#include "usblib.h"
#include "usbhost.h"
#include "pf_rtc.h"
#include <time.h>
#include "cpld.h"
#include "pf_timertick.h"
#include "bl_post.h"
#include "pf_tsc2007.h"
#include "board.h"
#include <string.h>


#define FORCEBOOT_WAITTIME  2000 //ms
#define FORCEBOOT_FILENAME  "forboot.dat"



unsigned int entryPoint = 0;
unsigned int DspEntryPoint = 0;

extern tUSBHCD g_sUSBHCD[];


#define BOOT_BOOT  0x01
#define BOOT_POST  0x02
#define BOOT_TS_CAL 0x04
#define BOOT_AUTO_BURN 0x08
#define BOOT_USB_LOADER 0x10


static uint32 getbootparam() {
    FIL file;
    unsigned int timemark;
    uint32 val, br;
    for (timemark = TimerTickGet();;) {
        if ((g_sUSBHCD[USB_INSTANCE_FOR_USBDISK].ulUSBHIntEvents & 0x02) != 0) {
            break;
        }
        if (TimerTickGet() >= timemark + 800) return 0;
    }

    timemark = TimerTickGet();
    while (1) {
        usbMscProcess();
        if ((TimerTickGet() - timemark) > FORCEBOOT_WAITTIME) {
            return 0;
        }
        if (g_usbMscState == USBMSC_DEVICE_READY) {
            FRESULT fret = f_open(&file, "2:/"FORCEBOOT_FILENAME, FA_READ);
            if (fret != FR_OK) {
                return 0;
            }
            fret = f_read(&file, &val, sizeof val,&br);
            if ((fret != FR_OK) || (br != sizeof val)) {
                f_close(&file);
                return BOOT_BOOT;
            }
            f_close(&file);
            return val | BOOT_BOOT;
        }
    }
}


FATFS inandfs;
BOOL isIDok;

extern BOOL isIDvailable(void);
extern void hmiInit(void);
extern void guiExec(void);
extern void probUdisk(void);
extern void displayUdisk(void);
extern void probIdisk_display(void);
extern void hmishow(void);
extern void shortcuthandler(int keycode);

extern mmcsdCtrlInfo mmcsdctr[2];


extern unsigned int boardErrFlag;

int main(void) {
    BlPlatformConfig();
    //LCDRasterStart();
    //LCDBackLightON(255);
    UARTPuts("Minde bootloader \n\r ", -1);
    int val = 0;
    uint32 bootparam = 0;
    isIDok = isIDvailable();
    bootparam = getbootparam();

#ifdef INNERBOOT
    bootparam |= BOOT_TS_CAL | BOOT_POST;
#endif

    if (!isIDok) {
        UARTPuts("ID error...\r\n\n", -1);
        goto BOOTLOADER;
    }
    
    if(boardErrFlag & BOARD_ERR_PICM){
        UARTPuts("Board Init PICM Error,Enter to bootloader..\r\n\n", -1);
        goto BOOTLOADER;
    }

    if (bootparam & BOOT_BOOT) {
        UARTPuts("Detect gotoboot flag,Enter to bootloader..\r\n\n", -1);
        goto BOOTLOADER;
    }

#ifndef INNERBOOT
    val = bootCopy();
    if (0 == val) {
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
    if (APP_ENTRY_ERROR == val) {
        UARTPuts("Application entry address error...\r\n\n", -1);
        goto BOOTLOADER;
    }
    if (APP_UNAVAILABLE == val) {
        UARTPuts("Application unavailable . Enter to bootloader...\r\n\n", -1);
        goto BOOTLOADER;
    }

    BOOTLOADER:
    LCDRasterStart();
    LCDBackLightON(255);
    registKeyHandler(shortcuthandler);
    if (bootparam & BOOT_POST) {
        UARTPuts("Detect POST flag,begin POST..\r\n\n", -1);
        post();
    }
    if (bootparam & BOOT_TS_CAL) {
        UARTPuts("Detect tscal flag,begin tscal..\r\n\n", -1);
        TouchCalibrate(1);
    } else {
        TouchCalibrate(0);
    }
#ifndef INNERBOOT
    if (bootparam & BOOT_USB_LOADER) {
        UARTPuts("Detect boot from usb flag, boot form usb file..\r\n\n", -1);
        booFromUsb();
    }
#endif
    hmiInit();
    f_mount(0, &inandfs);
    char errbuf[100]  = "ERROR:";
    if(boardErrFlag & BOARD_ERR_PICM){
      strcat(errbuf,"PICM");
      statBarPrint(1, errbuf); 
    }    
    while (1) {
        guiExec();
        usbMscProcess();
        probUdisk();
        displayUdisk();
        probIdisk_display();
        statLabelShowTime();
        tsc2007TouchProcess();
    }
}



