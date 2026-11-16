/*
 * Copyright (c) 2023, Texas Instruments Incorporated - http://www.ti.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ============ ti_msp_dl_config.h =============
 *  Configured MSPM0 DriverLib module declarations
 *
 *  DO NOT EDIT - This file is generated for the MSPM0G350X
 *  by the SysConfig tool.
 */
#ifndef ti_msp_dl_config_h
#define ti_msp_dl_config_h

#define CONFIG_MSPM0G350X
#define CONFIG_MSPM0G3507

#if defined(__ti_version__) || defined(__TI_COMPILER_VERSION__)
#define SYSCONFIG_WEAK __attribute__((weak))
#elif defined(__IAR_SYSTEMS_ICC__)
#define SYSCONFIG_WEAK __weak
#elif defined(__GNUC__)
#define SYSCONFIG_WEAK __attribute__((weak))
#endif

#include <ti/devices/msp/msp.h>
#include <ti/driverlib/driverlib.h>
#include <ti/driverlib/m0p/dl_core.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  ======== SYSCFG_DL_init ========
 *  Perform all required MSP DL initialization
 *
 *  This function should be called once at a point before any use of
 *  MSP DL.
 */


/* clang-format off */

#define POWER_STARTUP_DELAY                                                (16)



#define CPUCLK_FREQ                                                     80000000



/* Defines for Motor_PWM_01 */
#define Motor_PWM_01_INST                                                  TIMG0
#define Motor_PWM_01_INST_IRQHandler                            TIMG0_IRQHandler
#define Motor_PWM_01_INST_INT_IRQN                              (TIMG0_INT_IRQn)
#define Motor_PWM_01_INST_CLK_FREQ                                      40000000
/* GPIO defines for channel 0 */
#define GPIO_Motor_PWM_01_C0_PORT                                          GPIOA
#define GPIO_Motor_PWM_01_C0_PIN                                  DL_GPIO_PIN_12
#define GPIO_Motor_PWM_01_C0_IOMUX                               (IOMUX_PINCM34)
#define GPIO_Motor_PWM_01_C0_IOMUX_FUNC              IOMUX_PINCM34_PF_TIMG0_CCP0
#define GPIO_Motor_PWM_01_C0_IDX                             DL_TIMER_CC_0_INDEX
/* GPIO defines for channel 1 */
#define GPIO_Motor_PWM_01_C1_PORT                                          GPIOA
#define GPIO_Motor_PWM_01_C1_PIN                                  DL_GPIO_PIN_13
#define GPIO_Motor_PWM_01_C1_IOMUX                               (IOMUX_PINCM35)
#define GPIO_Motor_PWM_01_C1_IOMUX_FUNC              IOMUX_PINCM35_PF_TIMG0_CCP1
#define GPIO_Motor_PWM_01_C1_IDX                             DL_TIMER_CC_1_INDEX

/* Defines for Motor_PWM_23 */
#define Motor_PWM_23_INST                                                  TIMG6
#define Motor_PWM_23_INST_IRQHandler                            TIMG6_IRQHandler
#define Motor_PWM_23_INST_INT_IRQN                              (TIMG6_INT_IRQn)
#define Motor_PWM_23_INST_CLK_FREQ                                      80000000
/* GPIO defines for channel 0 */
#define GPIO_Motor_PWM_23_C0_PORT                                          GPIOA
#define GPIO_Motor_PWM_23_C0_PIN                                  DL_GPIO_PIN_29
#define GPIO_Motor_PWM_23_C0_IOMUX                                (IOMUX_PINCM4)
#define GPIO_Motor_PWM_23_C0_IOMUX_FUNC               IOMUX_PINCM4_PF_TIMG6_CCP0
#define GPIO_Motor_PWM_23_C0_IDX                             DL_TIMER_CC_0_INDEX
/* GPIO defines for channel 1 */
#define GPIO_Motor_PWM_23_C1_PORT                                          GPIOB
#define GPIO_Motor_PWM_23_C1_PIN                                  DL_GPIO_PIN_27
#define GPIO_Motor_PWM_23_C1_IOMUX                               (IOMUX_PINCM58)
#define GPIO_Motor_PWM_23_C1_IOMUX_FUNC              IOMUX_PINCM58_PF_TIMG6_CCP1
#define GPIO_Motor_PWM_23_C1_IDX                             DL_TIMER_CC_1_INDEX

/* Defines for PWM_Servo */
#define PWM_Servo_INST                                                     TIMG7
#define PWM_Servo_INST_IRQHandler                               TIMG7_IRQHandler
#define PWM_Servo_INST_INT_IRQN                                 (TIMG7_INT_IRQn)
#define PWM_Servo_INST_CLK_FREQ                                           800000
/* GPIO defines for channel 0 */
#define GPIO_PWM_Servo_C0_PORT                                             GPIOA
#define GPIO_PWM_Servo_C0_PIN                                     DL_GPIO_PIN_26
#define GPIO_PWM_Servo_C0_IOMUX                                  (IOMUX_PINCM59)
#define GPIO_PWM_Servo_C0_IOMUX_FUNC                 IOMUX_PINCM59_PF_TIMG7_CCP0
#define GPIO_PWM_Servo_C0_IDX                                DL_TIMER_CC_0_INDEX
/* GPIO defines for channel 1 */
#define GPIO_PWM_Servo_C1_PORT                                             GPIOA
#define GPIO_PWM_Servo_C1_PIN                                     DL_GPIO_PIN_27
#define GPIO_PWM_Servo_C1_IOMUX                                  (IOMUX_PINCM60)
#define GPIO_PWM_Servo_C1_IOMUX_FUNC                 IOMUX_PINCM60_PF_TIMG7_CCP1
#define GPIO_PWM_Servo_C1_IDX                                DL_TIMER_CC_1_INDEX



/* Defines for TIMER_0 */
#define TIMER_0_INST                                                     (TIMA0)
#define TIMER_0_INST_IRQHandler                                 TIMA0_IRQHandler
#define TIMER_0_INST_INT_IRQN                                   (TIMA0_INT_IRQn)
#define TIMER_0_INST_LOAD_VALUE                                           (799U)




/* Defines for IICOLED */
#define IICOLED_INST                                                        I2C1
#define IICOLED_INST_IRQHandler                                  I2C1_IRQHandler
#define IICOLED_INST_INT_IRQN                                      I2C1_INT_IRQn
#define IICOLED_BUS_SPEED_HZ                                              400000
#define GPIO_IICOLED_SDA_PORT                                              GPIOB
#define GPIO_IICOLED_SDA_PIN                                       DL_GPIO_PIN_3
#define GPIO_IICOLED_IOMUX_SDA                                   (IOMUX_PINCM16)
#define GPIO_IICOLED_IOMUX_SDA_FUNC                    IOMUX_PINCM16_PF_I2C1_SDA
#define GPIO_IICOLED_SCL_PORT                                              GPIOB
#define GPIO_IICOLED_SCL_PIN                                       DL_GPIO_PIN_2
#define GPIO_IICOLED_IOMUX_SCL                                   (IOMUX_PINCM15)
#define GPIO_IICOLED_IOMUX_SCL_FUNC                    IOMUX_PINCM15_PF_I2C1_SCL


/* Defines for UART_0 */
#define UART_0_INST                                                        UART0
#define UART_0_INST_FREQUENCY                                           40000000
#define UART_0_INST_IRQHandler                                  UART0_IRQHandler
#define UART_0_INST_INT_IRQN                                      UART0_INT_IRQn
#define GPIO_UART_0_RX_PORT                                                GPIOB
#define GPIO_UART_0_TX_PORT                                                GPIOB
#define GPIO_UART_0_RX_PIN                                         DL_GPIO_PIN_1
#define GPIO_UART_0_TX_PIN                                         DL_GPIO_PIN_0
#define GPIO_UART_0_IOMUX_RX                                     (IOMUX_PINCM13)
#define GPIO_UART_0_IOMUX_TX                                     (IOMUX_PINCM12)
#define GPIO_UART_0_IOMUX_RX_FUNC                      IOMUX_PINCM13_PF_UART0_RX
#define GPIO_UART_0_IOMUX_TX_FUNC                      IOMUX_PINCM12_PF_UART0_TX
#define UART_0_BAUD_RATE                                                (115200)
#define UART_0_IBRD_40_MHZ_115200_BAUD                                      (21)
#define UART_0_FBRD_40_MHZ_115200_BAUD                                      (45)




/* Defines for SPI_IMU660RB */
#define SPI_IMU660RB_INST                                                  SPI0
#define SPI_IMU660RB_INST_IRQHandler                            SPI0_IRQHandler
#define SPI_IMU660RB_INST_INT_IRQN                                SPI0_INT_IRQn
#define GPIO_SPI_IMU660RB_PICO_PORT                                       GPIOB
#define GPIO_SPI_IMU660RB_PICO_PIN                               DL_GPIO_PIN_17
#define GPIO_SPI_IMU660RB_IOMUX_PICO                            (IOMUX_PINCM43)
#define GPIO_SPI_IMU660RB_IOMUX_PICO_FUNC            IOMUX_PINCM43_PF_SPI0_PICO
#define GPIO_SPI_IMU660RB_POCI_PORT                                       GPIOB
#define GPIO_SPI_IMU660RB_POCI_PIN                               DL_GPIO_PIN_19
#define GPIO_SPI_IMU660RB_IOMUX_POCI                            (IOMUX_PINCM45)
#define GPIO_SPI_IMU660RB_IOMUX_POCI_FUNC            IOMUX_PINCM45_PF_SPI0_POCI
/* GPIO configuration for SPI_IMU660RB */
#define GPIO_SPI_IMU660RB_SCLK_PORT                                       GPIOB
#define GPIO_SPI_IMU660RB_SCLK_PIN                               DL_GPIO_PIN_18
#define GPIO_SPI_IMU660RB_IOMUX_SCLK                            (IOMUX_PINCM44)
#define GPIO_SPI_IMU660RB_IOMUX_SCLK_FUNC            IOMUX_PINCM44_PF_SPI0_SCLK



/* Port definition for Pin Group GPIO_CR */
#define GPIO_CR_PORT                                                     (GPIOA)

/* Defines for PIN_A330: GPIOA.30 with pinCMx 5 on package pin 37 */
#define GPIO_CR_PIN_A330_PIN                                    (DL_GPIO_PIN_30)
#define GPIO_CR_PIN_A330_IOMUX                                    (IOMUX_PINCM5)
/* Port definition for Pin Group GPIO_IMU660RB */
#define GPIO_IMU660RB_PORT                                               (GPIOA)

/* Defines for PIN_IMU660RB_CS: GPIOA.16 with pinCMx 38 on package pin 9 */
#define GPIO_IMU660RB_PIN_IMU660RB_CS_PIN                       (DL_GPIO_PIN_16)
#define GPIO_IMU660RB_PIN_IMU660RB_CS_IOMUX                      (IOMUX_PINCM38)
/* Defines for PIN_IMU660RB_INT1: GPIOA.17 with pinCMx 39 on package pin 10 */
// pins affected by this interrupt request:["PIN_IMU660RB_INT1"]
#define GPIO_IMU660RB_INT_IRQN                                  (GPIOA_INT_IRQn)
#define GPIO_IMU660RB_INT_IIDX                  (DL_INTERRUPT_GROUP1_IIDX_GPIOA)
#define GPIO_IMU660RB_PIN_IMU660RB_INT1_IIDX                (DL_GPIO_IIDX_DIO17)
#define GPIO_IMU660RB_PIN_IMU660RB_INT1_PIN                     (DL_GPIO_PIN_17)
#define GPIO_IMU660RB_PIN_IMU660RB_INT1_IOMUX                    (IOMUX_PINCM39)
/* Port definition for Pin Group Motor_01 */
#define Motor_01_PORT                                                    (GPIOB)

/* Defines for Motor0_GPIO: GPIOB.8 with pinCMx 25 on package pin 60 */
#define Motor_01_Motor0_GPIO_PIN                                 (DL_GPIO_PIN_8)
#define Motor_01_Motor0_GPIO_IOMUX                               (IOMUX_PINCM25)
/* Defines for Motor1_GPIO: GPIOB.9 with pinCMx 26 on package pin 61 */
#define Motor_01_Motor1_GPIO_PIN                                 (DL_GPIO_PIN_9)
#define Motor_01_Motor1_GPIO_IOMUX                               (IOMUX_PINCM26)
/* Defines for Motor0_ENA: GPIOB.7 with pinCMx 24 on package pin 59 */
// groups represented: ["Motor_23","Motor_01"]
// pins affected: ["Motor_2_ENA","Motor_3_ENA","Motor0_ENA","Motor1_ENA"]
#define GPIO_MULTIPLE_GPIOB_INT_IRQN                            (GPIOB_INT_IRQn)
#define GPIO_MULTIPLE_GPIOB_INT_IIDX            (DL_INTERRUPT_GROUP1_IIDX_GPIOB)
#define Motor_01_Motor0_ENA_IIDX                             (DL_GPIO_IIDX_DIO7)
#define Motor_01_Motor0_ENA_PIN                                  (DL_GPIO_PIN_7)
#define Motor_01_Motor0_ENA_IOMUX                                (IOMUX_PINCM24)
/* Defines for Motor0_ENB: GPIOB.6 with pinCMx 23 on package pin 58 */
#define Motor_01_Motor0_ENB_PIN                                  (DL_GPIO_PIN_6)
#define Motor_01_Motor0_ENB_IOMUX                                (IOMUX_PINCM23)
/* Defines for Motor1_ENA: GPIOB.13 with pinCMx 30 on package pin 1 */
#define Motor_01_Motor1_ENA_IIDX                            (DL_GPIO_IIDX_DIO13)
#define Motor_01_Motor1_ENA_PIN                                 (DL_GPIO_PIN_13)
#define Motor_01_Motor1_ENA_IOMUX                                (IOMUX_PINCM30)
/* Defines for Motor1_ENB: GPIOB.12 with pinCMx 29 on package pin 64 */
#define Motor_01_Motor1_ENB_PIN                                 (DL_GPIO_PIN_12)
#define Motor_01_Motor1_ENB_IOMUX                                (IOMUX_PINCM29)
/* Port definition for Pin Group Motor_23 */
#define Motor_23_PORT                                                    (GPIOB)

/* Defines for Motor2: GPIOB.26 with pinCMx 57 on package pin 28 */
#define Motor_23_Motor2_PIN                                     (DL_GPIO_PIN_26)
#define Motor_23_Motor2_IOMUX                                    (IOMUX_PINCM57)
/* Defines for Motor3: GPIOB.23 with pinCMx 51 on package pin 22 */
#define Motor_23_Motor3_PIN                                     (DL_GPIO_PIN_23)
#define Motor_23_Motor3_IOMUX                                    (IOMUX_PINCM51)
/* Defines for Motor_2_ENA: GPIOB.15 with pinCMx 32 on package pin 3 */
#define Motor_23_Motor_2_ENA_IIDX                           (DL_GPIO_IIDX_DIO15)
#define Motor_23_Motor_2_ENA_PIN                                (DL_GPIO_PIN_15)
#define Motor_23_Motor_2_ENA_IOMUX                               (IOMUX_PINCM32)
/* Defines for Motor_2_ENB: GPIOB.16 with pinCMx 33 on package pin 4 */
#define Motor_23_Motor_2_ENB_PIN                                (DL_GPIO_PIN_16)
#define Motor_23_Motor_2_ENB_IOMUX                               (IOMUX_PINCM33)
/* Defines for Motor_3_ENA: GPIOB.4 with pinCMx 17 on package pin 52 */
#define Motor_23_Motor_3_ENA_IIDX                            (DL_GPIO_IIDX_DIO4)
#define Motor_23_Motor_3_ENA_PIN                                 (DL_GPIO_PIN_4)
#define Motor_23_Motor_3_ENA_IOMUX                               (IOMUX_PINCM17)
/* Defines for Motor_3_ENB: GPIOB.5 with pinCMx 18 on package pin 53 */
#define Motor_23_Motor_3_ENB_PIN                                 (DL_GPIO_PIN_5)
#define Motor_23_Motor_3_ENB_IOMUX                               (IOMUX_PINCM18)
/* Port definition for Pin Group Button */
#define Button_PORT                                                      (GPIOA)

/* Defines for PIN_28: GPIOA.28 with pinCMx 3 on package pin 35 */
#define Button_PIN_28_PIN                                       (DL_GPIO_PIN_28)
#define Button_PIN_28_IOMUX                                       (IOMUX_PINCM3)
/* Defines for PIN_31: GPIOA.31 with pinCMx 6 on package pin 39 */
#define Button_PIN_31_PIN                                       (DL_GPIO_PIN_31)
#define Button_PIN_31_IOMUX                                       (IOMUX_PINCM6)
/* Defines for PIN_8: GPIOA.8 with pinCMx 19 on package pin 54 */
#define Button_PIN_8_PIN                                         (DL_GPIO_PIN_8)
#define Button_PIN_8_IOMUX                                       (IOMUX_PINCM19)
/* Defines for PIN_9: GPIOA.9 with pinCMx 20 on package pin 55 */
#define Button_PIN_9_PIN                                         (DL_GPIO_PIN_9)
#define Button_PIN_9_IOMUX                                       (IOMUX_PINCM20)
/* Defines for PIN_0: GPIOA.24 with pinCMx 54 on package pin 25 */
#define GPIO_Tracker_PIN_0_PORT                                          (GPIOA)
#define GPIO_Tracker_PIN_0_PIN                                  (DL_GPIO_PIN_24)
#define GPIO_Tracker_PIN_0_IOMUX                                 (IOMUX_PINCM54)
/* Defines for PIN_1: GPIOA.25 with pinCMx 55 on package pin 26 */
#define GPIO_Tracker_PIN_1_PORT                                          (GPIOA)
#define GPIO_Tracker_PIN_1_PIN                                  (DL_GPIO_PIN_25)
#define GPIO_Tracker_PIN_1_IOMUX                                 (IOMUX_PINCM55)
/* Defines for PIN_2: GPIOB.24 with pinCMx 52 on package pin 23 */
#define GPIO_Tracker_PIN_2_PORT                                          (GPIOB)
#define GPIO_Tracker_PIN_2_PIN                                  (DL_GPIO_PIN_24)
#define GPIO_Tracker_PIN_2_IOMUX                                 (IOMUX_PINCM52)
/* Defines for PIN_3: GPIOB.25 with pinCMx 56 on package pin 27 */
#define GPIO_Tracker_PIN_3_PORT                                          (GPIOB)
#define GPIO_Tracker_PIN_3_PIN                                  (DL_GPIO_PIN_25)
#define GPIO_Tracker_PIN_3_IOMUX                                 (IOMUX_PINCM56)
/* Defines for PIN_4: GPIOA.22 with pinCMx 47 on package pin 18 */
#define GPIO_Tracker_PIN_4_PORT                                          (GPIOA)
#define GPIO_Tracker_PIN_4_PIN                                  (DL_GPIO_PIN_22)
#define GPIO_Tracker_PIN_4_IOMUX                                 (IOMUX_PINCM47)
/* Defines for PIN_5: GPIOB.20 with pinCMx 48 on package pin 19 */
#define GPIO_Tracker_PIN_5_PORT                                          (GPIOB)
#define GPIO_Tracker_PIN_5_PIN                                  (DL_GPIO_PIN_20)
#define GPIO_Tracker_PIN_5_IOMUX                                 (IOMUX_PINCM48)
/* Defines for PIN_6: GPIOA.15 with pinCMx 37 on package pin 8 */
#define GPIO_Tracker_PIN_6_PORT                                          (GPIOA)
#define GPIO_Tracker_PIN_6_PIN                                  (DL_GPIO_PIN_15)
#define GPIO_Tracker_PIN_6_IOMUX                                 (IOMUX_PINCM37)
/* Defines for PIN_7: GPIOA.14 with pinCMx 36 on package pin 7 */
#define GPIO_Tracker_PIN_7_PORT                                          (GPIOA)
#define GPIO_Tracker_PIN_7_PIN                                  (DL_GPIO_PIN_14)
#define GPIO_Tracker_PIN_7_IOMUX                                 (IOMUX_PINCM36)
/* Port definition for Pin Group GPIO_Mos */
#define GPIO_Mos_PORT                                                    (GPIOB)

/* Defines for PIN_B22: GPIOB.22 with pinCMx 50 on package pin 21 */
#define GPIO_Mos_PIN_B22_PIN                                    (DL_GPIO_PIN_22)
#define GPIO_Mos_PIN_B22_IOMUX                                   (IOMUX_PINCM50)
/* Defines for PIN_B21: GPIOB.21 with pinCMx 49 on package pin 20 */
#define GPIO_Mos_PIN_B21_PIN                                    (DL_GPIO_PIN_21)
#define GPIO_Mos_PIN_B21_IOMUX                                   (IOMUX_PINCM49)
/* Defines for PIN_10: GPIOB.11 with pinCMx 28 on package pin 63 */
#define GPIO_duoyv_PIN_10_PORT                                           (GPIOB)
#define GPIO_duoyv_PIN_10_PIN                                   (DL_GPIO_PIN_11)
#define GPIO_duoyv_PIN_10_IOMUX                                  (IOMUX_PINCM28)
/* Defines for PIN_11: GPIOB.14 with pinCMx 31 on package pin 2 */
#define GPIO_duoyv_PIN_11_PORT                                           (GPIOB)
#define GPIO_duoyv_PIN_11_PIN                                   (DL_GPIO_PIN_14)
#define GPIO_duoyv_PIN_11_IOMUX                                  (IOMUX_PINCM31)
/* Defines for PIN_12: GPIOB.10 with pinCMx 27 on package pin 62 */
#define GPIO_duoyv_PIN_12_PORT                                           (GPIOB)
#define GPIO_duoyv_PIN_12_PIN                                   (DL_GPIO_PIN_10)
#define GPIO_duoyv_PIN_12_IOMUX                                  (IOMUX_PINCM27)
/* Defines for PIN_13: GPIOA.7 with pinCMx 14 on package pin 49 */
#define GPIO_duoyv_PIN_13_PORT                                           (GPIOA)
#define GPIO_duoyv_PIN_13_PIN                                    (DL_GPIO_PIN_7)
#define GPIO_duoyv_PIN_13_IOMUX                                  (IOMUX_PINCM14)

/* clang-format on */

void SYSCFG_DL_init(void);
void SYSCFG_DL_initPower(void);
void SYSCFG_DL_GPIO_init(void);
void SYSCFG_DL_SYSCTL_init(void);
void SYSCFG_DL_Motor_PWM_01_init(void);
void SYSCFG_DL_Motor_PWM_23_init(void);
void SYSCFG_DL_PWM_Servo_init(void);
void SYSCFG_DL_TIMER_0_init(void);
void SYSCFG_DL_IICOLED_init(void);
void SYSCFG_DL_UART_0_init(void);
void SYSCFG_DL_SPI_IMU660RB_init(void);


bool SYSCFG_DL_saveConfiguration(void);
bool SYSCFG_DL_restoreConfiguration(void);

#ifdef __cplusplus
}
#endif

#endif /* ti_msp_dl_config_h */
