/**
 * @file    PID.h
 * @brief   位置式 PID 控制器接口
 *
 * 移植自 STM32F103 项目 "6-4 PWM驱动舵机"
 */

#ifndef __PID_H
#define __PID_H

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
 * PID 结构体
 *===========================================================================*/
typedef struct {
    float Target;       /* 目标值 (设定点) */
    float Actual;       /* 实际值 (反馈) */
    float Out;          /* PID 输出 */

    float Kp;           /* 比例系数 */
    float Ki;           /* 积分系数 */
    float Kd;           /* 微分系数 */

    float Error0;       /* 当前误差 */
    float Error1;       /* 上一次误差 */
    float ErrorInt;     /* 误差积分 */

    float OutMax;       /* 输出上限 */
    float OutMin;       /* 输出下限 */
} PID_t;

/*===========================================================================
 * 函数声明
 *===========================================================================*/

/**
 * @brief  初始化 PID 结构体
 * @param  p:       PID 结构体指针
 * @param  Kp:      比例系数
 * @param  Ki:      积分系数
 * @param  Kd:      微分系数
 * @param  OutMax:  输出上限
 * @param  OutMin:  输出下限
 */
void PID_Init(PID_t *p, float Kp, float Ki, float Kd, float OutMax, float OutMin);

/**
 * @brief  PID 计算 (需定时调用)
 * @note   调用前需先更新 p->Target 和 p->Actual
 */
void PID_Update(PID_t *p);

/**
 * @brief  PID 复位 (清零所有误差和积分项)
 */
void PID_Reset(PID_t *p);

#ifdef __cplusplus
}
#endif

#endif /* __PID_H */
