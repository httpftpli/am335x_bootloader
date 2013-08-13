#ifndef __PLATFORM_CFG_H___
#define __PLATFORM_CFG_H___

#include "soc_AM335x.h"
#include "module.h"

//set witch usb instance use for udist : 0 or 1
#define USB_INSTANCE_FOR_USBDISK   0
//define USBDISK logic volume
#define FatFS_Drive_Index          2
//set if use use dma or pull
#define USB_USE_CPPI41DMA          0


//bellow config interrupt priority

#define INT_PRIORITY_MAX          (0x7f)
#define INT_PRIORITY_MIN           (0)
#define INT_PRIORITY_USER_NORMAL  (0x30)
#define INT_PRIORITY_USER_PRI     (0x10)
#define INT_PRIORITY_DMA_COMP     (INT_PRIORITY_USER_NORMAL - 3)
#define INT_PRIORITY_DMA_ERROR    (INT_PRIORITY_USER_NORMAL - 4) 
#define INT_PRIORITY_MMCSD0       (INT_PRIORITY_USER_NORMAL-2)
#define INT_PRIORITY_MMCSD1       (INT_PRIORITY_USER_NORMAL-1)
#define INT_PRIORITY_ADCTSC       (INT_PRIORITY_USER_NORMAL-1)
#define INT_PRIORITY_RTC          (INT_PRIORITY_USER_NORMAL-5)
#define INT_PRIORITY_DCAN0_LINE0  (INT_PRIORITY_USER_NORMAL-6)
#define INT_PRIORITY_DCAN1_LINE0  (INT_PRIORITY_USER_NORMAL-6)
#define INT_PRIORITY_TIMER0       (INT_PRIORITY_USER_NORMAL-7)
#define INT_PRIORITY_TIMER1       (INT_PRIORITY_USER_NORMAL-7)
#define INT_PRIORITY_TIMER2       (INT_PRIORITY_USER_NORMAL-7)
#define INT_PRIORITY_TIMER3       (INT_PRIORITY_USER_NORMAL-7)
#define INT_PRIORITY_TIMER4       (INT_PRIORITY_USER_NORMAL-7)
#define INT_PRIORITY_TIMER5       (INT_PRIORITY_USER_NORMAL-7)
#define INT_PRIORITY_TIMER6       (INT_PRIORITY_USER_NORMAL-7)
#define INT_PRIORITY_TIMER7       (INT_PRIORITY_USER_NORMAL-7)
#define INT_PRIORITY_EQEP2         (INT_PRIORITY_USER_NORMAL-8)
#define INT_PRIORITY_USB0        (INT_PRIORITY_USER_NORMAL-9)
#define INT_PRIORITY_USB1        (INT_PRIORITY_USER_NORMAL-10)
#if USB_USE_CPPI41DMA
#define INT_PRIORITY_USB_DMA     (INT_PRIORITY_USER_NORMAL-11)
#endif

#define INT_PRIORITY_GPIO0       (INT_PRIORITY_USER_NORMAL-12)
#define INT_PRIORITY_GPIO1       (INT_PRIORITY_USER_NORMAL-13)
#define INT_PRIORITY_GPIO2       (INT_PRIORITY_USER_NORMAL-14)
#define INT_PRIORITY_GPIO3       (INT_PRIORITY_USER_NORMAL-15)
#define INT_PRIORITY_LCD         (INT_PRIORITY_USER_NORMAL-16)

#define INT_PRIORITY_eCAP0       (INT_PRIORITY_USER_NORMAL-16)
#define INT_PRIORITY_eCAP1       (INT_PRIORITY_USER_NORMAL-16)
#define INT_PRIORITY_eCAP2       (INT_PRIORITY_USER_NORMAL-16)
#define INT_PRIORITY_UART1       (INT_PRIORITY_USER_NORMAL-16)
#define INT_PRIORITY_UART2       (INT_PRIORITY_USER_NORMAL-16)
#define INT_PRIORITY_UART3       (INT_PRIORITY_USER_NORMAL-16)
#define INT_PRIORITY_UART4       (INT_PRIORITY_USER_NORMAL-16)
#define INT_PRIORITY_UART5       (INT_PRIORITY_USER_NORMAL-16)
#define INT_PRIORITY_SPI0        (INT_PRIORITY_USER_NORMAL-17)
#define INT_PRIORITY_SPI1        (INT_PRIORITY_USER_NORMAL-18)



//config GPMC

//config whitch cs to be usbed
//bit map of CS use ,bit0 for CS0;bit1 for CS1;BIT6 for CS6
#define CS_USE   0x7f 

/*config cs bank baseAddr ,baseaddr  decoder from
 to A29, A28, A27, A26, A25, and A24 ,so baseAddr should 24 bit align*/
static const unsigned int CS_BASEADDR[7]= {1<<24,2<<24,3<<24,4<<24,5<<24,6<<24,7<<24}; 
/*config address line width ,7 demeasion for 7 CS*/
static const unsigned int CS_ADDRLINE_BIT[7] = {24,24,24,24,24,24,24};

//config timetick
//config witch timer used for timertick
#define TIMER_TIMERTICK   MODULE_ID_TIMER2


//config LCD backlight control gpio
#define GPIO_LCDBACKLIGHT_BASE   SOC_GPIO_0_REGS
#define GPIO_LCDBACKLIGHT_PIN    30
#define LCD_BACKLIG_ON           0  
#define LCD_BACKLIG_OFF          1

#define TFT_AT080TN52    0
#define TFT_AT070TN92    1
#define TFT_LSA40AT9001  2
//select TFT panel 
#define TFT_PANEL    TFT_LSA40AT9001  


#define UCS16_CODEC  1
#define ASCII_CODEC  2
#define UTF8_CODEC   3
#define CHARACTER_DIS_CODEC  ASCII_CODEC


#define ASCII_FONT16_IN_SOURCE     1
#define ASCII_FONT20_IN_SOURCE     0


#define BEEP_OFF() (void)0


#endif



