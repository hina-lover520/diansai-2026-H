/**
 * @file    Servo.h
 * @brief   舵机驱动 (TIMG7 CCP0=PA26, 50Hz)
 */

#ifndef __SERVO_H
#define __SERVO_H

#ifdef __cplusplus
extern "C" {
#endif

void Servo_Init(void);
void Servo_SetAngle(float angle);   /* 0° ~ 180° */

#ifdef __cplusplus
}
#endif

#endif
