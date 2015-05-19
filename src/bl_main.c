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


#define FORCEBOOT_WAITTIME  2000
#define FORCEBOOT_FILENAME  "forboot.dat"



unsigned int entryPoint = 0;
unsigned int DspEntryPoint = 0;

extern tUSBHCD g_sUSBHCD[];


#define BOOT_BOOT  0x01
#define BOOT_POST  0x02
#define BOOT_TS_CAL 0x04

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
            return  val | BOOT_BOOT;
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


int main(void) {
    BlPlatformConfig();
    UARTPuts("Minde bootloader \n\r ", -1);
    int val = 0;
    uint32 bootparam = 0;
    isIDok = isIDvailable();
    if (!isIDok) {
        UARTPuts("ID error...\r\n\n", -1);
        goto BOOTLOADER;
    }

    //goto BOOTLOADER;
    bootparam = getbootparam();
    if (bootparam & BOOT_BOOT) {
        goto BOOTLOADER;
    }


//#ifndef INNERBOOT
    val = bootCopy();
    if (0 == val) {
        jumptoApp();
    }
//#endif

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
    registKeyHandler(shortcuthandler);
    //xo2BurnInit();
    LCDRasterStart();
    LCDBackLightON(255);
    if (bootparam & BOOT_POST) {
        post();
    }
    TouchCalibrate(!!(bootparam & BOOT_TS_CAL));
    //TouchCalibrate(0);
    hmiInit();
    f_mount(0, &inandfs);
    while (1) {
        guiExec();
        usbMscProcess();
        probUdisk();
        displayUdisk();
        probIdisk_display();
        statLabelShowTime();
    }
}



