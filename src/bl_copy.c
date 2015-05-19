

#include "bl.h"
#include "hw_types.h"
#include "bl_platform.h"
#include "bl_copy.h"
#include "string.h"
#include "pf_platform.h"
#include "pf_bootloader.h"

extern void BootAbort(void);

extern int HSMMCSDInit(void);
extern unsigned int HSMMCSDImageCopy(void);


extern mmcsdCtrlInfo mmcsdctr[2];
extern mmcsdCardInfo card;

void copyInit(void) {
}


int bootCopy() {
    char bockbuf[512];
    RUNAPPHEAD *header = (RUNAPPHEAD *)bockbuf;
    int reval;
    reval = MMCSDP_Read(&mmcsdctr[0], header, APP_RUN_FLAG_SECTOR, 1);
    unsigned int imageaddr, imagelen;
    if (FALSE == reval) {
        return APP_COPY_ERROR;
    }
    if (header->image[0].imageCheck != APP_MAGIC_OK && header->image[1].imageCheck != APP_MAGIC_OK) {
        return APP_UNAVAILABLE;
    }
    imageaddr = header->image[header->iimage].imageCheck == APP_MAGIC_OK ?
                header->image[header->iimage].imageaddr
                : header->image[!header->iimage].imageaddr;
    imagelen = header->image[header->iimage].imageCheck == APP_MAGIC_OK ?
                header->image[header->iimage].imagelen
                : header->image[!header->iimage].imagelen;
    if (imageaddr % 512 != 0) {
        return APP_UNAVAILABLE;
    }
    if (SUCCESS == MMCSDP_Read(&mmcsdctr[0], (void *)APP_ENTRY, imageaddr / 512,
                               DIVUP(imagelen, 512))) {
        reval =  0;
    } else {
        reval =  APP_COPY_ERROR;
    }
    return reval;
}

BOOL isIDvailable() {
    unsigned int buf[512 / 4];
    memset32(buf, 0, sizeof buf / 4);
    unsigned int mac[4];
    mac[0] = ~HWREG(SOC_CONTROL_REGS + CONTROL_MAC_ID_LO(0));
    mac[1] = ~HWREG(SOC_CONTROL_REGS + CONTROL_MAC_ID_HI(0));
    mac[2] = ~HWREG(SOC_CONTROL_REGS + CONTROL_MAC_ID_LO(1));
    mac[3] = ~HWREG(SOC_CONTROL_REGS + CONTROL_MAC_ID_HI(1));
    MMCSDP_Read(mmcsdctr, buf, UID_SAVE_SECTOR, 1);
    if ((buf[0] != mac[0]) || (buf[1] != mac[1]) || (buf[2] != mac[2]) || (buf[3] != mac[3])) {
        return FALSE;
    }
    return TRUE;
}


void jumptoApp() {
    UARTPuts("Jumping to application...\r\n\n", -1);
    ((void (*)())APP_ENTRY)();
}




