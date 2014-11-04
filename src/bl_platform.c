
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
#ifdef YUANJI
MUX_VAL(CONTROL_PADCONF_GPMC_AD0, (IEN | PD | MODE0)) /* gpmc_ad0 */\
	MUX_VAL(CONTROL_PADCONF_GPMC_AD1, (IEN | PD | MODE0)) /* gpmc_ad1 */\
	MUX_VAL(CONTROL_PADCONF_GPMC_AD2, (IEN | PD | MODE0)) /* gpmc_ad2 */\
	MUX_VAL(CONTROL_PADCONF_GPMC_AD3, (IEN | PD | MODE0)) /* gpmc_ad3 */\
	MUX_VAL(CONTROL_PADCONF_GPMC_AD4, (IEN | PD | MODE0)) /* gpmc_ad4 */\
	MUX_VAL(CONTROL_PADCONF_GPMC_AD5, (IEN | PD | MODE0)) /* gpmc_ad5 */\
	MUX_VAL(CONTROL_PADCONF_GPMC_AD6, (IEN | PD | MODE0)) /* gpmc_ad6 */\
	MUX_VAL(CONTROL_PADCONF_GPMC_AD7, (IEN | PD | MODE0)) /* gpmc_ad7 */\
	MUX_VAL(CONTROL_PADCONF_GPMC_AD8, (IDIS | PD | MODE1)) /* lcd_data23 */\
	MUX_VAL(CONTROL_PADCONF_GPMC_AD9, (IDIS | PD | MODE1)) /* lcd_data22 */\
	MUX_VAL(CONTROL_PADCONF_GPMC_AD10, (IDIS | PD | MODE1)) /* lcd_data21 */\
	MUX_VAL(CONTROL_PADCONF_GPMC_AD11, (IDIS | PD | MODE1)) /* lcd_data20 */\
	MUX_VAL(CONTROL_PADCONF_GPMC_AD12, (IDIS | PD | MODE1)) /* lcd_data19 */\
	MUX_VAL(CONTROL_PADCONF_GPMC_AD13, (IDIS | PD | MODE1)) /* lcd_data18 */\
	MUX_VAL(CONTROL_PADCONF_GPMC_AD14, (IDIS | PD | MODE1)) /* lcd_data17 */\
	MUX_VAL(CONTROL_PADCONF_GPMC_AD15, (IDIS | PD | MODE1)) /* lcd_data16 */\
	MUX_VAL(CONTROL_PADCONF_GPMC_A0, (IDIS | PD | MODE0)) /* gpmc_a0_mux0 */\
	MUX_VAL(CONTROL_PADCONF_GPMC_A1, (IDIS | PD | MODE0)) /* gpmc_a1_mux0 */\
	MUX_VAL(CONTROL_PADCONF_GPMC_A2, (IDIS | PD | MODE0)) /* gpmc_a2_mux0 */\
	MUX_VAL(CONTROL_PADCONF_GPMC_A3, (IDIS | PD | MODE0)) /* gpmc_a3_mux0 */\
	MUX_VAL(CONTROL_PADCONF_GPMC_A4, (IDIS | PD | MODE0)) /* gpmc_a4_mux0 */\
	MUX_VAL(CONTROL_PADCONF_GPMC_A5, (IDIS | PD | MODE0)) /* gpmc_a5_mux0 */\
	MUX_VAL(CONTROL_PADCONF_GPMC_A6, (IDIS | PD | MODE0)) /* gpmc_a6_mux0 */\
	MUX_VAL(CONTROL_PADCONF_GPMC_A7, (IDIS | PD | MODE0)) /* gpmc_a7_mux0 */\
	MUX_VAL(CONTROL_PADCONF_GPMC_A8, (IDIS | PD | MODE0)) /* gpmc_a8_mux0 */\
	MUX_VAL(CONTROL_PADCONF_GPMC_A9, (IDIS | PD | MODE0)) /* gpmc_a9_mux0 */\
	MUX_VAL(CONTROL_PADCONF_GPMC_A10, (IDIS | PD | MODE0)) /* gpmc_a10 */\
	MUX_VAL(CONTROL_PADCONF_GPMC_A11, (IDIS | PD | MODE0)) /* gpmc_a11 */\
	MUX_VAL(CONTROL_PADCONF_GPMC_WAIT0, (IDIS | PU | MODE2)) /* gpmc_csn4 */\
	MUX_VAL(CONTROL_PADCONF_GPMC_WPN, (IDIS | PU | MODE2)) /* gpmc_csn5 */\
	MUX_VAL(CONTROL_PADCONF_GPMC_BEN1, (IDIS | PU | MODE2)) /* gpmc_csn6 */\
	MUX_VAL(CONTROL_PADCONF_GPMC_CSN0, (IDIS | PU | MODE0)) /* gpmc_csn0 */\
	MUX_VAL(CONTROL_PADCONF_GPMC_CSN1, (IDIS | PU | MODE0)) /* gpmc_csn1 */\
	MUX_VAL(CONTROL_PADCONF_GPMC_CSN2, (IDIS | PU | MODE0)) /* gpmc_csn2 */\
	MUX_VAL(CONTROL_PADCONF_GPMC_CSN3, (IDIS | PU | MODE0)) /* gpmc_csn3 */\
	MUX_VAL(CONTROL_PADCONF_GPMC_CLK, (IEN | PD | MODE7)) /* gpio2[1] */\
	MUX_VAL(CONTROL_PADCONF_GPMC_ADVN_ALE, (IEN | PU | MODE7)) /* gpio2[2] */\
	MUX_VAL(CONTROL_PADCONF_GPMC_OEN_REN, (IDIS | PU | MODE0)) /* gpmc_oen_ren */\
	MUX_VAL(CONTROL_PADCONF_GPMC_WEN, (IDIS | PU | MODE0)) /* gpmc_wen */\
	MUX_VAL(CONTROL_PADCONF_GPMC_BEN0_CLE, (IEN | PU | MODE7)) /* gpio2[5] */\
	MUX_VAL(CONTROL_PADCONF_LCD_DATA0, (IEN | OFF | MODE0)) /* lcd_data0 */\
	MUX_VAL(CONTROL_PADCONF_LCD_DATA1, (IEN | OFF | MODE0)) /* lcd_data1 */\
	MUX_VAL(CONTROL_PADCONF_LCD_DATA2, (IEN | OFF | MODE0)) /* lcd_data2 */\
	MUX_VAL(CONTROL_PADCONF_LCD_DATA3, (IEN | OFF | MODE0)) /* lcd_data3 */\
	MUX_VAL(CONTROL_PADCONF_LCD_DATA4, (IEN | OFF | MODE0)) /* lcd_data4 */\
	MUX_VAL(CONTROL_PADCONF_LCD_DATA5, (IEN | OFF | MODE0)) /* lcd_data5 */\
	MUX_VAL(CONTROL_PADCONF_LCD_DATA6, (IEN | OFF | MODE0)) /* lcd_data6 */\
	MUX_VAL(CONTROL_PADCONF_LCD_DATA7, (IEN | OFF | MODE0)) /* lcd_data7 */\
	MUX_VAL(CONTROL_PADCONF_LCD_DATA8, (IEN | OFF | MODE0)) /* lcd_data8 */\
	MUX_VAL(CONTROL_PADCONF_LCD_DATA9, (IEN | OFF | MODE0)) /* lcd_data9 */\
	MUX_VAL(CONTROL_PADCONF_LCD_DATA10, (IEN | OFF | MODE0)) /* lcd_data10 */\
	MUX_VAL(CONTROL_PADCONF_LCD_DATA11, (IEN | OFF | MODE0)) /* lcd_data11 */\
	MUX_VAL(CONTROL_PADCONF_LCD_DATA12, (IEN | OFF | MODE0)) /* lcd_data12 */\
	MUX_VAL(CONTROL_PADCONF_LCD_DATA13, (IEN | OFF | MODE0)) /* lcd_data13 */\
	MUX_VAL(CONTROL_PADCONF_LCD_DATA14, (IEN | OFF | MODE0)) /* lcd_data14 */\
	MUX_VAL(CONTROL_PADCONF_LCD_DATA15, (IEN | OFF | MODE0)) /* lcd_data15 */\
	MUX_VAL(CONTROL_PADCONF_LCD_VSYNC, (IDIS | OFF | MODE0)) /* lcd_vsync */\
	MUX_VAL(CONTROL_PADCONF_LCD_HSYNC, (IDIS | OFF | MODE0)) /* lcd_hsync */\
	MUX_VAL(CONTROL_PADCONF_LCD_PCLK, (IDIS | OFF | MODE0)) /* lcd_pclk */\
	MUX_VAL(CONTROL_PADCONF_LCD_AC_BIAS_EN, (IDIS | OFF | MODE0)) /* lcd_ac_bias_en */\
	MUX_VAL(CONTROL_PADCONF_MMC0_DAT3, (IEN | PU | MODE0)) /* mmc0_dat3 */\
	MUX_VAL(CONTROL_PADCONF_MMC0_DAT2, (IEN | PU | MODE0)) /* mmc0_dat2 */\
	MUX_VAL(CONTROL_PADCONF_MMC0_DAT1, (IEN | PU | MODE0)) /* mmc0_dat1 */\
	MUX_VAL(CONTROL_PADCONF_MMC0_DAT0, (IEN | PU | MODE0)) /* mmc0_dat0 */\
	MUX_VAL(CONTROL_PADCONF_MMC0_CLK, (IEN | PU | MODE0)) /* mmc0_clk */\
	MUX_VAL(CONTROL_PADCONF_MMC0_CMD, (IEN | PU | MODE0)) /* mmc0_cmd */\
	MUX_VAL(CONTROL_PADCONF_MII1_COL, (IEN | PD | MODE7)) /* gpio3[0] */\
	MUX_VAL(CONTROL_PADCONF_MII1_CRS, (IEN | PD | MODE1)) /* rmii1_crs_dv */\
	MUX_VAL(CONTROL_PADCONF_MII1_RX_ER, (IEN | PD | MODE1)) /* rmii1_rxer */\
	MUX_VAL(CONTROL_PADCONF_MII1_TX_EN, (IDIS | PD | MODE1)) /* rmii1_txen */\
	MUX_VAL(CONTROL_PADCONF_MII1_RX_DV, (IEN | PD | MODE7)) /* gpio3[4] */\
	MUX_VAL(CONTROL_PADCONF_MII1_TXD3, (IEN | PD | MODE7)) /* gpio0[16] */\
	MUX_VAL(CONTROL_PADCONF_MII1_TXD2, (IEN | PD | MODE7)) /* gpio0[17] */\
	MUX_VAL(CONTROL_PADCONF_MII1_TXD1, (IDIS | PD | MODE1)) /* rmii1_txd1 */\
	MUX_VAL(CONTROL_PADCONF_MII1_TXD0, (IDIS | PD | MODE1)) /* rmii1_txd0 */\
	MUX_VAL(CONTROL_PADCONF_MII1_TX_CLK, (IEN | PD | MODE7)) /* gpio3[9] */\
	MUX_VAL(CONTROL_PADCONF_MII1_RX_CLK, (IEN | PD | MODE7)) /* gpio3[10] */\
	MUX_VAL(CONTROL_PADCONF_MII1_RXD3, (IEN | PD | MODE1)) /* uart3_rxd_mux0 */\
	MUX_VAL(CONTROL_PADCONF_MII1_RXD2, (IDIS | PD | MODE1)) /* uart3_txd_mux0 */\
	MUX_VAL(CONTROL_PADCONF_MII1_RXD1, (IEN | PD | MODE1)) /* rmii1_rxd1 */\
	MUX_VAL(CONTROL_PADCONF_MII1_RXD0, (IEN | PD | MODE1)) /* rmii1_rxd0 */\
	MUX_VAL(CONTROL_PADCONF_RMII1_REF_CLK, (IEN | PD | MODE0)) /* rmii1_refclk */\
	MUX_VAL(CONTROL_PADCONF_MDIO, (IEN | PU | MODE0)) /* mdio_data */\
	MUX_VAL(CONTROL_PADCONF_MDC, (IDIS | PU | MODE0)) /* mdio_clk */\
	MUX_VAL(CONTROL_PADCONF_SPI0_SCLK, (IEN | OFF | MODE7)) /* gpio0[2] */\
	MUX_VAL(CONTROL_PADCONF_SPI0_D0, (IEN | OFF | MODE7)) /* gpio0[3] */\
	MUX_VAL(CONTROL_PADCONF_SPI0_D1, (IEN | OFF | MODE7)) /* gpio0[4] */\
	MUX_VAL(CONTROL_PADCONF_SPI0_CS0, (IEN | OFF | MODE7)) /* gpio0[5] */\
	MUX_VAL(CONTROL_PADCONF_SPI0_CS1, (IEN | OFF | MODE7)) /* gpio0[6] */\
	MUX_VAL(CONTROL_PADCONF_ECAP0_IN_PWM0_OUT, (IEN | OFF | MODE7)) /* gpio0[7] */\
	MUX_VAL(CONTROL_PADCONF_UART0_CTSN, (IEN | OFF | MODE7)) /* gpio1[8] */\
	MUX_VAL(CONTROL_PADCONF_UART0_RTSN, (IEN | OFF | MODE7)) /* gpio1[9] */\
	MUX_VAL(CONTROL_PADCONF_UART0_RXD, (IEN | OFF | MODE0)) /* uart0_rxd */\
	MUX_VAL(CONTROL_PADCONF_UART0_TXD, (IDIS | OFF | MODE0)) /* uart0_txd */\
	MUX_VAL(CONTROL_PADCONF_UART1_CTSN, (IDIS | OFF | MODE2)) /* dcan0_tx_mux2 */\
	MUX_VAL(CONTROL_PADCONF_UART1_RTSN, (IEN | OFF | MODE2)) /* dcan0_rx_mux2 */\
	MUX_VAL(CONTROL_PADCONF_UART1_RXD, (IDIS | OFF | MODE2)) /* dcan1_tx_mux1 */\
	MUX_VAL(CONTROL_PADCONF_UART1_TXD, (IEN | OFF | MODE2)) /* dcan1_rx_mux1 */\
	MUX_VAL(CONTROL_PADCONF_I2C0_SDA, (IEN | OFF | MODE0)) /* I2C0_SDA */\
	MUX_VAL(CONTROL_PADCONF_I2C0_SCL, (IEN | OFF | MODE0)) /* I2C0_SCL */\
	MUX_VAL(CONTROL_PADCONF_MCASP0_ACLKX, (IEN | PD | MODE7)) /* gpio3[14] */\
	MUX_VAL(CONTROL_PADCONF_MCASP0_FSX, (IEN | PD | MODE7)) /* gpio3[15] */\
	MUX_VAL(CONTROL_PADCONF_MCASP0_AXR0, (IEN | PD | MODE7)) /* gpio3[16] */\
	MUX_VAL(CONTROL_PADCONF_MCASP0_AHCLKR, (IEN | PD | MODE7)) /* gpio3[17] */\
	MUX_VAL(CONTROL_PADCONF_MCASP0_ACLKR, (IEN | PD | MODE1)) /* eQEP0A_in_mux0 */\
	MUX_VAL(CONTROL_PADCONF_MCASP0_FSR, (IEN | PD | MODE1)) /* eQEP0B_in_mux0 */\
	MUX_VAL(CONTROL_PADCONF_MCASP0_AXR1, (IEN | PD | MODE1)) /* eQEP0_index_mux0 */\
	MUX_VAL(CONTROL_PADCONF_MCASP0_AHCLKX, (IEN | PD | MODE1)) /* eQEP0_strobe_mux0 */\
	MUX_VAL(CONTROL_PADCONF_XDMA_EVENT_INTR0, (IEN | OFF | MODE7)) /* gpio0[19] */\
	MUX_VAL(CONTROL_PADCONF_XDMA_EVENT_INTR1, (IEN | OFF | MODE7)) /* gpio0[20] */\
	MUX_VAL(CONTROL_PADCONF_WARMRSTN, (IEN | OFF | MODE0)) /* nRESETIN_OUT */\
	MUX_VAL(CONTROL_PADCONF_EXTINTN, (IEN | OFF | MODE0)) /* nNMI */\
	MUX_VAL(CONTROL_PADCONF_TMS, (IEN | PU | MODE0)) /* TMS */\
	MUX_VAL(CONTROL_PADCONF_TDI, (IEN | PU | MODE0)) /* TDI */\
	MUX_VAL(CONTROL_PADCONF_TDO, (IDIS | PU | MODE0)) /* TDO */\
	MUX_VAL(CONTROL_PADCONF_TCK, (IEN | PU | MODE0)) /* TCK */\
	MUX_VAL(CONTROL_PADCONF_TRSTN, (IEN | PD | MODE0)) /* nTRST */\
	MUX_VAL(CONTROL_PADCONF_EMU0, (IEN | PU | MODE7)) /* gpio3[7] */\
	MUX_VAL(CONTROL_PADCONF_EMU1, (IEN | PU | MODE7)) /* gpio3[8] */\
	MUX_VAL(CONTROL_PADCONF_RTC_PWRONRSTN, (IEN | OFF | MODE0)) /* RTC_porz */\
	MUX_VAL(CONTROL_PADCONF_PMIC_POWER_EN, (IDIS | PU | MODE0)) /* PMIC_POWER_EN */\
	MUX_VAL(CONTROL_PADCONF_EXT_WAKEUP, (IEN | PD | MODE0)) /* EXT_WAKEUP */\
	MUX_VAL(CONTROL_PADCONF_RTC_KALDO_ENN, (IEN | OFF | MODE0)) /* ENZ_KALDO_1P8V */\
	MUX_VAL(CONTROL_PADCONF_USB1_DRVVBUS, (IDIS | PD | MODE0)) /* USB1_DRVVBUS */\
          ;
#else
  MUX_VAL(CONTROL_PADCONF_MMC0_DAT3, (IEN | PU | MODE0 )) /* MMC0_DAT3 */\
  MUX_VAL(CONTROL_PADCONF_MMC0_DAT2, (IEN | PU | MODE0 )) /* MMC0_DAT2 */\
  MUX_VAL(CONTROL_PADCONF_MMC0_DAT1, (IEN | PU | MODE0 )) /* MMC0_DAT1 */\
  MUX_VAL(CONTROL_PADCONF_MMC0_DAT0, (IEN | PU | MODE0 )) /* MMC0_DAT0 */\
  MUX_VAL(CONTROL_PADCONF_MMC0_CLK, (IEN | PU | MODE0 )) /* MMC0_CLK */\
  MUX_VAL(CONTROL_PADCONF_MMC0_CMD, (IEN | PU | MODE0 )) /* MMC0_CMD */\
  MUX_VAL(CONTROL_PADCONF_I2C0_SDA, (IEN | OFF | MODE0 )) /* I2C0_SDA */\
  MUX_VAL(CONTROL_PADCONF_I2C0_SCL, (IEN | OFF | MODE0 )) /* I2C0_SCL */\
MUX_VAL(CONTROL_PADCONF_SPI0_SCLK, (IEN | OFF | MODE0 )) /* SPI0_SCLK */\
MUX_VAL(CONTROL_PADCONF_SPI0_D0, (IEN | OFF | MODE0 )) /* SPI0_D0 */\
MUX_VAL(CONTROL_PADCONF_SPI0_D1, (IEN | OFF | MODE0 )) /* SPI0_D1 */\
MUX_VAL(CONTROL_PADCONF_SPI0_CS0, (IEN | PU | MODE0 )) /* SPI0_CS0 */\
MUX_VAL(CONTROL_PADCONF_GPMC_AD8, (IEN | OFF | MODE1 )) /* LCD_DATA23 */\
MUX_VAL(CONTROL_PADCONF_GPMC_AD9, (IEN | OFF | MODE1 )) /* LCD_DATA22 */\
MUX_VAL(CONTROL_PADCONF_GPMC_AD10, (IEN | OFF | MODE1 )) /* LCD_DATA21 */\
MUX_VAL(CONTROL_PADCONF_GPMC_AD11, (IEN | OFF | MODE1 )) /* LCD_DATA20 */\
MUX_VAL(CONTROL_PADCONF_GPMC_AD12, (IEN | OFF | MODE1 )) /* LCD_DATA19 */\
MUX_VAL(CONTROL_PADCONF_GPMC_AD13, (IEN | OFF | MODE1 )) /* LCD_DATA18 */\
MUX_VAL(CONTROL_PADCONF_GPMC_AD14, (IEN | OFF | MODE1 )) /* LCD_DATA17 */\
MUX_VAL(CONTROL_PADCONF_GPMC_AD15, (IEN | OFF | MODE1 )) /* LCD_DATA16 */\
MUX_VAL(CONTROL_PADCONF_GPMC_WAIT0, (IEN | PU | MODE6 )) /* UART4_RXD_MUX2 */\
MUX_VAL(CONTROL_PADCONF_GPMC_WPN, (IEN | PU | MODE6 )) /* UART4_TXD_MUX2 */\
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
MUX_VAL(CONTROL_PADCONF_LCD_AC_BIAS_EN, (IDIS | OFF | MODE0 )) /* LCD_AC_BIAS_EN */\
MUX_VAL(CONTROL_PADCONF_USB0_DRVVBUS, (IDIS | PD | MODE0 )) /* USB0_DRVVBUS */\
;
#endif
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
    TimerTickConfigure(MODULE_ID_TIMER2);
    TimerTickStart();
    usbMscInit();

    UARTStdioInit();
#ifdef YUANJI
    GPMCInitForNOR();
    USBIntConfigure(1);
    TouchScreenInit();
#else
    uartInit(MODULE_ID_UART4, 38400, 8,UART_PARITY_NONE,
            UART_FRAME_NUM_STB_1, UART_INT_RHR_CTI,UART_FCR_RX_TRIG_LVL_8,UART_FCR_RX_TRIG_LVL_8); //for keyboard
    USBIntConfigure(0);
#endif

    //SPIMasterInit(MODULE_ID_SPI0, 0, 12000000, 1, 1, 0,8);
    SPIMasterInit_poll(MODULE_ID_SPI0,0,12000000,0,0,0,8);
    LCDRasterInit();
    tsEnalbe();
    MMCSDP_CtrlInfoInit(&mmcsdctr[0], MODULE_ID_MMCSD0, 48000000, MMCSD_BUSWIDTH_4BIT, 0,
                       &card, NULL,NULL, NULL);
    MMCSDP_CtrlInit(&mmcsdctr[0]);
    MMCSDP_CardInit(&mmcsdctr[0],MMCSD_CARD_MMC);
    RTCInit();
}






