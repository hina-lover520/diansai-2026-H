/**
 * Motor.c — AT8236 (按用户指定逻辑)
 *
 * 左轮: IN1=PWM(PA12), IN2=DIR(PB8)
 * 右轮: IN1=PWM(PA13), IN2=DIR(PB9)
 *
 * motor_1: duty>0 正转 CC=duty IN2=0, duty<0 反转 CC=period+duty IN2=1, duty=0 刹车 CC=period IN2=1
 * motor_2: 右轮极性反, duty = -speed, 其余同上
 */

#include "Motor.h"

#define PWM_PERIOD  4000

void Motor_Init(void)
{
    DL_GPIO_clearPins(GPIOB, DL_GPIO_PIN_8 | DL_GPIO_PIN_9);
    DL_TimerG_startCounter(TIMG0);
}

/* ---- motor_1 (左轮) ---- */
static void motor_1_setduty(int speed)
{
    int duty = -speed;

    if (duty > 0)
    {
        DL_TimerG_setCaptureCompareValue(TIMG0, duty, DL_TIMER_CC_0_INDEX);
        DL_GPIO_clearPins(GPIOB, DL_GPIO_PIN_8);
    }
    else if (duty < 0)
    {
        DL_TimerG_setCaptureCompareValue(TIMG0, 40000 + duty, DL_TIMER_CC_0_INDEX);
        DL_GPIO_setPins(GPIOB, DL_GPIO_PIN_8);
    }
    else
    {
        DL_TimerG_setCaptureCompareValue(TIMG0, 40000, DL_TIMER_CC_0_INDEX);
        DL_GPIO_setPins(GPIOB, DL_GPIO_PIN_8);
    }
}

/* ---- motor_2 (右轮, 极性反) ---- */
static void motor_2_setduty(int speed)
{
    int duty = speed;

    if (duty > 0)
    {
        DL_TimerG_setCaptureCompareValue(TIMG0, duty, DL_TIMER_CC_1_INDEX);
        DL_GPIO_clearPins(GPIOB, DL_GPIO_PIN_9);
    }
    else if (duty < 0)
    {
        DL_TimerG_setCaptureCompareValue(TIMG0, 40000 + duty, DL_TIMER_CC_1_INDEX);
        DL_GPIO_setPins(GPIOB, DL_GPIO_PIN_9);
    }
    else
    {
        DL_TimerG_setCaptureCompareValue(TIMG0, 40000, DL_TIMER_CC_1_INDEX);
        DL_GPIO_setPins(GPIOB, DL_GPIO_PIN_9);
    }
}

void Motor_SetSpeed(uint8_t motor, int16_t speed)
{
    if (motor == MOTOR_LEFT)
        motor_1_setduty(speed);
    else
        motor_2_setduty(speed);
}

void Motor_SetDifferential(int16_t L, int16_t R)
{
    motor_1_setduty(L);
    motor_2_setduty(R);
}

void Motor_StopAll(void)
{
    motor_1_setduty(0);
    motor_2_setduty(0);
}
