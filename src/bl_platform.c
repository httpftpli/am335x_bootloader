
#include "hw_types.h"
#include "hw_cm_cefuse.h"
#include "hw_cm_device.h"
#include "hw_cm_dpll.h"
#include "hw_cm_gfx.h"
#include "hw_cm_mpu.h"
#include "hw_cm_per.h"
#include "hw_cm_rtc.h"
#include "hw_cm_wkup.h"
#include "hw_control_AM335x.h"
#include "hw_emif4d.h"
#include "bl.h"
#include "gpmc.h"
#include "bl_copy.h"
#include "pf_platform_cfg.h"  
#include "bl_platform.h"
#include "uartStdio.h"
#include "watchdog.h"
#include "hsi2c.h"
#include "gpio_v2.h"
#include "platform.h"
#include "hw_tps65217.h"
#include "uartstdio.h"
#include "soc_AM335X.h"
#include "pf_hsi2c.h"
#include "pf_usbmsc.h"
#include "pf_timertick.h"
#include "pf_mux.h"
#include "interrupt.h"
#include "pf_int.h"
#include "pf_uart.h"
#include "pf_lcd.h"
#include "mmcsd_proto.h"
#include "pf_tsc.h"
#include "module.h"
#include "mmath.h"


/******************************************************************************
**                     Internal Macro Definitions
*******************************************************************************/
#define INTVECMAX                          (9)
#define PAD_CTRL_PULLUDDISABLE             (BIT(3))
#define PAD_CTRL_PULLUPSEL                 (BIT(4))
#define PAD_CTRL_RXACTIVE                  (BIT(5))
#define PAD_CTRL_SLOWSLEW                  (BIT(6))
#define PAD_CTRL_MUXMODE(n)                ((n))

/*	I2C instance	*/
#define  I2C_0				   (0x0u)
/*	System clock fed to I2C module - 48Mhz	*/
#define  I2C_SYSTEM_CLOCK		   (48000000u)
/*	Internal clock used by I2C module - 12Mhz	*/
#define  I2C_INTERNAL_CLOCK		   (12000000u)
/*	I2C bus speed or frequency - 100Khz	*/
#define	 I2C_OUTPUT_CLOCK		   (100000u)
/*	I2C interrupt flags to clear	*/
#define  I2C_INTERRUPT_FLAG_TO_CLR         (0x7FF)
#define  PMIC_SR_I2C_SLAVE_ADDR            (0x12)
#define SMPS_DRIVE_SCLSR_EN1	           (0x0u)
#define SMPS_DRIVE_SDASR_EN2	           (0x1u)

/* TODO : These are not there in the control module header file */
#define DDR_PHY_CTRL_REGS                  (SOC_CONTROL_REGS + 0x2000)
#define CMD0_SLAVE_RATIO_0                 (DDR_PHY_CTRL_REGS + 0x1C)
#define CMD0_SLAVE_FORCE_0                 (DDR_PHY_CTRL_REGS + 0x20)
#define CMD0_SLAVE_DELAY_0                 (DDR_PHY_CTRL_REGS + 0x24)
#define CMD0_LOCK_DIFF_0                   (DDR_PHY_CTRL_REGS + 0x28)
#define CMD0_INVERT_CLKOUT_0               (DDR_PHY_CTRL_REGS + 0x2C)
#define CMD1_SLAVE_RATIO_0                 (DDR_PHY_CTRL_REGS + 0x50)
#define CMD1_SLAVE_FORCE_0                 (DDR_PHY_CTRL_REGS + 0x54)
#define CMD1_SLAVE_DELAY_0                 (DDR_PHY_CTRL_REGS + 0x58)
#define CMD1_LOCK_DIFF_0                   (DDR_PHY_CTRL_REGS + 0x5C)
#define CMD1_INVERT_CLKOUT_0               (DDR_PHY_CTRL_REGS + 0x60)
#define CMD2_SLAVE_RATIO_0                 (DDR_PHY_CTRL_REGS + 0x84)
#define CMD2_SLAVE_FORCE_0                 (DDR_PHY_CTRL_REGS + 0x88)
#define CMD2_SLAVE_DELAY_0                 (DDR_PHY_CTRL_REGS + 0x8C)
#define CMD2_LOCK_DIFF_0                   (DDR_PHY_CTRL_REGS + 0x90)
#define CMD2_INVERT_CLKOUT_0               (DDR_PHY_CTRL_REGS + 0x94)
#define DATA0_RD_DQS_SLAVE_RATIO_0         (DDR_PHY_CTRL_REGS + 0xC8)
#define DATA0_RD_DQS_SLAVE_RATIO_1         (DDR_PHY_CTRL_REGS + 0xCC)
#define DATA0_WR_DQS_SLAVE_RATIO_0         (DDR_PHY_CTRL_REGS + 0xDC)
#define DATA0_WR_DQS_SLAVE_RATIO_1         (DDR_PHY_CTRL_REGS + 0xE0)
#define DATA0_WRLVL_INIT_RATIO_0           (DDR_PHY_CTRL_REGS + 0xF0)
#define DATA0_WRLVL_INIT_RATIO_1           (DDR_PHY_CTRL_REGS + 0xF4)
#define DATA0_GATELVL_INIT_RATIO_0         (DDR_PHY_CTRL_REGS + 0xFC)
#define DATA0_GATELVL_INIT_RATIO_1         (DDR_PHY_CTRL_REGS + 0x100)
#define DATA0_FIFO_WE_SLAVE_RATIO_0        (DDR_PHY_CTRL_REGS + 0x108)
#define DATA0_FIFO_WE_SLAVE_RATIO_1        (DDR_PHY_CTRL_REGS + 0x10C)
#define DATA0_WR_DATA_SLAVE_RATIO_0        (DDR_PHY_CTRL_REGS + 0x120)
#define DATA0_WR_DATA_SLAVE_RATIO_1        (DDR_PHY_CTRL_REGS + 0x124)
#define DATA0_USE_RANK0_DELAYS_0           (DDR_PHY_CTRL_REGS + 0x134)
#define DATA0_LOCK_DIFF_0                  (DDR_PHY_CTRL_REGS + 0x138)
#define DATA1_RD_DQS_SLAVE_RATIO_0         (DDR_PHY_CTRL_REGS + 0x16c)
#define DATA1_RD_DQS_SLAVE_RATIO_1         (DDR_PHY_CTRL_REGS + 0x170)
#define DATA1_WR_DQS_SLAVE_RATIO_0         (DDR_PHY_CTRL_REGS + 0x180)
#define DATA1_WR_DQS_SLAVE_RATIO_1         (DDR_PHY_CTRL_REGS + 0x184)
#define DATA1_WRLVL_INIT_RATIO_0           (DDR_PHY_CTRL_REGS + 0x194)
#define DATA1_WRLVL_INIT_RATIO_1           (DDR_PHY_CTRL_REGS + 0x198)
#define DATA1_GATELVL_INIT_RATIO_0         (DDR_PHY_CTRL_REGS + 0x1a0)
#define DATA1_GATELVL_INIT_RATIO_1         (DDR_PHY_CTRL_REGS + 0x1a4)
#define DATA1_FIFO_WE_SLAVE_RATIO_0        (DDR_PHY_CTRL_REGS + 0x1ac)
#define DATA1_FIFO_WE_SLAVE_RATIO_1        (DDR_PHY_CTRL_REGS + 0x1b0)
#define DATA1_WR_DATA_SLAVE_RATIO_0        (DDR_PHY_CTRL_REGS + 0x1c4)
#define DATA1_WR_DATA_SLAVE_RATIO_1        (DDR_PHY_CTRL_REGS + 0x1c8)
#define DATA1_USE_RANK0_DELAYS_0           (DDR_PHY_CTRL_REGS + 0x1d8)
#define DATA1_LOCK_DIFF_0                  (DDR_PHY_CTRL_REGS + 0x1dc)


/* DDR3 init values */
#define DDR3_CMD0_SLAVE_RATIO_0            (0x40)
#define DDR3_CMD0_INVERT_CLKOUT_0          (0x1)
#define DDR3_CMD1_SLAVE_RATIO_0            (0x40)
#define DDR3_CMD1_INVERT_CLKOUT_0          (0x1)
#define DDR3_CMD2_SLAVE_RATIO_0            (0x40)
#define DDR3_CMD2_INVERT_CLKOUT_0          (0x1)

#define DDR3_DATA0_RD_DQS_SLAVE_RATIO_0    (0x3B)
#define DDR3_DATA0_WR_DQS_SLAVE_RATIO_0    (0x85)
#define DDR3_DATA0_FIFO_WE_SLAVE_RATIO_0   (0x100)
#define DDR3_DATA0_WR_DATA_SLAVE_RATIO_0   (0xC1)

#define DDR3_DATA0_RD_DQS_SLAVE_RATIO_1    (0x3B)
#define DDR3_DATA0_WR_DQS_SLAVE_RATIO_1    (0x85)
#define DDR3_DATA0_FIFO_WE_SLAVE_RATIO_1   (0x100)
#define DDR3_DATA0_WR_DATA_SLAVE_RATIO_1   (0xC1)

#define DDR3_CONTROL_DDR_CMD_IOCTRL_0      (0x18B)
#define DDR3_CONTROL_DDR_CMD_IOCTRL_1      (0x18B)
#define DDR3_CONTROL_DDR_CMD_IOCTRL_2      (0x18B)

#define DDR3_CONTROL_DDR_DATA_IOCTRL_0      (0x18B)
#define DDR3_CONTROL_DDR_DATA_IOCTRL_1      (0x18B)

//#define DDR3_CONTROL_DDR_IO_CTRL           (0x0fffffff)
#define DDR3_CONTROL_DDR_IO_CTRL           (0xefffffff)

#define DDR3_EMIF_DDR_PHY_CTRL_1           (0x06)
#define DDR3_EMIF_DDR_PHY_CTRL_1_SHDW      (0x06)
#define DDR3_EMIF_DDR_PHY_CTRL_2           (0x06)

#define DDR3_EMIF_SDRAM_TIM_1              (0x0888A39B)
#define DDR3_EMIF_SDRAM_TIM_1_SHDW         (0x0888A39B)

#define DDR3_EMIF_SDRAM_TIM_2              (0x26337FDA)
#define DDR3_EMIF_SDRAM_TIM_2_SHDW         (0x26337FDA)

#define DDR3_EMIF_SDRAM_TIM_3              (0x501F830F)
#define DDR3_EMIF_SDRAM_TIM_3_SHDM         (0x501F830F)

#define DDR3_EMIF_SDRAM_REF_CTRL_VAL1      (0x0000093B)
#define DDR3_EMIF_SDRAM_REF_CTRL_SHDW_VAL1 (0x0000093B)

#define DDR3_EMIF_ZQ_CONFIG_VAL            (0x50074BE4)
#define DDR3_EMIF_SDRAM_CONFIG             (0x61C04AB2)//termination = 1 (RZQ/4)
                                                       //dynamic ODT = 2 (RZQ/2)
                                                       //SDRAM drive = 0 (RZQ/6)
                                                       //CWL = 0 (CAS write latency = 5)
                                                       //CL = 2 (CAS latency = 5)
                                                       //ROWSIZE = 5 (14 row bits)
                                                       //PAGESIZE = 2 (10 column bits)
													   
/* DDR2 init values */

#define DDR2_CMD0_SLAVE_RATIO_0            (0x80)
#define DDR2_CMD0_SLAVE_FORCE_0            (0x0)
#define DDR2_CMD0_SLAVE_DELAY_0            (0x0)
#define DDR2_CMD0_LOCK_DIFF_0              (0x4)
#define DDR2_CMD0_INVERT_CLKOUT_0          (0x0)

#define DDR2_CMD1_SLAVE_RATIO_0            (0x80)
#define DDR2_CMD1_SLAVE_FORCE_0            (0x0)
#define DDR2_CMD1_SLAVE_DELAY_0            (0x0)
#define DDR2_CMD1_LOCK_DIFF_0              (0x4)
#define DDR2_CMD1_INVERT_CLKOUT_0          (0x0)

#define DDR2_CMD2_SLAVE_RATIO_0            (0x80)
#define DDR2_CMD2_SLAVE_FORCE_0            (0x0)
#define DDR2_CMD2_SLAVE_DELAY_0            (0x0)
#define DDR2_CMD2_LOCK_DIFF_0              (0x4)
#define DDR2_CMD2_INVERT_CLKOUT_0          (0x0)

#define DDR2_DATA0_RD_DQS_SLAVE_RATIO_0    ((0x40 << 30)|(0x40 << 20)|(0x40 << 10)|(0x40 << 0))
#define DDR2_DATA0_RD_DQS_SLAVE_RATIO_1    (0x40 >> 2)
#define DDR2_DATA0_WR_DQS_SLAVE_RATIO_0    (( 0x00 << 30)|( 0x00 << 20)|(0x00 << 10)|(0x00 << 0))
#define DDR2_DATA0_WR_DQS_SLAVE_RATIO_1    (0x00 >> 2)
#define DDR2_DATA0_WRLVL_INIT_RATIO_0      ((0x00 << 30)|(0x00 << 20)|(0x00 << 10)|(0x00 << 0)) 
#define DDR2_DATA0_WRLVL_INIT_RATIO_1      (0x00 >> 2)
#define DDR2_DATA0_GATELVL_INIT_RATIO_0    ((0x00 << 30)|(0x00 << 20)|(0x00 << 10)|(0x00 << 0))
#define DDR2_DATA0_GATELVL_INIT_RATIO_1    (0x00 >> 2)
#define DDR2_DATA0_FIFO_WE_SLAVE_RATIO_0   ((0x56 << 30)|(0x56 << 20)|(0x56<< 10)|(0x56 << 0))
#define DDR2_DATA0_FIFO_WE_SLAVE_RATIO_1   (0x56 >> 2)
#define DDR2_DATA0_WR_DATA_SLAVE_RATIO_0   (0x40 << 30)|(0x40 << 20)|(0x40 << 10)|(0x40 << 0)
#define DDR2_DATA0_WR_DATA_SLAVE_RATIO_1   (0x40 >> 2)
#define DDR2_DATA0_LOCK_DIFF_0             (0x00)

#define DDR2_DATA1_RD_DQS_SLAVE_RATIO_0    ((0x40 << 30)|(0x40 << 20)|(0x40 << 10)|(0x40 << 0))
#define DDR2_DATA1_RD_DQS_SLAVE_RATIO_1    (0x40 >> 2)
#define DDR2_DATA1_WR_DQS_SLAVE_RATIO_0    (( 0x00 << 30)|( 0x00 << 20)|(0x00 << 10)|(0x00 << 0))
#define DDR2_DATA1_WR_DQS_SLAVE_RATIO_1    (0x00 >> 2)
#define DDR2_DATA1_WRLVL_INIT_RATIO_0      ((0x00 << 30)|(0x00 << 20)|(0x00 << 10)|(0x00 << 0)) 
#define DDR2_DATA1_WRLVL_INIT_RATIO_1      (0x00 >> 2)
#define DDR2_DATA1_GATELVL_INIT_RATIO_0    ((0x00 << 30)|(0x00 << 20)|(0x00 << 10)|(0x00 << 0))
#define DDR2_DATA1_GATELVL_INIT_RATIO_1    (0x00 >> 2)
#define DDR2_DATA1_FIFO_WE_SLAVE_RATIO_0   ((0x56 << 30)|(0x56 << 20)|(0x56<< 10)|(0x56 << 0))
#define DDR2_DATA1_FIFO_WE_SLAVE_RATIO_1   (0x56 >> 2)
#define DDR2_DATA1_WR_DATA_SLAVE_RATIO_0   (0x40 << 30)|(0x40 << 20)|(0x40 << 10)|(0x40 << 0)
#define DDR2_DATA1_WR_DATA_SLAVE_RATIO_1   (0x40 >> 2)
#define DDR2_DATA1_LOCK_DIFF_0             (0x00)

#define DDR2_DATA0_USE_RANK0_DELAYS_0      (1)
#define DDR2_DATA1_USE_RANK0_DELAYS_0      (1)

#define DDR2_CONTROL_DDR_CMD_IOCTRL_0      (0x18B)
#define DDR2_CONTROL_DDR_CMD_IOCTRL_1      (0x18B)
#define DDR2_CONTROL_DDR_CMD_IOCTRL_2      (0x18B)
#define DDR2_CONTROL_DDR_DATA_IOCTRL_0     (0x18B)
#define DDR2_CONTROL_DDR_DATA_IOCTRL_1     (0x18B)

#define DDR2_CONTROL_DDR_IO_CTRL           (0x0fffffff)
#define DDR2_EMIF_DDR_PHY_CTRL_1           (0x05)
#define DDR2_EMIF_DDR_PHY_CTRL_1_SHDW      (0x05)
#define DDR2_EMIF_DDR_PHY_CTRL_2           (0x05)

#define DDR2_EMIF_SDRAM_TIM_1		   (0x0666B3D6)
#define DDR2_EMIF_SDRAM_TIM_1_SHDW         (0x0666B3D6)
#define DDR2_EMIF_SDRAM_TIM_2              (0x143731DA)
#define DDR2_EMIF_SDRAM_TIM_2_SHDW	   (0x143731DA)
#define DDR2_EMIF_SDRAM_TIM_3              (0x00000347)
#define DDR2_EMIF_SDRAM_TIM_3_SHDM         (0x00000347)
//CAS Latency:4 Row Size:15 Bank:8 Page Size:1024(word)   
#define DDR2_EMIF_SDRAM_CONFIG             (0x41805332) 
#define DDR2_EMIF_SDRAM_REF_CTRL_VAL1	   (0x00004650)
#define DDR2_EMIF_SDRAM_REF_CTRL_SHDW_VAL1 (0x00004650)

#define DDR2_EMIF_SDRAM_REF_CTRL_VAL2	   (0x0000081a)
#define DDR2_EMIF_SDRAM_REF_CTRL_SHDW_VAL2 (0x0000081a)

#define GPIO_INSTANCE_PIN_NUMBER      (7)



/* \brief This function initializes the CORE PLL 
 * 
 * \param none
 *
 * \return none
 *
 */
void CorePLLInit(void)
{
    volatile unsigned int regVal = 0;

    /* Enable the Core PLL */

    /* Put the PLL in bypass mode */
    regVal = HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKMODE_DPLL_CORE) &
                ~CM_WKUP_CM_CLKMODE_DPLL_CORE_DPLL_EN;

    regVal |= CM_WKUP_CM_CLKMODE_DPLL_CORE_DPLL_EN_DPLL_MN_BYP_MODE;

    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKMODE_DPLL_CORE) = regVal;

    while(!(HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_IDLEST_DPLL_CORE) &
                      CM_WKUP_CM_IDLEST_DPLL_CORE_ST_MN_BYPASS));

    /* Set the multipler and divider values for the PLL */
    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKSEL_DPLL_CORE) =
        ((COREPLL_M << CM_WKUP_CM_CLKSEL_DPLL_CORE_DPLL_MULT_SHIFT) |
         (COREPLL_N << CM_WKUP_CM_CLKSEL_DPLL_CORE_DPLL_DIV_SHIFT));

    /* Configure the High speed dividers */
    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_DIV_M4_DPLL_CORE) &=
                   ~CM_WKUP_CM_DIV_M4_DPLL_CORE_HSDIVIDER_CLKOUT1_DIV;
    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_DIV_M4_DPLL_CORE) |=
      (COREPLL_HSD_M4 << CM_WKUP_CM_DIV_M4_DPLL_CORE_HSDIVIDER_CLKOUT1_DIV_SHIFT);

    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_DIV_M5_DPLL_CORE) &=
                  ~CM_WKUP_CM_DIV_M5_DPLL_CORE_HSDIVIDER_CLKOUT2_DIV;
    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_DIV_M5_DPLL_CORE) |=
        (COREPLL_HSD_M5 << CM_WKUP_CM_DIV_M5_DPLL_CORE_HSDIVIDER_CLKOUT2_DIV_SHIFT);


    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_DIV_M6_DPLL_CORE) &=
                                ~CM_WKUP_CM_DIV_M6_DPLL_CORE_HSDIVIDER_CLKOUT3_DIV;
    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_DIV_M6_DPLL_CORE) |=
        (COREPLL_HSD_M6 << CM_WKUP_CM_DIV_M6_DPLL_CORE_HSDIVIDER_CLKOUT3_DIV_SHIFT);

    /* Now LOCK the PLL by enabling it */
    regVal = HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKMODE_DPLL_CORE) &
                ~CM_WKUP_CM_CLKMODE_DPLL_CORE_DPLL_EN;

    regVal |= CM_WKUP_CM_CLKMODE_DPLL_CORE_DPLL_EN;

    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKMODE_DPLL_CORE) = regVal;

    while(!(HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_IDLEST_DPLL_CORE) &
                        CM_WKUP_CM_IDLEST_DPLL_CORE_ST_DPLL_CLK));
}

/* \brief This function initializes the DISPLAY PLL
 * 
 * \param none
 *
 * \return none
 *
 */
void DisplayPLLInit(void)
{
    volatile unsigned int regVal = 0;

    /* Put the PLL in bypass mode */
    regVal = HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKMODE_DPLL_DISP) &
                ~CM_WKUP_CM_CLKMODE_DPLL_DISP_DPLL_EN;

    regVal |= CM_WKUP_CM_CLKMODE_DPLL_DISP_DPLL_EN_DPLL_MN_BYP_MODE;

    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKMODE_DPLL_DISP) = regVal;

    while(!(HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_IDLEST_DPLL_DISP) &
                        CM_WKUP_CM_IDLEST_DPLL_DISP_ST_MN_BYPASS));

    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKSEL_DPLL_DISP) &=
                           ~(CM_WKUP_CM_CLKSEL_DPLL_DISP_DPLL_DIV |
                             CM_WKUP_CM_CLKSEL_DPLL_DISP_DPLL_MULT);

    /* Set the multipler and divider values for the PLL */
    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKSEL_DPLL_DISP) |=
        ((DISPLL_M << CM_WKUP_CM_CLKSEL_DPLL_DISP_DPLL_MULT_SHIFT) |
         (DISPLL_N << CM_WKUP_CM_CLKSEL_DPLL_DISP_DPLL_DIV_SHIFT));

    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_DIV_M2_DPLL_DISP) &=
                        ~CM_WKUP_CM_DIV_M2_DPLL_DISP_DPLL_CLKOUT_DIV;
    /* Set the CLKOUT2 divider */
    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_DIV_M2_DPLL_DISP) |= DISPLL_M2;

    /* Now LOCK the PLL by enabling it */
    regVal = HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKMODE_DPLL_DISP) &
                ~CM_WKUP_CM_CLKMODE_DPLL_DISP_DPLL_EN;

    regVal |= CM_WKUP_CM_CLKMODE_DPLL_DISP_DPLL_EN;

    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKMODE_DPLL_DISP) = regVal;

    while(!(HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_IDLEST_DPLL_DISP) &
                         CM_WKUP_CM_IDLEST_DPLL_DISP_ST_DPLL_CLK));
}

/* \brief This function initializes the PER PLL
 * 
 * \param none
 *
 * \return none
 *
 */
void PerPLLInit(void)
{
    volatile unsigned int regVal = 0;

    /* Put the PLL in bypass mode */
    regVal = HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKMODE_DPLL_PER) &
                ~CM_WKUP_CM_CLKMODE_DPLL_PER_DPLL_EN;

    regVal |= CM_WKUP_CM_CLKMODE_DPLL_PER_DPLL_EN_DPLL_MN_BYP_MODE;

    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKMODE_DPLL_PER) = regVal;

    while(!(HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_IDLEST_DPLL_PER) &
                      CM_WKUP_CM_IDLEST_DPLL_PER_ST_MN_BYPASS));

    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKSEL_DPLL_PERIPH) &=
                             ~(CM_WKUP_CM_CLKSEL_DPLL_PERIPH_DPLL_MULT |
                                    CM_WKUP_CM_CLKSEL_DPLL_PERIPH_DPLL_DIV);

    /* Set the multipler and divider values for the PLL */
    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKSEL_DPLL_PERIPH) |=
        ((PERPLL_M << CM_WKUP_CM_CLKSEL_DPLL_PERIPH_DPLL_MULT_SHIFT) |
         (PERPLL_N << CM_WKUP_CM_CLKSEL_DPLL_PERIPH_DPLL_DIV_SHIFT));

    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_DIV_M2_DPLL_PER) &=
                          ~CM_WKUP_CM_DIV_M2_DPLL_PER_DPLL_CLKOUT_DIV;

    /* Set the CLKOUT2 divider */
    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_DIV_M2_DPLL_PER) |= PERPLL_M2;

    /* Now LOCK the PLL by enabling it */
    regVal = HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKMODE_DPLL_PER) &
                ~CM_WKUP_CM_CLKMODE_DPLL_PER_DPLL_EN;

    regVal |= CM_WKUP_CM_CLKMODE_DPLL_PER_DPLL_EN;

    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKMODE_DPLL_PER) = regVal;

    while(!(HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_IDLEST_DPLL_PER) &
                           CM_WKUP_CM_IDLEST_DPLL_PER_ST_DPLL_CLK));

}

/* \brief This function initializes the DDR PLL
 * 
 * \param none
 *
 * \return none
 *
 */
void DDRPLLInit(void)
{
    volatile unsigned int regVal = 0;

    /* Put the PLL in bypass mode */
    regVal = HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKMODE_DPLL_DDR) &
                 ~CM_WKUP_CM_CLKMODE_DPLL_DDR_DPLL_EN;

    regVal |= CM_WKUP_CM_CLKMODE_DPLL_MPU_DPLL_EN_DPLL_MN_BYP_MODE;

    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKMODE_DPLL_DDR) = regVal;

    while(!(HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_IDLEST_DPLL_DDR) &
                      CM_WKUP_CM_IDLEST_DPLL_DDR_ST_MN_BYPASS));

    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKSEL_DPLL_DDR) &=
                     ~(CM_WKUP_CM_CLKSEL_DPLL_DDR_DPLL_MULT |
                           CM_WKUP_CM_CLKSEL_DPLL_DDR_DPLL_DIV);

    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKSEL_DPLL_DDR) |=
        ((DDRPLL_M << CM_WKUP_CM_CLKSEL_DPLL_DDR_DPLL_MULT_SHIFT) |
         (DDRPLL_N << CM_WKUP_CM_CLKSEL_DPLL_DDR_DPLL_DIV_SHIFT));

    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_DIV_M2_DPLL_DDR) &=
                       ~CM_WKUP_CM_DIV_M2_DPLL_DDR_DPLL_CLKOUT_DIV;

    /* Set the CLKOUT divider */
    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_DIV_M2_DPLL_DDR) |= DDRPLL_M2;

    /* Now LOCK the PLL by enabling it */
    regVal = HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKMODE_DPLL_DDR) &
                ~CM_WKUP_CM_CLKMODE_DPLL_DDR_DPLL_EN;

    regVal |= CM_WKUP_CM_CLKMODE_DPLL_DDR_DPLL_EN;

    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKMODE_DPLL_DDR) = regVal;

    while(!(HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_IDLEST_DPLL_DDR) &
                           CM_WKUP_CM_IDLEST_DPLL_DDR_ST_DPLL_CLK));
}

/*
 * \brief This function initializes the MPU PLL
 *
 * \param  none
 *
 * \return none
 */
void MPUPLLInit(void)
{
    volatile unsigned int regVal = 0;

    /* Put the PLL in bypass mode */
    regVal = HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKMODE_DPLL_MPU) &
                ~CM_WKUP_CM_CLKMODE_DPLL_MPU_DPLL_EN;

    regVal |= CM_WKUP_CM_CLKMODE_DPLL_MPU_DPLL_EN_DPLL_MN_BYP_MODE;

    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKMODE_DPLL_MPU) = regVal;

    /* Wait for DPLL to go in to bypass mode */
    while(!(HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_IDLEST_DPLL_MPU) &
                CM_WKUP_CM_IDLEST_DPLL_MPU_ST_MN_BYPASS));

    /* Clear the MULT and DIV field of DPLL_MPU register */
    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKSEL_DPLL_MPU) &=
       ~(CM_WKUP_CM_CLKSEL_DPLL_MPU_DPLL_MULT |
            CM_WKUP_CM_CLKSEL_DPLL_MPU_DPLL_DIV);


    /* Set the multiplier and divider values for the PLL */
    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKSEL_DPLL_MPU) |=
       ((MPUPLL_M << CM_WKUP_CM_CLKSEL_DPLL_MPU_DPLL_MULT_SHIFT) |
           (MPUPLL_N << CM_WKUP_CM_CLKSEL_DPLL_MPU_DPLL_DIV_SHIFT)); 


    regVal = HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_DIV_M2_DPLL_MPU);

    regVal = regVal & ~CM_WKUP_CM_DIV_M2_DPLL_MPU_DPLL_CLKOUT_DIV;

    regVal = regVal | MPUPLL_M2;

    /* Now LOCK the PLL by enabling it */
    regVal = HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKMODE_DPLL_MPU) &
                ~CM_WKUP_CM_CLKMODE_DPLL_MPU_DPLL_EN;

    regVal |= CM_WKUP_CM_CLKMODE_DPLL_MPU_DPLL_EN;

    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKMODE_DPLL_MPU) = regVal;

    while(!(HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_IDLEST_DPLL_MPU) &
                             CM_WKUP_CM_IDLEST_DPLL_MPU_ST_DPLL_CLK));
}

/* \brief This function initializes the interface clock 
 * 
 * \param none
 *
 * \return none
 *
 */
void InterfaceClkInit(void)
{
    HWREG(SOC_CM_PER_REGS + CM_PER_L3_CLKCTRL) |=
                                   CM_PER_L3_CLKCTRL_MODULEMODE_ENABLE;

    while((HWREG(SOC_CM_PER_REGS + CM_PER_L3_CLKCTRL) &
        CM_PER_L3_CLKCTRL_MODULEMODE) != CM_PER_L3_CLKCTRL_MODULEMODE_ENABLE);

    HWREG(SOC_CM_PER_REGS + CM_PER_L4LS_CLKCTRL) |=
                                       CM_PER_L4LS_CLKCTRL_MODULEMODE_ENABLE;

    while((HWREG(SOC_CM_PER_REGS + CM_PER_L4LS_CLKCTRL) &
      CM_PER_L4LS_CLKCTRL_MODULEMODE) != CM_PER_L4LS_CLKCTRL_MODULEMODE_ENABLE);

    HWREG(SOC_CM_PER_REGS + CM_PER_L4FW_CLKCTRL) |=
                                 CM_PER_L4FW_CLKCTRL_MODULEMODE_ENABLE;

    while((HWREG(SOC_CM_PER_REGS + CM_PER_L4FW_CLKCTRL) &
      CM_PER_L4FW_CLKCTRL_MODULEMODE) != CM_PER_L4FW_CLKCTRL_MODULEMODE_ENABLE);

    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_L4WKUP_CLKCTRL) |=
                                          CM_PER_L4FW_CLKCTRL_MODULEMODE_ENABLE;
    while((HWREG(SOC_CM_WKUP_REGS + CM_WKUP_L4WKUP_CLKCTRL) &
                        CM_WKUP_L4WKUP_CLKCTRL_MODULEMODE) !=
                                         CM_PER_L4FW_CLKCTRL_MODULEMODE_ENABLE);

    HWREG(SOC_CM_PER_REGS + CM_PER_L3_INSTR_CLKCTRL) |=
                                      CM_PER_L3_INSTR_CLKCTRL_MODULEMODE_ENABLE;

    while((HWREG(SOC_CM_PER_REGS + CM_PER_L3_INSTR_CLKCTRL) &
                        CM_PER_L3_INSTR_CLKCTRL_MODULEMODE) !=
                        CM_PER_L3_INSTR_CLKCTRL_MODULEMODE_ENABLE);

    HWREG(SOC_CM_PER_REGS + CM_PER_L4HS_CLKCTRL) |=
                              CM_PER_L4HS_CLKCTRL_MODULEMODE_ENABLE;

    while((HWREG(SOC_CM_PER_REGS + CM_PER_L4HS_CLKCTRL) &
                        CM_PER_L4HS_CLKCTRL_MODULEMODE) !=
                  CM_PER_L4HS_CLKCTRL_MODULEMODE_ENABLE);
}

/* \brief This function initializes the power domain transition.
 * 
 * \param none
 *
 * \return none
 *
 */
void PowerDomainTransitionInit(void)
{
    HWREG(SOC_CM_PER_REGS + CM_PER_L3_CLKSTCTRL) |=
                             CM_PER_L3_CLKSTCTRL_CLKTRCTRL_SW_WKUP;

    HWREG(SOC_CM_PER_REGS + CM_PER_L4LS_CLKSTCTRL) |=
                             CM_PER_L4LS_CLKSTCTRL_CLKTRCTRL_SW_WKUP;

    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CLKSTCTRL) |=
                             CM_WKUP_CLKSTCTRL_CLKTRCTRL_SW_WKUP;

    HWREG(SOC_CM_PER_REGS + CM_PER_L4FW_CLKSTCTRL) |=
                              CM_PER_L4FW_CLKSTCTRL_CLKTRCTRL_SW_WKUP;

    HWREG(SOC_CM_PER_REGS + CM_PER_L3S_CLKSTCTRL) |=
                                CM_PER_L3S_CLKSTCTRL_CLKTRCTRL_SW_WKUP;
}

/*
 * \brief This function sets up various PLLs  
 *
 * \param  none
 *
 * \return none
 */
void PLLInit(void)
{
    MPUPLLInit();
    CorePLLInit();
    PerPLLInit();
    DDRPLLInit();
    InterfaceClkInit();
    PowerDomainTransitionInit();
    //DisplayPLLInit();
}



unsigned char  TPS65217RegRead(unsigned char regOffset)
{
    unsigned char val;
    I2CMasterReadEx(SOC_I2C_0_REGS,0x24,&regOffset,1,&val,1);
    return val;
}

/**
 *  \brief            - Generic function that can write a TPS65217 PMIC
 *                      register or bit field regardless of protection
 *                      level.
 *
 * \param regOffset:  - Register address to write.
 *
 * \param dest_val    - Value to write.
 *
 * \param mask        - Bit mask (8 bits) to be applied.  Function will only
 *                      change bits that are set in the bit mask.
 *
 * \return:            BOOL.
 */
BOOL TPS65217RegWrite(unsigned char regOffset,
                      unsigned char dest_val, unsigned char mask) {
   unsigned char read_val;
   unsigned char xor_reg;
   unsigned char pswreg = 0x55;
   unsigned char port_level;
   unsigned char i2cwritebuf[2];
   unsigned char i2cregaddr;
   if (regOffset <= 12) {
      port_level = PROT_LEVEL_NONE;
   } else if ((regOffset <= 21) && ((regOffset >= 14))) {
      port_level = PROT_LEVEL_2;
   } else {
      port_level = PROT_LEVEL_1;
   }

   if (mask != MASK_ALL_BITS) {
      I2CMasterReadEx(SOC_I2C_0_REGS, 0x24, &regOffset, 1, &read_val, 1);
      read_val &= (~mask);
      read_val |= (dest_val & mask);
      dest_val = read_val;
   }
   if (port_level == PROT_LEVEL_NONE) {
      i2cwritebuf[0] = regOffset;
      i2cwritebuf[1] = dest_val;
      return I2CMasterWrite(SOC_I2C_0_REGS, 0x24, i2cwritebuf, 2);
   }

   if (port_level != PROT_LEVEL_NONE) {
      xor_reg = regOffset ^ PASSWORD_UNLOCK;
      i2cwritebuf[0] = PASSWORD;
      i2cwritebuf[1] = xor_reg;
      I2CMasterWrite(SOC_I2C_0_REGS, 0x24, i2cwritebuf, 2);
      i2cwritebuf[0] = regOffset;
      i2cwritebuf[1] = dest_val;
      I2CMasterWrite(SOC_I2C_0_REGS, 0x24, i2cwritebuf, 2);
      if (port_level == PROT_LEVEL_1) {
         i2cregaddr = PASSWORD;
         I2CMasterReadEx(SOC_I2C_0_REGS, 0x24, &i2cregaddr, 1, &pswreg, 1);
         if (pswreg  == 0) {
            return TRUE;
         } else {
            return FALSE;
         }
      }
      //port_level == 2
      i2cwritebuf[0] = PASSWORD;
      i2cwritebuf[1] = xor_reg;
      I2CMasterWrite(SOC_I2C_0_REGS, 0x24, i2cwritebuf, 2);
      i2cwritebuf[0] = regOffset;
      i2cwritebuf[1] = dest_val;
      I2CMasterWrite(SOC_I2C_0_REGS, 0x24, i2cwritebuf, 2);
      i2cregaddr = PASSWORD;
      I2CMasterReadEx(SOC_I2C_0_REGS, 0x24, &i2cregaddr, 1, &pswreg, 1);
      if (pswreg == 0) {
         return TRUE;
      }
   }
   return FALSE;
}
    


/**
 *  \brief              - Controls output voltage setting for the DCDC1,
 *                        DCDC2, or DCDC3 control registers in the PMIC.
 *
 * \param  dc_cntrl_reg   DCDC Control Register address.
 *                        Must be DEFDCDC1, DEFDCDC2, or DEFDCDC3.
 *
 * \param  volt_sel       Register value to set.  See PMIC TRM for value set.
 *
 * \return:               None.
 */
void TPS65217VoltageUpdate(unsigned char dc_cntrl_reg, unsigned char volt_sel)
{
    /* set voltage level */
    TPS65217RegWrite(dc_cntrl_reg, volt_sel, MASK_ALL_BITS);

    /* set GO bit to initiate voltage transition */
    TPS65217RegWrite(DEFSLEW, DCDC_GO, DCDC_GO);
}


/*
 * \brief Configures the VDD OP voltage. 
 *
 * \param  none.
 *
 * \return none
 */

void ConfigVddOpVoltage(void)
{    
    unsigned char vol_sel = DCDC_VOLT_SEL_1100MV;
#if (OPP == OPP_SR_TURBO)
    vol_sel = DCDC_VOLT_SEL_1275MV;
#elif (OPP == OPP120)
    vol_sel = DCDC_VOLT_SEL_1200MV;
#elif (OPP == OPP100)
    vol_sel = DCDC_VOLT_SEL_1100MV;
#endif
    /* Set DCDC2 (MPU) voltage to 1.275V */
    TPS65217VoltageUpdate(DEFDCDC2, vol_sel);
    /* Set LDO3, LDO4 output voltage to 3.3V */
    //TPS65217RegWrite(DEFLS1, LDO_VOLTAGE_OUT_3_3, LDO_MASK);
    //TPS65217RegWrite(DEFLS2, LDO_VOLTAGE_OUT_3_3, LDO_MASK);
}


/*
 * \brief This function sets up the pinmux for EMIF/DDR interface
 *
 * \param  instance This is used to specify instance of EMIF to be configured
 *
 * \return none
 */

void EMIFPinMuxSetup(unsigned int instance)
{
    unsigned int i;

    /* TODO : VTP - not clear on PUPD, VREF - not clear on PinType
     *           STRBEN(n) - not clear on PUPD
     */
    HWREG(SOC_CONTROL_REGS + CONTROL_CONF_DDR_RESETN) =
        PAD_CTRL_MUXMODE(0);
    HWREG(SOC_CONTROL_REGS + CONTROL_CONF_DDR_CSN0) =
        (PAD_CTRL_MUXMODE(0) | PAD_CTRL_PULLUPSEL);
    HWREG(SOC_CONTROL_REGS + CONTROL_CONF_DDR_CKE) =
        PAD_CTRL_MUXMODE(0);
    HWREG(SOC_CONTROL_REGS + CONTROL_CONF_DDR_CK) =
        PAD_CTRL_MUXMODE(0);
    HWREG(SOC_CONTROL_REGS + CONTROL_CONF_DDR_NCK) =
        (PAD_CTRL_MUXMODE(0) | PAD_CTRL_PULLUPSEL);
    HWREG(SOC_CONTROL_REGS + CONTROL_CONF_DDR_CASN) =
        (PAD_CTRL_MUXMODE(0) | PAD_CTRL_PULLUPSEL);
    HWREG(SOC_CONTROL_REGS + CONTROL_CONF_DDR_RASN) =
        (PAD_CTRL_MUXMODE(0) | PAD_CTRL_PULLUPSEL);
    HWREG(SOC_CONTROL_REGS + CONTROL_CONF_DDR_WEN) =
        (PAD_CTRL_MUXMODE(0) | PAD_CTRL_PULLUPSEL);
    HWREG(SOC_CONTROL_REGS + CONTROL_CONF_DDR_BA(0)) =
        (PAD_CTRL_MUXMODE(0) | PAD_CTRL_PULLUPSEL);
    HWREG(SOC_CONTROL_REGS + CONTROL_CONF_DDR_BA(1)) =
        (PAD_CTRL_MUXMODE(0) | PAD_CTRL_PULLUPSEL);
    HWREG(SOC_CONTROL_REGS + CONTROL_CONF_DDR_BA(2)) =
        (PAD_CTRL_MUXMODE(0) | PAD_CTRL_PULLUPSEL);

    /* Configure all the address/data lines */
    for (i = 0; i < 15; i++)
    {
        HWREG(SOC_CONTROL_REGS + CONTROL_CONF_DDR_A(i)) =
            (PAD_CTRL_MUXMODE(0) | PAD_CTRL_PULLUPSEL);

        HWREG(SOC_CONTROL_REGS + CONTROL_CONF_DDR_D(i)) =
            (PAD_CTRL_MUXMODE(0) | PAD_CTRL_RXACTIVE);
    }

    HWREG(SOC_CONTROL_REGS + CONTROL_CONF_DDR_ODT) =
        PAD_CTRL_MUXMODE(0);

    for (i = 0; i < 2; i++)
    {
        HWREG(SOC_CONTROL_REGS + CONTROL_CONF_DDR_DQM(i)) =
            (PAD_CTRL_MUXMODE(0) | PAD_CTRL_PULLUPSEL);
        HWREG(SOC_CONTROL_REGS + CONTROL_CONF_DDR_DQS(i)) =
            (PAD_CTRL_MUXMODE(0) | PAD_CTRL_RXACTIVE);
        HWREG(SOC_CONTROL_REGS + CONTROL_CONF_DDR_DQSN(i)) =
            (PAD_CTRL_MUXMODE(0) | PAD_CTRL_PULLUPSEL | PAD_CTRL_RXACTIVE);
    }

    HWREG(SOC_CONTROL_REGS + CONTROL_CONF_DDR_VTP) =
        PAD_CTRL_MUXMODE(0);

    for (i = 0; i < 2; i++)
    {
        HWREG(SOC_CONTROL_REGS + CONTROL_CONF_DDR_STRBEN(i)) =
            (PAD_CTRL_MUXMODE(0) + PAD_CTRL_RXACTIVE);
    }
}



static void PinMuxSetup(void){
  MUX_VAL(CONTROL_PADCONF_MMC0_DAT3, (IEN | PU | MODE0 )) /* MMC0_DAT3 */\
  MUX_VAL(CONTROL_PADCONF_MMC0_DAT2, (IEN | PU | MODE0 )) /* MMC0_DAT2 */\
  MUX_VAL(CONTROL_PADCONF_MMC0_DAT1, (IEN | PU | MODE0 )) /* MMC0_DAT1 */\
  MUX_VAL(CONTROL_PADCONF_MMC0_DAT0, (IEN | PU | MODE0 )) /* MMC0_DAT0 */\
  MUX_VAL(CONTROL_PADCONF_MMC0_CLK, (IEN | PU | MODE0 )) /* MMC0_CLK */\
  MUX_VAL(CONTROL_PADCONF_MMC0_CMD, (IEN | PU | MODE0 )) /* MMC0_CMD */\
  MUX_VAL(CONTROL_PADCONF_SPI0_CS1, (IEN | OFF | MODE5 )) /* MMC0_SDCD_MUX0 */\
  MUX_VAL(CONTROL_PADCONF_GPMC_AD0,(IEN | PU | MODE1)) /* MMC1_DAT0_MUX2 */\
  MUX_VAL(CONTROL_PADCONF_GPMC_AD1,(IEN | PU | MODE1)) /* MMC1_DAT1_MUX2 */\
  MUX_VAL(CONTROL_PADCONF_GPMC_AD2,(IEN | PU | MODE1)) /* MMC1_DAT2_MUX2 */\
  MUX_VAL(CONTROL_PADCONF_GPMC_AD3,(IEN | PU | MODE1)) /* MMC1_DAT3_MUX2 */\
  MUX_VAL(CONTROL_PADCONF_GPMC_AD4,(IEN | PU | MODE1)) /* MMC1_DAT4_MUX2 */\
  MUX_VAL(CONTROL_PADCONF_GPMC_AD5,(IEN | PU | MODE1)) /* MMC1_DAT5_MUX2 */\
  MUX_VAL(CONTROL_PADCONF_GPMC_AD6,(IEN | PU | MODE1)) /* MMC1_DAT6_MUX2 */\
  MUX_VAL(CONTROL_PADCONF_GPMC_AD7,(IEN | PU | MODE1)) /* MMC1_DAT7_MUX2 */\
  MUX_VAL(CONTROL_PADCONF_GPMC_CSN1,(IEN | PU | MODE2)) /* MMC1_CLK_MUX0 */\
  MUX_VAL(CONTROL_PADCONF_GPMC_CSN2,(IEN | PU | MODE2)) /* MMC1_CMD_MUX0 */\
  MUX_VAL(CONTROL_PADCONF_GPMC_A2, (IDIS | PD | MODE6 )) /* EHRPWM1A_MUX1 */\
  MUX_VAL(CONTROL_PADCONF_UART1_CTSN, (IEN | OFF | MODE2 )) /* DCAN0_TX_MUX2 */\
  MUX_VAL(CONTROL_PADCONF_UART1_RTSN, (IEN | OFF | MODE2 )) /* DCAN0_RX_MUX2 */\
  MUX_VAL(CONTROL_PADCONF_I2C0_SDA, (IEN | OFF | MODE0 )) /* I2C0_SDA */\
  MUX_VAL(CONTROL_PADCONF_I2C0_SCL, (IEN | OFF | MODE0 )) /* I2C0_SCL */\
  MUX_VAL(CONTROL_PADCONF_SPI0_D1, (IEN | OFF | MODE2 )) /* I2C1_SDA_MUX3 */\
  MUX_VAL(CONTROL_PADCONF_SPI0_CS0, (IEN | OFF | MODE2 )) /* I2C1_SCL_MUX3 */\
  MUX_VAL(CONTROL_PADCONF_LCD_DATA4, (IEN | OFF | MODE3 )) /* EQEP2A_IN_MUX0 */\
  MUX_VAL(CONTROL_PADCONF_LCD_DATA5, (IEN | OFF | MODE3 )) /* EQEP2B_IN_MUX0 */\
MUX_VAL(CONTROL_PADCONF_GPMC_AD0, (IEN | OFF | MODE0 )) /* GPMC_AD0 */\
MUX_VAL(CONTROL_PADCONF_GPMC_AD1, (IEN | OFF | MODE0 )) /* GPMC_AD1 */\
MUX_VAL(CONTROL_PADCONF_GPMC_AD2, (IEN | OFF | MODE0 )) /* GPMC_AD2 */\
MUX_VAL(CONTROL_PADCONF_GPMC_AD3, (IEN | OFF | MODE0 )) /* GPMC_AD3 */\
MUX_VAL(CONTROL_PADCONF_GPMC_AD4, (IEN | OFF | MODE0 )) /* GPMC_AD4 */\
MUX_VAL(CONTROL_PADCONF_GPMC_AD5, (IEN | OFF | MODE0 )) /* GPMC_AD5 */\
MUX_VAL(CONTROL_PADCONF_GPMC_AD6, (IEN | OFF | MODE0 )) /* GPMC_AD6 */\
MUX_VAL(CONTROL_PADCONF_GPMC_AD7, (IEN | OFF | MODE0 )) /* GPMC_AD7 */\
MUX_VAL(CONTROL_PADCONF_GPMC_AD8, (IEN | OFF | MODE0 )) /* GPMC_AD8 */\
MUX_VAL(CONTROL_PADCONF_GPMC_AD9, (IEN | OFF | MODE0 )) /* GPMC_AD9 */\
MUX_VAL(CONTROL_PADCONF_GPMC_AD10, (IEN | OFF | MODE0 )) /* GPMC_AD10 */\
MUX_VAL(CONTROL_PADCONF_GPMC_AD11, (IEN | OFF | MODE0 )) /* GPMC_AD11 */\
MUX_VAL(CONTROL_PADCONF_GPMC_AD12, (IEN | OFF | MODE0 )) /* GPMC_AD12 */\
MUX_VAL(CONTROL_PADCONF_GPMC_AD13, (IEN | OFF | MODE0 )) /* GPMC_AD13 */\
MUX_VAL(CONTROL_PADCONF_GPMC_AD14, (IEN | OFF | MODE0 )) /* GPMC_AD14 */\
MUX_VAL(CONTROL_PADCONF_GPMC_AD15, (IEN | OFF | MODE0 )) /* GPMC_AD15 */\
MUX_VAL(CONTROL_PADCONF_GPMC_A0, (IDIS | OFF | MODE0 )) /* GPMC_A0_MUX0 */\
MUX_VAL(CONTROL_PADCONF_GPMC_A1, (IDIS | OFF | MODE0 )) /* GPMC_A1_MUX0 */\
MUX_VAL(CONTROL_PADCONF_GPMC_A2, (IDIS | OFF | MODE0 )) /* GPMC_A2_MUX0 */\
MUX_VAL(CONTROL_PADCONF_GPMC_A3, (IDIS | OFF | MODE0 )) /* GPMC_A3_MUX0 */\
MUX_VAL(CONTROL_PADCONF_GPMC_A4, (IDIS | OFF | MODE0 )) /* GPMC_A4_MUX0 */\
MUX_VAL(CONTROL_PADCONF_GPMC_A5, (IDIS | OFF | MODE0 )) /* GPMC_A5_MUX0 */\
MUX_VAL(CONTROL_PADCONF_GPMC_A6, (IDIS | OFF | MODE0 )) /* GPMC_A6_MUX0 */\
MUX_VAL(CONTROL_PADCONF_GPMC_A7, (IDIS | OFF | MODE0 )) /* GPMC_A7_MUX0 */\
MUX_VAL(CONTROL_PADCONF_GPMC_A8, (IDIS | OFF | MODE0 )) /* GPMC_A8_MUX0 */\
MUX_VAL(CONTROL_PADCONF_GPMC_A9, (IDIS | OFF | MODE0 )) /* GPMC_A9_MUX0 */\
MUX_VAL(CONTROL_PADCONF_GPMC_A10, (IDIS | OFF | MODE0 )) /* GPMC_A10_MUX0 */\
MUX_VAL(CONTROL_PADCONF_GPMC_A11, (IDIS | OFF | MODE0 )) /* GPMC_A11_MUX0 */\
MUX_VAL(CONTROL_PADCONF_GPMC_WAIT0, (IEN | PU | MODE7 )) /* GPIO0[30] */\
MUX_VAL(CONTROL_PADCONF_GPMC_WPN, (IEN | PU | MODE7 )) /* GPIO0[31] */\
MUX_VAL(CONTROL_PADCONF_GPMC_BEN1, (IEN | PU | MODE7 )) /* GPIO1[28] */\
MUX_VAL(CONTROL_PADCONF_GPMC_CSN0, (IDIS | PU | MODE0 )) /* GPMC_CSN0 */\
MUX_VAL(CONTROL_PADCONF_GPMC_CSN1, (IDIS | PU | MODE0 )) /* GPMC_CSN1 */\
MUX_VAL(CONTROL_PADCONF_GPMC_CSN2, (IDIS | PU | MODE0 )) /* GPMC_CSN2 */\
MUX_VAL(CONTROL_PADCONF_GPMC_CSN3, (IDIS | PU | MODE0 )) /* GPMC_CSN3 */\
MUX_VAL(CONTROL_PADCONF_GPMC_CLK, (IEN | PU | MODE2 )) /* GPMC_WAIT1 */\
MUX_VAL(CONTROL_PADCONF_GPMC_ADVN_ALE, (IDIS | PU | MODE0 )) /* GPMC_ADVN_ALE */\
MUX_VAL(CONTROL_PADCONF_GPMC_OEN_REN, (IDIS | PU | MODE0 )) /* GPMC_OEN_REN */\
MUX_VAL(CONTROL_PADCONF_GPMC_WEN, (IDIS | PU | MODE0 )) /* GPMC_WEN */\
MUX_VAL(CONTROL_PADCONF_GPMC_BEN0_CLE, (IEN | PU | MODE7 )) /* GPIO2[5] */\
MUX_VAL(CONTROL_PADCONF_LCD_DATA0, (IEN | OFF | MODE0 )) /* LCD_DATA0 */\
MUX_VAL(CONTROL_PADCONF_LCD_DATA1, (IEN | OFF | MODE0 )) /* LCD_DATA1 */\
MUX_VAL(CONTROL_PADCONF_LCD_DATA2, (IEN | OFF | MODE0 )) /* LCD_DATA2 */\
MUX_VAL(CONTROL_PADCONF_LCD_DATA3, (IEN | OFF | MODE0 )) /* LCD_DATA3 */\
MUX_VAL(CONTROL_PADCONF_LCD_DATA4, (IEN | OFF | MODE0 )) /* LCD_DATA4 */\
MUX_VAL(CONTROL_PADCONF_LCD_DATA5, (IEN | OFF | MODE0 )) /* LCD_DATA5 */\
MUX_VAL(CONTROL_PADCONF_LCD_DATA6, (IEN | OFF | MODE0 )) /* LCD_DATA6 */\
MUX_VAL(CONTROL_PADCONF_LCD_DATA7, (IEN | OFF | MODE0 )) /* LCD_DATA7 */\
MUX_VAL(CONTROL_PADCONF_LCD_DATA8, (IEN | OFF | MODE0 )) /* LCD_DATA8 */\
MUX_VAL(CONTROL_PADCONF_LCD_DATA9, (IEN | OFF | MODE0 )) /* LCD_DATA9 */\
MUX_VAL(CONTROL_PADCONF_LCD_DATA10, (IEN | OFF | MODE0 )) /* LCD_DATA10 */\
MUX_VAL(CONTROL_PADCONF_LCD_DATA11, (IEN | OFF | MODE0 )) /* LCD_DATA11 */\
MUX_VAL(CONTROL_PADCONF_LCD_DATA12, (IEN | OFF | MODE0 )) /* LCD_DATA12 */\
MUX_VAL(CONTROL_PADCONF_LCD_DATA13, (IEN | OFF | MODE0 )) /* LCD_DATA13 */\
MUX_VAL(CONTROL_PADCONF_LCD_DATA14, (IEN | OFF | MODE0 )) /* LCD_DATA14 */\
MUX_VAL(CONTROL_PADCONF_LCD_DATA15, (IEN | OFF | MODE0 )) /* LCD_DATA15 */\
MUX_VAL(CONTROL_PADCONF_LCD_VSYNC, (IDIS | OFF | MODE0 )) /* LCD_VSYNC */\
MUX_VAL(CONTROL_PADCONF_LCD_HSYNC, (IDIS | OFF | MODE0 )) /* LCD_HSYNC */\
MUX_VAL(CONTROL_PADCONF_LCD_PCLK, (IDIS | OFF | MODE0 )) /* LCD_PCLK */\
MUX_VAL(CONTROL_PADCONF_GPMC_WAIT0, (IEN | PU | MODE7 )) /* GPIO0[30] */\
MUX_VAL(CONTROL_PADCONF_LCD_AC_BIAS_EN, (IDIS | OFF | MODE0 )) /* LCD_AC_BIAS_EN */\
MUX_VAL(CONTROL_PADCONF_ECAP0_IN_PWM0_OUT, (IEN | PU | MODE0 )) /* ECAP0_IN_PWM0_OUT */\
MUX_VAL(CONTROL_PADCONF_AIN7, (IEN | OFF | MODE0 )) /* AIN7 */\
MUX_VAL(CONTROL_PADCONF_AIN6, (IEN | OFF | MODE0 )) /* AIN6 */\
MUX_VAL(CONTROL_PADCONF_AIN5, (IEN | OFF | MODE0 )) /* AIN5 */\
MUX_VAL(CONTROL_PADCONF_AIN4, (IEN | OFF | MODE0 )) /* AIN4 */\
MUX_VAL(CONTROL_PADCONF_AIN3, (IEN | OFF | MODE0 )) /* AIN3 */\
MUX_VAL(CONTROL_PADCONF_AIN2, (IEN | OFF | MODE0 )) /* AIN2 */\
MUX_VAL(CONTROL_PADCONF_AIN1, (IEN | OFF | MODE0 )) /* AIN1 */\
MUX_VAL(CONTROL_PADCONF_AIN0, (IEN | OFF | MODE0 )) /* AIN0 */\
MUX_VAL(CONTROL_PADCONF_VREFP, (IEN | OFF | MODE0 )) /* VREFP */\
MUX_VAL(CONTROL_PADCONF_VREFN, (IEN | OFF | MODE0 )) /* VREFN */\
MUX_VAL(CONTROL_PADCONF_UART1_RXD, (IEN | OFF | MODE0 )) /* UART1_RXD */\
MUX_VAL(CONTROL_PADCONF_UART1_TXD, (IDIS | OFF | MODE0 )) /* UART1_TXD */\
MUX_VAL(CONTROL_PADCONF_MII1_RXD1, (IEN | PU | MODE7 )) /* GPIO2[20] */\
;
}

/*
 * \brief This function sets up the DDR PHY
 *
 * \param  none
 *
 * \return none
 */

static void DDR2PhyInit(void)
{
    /* Enable VTP */
    HWREG(SOC_CONTROL_REGS + CONTROL_VTP_CTRL) |= CONTROL_VTP_CTRL_ENABLE;
    HWREG(SOC_CONTROL_REGS + CONTROL_VTP_CTRL) &= ~CONTROL_VTP_CTRL_CLRZ;
    HWREG(SOC_CONTROL_REGS + CONTROL_VTP_CTRL) |= CONTROL_VTP_CTRL_CLRZ;
    while((HWREG(SOC_CONTROL_REGS + CONTROL_VTP_CTRL) & CONTROL_VTP_CTRL_READY) !=
                CONTROL_VTP_CTRL_READY);

    /* DDR PHY CMD0 Register configuration */
    HWREG(CMD0_SLAVE_RATIO_0)   = DDR2_CMD0_SLAVE_RATIO_0;
    HWREG(CMD0_SLAVE_FORCE_0)   = DDR2_CMD0_SLAVE_FORCE_0;
    HWREG(CMD0_SLAVE_DELAY_0)   = DDR2_CMD0_SLAVE_DELAY_0;
    HWREG(CMD0_LOCK_DIFF_0)     = DDR2_CMD0_LOCK_DIFF_0;
    HWREG(CMD0_INVERT_CLKOUT_0) = DDR2_CMD0_INVERT_CLKOUT_0;

    /* DDR PHY CMD1 Register configuration */
    HWREG(CMD1_SLAVE_RATIO_0)   = DDR2_CMD1_SLAVE_RATIO_0;
    HWREG(CMD1_SLAVE_FORCE_0)   =  DDR2_CMD1_SLAVE_FORCE_0;
    HWREG(CMD1_SLAVE_DELAY_0)   = DDR2_CMD1_SLAVE_DELAY_0;
    HWREG(CMD1_LOCK_DIFF_0)     = DDR2_CMD1_LOCK_DIFF_0;
    HWREG(CMD1_INVERT_CLKOUT_0) = DDR2_CMD1_INVERT_CLKOUT_0;

    /* DDR PHY CMD2 Register configuration */
    HWREG(CMD2_SLAVE_RATIO_0)   = DDR2_CMD2_SLAVE_RATIO_0;
    HWREG(CMD2_SLAVE_FORCE_0)   = DDR2_CMD2_SLAVE_FORCE_0;
    HWREG(CMD2_SLAVE_DELAY_0)   = DDR2_CMD2_SLAVE_DELAY_0;
    HWREG(CMD2_LOCK_DIFF_0)     = DDR2_CMD2_LOCK_DIFF_0;
    HWREG(CMD2_INVERT_CLKOUT_0) = DDR2_CMD2_INVERT_CLKOUT_0;

    /* DATA macro configuration */
    HWREG(DATA0_RD_DQS_SLAVE_RATIO_0)  = DDR2_DATA0_RD_DQS_SLAVE_RATIO_0;
    HWREG(DATA0_RD_DQS_SLAVE_RATIO_1)  = DDR2_DATA0_RD_DQS_SLAVE_RATIO_1;
    HWREG(DATA0_WR_DQS_SLAVE_RATIO_0)  = DDR2_DATA0_WR_DQS_SLAVE_RATIO_0;
    HWREG(DATA0_WR_DQS_SLAVE_RATIO_1)  = DDR2_DATA0_WR_DQS_SLAVE_RATIO_1;
    HWREG(DATA0_WRLVL_INIT_RATIO_0)    = DDR2_DATA0_WRLVL_INIT_RATIO_0;
    HWREG(DATA0_WRLVL_INIT_RATIO_1)    = DDR2_DATA0_WRLVL_INIT_RATIO_1;
    HWREG(DATA0_GATELVL_INIT_RATIO_0)  = DDR2_DATA0_GATELVL_INIT_RATIO_0;
    HWREG(DATA0_GATELVL_INIT_RATIO_1)  = DDR2_DATA0_GATELVL_INIT_RATIO_1;
    HWREG(DATA0_FIFO_WE_SLAVE_RATIO_0) = DDR2_DATA0_FIFO_WE_SLAVE_RATIO_0;
    HWREG(DATA0_FIFO_WE_SLAVE_RATIO_1) = DDR2_DATA0_FIFO_WE_SLAVE_RATIO_1;
    HWREG(DATA0_WR_DATA_SLAVE_RATIO_0) = DDR2_DATA0_WR_DATA_SLAVE_RATIO_0;
    HWREG(DATA0_WR_DATA_SLAVE_RATIO_1) = DDR2_DATA0_WR_DATA_SLAVE_RATIO_1;
    HWREG(DATA0_LOCK_DIFF_0)           = DDR2_DATA0_LOCK_DIFF_0;


    HWREG(DATA1_RD_DQS_SLAVE_RATIO_0)  = DDR2_DATA1_RD_DQS_SLAVE_RATIO_0;
    HWREG(DATA1_RD_DQS_SLAVE_RATIO_1)  = DDR2_DATA1_RD_DQS_SLAVE_RATIO_1;
    HWREG(DATA1_WR_DQS_SLAVE_RATIO_0)  = DDR2_DATA1_WR_DQS_SLAVE_RATIO_0;
    HWREG(DATA1_WR_DQS_SLAVE_RATIO_1)  = DDR2_DATA1_WR_DQS_SLAVE_RATIO_1;
    HWREG(DATA1_WRLVL_INIT_RATIO_0)    = DDR2_DATA1_WRLVL_INIT_RATIO_0;
    HWREG(DATA1_WRLVL_INIT_RATIO_1)    = DDR2_DATA1_WRLVL_INIT_RATIO_1;
    HWREG(DATA1_GATELVL_INIT_RATIO_0)  = DDR2_DATA1_GATELVL_INIT_RATIO_0;
    HWREG(DATA1_GATELVL_INIT_RATIO_1)  = DDR2_DATA1_GATELVL_INIT_RATIO_1;
    HWREG(DATA1_FIFO_WE_SLAVE_RATIO_0) = DDR2_DATA1_FIFO_WE_SLAVE_RATIO_0;
    HWREG(DATA1_FIFO_WE_SLAVE_RATIO_1) = DDR2_DATA1_FIFO_WE_SLAVE_RATIO_1;
    HWREG(DATA1_WR_DATA_SLAVE_RATIO_0) = DDR2_DATA1_WR_DATA_SLAVE_RATIO_0;
    HWREG(DATA1_WR_DATA_SLAVE_RATIO_1) = DDR2_DATA1_WR_DATA_SLAVE_RATIO_1;
    HWREG(DATA1_LOCK_DIFF_0)           =  DDR2_DATA1_LOCK_DIFF_0;

    HWREG(DATA0_USE_RANK0_DELAYS_0)    = DDR2_DATA0_USE_RANK0_DELAYS_0;
    HWREG(DATA1_USE_RANK0_DELAYS_0)    = DDR2_DATA1_USE_RANK0_DELAYS_0;

}



/* \brief This function initializes the DDR2
 * 
 * \param none
 *
 * \return none
 *
 */
void DDR2Init(void)
{
    volatile unsigned int delay = 5000;

    /* DDR2 Phy Initialization */
    DDR2PhyInit();

    HWREG(SOC_CONTROL_REGS + CONTROL_DDR_CMD_IOCTRL(0)) =
                                                 DDR2_CONTROL_DDR_CMD_IOCTRL_0;
    HWREG(SOC_CONTROL_REGS + CONTROL_DDR_CMD_IOCTRL(1)) =
                                                 DDR2_CONTROL_DDR_CMD_IOCTRL_1;
    HWREG(SOC_CONTROL_REGS + CONTROL_DDR_CMD_IOCTRL(2)) =
                                                 DDR2_CONTROL_DDR_CMD_IOCTRL_2;
    HWREG(SOC_CONTROL_REGS + CONTROL_DDR_DATA_IOCTRL(0)) =
                                                 DDR2_CONTROL_DDR_DATA_IOCTRL_0;
    HWREG(SOC_CONTROL_REGS + CONTROL_DDR_DATA_IOCTRL(1)) =
                                                 DDR2_CONTROL_DDR_DATA_IOCTRL_1;

    /* IO to work for DDR2 */
    HWREG(SOC_CONTROL_REGS + CONTROL_DDR_IO_CTRL) &= DDR2_CONTROL_DDR_IO_CTRL;

    HWREG(SOC_CONTROL_REGS + CONTROL_DDR_CKE_CTRL) |= CONTROL_DDR_CKE_CTRL_DDR_CKE_CTRL;


    HWREG(SOC_EMIF_0_REGS + EMIF_DDR_PHY_CTRL_1) = DDR2_EMIF_DDR_PHY_CTRL_1;
    HWREG(SOC_EMIF_0_REGS + EMIF_DDR_PHY_CTRL_1_SHDW) =
                                                 DDR2_EMIF_DDR_PHY_CTRL_1_SHDW;
    HWREG(SOC_EMIF_0_REGS + EMIF_DDR_PHY_CTRL_2) = DDR2_EMIF_DDR_PHY_CTRL_2;

    HWREG(SOC_EMIF_0_REGS + EMIF_SDRAM_TIM_1)      =  DDR2_EMIF_SDRAM_TIM_1;
    HWREG(SOC_EMIF_0_REGS + EMIF_SDRAM_TIM_1_SHDW) = DDR2_EMIF_SDRAM_TIM_1_SHDW;
    HWREG(SOC_EMIF_0_REGS + EMIF_SDRAM_TIM_2)      = DDR2_EMIF_SDRAM_TIM_2;
    HWREG(SOC_EMIF_0_REGS + EMIF_SDRAM_TIM_2_SHDW) = DDR2_EMIF_SDRAM_TIM_2_SHDW;
    HWREG(SOC_EMIF_0_REGS + EMIF_SDRAM_TIM_3)      = DDR2_EMIF_SDRAM_TIM_3;
    HWREG(SOC_EMIF_0_REGS + EMIF_SDRAM_TIM_3_SHDW) = DDR2_EMIF_SDRAM_TIM_3_SHDM;
    HWREG(SOC_EMIF_0_REGS + EMIF_SDRAM_CONFIG)     = DDR2_EMIF_SDRAM_CONFIG;
    HWREG(SOC_EMIF_0_REGS + EMIF_SDRAM_REF_CTRL)   = DDR2_EMIF_SDRAM_REF_CTRL_VAL1;
    HWREG(SOC_EMIF_0_REGS + EMIF_SDRAM_REF_CTRL_SHDW) =
                                                 DDR2_EMIF_SDRAM_REF_CTRL_SHDW_VAL1;

    while(delay--);

    HWREG(SOC_EMIF_0_REGS + EMIF_SDRAM_REF_CTRL) = DDR2_EMIF_SDRAM_REF_CTRL_VAL2;
    HWREG(SOC_EMIF_0_REGS + EMIF_SDRAM_REF_CTRL_SHDW) =
                                                 DDR2_EMIF_SDRAM_REF_CTRL_SHDW_VAL2;

    HWREG(SOC_EMIF_0_REGS + EMIF_SDRAM_CONFIG)   = DDR2_EMIF_SDRAM_CONFIG;
	
    /* The CONTROL_SECURE_EMIF_SDRAM_CONFIG register exports SDRAM configuration 
       information to the EMIF */
    HWREG(SOC_CONTROL_REGS + CONTROL_SECURE_EMIF_SDRAM_CONFIG) = DDR2_EMIF_SDRAM_CONFIG;

}


/* \brief This function initializes the EMIF
 * 
 * \param none
 *
 * \return none
 *
 */
void EMIFInit(void)
{
    volatile unsigned int regVal;

    /* Enable the clocks for EMIF */
    regVal = HWREG(SOC_CM_PER_REGS + CM_PER_EMIF_FW_CLKCTRL) &
                ~(CM_PER_EMIF_FW_CLKCTRL_MODULEMODE);

    regVal |= CM_PER_EMIF_FW_CLKCTRL_MODULEMODE_ENABLE;

    HWREG(SOC_CM_PER_REGS + CM_PER_EMIF_FW_CLKCTRL) = regVal;

    regVal = HWREG(SOC_CM_PER_REGS + CM_PER_EMIF_CLKCTRL) &
                ~(CM_PER_EMIF_CLKCTRL_MODULEMODE);

    regVal |= CM_PER_EMIF_CLKCTRL_MODULEMODE_ENABLE;

    HWREG(SOC_CM_PER_REGS + CM_PER_EMIF_CLKCTRL) = regVal;

    while((HWREG(SOC_CM_PER_REGS + CM_PER_L3_CLKSTCTRL) &
          (CM_PER_L3_CLKSTCTRL_CLKACTIVITY_EMIF_GCLK |
           CM_PER_L3_CLKSTCTRL_CLKACTIVITY_L3_GCLK)) !=
          (CM_PER_L3_CLKSTCTRL_CLKACTIVITY_EMIF_GCLK |
           CM_PER_L3_CLKSTCTRL_CLKACTIVITY_L3_GCLK));
}



static void L3L4ClockInit(void){
   /*domain power state transition*/
    HWREG(SOC_CM_PER_REGS + CM_PER_L3S_CLKSTCTRL) =
                             CM_PER_L3S_CLKSTCTRL_CLKTRCTRL_SW_WKUP; //L3S
    while((HWREG(SOC_CM_PER_REGS + CM_PER_L3S_CLKSTCTRL) &
     CM_PER_L3S_CLKSTCTRL_CLKTRCTRL) != CM_PER_L3S_CLKSTCTRL_CLKTRCTRL_SW_WKUP);

    HWREG(SOC_CM_PER_REGS + CM_PER_L3_CLKSTCTRL) =
                             CM_PER_L3_CLKSTCTRL_CLKTRCTRL_SW_WKUP;//L3
    while((HWREG(SOC_CM_PER_REGS + CM_PER_L3_CLKSTCTRL) &
     CM_PER_L3_CLKSTCTRL_CLKTRCTRL) != CM_PER_L3_CLKSTCTRL_CLKTRCTRL_SW_WKUP);

   /*clocl control */
    HWREG(SOC_CM_PER_REGS + CM_PER_L3_INSTR_CLKCTRL) =
                             CM_PER_L3_INSTR_CLKCTRL_MODULEMODE_ENABLE;//L3_INSTR

    while((HWREG(SOC_CM_PER_REGS + CM_PER_L3_INSTR_CLKCTRL) &
                               CM_PER_L3_INSTR_CLKCTRL_MODULEMODE) !=
                                   CM_PER_L3_INSTR_CLKCTRL_MODULEMODE_ENABLE);

    HWREG(SOC_CM_PER_REGS + CM_PER_L3_CLKCTRL) =    
                             CM_PER_L3_CLKCTRL_MODULEMODE_ENABLE;//L3_interconnect

    while((HWREG(SOC_CM_PER_REGS + CM_PER_L3_CLKCTRL) &
        CM_PER_L3_CLKCTRL_MODULEMODE) != CM_PER_L3_CLKCTRL_MODULEMODE_ENABLE);


/*domain power state transition*/
    HWREG(SOC_CM_PER_REGS + CM_PER_OCPWP_L3_CLKSTCTRL) =
                             CM_PER_OCPWP_L3_CLKSTCTRL_CLKTRCTRL_SW_WKUP; //OCPWP_L3
    while((HWREG(SOC_CM_PER_REGS + CM_PER_OCPWP_L3_CLKSTCTRL) &
                              CM_PER_OCPWP_L3_CLKSTCTRL_CLKTRCTRL) !=
                                CM_PER_OCPWP_L3_CLKSTCTRL_CLKTRCTRL_SW_WKUP);

    HWREG(SOC_CM_PER_REGS + CM_PER_L4LS_CLKSTCTRL) =
                             CM_PER_L4LS_CLKSTCTRL_CLKTRCTRL_SW_WKUP;//L4LS
    while((HWREG(SOC_CM_PER_REGS + CM_PER_L4LS_CLKSTCTRL) &
                             CM_PER_L4LS_CLKSTCTRL_CLKTRCTRL) !=
                               CM_PER_L4LS_CLKSTCTRL_CLKTRCTRL_SW_WKUP);
 /*clocl control */
    HWREG(SOC_CM_PER_REGS + CM_PER_L4LS_CLKCTRL) =
                             CM_PER_L4LS_CLKCTRL_MODULEMODE_ENABLE;//L4LS
    while((HWREG(SOC_CM_PER_REGS + CM_PER_L4LS_CLKCTRL) &
      CM_PER_L4LS_CLKCTRL_MODULEMODE) != CM_PER_L4LS_CLKCTRL_MODULEMODE_ENABLE);

 /*poll L3L4 clock*/
    while(!(HWREG(SOC_CM_PER_REGS + CM_PER_L3S_CLKSTCTRL) &
            CM_PER_L3S_CLKSTCTRL_CLKACTIVITY_L3S_GCLK));

    while(!(HWREG(SOC_CM_PER_REGS + CM_PER_L3_CLKSTCTRL) &
            CM_PER_L3_CLKSTCTRL_CLKACTIVITY_L3_GCLK));

    while(!(HWREG(SOC_CM_PER_REGS + CM_PER_OCPWP_L3_CLKSTCTRL) &
           (CM_PER_OCPWP_L3_CLKSTCTRL_CLKACTIVITY_OCPWP_L3_GCLK |
            CM_PER_OCPWP_L3_CLKSTCTRL_CLKACTIVITY_OCPWP_L4_GCLK))); 

    while(!(HWREG(SOC_CM_PER_REGS + CM_PER_L4LS_CLKSTCTRL) &
           (CM_PER_L4LS_CLKSTCTRL_CLKACTIVITY_L4LS_GCLK |
            CM_PER_L4LS_CLKSTCTRL_CLKACTIVITY_CAN_CLK)));
}



static void EDMAInit(){
   EDMAModuleClkConfig();
   EDMA3Init(SOC_EDMA30CC_0_REGS);
   //enable 64 edma interrupt
   for (int i=0;i<64;i++) {
      EDMA3EnableEvtIntr(SOC_EDMA30CC_0_REGS,i);
   }
}


extern mmcsdCtrlInfo mmcsdctr[2];
mmcsdCardInfo card;



void BlPlatformConfig(void)
{
    /* Enable the control module */
    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CONTROL_CLKCTRL) =
            CM_WKUP_CONTROL_CLKCTRL_MODULEMODE_ENABLE; 
    PinMuxSetup();
    IntAINTCInit();
    IntMasterIRQEnable();
    I2CInit(MODULE_ID_I2C0,400000, NULL, 0);
    ConfigVddOpVoltage();
    HWREG(SOC_WDT_1_REGS + WDT_WSPR) = 0xAAAAu;
    while(HWREG(SOC_WDT_1_REGS + WDT_WWPS) != 0x00);

    HWREG(SOC_WDT_1_REGS + WDT_WSPR) = 0x5555u;
    while(HWREG(SOC_WDT_1_REGS + WDT_WWPS) != 0x00);

    PLLInit();
    L3L4ClockInit();
    /* EMIF Initialization */
    EMIFInit(); 
    /* DDR Initialization */
    DDR2Init();
    
    EDMAInit();
    UARTStdioInit();
    uartInit(MODULE_ID_UART1, 57600, 8,UART_PARITY_NONE,
            UART_FRAME_NUM_STB_1, UART_INT_RHR_CTI,UART_FCR_RX_TRIG_LVL_8,UART_FCR_RX_TRIG_LVL_8); //for keyboard
            
    GPIOInit(MODULE_ID_GPIO0,0,0); //for lcd blacklight
    
    GPIOInit(GPIO_BEEP_MODULE,0,0); //for beep pin
    GPIODirModeSet(modulelist[GPIO_BEEP_MODULE].baseAddr, 20,GPIO_DIR_OUTPUT);  //beep pin
    GPIOPinWrite(modulelist[GPIO_BEEP_MODULE].baseAddr,20,1);
    GPIODirModeSet(modulelist[GPIO_BEEP_MODULE].baseAddr, 20,GPIO_DIR_OUTPUT);
    
    TimerTickConfigure();    
    TimerTickStart();
    LCDRasterInit(MODULE_ID_LCDC);
    TouchScreenInit();
    tsEnalbe();

    usbMscInit(); 
    USBIntConfigure(0);
    MMCSDP_CtrlInfoInit(&mmcsdctr[0], MODULE_ID_MMCSD0, 24000000, MMCSD_BUSWIDTH_4BIT, 0,
                       &card, NULL,NULL, NULL);
    MMCSDP_CtrlInit(&mmcsdctr[0]);
    MMCSDP_CardInit(&mmcsdctr[0],MMCSD_CARD_MMC);
}



