
#ifndef __CPLD__H__
#define __CPLD__H__

#include "pf_platform_cfg.h"
#include "io_macros.h"

#define GPMC_BANK0(A) (*(volatile unsigned char *)(CS_BASEADDR[0]+(A)))
#define GPMC_BANK1(A) (*(volatile unsigned char *)(CS_BASEADDR[1]+(A)))
#define GPMC_BANK2(A) (*(volatile unsigned char *)(CS_BASEADDR[2]+(A)))
#define GPMC_BANK3(A) (*(volatile unsigned char *)(CS_BASEADDR[3]+(A)))
#define GPMC_BANK4(A) (*(volatile unsigned char *)(CS_BASEADDR[4]+(A)))
#define GPMC_BANK5(A) (*(volatile unsigned char *)(CS_BASEADDR[5]+(A)))
#define GPMC_BANK6(A) (*(volatile unsigned char *)(CS_BASEADDR[6]+(A)))

#define JBMQ_BASE      (CS_BASEADDR[0])
#define SEL0_BASE	   (CS_BASEADDR[1] + 0)
#define SEL1_BASE	   (CS_BASEADDR[1] + 0X200)
#define SEL2_BASE	   (CS_BASEADDR[1] + 0X400)
#define SEL3_BASE	   (CS_BASEADDR[1] + 0X600)
#define SEL4_BASE	   (CS_BASEADDR[1] + 0X800)
#define SEL5_BASE	   (CS_BASEADDR[1] + 0XA00)
#define OUT_BASE       (CS_BASEADDR[2])
#define ERR_BASE       (CS_BASEADDR[3])

typedef struct                          
{
    __REG8 Data[256];     //选针器发送数据
    __REG8 Wr_En;         //写使能
    __REG8 Rd_Mode;       //读取模式 0:正常输出 1:MCU读取
    __REG8 Numbs;         //选针器输出个数
    __REG8 Base;          //选针器输出地址起始
	__REG8 SendEn;        //选针器发送标记
} SEL_REG_TypeDef;

//绝对值编码器寄存器
typedef struct
{
    __REG8 BMQV1_L;        //编码器值
    __REG8 BMQV1_H;
    __REG8 ZVAL1_L;        //针中断值1
    __REG8 ZVAL1_H;
    __REG8 ZVAL2_L;        //针中断值2
    __REG8 ZVAL2_H;
    union                  //针中断使能
	{
      struct 
      {
          __REG8 EN: 1;
          __REG8 REV5 : 7;
      };
	};
}JBMQ_REG_TypeDef;

typedef struct
{
	union
	{
      struct 
      {
          __REG8 HAND : 1;
          __REG8 OIL  : 1;
          __REG8 FUN  : 1;
          __REG8 TLT  : 1;
          __REG8 BLT  : 1;
          __REG8 MOTO : 1;
          __REG8 STOP : 1;
          __REG8 RUN  : 1;

      };
      __REG8 VAL;
	};
}LED_REG_TypeDef;

typedef struct
{
    __REG8 KEY_COL;
    __REG8 KEY_DATA;
}KEY_REG_TypeDef;

typedef struct
{
    __REG8 LCD_PWM;
}LCD_REG_TypeDef;

typedef struct
{
	union
	{
      struct 
      {
          __REG8 SEL1_BUSY  : 1;
          __REG8 SEL2_BUSY  : 1;
          __REG8 SEL3_BUSY  : 1;
          __REG8 SEL4_BUSY  : 1;
          __REG8 SEL5_BUSY  : 1;
          __REG8 SEL6_BUSY  : 1;
          __REG8 JBMQ_ERRO  : 1;
          __REG8 REVER      : 1;

      };
      __REG8 VAL;
	};
}ERR_REG_TypeDef;

typedef struct
{
	union
	{
      struct 
      {
          __REG8 ERR_ARLARM  : 1;
          __REG8 ERR_ELECTI  : 1;
          __REG8             : 6;
      };
      __REG8 VAL;
	};
}JDQ_REG_TypeDef;

#define SEL0_REG       ((SEL_REG_TypeDef    *) SEL0_BASE   )
#define SEL1_REG       ((SEL_REG_TypeDef    *) SEL1_BASE   )
#define SEL2_REG       ((SEL_REG_TypeDef    *) SEL2_BASE   )
#define SEL3_REG       ((SEL_REG_TypeDef    *) SEL3_BASE   )
#define SEL4_REG       ((SEL_REG_TypeDef    *) SEL4_BASE   )
#define SEL5_REG       ((SEL_REG_TypeDef    *) SEL5_BASE   )

#define JBMQ_REG       ((JBMQ_REG_TypeDef   *) JBMQ_BASE   )

#define INEN_REG       (*((__REG8           *) (OUT_BASE + 0X00)))    
#define LED_REG        ((LED_REG_TypeDef    *) (OUT_BASE + 0X10))
#define KEY_REG        ((KEY_REG_TypeDef    *) (OUT_BASE + 0X20))
#define JDQ_REG        ((JDQ_REG_TypeDef    *) (OUT_BASE + 0X30))
#define LCD_REG        ((LCD_REG_TypeDef    *) (OUT_BASE + 0X40))

#define ERR_REG        ((ERR_REG_TypeDef    *) (ERR_BASE + 0X00))

#endif
