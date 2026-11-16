/**
 * @file    Motor.h
 * @brief   TB6612 / AT8236 双路电机驱动 (PWM + 方向引脚)
 *
 * 硬件连接:
 *   左轮 (Motor1): PWM=TIMg0 CCP0 (PA12), DIR=PB8
 *   右轮 (Motor2): PWM=TIMg0 CCP1 (PA13), DIR=PB9
 *
 * 控制协议:
 *   speed > 0 → 正转: PWM输出, DIR=LOW
 *   speed < 0 → 反转: PWM输出, DIR=HIGH
 *   speed = 0 → 惰行: PWM=0,   DIR=LOW
 */

#ifndef __MOTOR_H
#define __MOTOR_H

#include "ti_msp_dl_config.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MOTOR_LEFT   1    /* 左轮 */
#define MOTOR_RIGHT  2    /* 右轮 */

void Motor_Init(void);
void Motor_SetSpeed(uint8_t motor, int16_t duty);
void Motor_SetLeftSpeed(int16_t duty);
void Motor_SetRightSpeed(int16_t duty);
void Motor_SetDifferential(int16_t left_duty, int16_t right_duty);
void Motor_StopAll(void);

#ifdef __cplusplus
}
#endif

#endif /* __MOTOR_H */
