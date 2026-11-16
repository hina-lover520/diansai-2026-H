/**
 * Servo.c — 舵机驱动 (TIMG7 CCP0=PA26, 50Hz)
 *
 * PWM: period=16000, 800kHz → 50Hz, 下降沿计数
 * 0°=500us=400ticks→CC=15600  180°=2500us=2000ticks→CC=14000
 */

#include "ti_msp_dl_config.h"
#include "Servo.h"

void Servo_Init(void)
{
    DL_TimerG_startCounter(TIMG7);
}

void Servo_SetAngle(float angle)
{
    if (angle > 180.0f) angle = 180.0f;
    if (angle < 0.0f)   angle = 0.0f;

    uint16_t high_ticks = (uint16_t)(400.0f + 1600.0f * angle / 180.0f);
    uint16_t cc = 16000 - high_ticks;

    DL_TimerG_setCaptureCompareValue(TIMG7, cc, DL_TIMER_CC_0_INDEX);
}
