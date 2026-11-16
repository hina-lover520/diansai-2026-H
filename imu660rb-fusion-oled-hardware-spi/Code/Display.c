/**
 * @file    Display.c
 * @brief   OLED 显示模块实现 (128x64, 硬件 I2C)
 *
 * 布局:
 *   Line 0 (y=0, 16px):  计时器 "T:00:12.5"
 *   Line 2 (y=2, 8px):   K230误差 "errX:+012 errY:-005"
 *   Line 3 (y=3, 8px):   置信度+传感器 "conf:0.93 --##--##"
 *   Line 4 (y=4, 8px):   状态+速度 "S:FORWARD L800 R800"
 *   Line 5 (y=5, 8px):   串口原始回显
 */

#include "Display.h"
#include "Serial.h"
#include "oled_software_i2c.h"
#include "clock.h"
#include <string.h>
#include <stdio.h>

/*===========================================================================
 * 计时器
 *===========================================================================*/
static uint32_t timer_start_ms   = 0;
static uint32_t timer_elapsed_ms = 0;
static uint8_t  timer_running    = 0;

void Display_TimerStart(void)
{
    if (!timer_running)
    {
        timer_start_ms = tick_ms;
        timer_running = 1;
    }
}

void Display_TimerStop(void)
{
    if (timer_running)
    {
        timer_elapsed_ms += (tick_ms - timer_start_ms);
        timer_running = 0;
    }
}

void Display_TimerReset(void)
{
    timer_elapsed_ms = 0;
    timer_start_ms   = tick_ms;
    timer_running    = 0;
}

uint32_t Display_TimerGetMs(void)
{
    if (timer_running)
        return timer_elapsed_ms + (tick_ms - timer_start_ms);
    return timer_elapsed_ms;
}

uint8_t Display_TimerIsRunning(void)
{
    return timer_running;
}

/*===========================================================================
 * K230 钢珠误差数据
 *===========================================================================*/
static int16_t g_ball_err_x   = 0;
static int16_t g_ball_err_y   = 0;
static float   g_ball_conf    = 0.0f;
static uint8_t g_ball_detected = 0;

void Display_SetBallData(int16_t error_x, int16_t error_y,
                         float conf, uint8_t detected)
{
    g_ball_err_x    = error_x;
    g_ball_err_y    = error_y;
    g_ball_conf     = conf;
    g_ball_detected = detected;
}

/*===========================================================================
 * 推屏 / 循迹 / 电机数据
 *===========================================================================*/
static char     rx_line[SERIAL_LINE_BUF_SIZE];
static char     track_state_str[12];
static uint8_t  track_raw_byte = 0xFF;
static float    motor_left     = 0;
static float    motor_right    = 0;
static float    servo_angle    = 90.0f;
static uint8_t  balance_enabled = 0;

void Display_SetRxData(const char *data)
{
    if (data)
    {
        strncpy(rx_line, data, sizeof(rx_line) - 1);
        rx_line[sizeof(rx_line) - 1] = '\0';
    }
}

void Display_SetTrackerInfo(const char *state_name, uint8_t tracker_raw)
{
    if (state_name)
    {
        strncpy(track_state_str, state_name, sizeof(track_state_str) - 1);
        track_state_str[sizeof(track_state_str) - 1] = '\0';
    }
    track_raw_byte = tracker_raw;
}

void Display_SetMotorSpeed(float left, float right)
{
    motor_left  = left;
    motor_right = right;
}

void Display_SetServoAngle(float angle)
{
    servo_angle = angle;
}

void Display_SetBalanceEnabled(uint8_t enabled)
{
    balance_enabled = enabled;
}

/*===========================================================================
 * Ball data getters
 *===========================================================================*/
int16_t Display_GetBallErrX(void)
{
    return g_ball_err_x;
}

uint8_t Display_IsBallDetected(void)
{
    return g_ball_detected;
}

/*===========================================================================
 * 内部: 格式化时间 mm:ss.d
 *===========================================================================*/
static void FormatTime(uint32_t ms, char *buf, uint8_t size)
{
    uint16_t sec = ms / 1000;
    snprintf(buf, size, "T:%02d:%02d.%01d", sec / 60, sec % 60, (ms % 1000) / 100);
}

/*===========================================================================
 * 内部: 格式化传感器阵列 "T1..T8"  (#=黑线, -=白底)
 *===========================================================================*/
static void FormatTrackerBar(uint8_t raw, char *buf)
{
    for (int i = 0; i < 8; i++)
        buf[i] = (raw & (1 << i)) ? '#' : '-';
    buf[8] = '\0';
}

/*===========================================================================
 * Display_Init
 *===========================================================================*/
void Display_Init(void)
{
    OLED_Init();
    OLED_Clear();

    rx_line[0] = '\0';
    strcpy(track_state_str, "STOP");
    track_raw_byte = 0xFF;
    motor_left  = 0;
    motor_right = 0;
    g_ball_err_x = 0;
    g_ball_err_y = 0;
    g_ball_conf  = 0.0f;
    g_ball_detected = 0;

    OLED_ShowString(0, 2, (uint8_t *)"K230 Tracker", 16);
    OLED_ShowString(0, 5, (uint8_t *)"Ready...", 8);
    delay_ms(800);
    OLED_Clear();
}

/*===========================================================================
 * Display_Update — 刷新 OLED
 *
 * Line 0 (16px): T:00:12.5
 * Line 2 (8px):  errX:+012 errY:-005
 * Line 3 (8px):  conf:0.93 --##--##
 * Line 4 (8px):  S:FORWARD L800 R800
 * Line 5 (8px):  RX:原始数据
 *===========================================================================*/
void Display_Update(void)
{
    char buf[22];   /* 128/6 ≈ 21 chars per 6x8 line */
    char bar[9];

    /* ---- Line 0: 计时器 (16px) ---- */
    FormatTime(Display_TimerGetMs(), buf, sizeof(buf));
    OLED_ShowString(0, 0, (uint8_t *)buf, 16);

    /* ---- Line 2: K230 像素误差 (8px) ---- */
    if (g_ball_detected)
    {
        snprintf(buf, sizeof(buf), "errX:%+04d errY:%+04d",
                 g_ball_err_x, g_ball_err_y);   //依旧结合数组函数
    } 
    else
    {
        snprintf(buf, sizeof(buf), "errX:---- errY:----");
    }
    OLED_ShowString(0, 2, (uint8_t *)buf, 8);

    /* ---- Line 3: 置信度 + 传感器阵列 (8px) ---- */
    FormatTrackerBar(track_raw_byte, bar);
    snprintf(buf, sizeof(buf), "C:%-4.2f %s", g_ball_conf, bar);
    OLED_ShowString(0, 3, (uint8_t *)buf, 8);

    /* ---- Line 4: 状态 + 速度 (8px) ---- */
    snprintf(buf, sizeof(buf), "S:%-7s L%-4.0f R%-4.0f",
             track_state_str, motor_left, motor_right);
    OLED_ShowString(0, 4, (uint8_t *)buf, 8);

    /* ---- Line 5: 舵机角度 + 串口原始数据 (8px, 去换行) ---- */
    {
        size_t len = strlen(rx_line);
        while (len > 0 && (rx_line[len - 1] == '\n' || rx_line[len - 1] == '\r'))
            rx_line[--len] = '\0';
    }
    if (balance_enabled)
    {
        snprintf(buf, sizeof(buf), "SV:%05.1f RX:%.9s", servo_angle, rx_line);
    }
    else
    {
        snprintf(buf, sizeof(buf), "SV:--OFF RX:%.9s", rx_line);
    }
    OLED_ShowString(0, 5, (uint8_t *)buf, 8);
}
