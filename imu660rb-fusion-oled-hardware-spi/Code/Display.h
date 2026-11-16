/**
 * @file    Display.h
 * @brief   OLED 显示模块 (0.96" SSD1306 128x64)
 *
 * 功能:
 *   - 发车后计时 (mm:ss.d)
 *   - K230 摄像头数据 (error_x, error_y, conf)
 *   - 八路循迹传感器 + 状态
 *   - 电机速度
 *
 * 屏幕布局 (128x64):
 *   Line 0 (y=0, 16px):  "T:00:12.5"            计时器
 *   Line 2 (y=2, 8px):   "errX:+012 errY:-005"  K230像素误差
 *   Line 3 (y=3, 8px):   "conf:0.93 --##--##"   置信度+传感器
 *   Line 4 (y=4, 8px):   "S:FORWARD L800 R800"  状态+速度
 *   Line 5 (y=5, 8px):   "RX: hello world"      串口原始数据
 */

#ifndef __DISPLAY_H
#define __DISPLAY_H

#include "ti_msp_dl_config.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
 * 计时器接口
 *===========================================================================*/

void Display_TimerStart(void);
void Display_TimerStop(void);
void Display_TimerReset(void);
uint32_t Display_TimerGetMs(void);
uint8_t  Display_TimerIsRunning(void);

/*===========================================================================
 * K230 摄像头数据接口
 *===========================================================================*/

/** @brief 设置 K230 发送的钢珠误差数据
 *  @param  error_x: X轴像素误差 (>0偏右, <0偏左)
 *  @param  error_y: Y轴像素误差 (>0偏下, <0偏上)
 *  @param  conf:    检测置信度 (0.00~1.00)
 *  @param  detected: 1=检测到钢珠, 0=未检测到
 */
void Display_SetBallData(int16_t error_x, int16_t error_y,
                         float conf, uint8_t detected);

/*===========================================================================
 * 其他显示数据
 *===========================================================================*/

void Display_Init(void);
void Display_SetRxData(const char *data);
void Display_SetTrackerInfo(const char *state_name, uint8_t tracker_raw);
void Display_SetMotorSpeed(float left, float right);
void Display_SetServoAngle(float angle);
void Display_SetBalanceEnabled(uint8_t enabled);
void Display_Update(void);

/*===========================================================================
 * Ball data getters — 供 PID 控制循环读取
 *===========================================================================*/
int16_t Display_GetBallErrX(void);
uint8_t  Display_IsBallDetected(void);

#ifdef __cplusplus
}
#endif

#endif /* __DISPLAY_H */
