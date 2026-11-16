/*
 * Copyright (c) 2023, Texas Instruments Incorporated
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
 *  ============ ti_msp_dl_config.c =============
 *  Configured MSPM0 DriverLib module definitions
 *
 *  DO NOT EDIT - This file is generated for the MSPM0G350X
 *  by the SysConfig tool.
 */

#include "ti_msp_dl_config.h"

DL_TimerG_backupConfig gMotor_PWM_23Backup;
DL_TimerG_backupConfig gPWM_ServoBackup;
DL_TimerA_backupConfig gTIMER_0Backup;
DL_SPI_backupConfig gSPI_IMU660RBBackup;

/*
 *  ======== SYSCFG_DL_init ========
 *  Perform any initialization needed before using any board APIs
 */
SYSCONFIG_WEAK void SYSCFG_DL_init(void)
{
    SYSCFG_DL_initPower();
    SYSCFG_DL_GPIO_init();
    /* Module-Specific Initializations*/
    SYSCFG_DL_SYSCTL_init();
    SYSCFG_DL_Motor_PWM_01_init();
    SYSCFG_DL_Motor_PWM_23_init();
    SYSCFG_DL_PWM_Servo_init();
    SYSCFG_DL_TIMER_0_init();
    SYSCFG_DL_IICOLED_init();
    SYSCFG_DL_UART_0_init();
    SYSCFG_DL_SPI_IMU660RB_init();
    /* Ensure backup structures have no valid state */
	gMotor_PWM_23Backup.backupRdy 	= false;
	gPWM_ServoBackup.backupRdy 	= false;
	gTIMER_0Backup.backupRdy 	= false;

	gSPI_IMU660RBBackup.backupRdy 	= false;

}
/*
 * User should take care to save and restore register configuration in application.
 * See Retention Configuration section for more details.
 */
SYSCONFIG_WEAK bool SYSCFG_DL_saveConfiguration(void)
{
    bool retStatus = true;

	retStatus &= DL_TimerG_saveConfiguration(Motor_PWM_23_INST, &gMotor_PWM_23Backup);
	retStatus &= DL_TimerG_saveConfiguration(PWM_Servo_INST, &gPWM_ServoBackup);
	retStatus &= DL_TimerA_saveConfiguration(TIMER_0_INST, &gTIMER_0Backup);
	retStatus &= DL_SPI_saveConfiguration(SPI_IMU660RB_INST, &gSPI_IMU660RBBackup);

    return retStatus;
}


SYSCONFIG_WEAK bool SYSCFG_DL_restoreConfiguration(void)
{
    bool retStatus = true;

	retStatus &= DL_TimerG_restoreConfiguration(Motor_PWM_23_INST, &gMotor_PWM_23Backup, false);
	retStatus &= DL_TimerG_restoreConfiguration(PWM_Servo_INST, &gPWM_ServoBackup, false);
	retStatus &= DL_TimerA_restoreConfiguration(TIMER_0_INST, &gTIMER_0Backup, false);
	retStatus &= DL_SPI_restoreConfiguration(SPI_IMU660RB_INST, &gSPI_IMU660RBBackup);

    return retStatus;
}

SYSCONFIG_WEAK void SYSCFG_DL_initPower(void)
{
    DL_GPIO_reset(GPIOA);
    DL_GPIO_reset(GPIOB);
    DL_TimerG_reset(Motor_PWM_01_INST);
    DL_TimerG_reset(Motor_PWM_23_INST);
    DL_TimerG_reset(PWM_Servo_INST);
    DL_TimerA_reset(TIMER_0_INST);
    DL_I2C_reset(IICOLED_INST);
    DL_UART_Main_reset(UART_0_INST);
    DL_SPI_reset(SPI_IMU660RB_INST);

    DL_GPIO_enablePower(GPIOA);
    DL_GPIO_enablePower(GPIOB);
    DL_TimerG_enablePower(Motor_PWM_01_INST);
    DL_TimerG_enablePower(Motor_PWM_23_INST);
    DL_TimerG_enablePower(PWM_Servo_INST);
    DL_TimerA_enablePower(TIMER_0_INST);
    DL_I2C_enablePower(IICOLED_INST);
    DL_UART_Main_enablePower(UART_0_INST);
    DL_SPI_enablePower(SPI_IMU660RB_INST);
    delay_cycles(POWER_STARTUP_DELAY);
}

SYSCONFIG_WEAK void SYSCFG_DL_GPIO_init(void)
{

    DL_GPIO_initPeripheralOutputFunction(GPIO_Motor_PWM_01_C0_IOMUX,GPIO_Motor_PWM_01_C0_IOMUX_FUNC);
    DL_GPIO_enableOutput(GPIO_Motor_PWM_01_C0_PORT, GPIO_Motor_PWM_01_C0_PIN);
    DL_GPIO_initPeripheralOutputFunction(GPIO_Motor_PWM_01_C1_IOMUX,GPIO_Motor_PWM_01_C1_IOMUX_FUNC);
    DL_GPIO_enableOutput(GPIO_Motor_PWM_01_C1_PORT, GPIO_Motor_PWM_01_C1_PIN);
    DL_GPIO_initPeripheralOutputFunction(GPIO_Motor_PWM_23_C0_IOMUX,GPIO_Motor_PWM_23_C0_IOMUX_FUNC);
    DL_GPIO_enableOutput(GPIO_Motor_PWM_23_C0_PORT, GPIO_Motor_PWM_23_C0_PIN);
    DL_GPIO_initPeripheralOutputFunction(GPIO_Motor_PWM_23_C1_IOMUX,GPIO_Motor_PWM_23_C1_IOMUX_FUNC);
    DL_GPIO_enableOutput(GPIO_Motor_PWM_23_C1_PORT, GPIO_Motor_PWM_23_C1_PIN);
    DL_GPIO_initPeripheralOutputFunction(GPIO_PWM_Servo_C0_IOMUX,GPIO_PWM_Servo_C0_IOMUX_FUNC);
    DL_GPIO_enableOutput(GPIO_PWM_Servo_C0_PORT, GPIO_PWM_Servo_C0_PIN);
    DL_GPIO_initPeripheralOutputFunction(GPIO_PWM_Servo_C1_IOMUX,GPIO_PWM_Servo_C1_IOMUX_FUNC);
    DL_GPIO_enableOutput(GPIO_PWM_Servo_C1_PORT, GPIO_PWM_Servo_C1_PIN);

    DL_GPIO_initPeripheralInputFunctionFeatures(GPIO_IICOLED_IOMUX_SDA,
        GPIO_IICOLED_IOMUX_SDA_FUNC, DL_GPIO_INVERSION_DISABLE,
        DL_GPIO_RESISTOR_NONE, DL_GPIO_HYSTERESIS_DISABLE,
        DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_initPeripheralInputFunctionFeatures(GPIO_IICOLED_IOMUX_SCL,
        GPIO_IICOLED_IOMUX_SCL_FUNC, DL_GPIO_INVERSION_DISABLE,
        DL_GPIO_RESISTOR_NONE, DL_GPIO_HYSTERESIS_DISABLE,
        DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_enableHiZ(GPIO_IICOLED_IOMUX_SDA);
    DL_GPIO_enableHiZ(GPIO_IICOLED_IOMUX_SCL);

    DL_GPIO_initPeripheralOutputFunction(
        GPIO_UART_0_IOMUX_TX, GPIO_UART_0_IOMUX_TX_FUNC);
    DL_GPIO_initPeripheralInputFunction(
        GPIO_UART_0_IOMUX_RX, GPIO_UART_0_IOMUX_RX_FUNC);

    DL_GPIO_initPeripheralOutputFunction(
        GPIO_SPI_IMU660RB_IOMUX_SCLK, GPIO_SPI_IMU660RB_IOMUX_SCLK_FUNC);
    DL_GPIO_initPeripheralOutputFunction(
        GPIO_SPI_IMU660RB_IOMUX_PICO, GPIO_SPI_IMU660RB_IOMUX_PICO_FUNC);
    DL_GPIO_initPeripheralInputFunction(
        GPIO_SPI_IMU660RB_IOMUX_POCI, GPIO_SPI_IMU660RB_IOMUX_POCI_FUNC);

    DL_GPIO_initDigitalOutput(GPIO_CR_PIN_A330_IOMUX);

    DL_GPIO_initDigitalOutput(GPIO_IMU660RB_PIN_IMU660RB_CS_IOMUX);

    DL_GPIO_initDigitalInputFeatures(GPIO_IMU660RB_PIN_IMU660RB_INT1_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_NONE,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalOutput(Motor_01_Motor0_GPIO_IOMUX);

    DL_GPIO_initDigitalOutput(Motor_01_Motor1_GPIO_IOMUX);

    DL_GPIO_initDigitalInputFeatures(Motor_01_Motor0_ENA_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_NONE,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(Motor_01_Motor0_ENB_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_NONE,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(Motor_01_Motor1_ENA_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_NONE,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(Motor_01_Motor1_ENB_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_NONE,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalOutput(Motor_23_Motor2_IOMUX);

    DL_GPIO_initDigitalOutput(Motor_23_Motor3_IOMUX);

    DL_GPIO_initDigitalInputFeatures(Motor_23_Motor_2_ENA_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_NONE,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(Motor_23_Motor_2_ENB_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_NONE,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(Motor_23_Motor_3_ENA_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_NONE,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(Motor_23_Motor_3_ENB_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_NONE,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(Button_PIN_28_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(Button_PIN_31_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(Button_PIN_8_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(Button_PIN_9_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(GPIO_Tracker_PIN_0_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(GPIO_Tracker_PIN_1_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(GPIO_Tracker_PIN_2_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(GPIO_Tracker_PIN_3_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(GPIO_Tracker_PIN_4_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(GPIO_Tracker_PIN_5_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(GPIO_Tracker_PIN_6_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(GPIO_Tracker_PIN_7_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalOutput(GPIO_Mos_PIN_B22_IOMUX);

    DL_GPIO_initDigitalOutput(GPIO_Mos_PIN_B21_IOMUX);

    DL_GPIO_initDigitalOutput(GPIO_duoyv_PIN_10_IOMUX);

    DL_GPIO_initDigitalOutput(GPIO_duoyv_PIN_11_IOMUX);

    DL_GPIO_initDigitalOutput(GPIO_duoyv_PIN_12_IOMUX);

    DL_GPIO_initDigitalOutput(GPIO_duoyv_PIN_13_IOMUX);

    DL_GPIO_clearPins(GPIOA, GPIO_CR_PIN_A330_PIN |
		GPIO_IMU660RB_PIN_IMU660RB_CS_PIN |
		GPIO_duoyv_PIN_13_PIN);
    DL_GPIO_enableOutput(GPIOA, GPIO_CR_PIN_A330_PIN |
		GPIO_IMU660RB_PIN_IMU660RB_CS_PIN |
		GPIO_duoyv_PIN_13_PIN);
    DL_GPIO_setUpperPinsPolarity(GPIOA, DL_GPIO_PIN_17_EDGE_RISE);
    DL_GPIO_clearInterruptStatus(GPIOA, GPIO_IMU660RB_PIN_IMU660RB_INT1_PIN);
    DL_GPIO_enableInterrupt(GPIOA, GPIO_IMU660RB_PIN_IMU660RB_INT1_PIN);
    DL_GPIO_clearPins(GPIOB, Motor_01_Motor0_GPIO_PIN |
		Motor_01_Motor1_GPIO_PIN |
		Motor_23_Motor2_PIN |
		Motor_23_Motor3_PIN |
		GPIO_Mos_PIN_B22_PIN |
		GPIO_Mos_PIN_B21_PIN |
		GPIO_duoyv_PIN_10_PIN |
		GPIO_duoyv_PIN_11_PIN |
		GPIO_duoyv_PIN_12_PIN);
    DL_GPIO_enableOutput(GPIOB, Motor_01_Motor0_GPIO_PIN |
		Motor_01_Motor1_GPIO_PIN |
		Motor_23_Motor2_PIN |
		Motor_23_Motor3_PIN |
		GPIO_Mos_PIN_B22_PIN |
		GPIO_Mos_PIN_B21_PIN |
		GPIO_duoyv_PIN_10_PIN |
		GPIO_duoyv_PIN_11_PIN |
		GPIO_duoyv_PIN_12_PIN);
    DL_GPIO_setLowerPinsPolarity(GPIOB, DL_GPIO_PIN_7_EDGE_RISE |
		DL_GPIO_PIN_13_EDGE_RISE |
		DL_GPIO_PIN_15_EDGE_RISE |
		DL_GPIO_PIN_4_EDGE_RISE);
    DL_GPIO_clearInterruptStatus(GPIOB, Motor_01_Motor0_ENA_PIN |
		Motor_01_Motor1_ENA_PIN |
		Motor_23_Motor_2_ENA_PIN |
		Motor_23_Motor_3_ENA_PIN);
    DL_GPIO_enableInterrupt(GPIOB, Motor_01_Motor0_ENA_PIN |
		Motor_01_Motor1_ENA_PIN |
		Motor_23_Motor_2_ENA_PIN |
		Motor_23_Motor_3_ENA_PIN);

}


static const DL_SYSCTL_SYSPLLConfig gSYSPLLConfig = {
    .inputFreq              = DL_SYSCTL_SYSPLL_INPUT_FREQ_16_32_MHZ,
	.rDivClk2x              = 1,
	.rDivClk1               = 0,
	.rDivClk0               = 0,
	.enableCLK2x            = DL_SYSCTL_SYSPLL_CLK2X_ENABLE,
	.enableCLK1             = DL_SYSCTL_SYSPLL_CLK1_DISABLE,
	.enableCLK0             = DL_SYSCTL_SYSPLL_CLK0_DISABLE,
	.sysPLLMCLK             = DL_SYSCTL_SYSPLL_MCLK_CLK2X,
	.sysPLLRef              = DL_SYSCTL_SYSPLL_REF_SYSOSC,
	.qDiv                   = 4,
	.pDiv                   = DL_SYSCTL_SYSPLL_PDIV_2
};
SYSCONFIG_WEAK void SYSCFG_DL_SYSCTL_init(void)
{

	//Low Power Mode is configured to be SLEEP0
    DL_SYSCTL_setBORThreshold(DL_SYSCTL_BOR_THRESHOLD_LEVEL_0);
    DL_SYSCTL_setFlashWaitState(DL_SYSCTL_FLASH_WAIT_STATE_2);

    
	DL_SYSCTL_setSYSOSCFreq(DL_SYSCTL_SYSOSC_FREQ_BASE);
	/* Set default configuration */
	DL_SYSCTL_disableHFXT();
	DL_SYSCTL_disableSYSPLL();
    DL_SYSCTL_configSYSPLL((DL_SYSCTL_SYSPLLConfig *) &gSYSPLLConfig);
    DL_SYSCTL_setULPCLKDivider(DL_SYSCTL_ULPCLK_DIV_2);
    DL_SYSCTL_setMCLKSource(SYSOSC, HSCLK, DL_SYSCTL_HSCLK_SOURCE_SYSPLL);
    /* INT_GROUP1 Priority */
    NVIC_SetPriority(GPIOA_INT_IRQn, 3);

}


/*
 * Timer clock configuration to be sourced by  / 1 (40000000 Hz)
 * timerClkFreq = (timerClkSrc / (timerClkDivRatio * (timerClkPrescale + 1)))
 *   40000000 Hz = 40000000 Hz / (1 * (0 + 1))
 */
static const DL_TimerG_ClockConfig gMotor_PWM_01ClockConfig = {
    .clockSel = DL_TIMER_CLOCK_BUSCLK,
    .divideRatio = DL_TIMER_CLOCK_DIVIDE_1,
    .prescale = 0U
};

static const DL_TimerG_PWMConfig gMotor_PWM_01Config = {
    .pwmMode = DL_TIMER_PWM_MODE_EDGE_ALIGN,
    .period = 40000,
    .isTimerWithFourCC = false,
    .startTimer = DL_TIMER_START,
};

SYSCONFIG_WEAK void SYSCFG_DL_Motor_PWM_01_init(void) {

    DL_TimerG_setClockConfig(
        Motor_PWM_01_INST, (DL_TimerG_ClockConfig *) &gMotor_PWM_01ClockConfig);

    DL_TimerG_initPWMMode(
        Motor_PWM_01_INST, (DL_TimerG_PWMConfig *) &gMotor_PWM_01Config);

    // Set Counter control to the smallest CC index being used
    DL_TimerG_setCounterControl(Motor_PWM_01_INST,DL_TIMER_CZC_CCCTL0_ZCOND,DL_TIMER_CAC_CCCTL0_ACOND,DL_TIMER_CLC_CCCTL0_LCOND);

    DL_TimerG_setCaptureCompareOutCtl(Motor_PWM_01_INST, DL_TIMER_CC_OCTL_INIT_VAL_LOW,
		DL_TIMER_CC_OCTL_INV_OUT_DISABLED, DL_TIMER_CC_OCTL_SRC_FUNCVAL,
		DL_TIMERG_CAPTURE_COMPARE_0_INDEX);

    DL_TimerG_setCaptCompUpdateMethod(Motor_PWM_01_INST, DL_TIMER_CC_UPDATE_METHOD_IMMEDIATE, DL_TIMERG_CAPTURE_COMPARE_0_INDEX);
    DL_TimerG_setCaptureCompareValue(Motor_PWM_01_INST, 28000, DL_TIMER_CC_0_INDEX);

    DL_TimerG_setCaptureCompareOutCtl(Motor_PWM_01_INST, DL_TIMER_CC_OCTL_INIT_VAL_LOW,
		DL_TIMER_CC_OCTL_INV_OUT_DISABLED, DL_TIMER_CC_OCTL_SRC_FUNCVAL,
		DL_TIMERG_CAPTURE_COMPARE_1_INDEX);

    DL_TimerG_setCaptCompUpdateMethod(Motor_PWM_01_INST, DL_TIMER_CC_UPDATE_METHOD_IMMEDIATE, DL_TIMERG_CAPTURE_COMPARE_1_INDEX);
    DL_TimerG_setCaptureCompareValue(Motor_PWM_01_INST, 15999, DL_TIMER_CC_1_INDEX);

    DL_TimerG_enableClock(Motor_PWM_01_INST);


    
    DL_TimerG_setCCPDirection(Motor_PWM_01_INST , DL_TIMER_CC0_OUTPUT | DL_TIMER_CC1_OUTPUT );


}
/*
 * Timer clock configuration to be sourced by  / 1 (80000000 Hz)
 * timerClkFreq = (timerClkSrc / (timerClkDivRatio * (timerClkPrescale + 1)))
 *   80000000 Hz = 80000000 Hz / (1 * (0 + 1))
 */
static const DL_TimerG_ClockConfig gMotor_PWM_23ClockConfig = {
    .clockSel = DL_TIMER_CLOCK_BUSCLK,
    .divideRatio = DL_TIMER_CLOCK_DIVIDE_1,
    .prescale = 0U
};

static const DL_TimerG_PWMConfig gMotor_PWM_23Config = {
    .pwmMode = DL_TIMER_PWM_MODE_EDGE_ALIGN,
    .period = 4000,
    .isTimerWithFourCC = false,
    .startTimer = DL_TIMER_START,
};

SYSCONFIG_WEAK void SYSCFG_DL_Motor_PWM_23_init(void) {

    DL_TimerG_setClockConfig(
        Motor_PWM_23_INST, (DL_TimerG_ClockConfig *) &gMotor_PWM_23ClockConfig);

    DL_TimerG_initPWMMode(
        Motor_PWM_23_INST, (DL_TimerG_PWMConfig *) &gMotor_PWM_23Config);

    // Set Counter control to the smallest CC index being used
    DL_TimerG_setCounterControl(Motor_PWM_23_INST,DL_TIMER_CZC_CCCTL0_ZCOND,DL_TIMER_CAC_CCCTL0_ACOND,DL_TIMER_CLC_CCCTL0_LCOND);

    DL_TimerG_setCaptureCompareOutCtl(Motor_PWM_23_INST, DL_TIMER_CC_OCTL_INIT_VAL_LOW,
		DL_TIMER_CC_OCTL_INV_OUT_DISABLED, DL_TIMER_CC_OCTL_SRC_FUNCVAL,
		DL_TIMERG_CAPTURE_COMPARE_0_INDEX);

    DL_TimerG_setCaptCompUpdateMethod(Motor_PWM_23_INST, DL_TIMER_CC_UPDATE_METHOD_IMMEDIATE, DL_TIMERG_CAPTURE_COMPARE_0_INDEX);
    DL_TimerG_setCaptureCompareValue(Motor_PWM_23_INST, 2800, DL_TIMER_CC_0_INDEX);

    DL_TimerG_setCaptureCompareOutCtl(Motor_PWM_23_INST, DL_TIMER_CC_OCTL_INIT_VAL_LOW,
		DL_TIMER_CC_OCTL_INV_OUT_DISABLED, DL_TIMER_CC_OCTL_SRC_FUNCVAL,
		DL_TIMERG_CAPTURE_COMPARE_1_INDEX);

    DL_TimerG_setCaptCompUpdateMethod(Motor_PWM_23_INST, DL_TIMER_CC_UPDATE_METHOD_IMMEDIATE, DL_TIMERG_CAPTURE_COMPARE_1_INDEX);
    DL_TimerG_setCaptureCompareValue(Motor_PWM_23_INST, 1599, DL_TIMER_CC_1_INDEX);

    DL_TimerG_enableClock(Motor_PWM_23_INST);


    
    DL_TimerG_setCCPDirection(Motor_PWM_23_INST , DL_TIMER_CC0_OUTPUT | DL_TIMER_CC1_OUTPUT );


}
/*
 * Timer clock configuration to be sourced by  / 1 (80000000 Hz)
 * timerClkFreq = (timerClkSrc / (timerClkDivRatio * (timerClkPrescale + 1)))
 *   800000 Hz = 80000000 Hz / (1 * (99 + 1))
 */
static const DL_TimerG_ClockConfig gPWM_ServoClockConfig = {
    .clockSel = DL_TIMER_CLOCK_BUSCLK,
    .divideRatio = DL_TIMER_CLOCK_DIVIDE_1,
    .prescale = 99U
};

static const DL_TimerG_PWMConfig gPWM_ServoConfig = {
    .pwmMode = DL_TIMER_PWM_MODE_EDGE_ALIGN,
    .period = 16000,
    .isTimerWithFourCC = false,
    .startTimer = DL_TIMER_START,
};

SYSCONFIG_WEAK void SYSCFG_DL_PWM_Servo_init(void) {

    DL_TimerG_setClockConfig(
        PWM_Servo_INST, (DL_TimerG_ClockConfig *) &gPWM_ServoClockConfig);

    DL_TimerG_initPWMMode(
        PWM_Servo_INST, (DL_TimerG_PWMConfig *) &gPWM_ServoConfig);

    // Set Counter control to the smallest CC index being used
    DL_TimerG_setCounterControl(PWM_Servo_INST,DL_TIMER_CZC_CCCTL0_ZCOND,DL_TIMER_CAC_CCCTL0_ACOND,DL_TIMER_CLC_CCCTL0_LCOND);

    DL_TimerG_setCaptureCompareOutCtl(PWM_Servo_INST, DL_TIMER_CC_OCTL_INIT_VAL_LOW,
		DL_TIMER_CC_OCTL_INV_OUT_DISABLED, DL_TIMER_CC_OCTL_SRC_FUNCVAL,
		DL_TIMERG_CAPTURE_COMPARE_0_INDEX);

    DL_TimerG_setCaptCompUpdateMethod(PWM_Servo_INST, DL_TIMER_CC_UPDATE_METHOD_IMMEDIATE, DL_TIMERG_CAPTURE_COMPARE_0_INDEX);
    DL_TimerG_setCaptureCompareValue(PWM_Servo_INST, 16000, DL_TIMER_CC_0_INDEX);

    DL_TimerG_setCaptureCompareOutCtl(PWM_Servo_INST, DL_TIMER_CC_OCTL_INIT_VAL_LOW,
		DL_TIMER_CC_OCTL_INV_OUT_DISABLED, DL_TIMER_CC_OCTL_SRC_FUNCVAL,
		DL_TIMERG_CAPTURE_COMPARE_1_INDEX);

    DL_TimerG_setCaptCompUpdateMethod(PWM_Servo_INST, DL_TIMER_CC_UPDATE_METHOD_IMMEDIATE, DL_TIMERG_CAPTURE_COMPARE_1_INDEX);
    DL_TimerG_setCaptureCompareValue(PWM_Servo_INST, 16000, DL_TIMER_CC_1_INDEX);

    DL_TimerG_enableClock(PWM_Servo_INST);


    
    DL_TimerG_setCCPDirection(PWM_Servo_INST , DL_TIMER_CC0_OUTPUT | DL_TIMER_CC1_OUTPUT );


}



/*
 * Timer clock configuration to be sourced by BUSCLK /  (80000000 Hz)
 * timerClkFreq = (timerClkSrc / (timerClkDivRatio * (timerClkPrescale + 1)))
 *   800000 Hz = 80000000 Hz / (1 * (99 + 1))
 */
static const DL_TimerA_ClockConfig gTIMER_0ClockConfig = {
    .clockSel    = DL_TIMER_CLOCK_BUSCLK,
    .divideRatio = DL_TIMER_CLOCK_DIVIDE_1,
    .prescale    = 99U,
};

/*
 * Timer load value (where the counter starts from) is calculated as (timerPeriod * timerClockFreq) - 1
 * TIMER_0_INST_LOAD_VALUE = (1ms * 800000 Hz) - 1
 */
static const DL_TimerA_TimerConfig gTIMER_0TimerConfig = {
    .period     = TIMER_0_INST_LOAD_VALUE,
    .timerMode  = DL_TIMER_TIMER_MODE_PERIODIC,
    .startTimer = DL_TIMER_START,
};

SYSCONFIG_WEAK void SYSCFG_DL_TIMER_0_init(void) {

    DL_TimerA_setClockConfig(TIMER_0_INST,
        (DL_TimerA_ClockConfig *) &gTIMER_0ClockConfig);

    DL_TimerA_initTimerMode(TIMER_0_INST,
        (DL_TimerA_TimerConfig *) &gTIMER_0TimerConfig);
    DL_TimerA_enableClock(TIMER_0_INST);





}


static const DL_I2C_ClockConfig gIICOLEDClockConfig = {
    .clockSel = DL_I2C_CLOCK_BUSCLK,
    .divideRatio = DL_I2C_CLOCK_DIVIDE_1,
};

SYSCONFIG_WEAK void SYSCFG_DL_IICOLED_init(void) {

    DL_I2C_setClockConfig(IICOLED_INST,
        (DL_I2C_ClockConfig *) &gIICOLEDClockConfig);
    DL_I2C_setAnalogGlitchFilterPulseWidth(IICOLED_INST,
        DL_I2C_ANALOG_GLITCH_FILTER_WIDTH_50NS);
    DL_I2C_enableAnalogGlitchFilter(IICOLED_INST);

    /* Configure Controller Mode */
    DL_I2C_resetControllerTransfer(IICOLED_INST);
    /* Set frequency to 400000 Hz*/
    DL_I2C_setTimerPeriod(IICOLED_INST, 9);
    DL_I2C_setControllerTXFIFOThreshold(IICOLED_INST, DL_I2C_TX_FIFO_LEVEL_EMPTY);
    DL_I2C_setControllerRXFIFOThreshold(IICOLED_INST, DL_I2C_RX_FIFO_LEVEL_BYTES_1);
    DL_I2C_enableControllerClockStretching(IICOLED_INST);


    /* Enable module */
    DL_I2C_enableController(IICOLED_INST);


}

static const DL_UART_Main_ClockConfig gUART_0ClockConfig = {
    .clockSel    = DL_UART_MAIN_CLOCK_BUSCLK,
    .divideRatio = DL_UART_MAIN_CLOCK_DIVIDE_RATIO_1
};

static const DL_UART_Main_Config gUART_0Config = {
    .mode        = DL_UART_MAIN_MODE_NORMAL,
    .direction   = DL_UART_MAIN_DIRECTION_TX_RX,
    .flowControl = DL_UART_MAIN_FLOW_CONTROL_NONE,
    .parity      = DL_UART_MAIN_PARITY_NONE,
    .wordLength  = DL_UART_MAIN_WORD_LENGTH_8_BITS,
    .stopBits    = DL_UART_MAIN_STOP_BITS_ONE
};

SYSCONFIG_WEAK void SYSCFG_DL_UART_0_init(void)
{
    DL_UART_Main_setClockConfig(UART_0_INST, (DL_UART_Main_ClockConfig *) &gUART_0ClockConfig);

    DL_UART_Main_init(UART_0_INST, (DL_UART_Main_Config *) &gUART_0Config);
    /*
     * Configure baud rate by setting oversampling and baud rate divisors.
     *  Target baud rate: 115200
     *  Actual baud rate: 115190.78
     */
    DL_UART_Main_setOversampling(UART_0_INST, DL_UART_OVERSAMPLING_RATE_16X);
    DL_UART_Main_setBaudRateDivisor(UART_0_INST, UART_0_IBRD_40_MHZ_115200_BAUD, UART_0_FBRD_40_MHZ_115200_BAUD);



    DL_UART_Main_enable(UART_0_INST);
}

static const DL_SPI_Config gSPI_IMU660RB_config = {
    .mode        = DL_SPI_MODE_CONTROLLER,
    .frameFormat = DL_SPI_FRAME_FORMAT_MOTO3_POL0_PHA0,
    .parity      = DL_SPI_PARITY_NONE,
    .dataSize    = DL_SPI_DATA_SIZE_8,
    .bitOrder    = DL_SPI_BIT_ORDER_MSB_FIRST,
};

static const DL_SPI_ClockConfig gSPI_IMU660RB_clockConfig = {
    .clockSel    = DL_SPI_CLOCK_BUSCLK,
    .divideRatio = DL_SPI_CLOCK_DIVIDE_RATIO_1
};

SYSCONFIG_WEAK void SYSCFG_DL_SPI_IMU660RB_init(void) {
    DL_SPI_setClockConfig(SPI_IMU660RB_INST, (DL_SPI_ClockConfig *) &gSPI_IMU660RB_clockConfig);

    DL_SPI_init(SPI_IMU660RB_INST, (DL_SPI_Config *) &gSPI_IMU660RB_config);

    /* Configure Controller mode */
    /*
     * Set the bit rate clock divider to generate the serial output clock
     *     outputBitRate = (spiInputClock) / ((1 + SCR) * 2)
     *     8000000 = (80000000)/((1 + 4) * 2)
     */
    DL_SPI_setBitRateSerialClockDivider(SPI_IMU660RB_INST, 4);
    /* Set RX and TX FIFO threshold levels */
    DL_SPI_setFIFOThreshold(SPI_IMU660RB_INST, DL_SPI_RX_FIFO_LEVEL_1_2_FULL, DL_SPI_TX_FIFO_LEVEL_1_2_EMPTY);

    /* Enable module */
    DL_SPI_enable(SPI_IMU660RB_INST);
}

