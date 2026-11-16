/*
 * 车载平衡滚球运动控制系统 — 八路循迹 + 串口 + PID舵机平衡
 * 赛道: 1.5m 直道 ×2 + 0.5m 半圆 ×2 (顺时针)
 * PWM 1000Hz (period=40000)
 */

#include "ti_msp_dl_config.h"
#include "main.h"
#include "clock.h"
#include "Motor.h"
#include "Serial.h"
#include "Servo.h"
#include "PID.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/*===========================================================================
 * 传感器引脚
 *===========================================================================*/
#define T1_PORT  GPIOA
#define T1_PIN   DL_GPIO_PIN_24
#define T2_PORT  GPIOA
#define T2_PIN   DL_GPIO_PIN_25
#define T3_PORT  GPIOB
#define T3_PIN   DL_GPIO_PIN_24
#define T4_PORT  GPIOB
#define T4_PIN   DL_GPIO_PIN_25
#define T5_PORT  GPIOA
#define T5_PIN   DL_GPIO_PIN_22
#define T6_PORT  GPIOB
#define T6_PIN   DL_GPIO_PIN_20
#define T7_PORT  GPIOA
#define T7_PIN   DL_GPIO_PIN_15
#define T8_PORT  GPIOA
#define T8_PIN   DL_GPIO_PIN_14

/*===========================================================================
 * 速度 (period=40000, 正转 CC=duty)
 *===========================================================================*/
#define FWD_FAST   18000
#define FWD_MID    23000
#define FWD_SLOW   30000
#define REV_SLOW   10000

// #define FWD_FAST   15000
// #define FWD_MID    20000
// #define FWD_SLOW   28000
// #define REV_SLOW   10000

/*===========================================================================
 * PID 平衡参数
 *===========================================================================*/
#define SERVO_CENTER    94.0f
#define DEAD_ZONE       70.0f       /* ±70px 以内视为中点, 冻结PID */
#define SERVO_MIN       0.0f
#define SERVO_MAX       180.0f
#define PID_KP          0.008f
#define PID_KI          0.0f
#define PID_KD          0.10f

// #define PID_KP          0.003f
// #define PID_KI          0.0001f
// #define PID_KD          0.08f
#define PID_OUT_MAX     90.0f
#define LOSS_TIMEOUT_MS 500

/*===========================================================================
 * 传感器引脚初始化
 *===========================================================================*/
static void tracker_pin_init(void)
{
    DL_GPIO_initDigitalInputFeatures(IOMUX_PINCM54,
        DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
        DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_enableHiZ(IOMUX_PINCM54);
    DL_GPIO_initDigitalInputFeatures(IOMUX_PINCM55,
        DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
        DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_enableHiZ(IOMUX_PINCM55);
    DL_GPIO_initDigitalInputFeatures(IOMUX_PINCM52,
        DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
        DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_enableHiZ(IOMUX_PINCM52);
    DL_GPIO_initDigitalInputFeatures(IOMUX_PINCM56,
        DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
        DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_enableHiZ(IOMUX_PINCM56);
    DL_GPIO_initDigitalInputFeatures(IOMUX_PINCM47,
        DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
        DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_enableHiZ(IOMUX_PINCM47);
    DL_GPIO_initDigitalInputFeatures(IOMUX_PINCM48,
        DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
        DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_enableHiZ(IOMUX_PINCM48);
    DL_GPIO_initDigitalInputFeatures(IOMUX_PINCM37,
        DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
        DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_enableHiZ(IOMUX_PINCM37);
    DL_GPIO_initDigitalInputFeatures(IOMUX_PINCM36,
        DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
        DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_enableHiZ(IOMUX_PINCM36);
}

/*===========================================================================
 * main
 *===========================================================================*/
int main(void)
{
    SYSCFG_DL_init();
    SysTick_Init();

    DL_GPIO_initDigitalOutput(IOMUX_PINCM15);
    DL_GPIO_initDigitalOutput(IOMUX_PINCM16);
    DL_GPIO_enableOutput(GPIOB, DL_GPIO_PIN_2 | DL_GPIO_PIN_3);

    OLED_Init();
    OLED_Clear();
    tracker_pin_init();
    Motor_Init();
    Serial_Init();
    Servo_Init();

    /* 计时 & 停车 */
    uint8_t  racing     = 0;
    uint32_t start_ms   = 0;
    uint32_t race_ms    = 0;
    uint32_t stop_half  = 30;
    uint32_t disp_tick  = 0;

    /* 串口 */
    char    k230_line[32] = "";
    int16_t k230_ex = 0, k230_ey = 0;
    float   k230_cf = 0.0f;
    uint8_t k230_ok = 0;

    /* 舵机 + PID */
    float    servo_angle = SERVO_CENTER;
    uint8_t  balance_on  = 0;
    PID_t    pid;
    uint32_t loss_tick   = 0;
    uint32_t pid_tick    = 0;
    int16_t  last_ex     = 0;
    uint8_t  loss_cnt    = 0;       /* 连续丢球帧数 */
    int16_t  pid_target  = 0;       /* PID目标像素 (-167=左5cm, 0=中心, 282=右5cm) */

    PID_Init(&pid, PID_KP, PID_KI, PID_KD, PID_OUT_MAX, -PID_OUT_MAX);
    Servo_SetAngle(SERVO_CENTER);

    while (1)
    {
        /* ---- 按键 ---- */
        uint8_t key = Key_Scan();
        if (key == KEY1_PRESS)
            { racing = 1; start_ms = tick_ms; }
        else if (key == KEY2_PRESS)
        {
            if (balance_on)
                pid_target = -167;       /* 目标 -5cm */
            else if (!racing)
                { stop_half += 1; if (stop_half > 240) stop_half = 240; }
        }
        else if (key == KEY3_PRESS)
        {
            if (balance_on)
                pid_target = 282;        /* 目标 +5cm */
            else if (!racing)
                { if (stop_half > 1) stop_half -= 1; }
        }
        else if (key == KEY4_PRESS)
        {
            balance_on = !balance_on;
            if (balance_on)
                { PID_Reset(&pid); pid_target = 0; servo_angle = SERVO_CENTER; }
            Servo_SetAngle(servo_angle);
        }

        /* ---- 读传感器 ---- */
        uint8_t t1 = (DL_GPIO_readPins(T1_PORT, T1_PIN) != 0);
        uint8_t t2 = (DL_GPIO_readPins(T2_PORT, T2_PIN) != 0);
        uint8_t t3 = (DL_GPIO_readPins(T3_PORT, T3_PIN) != 0);
        uint8_t t4 = (DL_GPIO_readPins(T4_PORT, T4_PIN) != 0);
        uint8_t t5 = (DL_GPIO_readPins(T5_PORT, T5_PIN) != 0);
        uint8_t t6 = (DL_GPIO_readPins(T6_PORT, T6_PIN) != 0);
        uint8_t t7 = (DL_GPIO_readPins(T7_PORT, T7_PIN) != 0);
        uint8_t t8 = (DL_GPIO_readPins(T8_PORT, T8_PIN) != 0);

        /* ---- 串口: 排空, 取最新一帧 ---- */
        {
            char line[32];
            while (Serial_ReadLine(line, sizeof(line)) > 0)
            {
                size_t len = strlen(line);
                while (len && (line[len-1]=='\r'||line[len-1]=='\n'))
                    line[--len] = 0;
                strncpy(k230_line, line, sizeof(k230_line)-1);

                char *p  = line;
                char *c1 = strchr(p, ',');
                char *c2 = c1 ? strchr(c1+1, ',') : NULL;

                if (c1 && c2)
                {
                    *c1 = 0; *c2 = 0;
                    k230_ex = (int16_t)atoi(p);
                    k230_ey = (int16_t)atoi(c1+1);
                    k230_cf = atof(c2+1);
                    k230_ok = (k230_ex != 0 || k230_ey != 0 || k230_cf > 0.01f);
                }
                else { k230_ok = 0; }
            }
        }

        /* ---- PID 平衡控制 (100Hz) ---- */
        if (balance_on)
        {
            pid_tick++;
            if (pid_tick >= 2)
            {
                pid_tick = 0;

                if (k230_ok)
                {
                    loss_tick = 0;
                    loss_cnt  = 0;
                    last_ex   = k230_ex;

                    int16_t err = k230_ex - pid_target;   /* 相对目标的误差 */
                    if (err > -DEAD_ZONE && err < DEAD_ZONE)
                    {
                        PID_Reset(&pid);
                    }
                    else
                    {
                        pid.Target = (float)pid_target;
                        pid.Actual = (float)k230_ex;
                        PID_Update(&pid);
                        servo_angle = SERVO_CENTER + pid.Out;
                    }
                }
                else
                {
                    /* 连续3帧全零才认为真丢球, 过滤K230闪断 */
                    loss_cnt++;
                    if (loss_cnt >= 3)
                    {
                        if (loss_tick == 0)
                            loss_tick = tick_ms;

                        if ((tick_ms - loss_tick) > LOSS_TIMEOUT_MS)
                        {
                            PID_Reset(&pid);
                            servo_angle += (110.0f - servo_angle) * 0.08f;
                            if (fabsf(servo_angle - 110.0f) < 0.3f)
                                servo_angle = 110.0f;
                        }
                        else
                        {
                            float push = (last_ex < 0) ? -5.0f :
                                         (last_ex > 0) ?  5.0f : 0.0f;
                            servo_angle += push;
                        }
                    }
                }

                if (servo_angle > SERVO_MAX) servo_angle = SERVO_MAX;
                if (servo_angle < SERVO_MIN) servo_angle = SERVO_MIN;
                Servo_SetAngle(servo_angle);
            }
        }

        /* ---- 3灯及以上 → 停车 ---- */
        uint8_t black_cnt = (!t1) + (!t2) + (!t3) + (!t4)
                          + (!t5) + (!t6) + (!t7) + (!t8);

        /* ---- 循迹决策 ---- */
        int L, R;
        if (black_cnt >= 3)
            { L = 0; R = 0; racing = 0; }
        else if (t1 && t2 && t3 && t4 && t5 && t6 && t7 && t8)
            { L =  0; R = FWD_MID; }
        else if (!t4 || !t5)
            { L = FWD_MID;  R =  FWD_MID;  }
        else if (!t3)
            { L = FWD_MID;  R =  FWD_SLOW; }
        else if (!t6)
            { L = FWD_SLOW; R =  FWD_MID;  }
        else if (!t2)
            { L = FWD_FAST; R = 0;        }
        else if (!t7)
            { L = 0;        R =  FWD_FAST; }
        else if (!t1)
            { L = FWD_FAST; R = 0;        }
        else if (!t8)
            { L = 0;        R =  FWD_FAST; }
        else
            { L = 0; R = 0; }

        /* ---- 超时强停 ---- */
        if (racing)
        {
            race_ms = tick_ms - start_ms;
            if (race_ms / 500 >= stop_half)
                { racing = 0; race_ms = stop_half * 500; }
        }
        if (!racing) { L = 0; R = 0; }

        Motor_SetDifferential(R, L);
        delay_cycles(160000);

        /* ---- OLED (10Hz) ---- */
        disp_tick++;
        if (disp_tick >= 20)
        {
            disp_tick = 0;
            char buf[22];

            snprintf(buf, sizeof(buf), "ex:%d cf:%.1f %s",
                k230_ex, k230_cf, k230_ok ? "" : "(no)");
            OLED_ShowString(0, 0, (uint8_t *)buf, 8);

            snprintf(buf, sizeof(buf), "RX:%.21s",
                k230_line[0] ? k230_line : "----");
            OLED_ShowString(0, 1, (uint8_t *)buf, 8);

            {
                uint32_t t = racing ? (tick_ms - start_ms) : race_ms;
                uint16_t s  = t / 1000;
                uint8_t  ds = (t % 1000) / 100;
                snprintf(buf, sizeof(buf), "%cT:%02d:%02d.%01d %s",
                    racing ? ' ' : 'S',
                    s/60, s%60, ds,
                    balance_on ? (pid_target==0?"C":pid_target<0?"L":"R") : "");
            }
            OLED_ShowString(0, 2, (uint8_t *)buf, 8);

            snprintf(buf, sizeof(buf), "%c%c%c%c%c%c%c%c SV:%3.0f",
                t1?'-':'#', t2?'-':'#', t3?'-':'#', t4?'-':'#',
                t5?'-':'#', t6?'-':'#', t7?'-':'#', t8?'-':'#', servo_angle);
            OLED_ShowString(0, 4, (uint8_t *)buf, 8);
        }

        delay_ms(5);
    }
}
