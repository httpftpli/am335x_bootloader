#include "bl_copy.h"
#include "bl_platform.h"
#include "bl.h"
#include "type.h"
#include "platform.h"
#include "usblib.h"
#include "usbhost.h"
#include "pf_rtc.h"
#include <time.h>
#include "cpld.h"
#include "pf_timertick.h"


#define FORCEBOOT_WAITTIME  2000
#define FORCEBOOT_FILENAME  "forboot.dat"
#define UDISK_USBINDEX      0




unsigned int entryPoint = 0;
unsigned int DspEntryPoint = 0;

extern tUSBHCD g_sUSBHCD[];
static BOOL forceEnterBoot() {
    FIL file;
    delay(350);
    if ((g_sUSBHCD[UDISK_USBINDEX].ulUSBHIntEvents & 0x02) == 0) {
        return false;
    } else {
        unsigned int timemark = TimerTickGet();
        while (1) {
            usbMscProcess();
            if ((TimerTickGet() - timemark) > FORCEBOOT_WAITTIME) {
                return false;
            }
            if (g_usbMscState == USBMSC_DEVICE_READY) {
                FRESULT fret = f_open(&file, "2:/"FORCEBOOT_FILENAME, FA_READ);
                if (fret == FR_OK) {
                    f_close(&file);
                    return true;
                } else {
                    return false;
                }
            }
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

#ifdef YUANJI
void __lcd_back_ligth_ctr(unsigned char lightpwm)
{
	LCD_REG->LCD_PWM = lightpwm;
}
#else
void __lcd_back_ligth_ctr(unsigned char lightpwm) {
    unsigned char buf[8] = { 0xbb, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0d };
    buf[2] = lightpwm;
    while (!UARTSendNoBlock(UART_LCDBACKLIGHT_MODULE, buf, sizeof buf));
}
#endif

int main(void) {
    BlPlatformConfig();
    UARTPuts("Minde bootloader \n\r ", -1);
    int val = 0;
    isIDok = isIDvailable();
    if(!isIDok){
       UARTPuts("ID error...\r\n\n", -1);
       goto BOOTLOADER;
    }

    //goto BOOTLOADER;

    if (forceEnterBoot()) {
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
    registKeyHandler(shortcuthandler);
    //xo2BurnInit();
    LCDRasterStart();
    LCDBackLightON(255);
    post();
    TouchCalibrate(0);
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



