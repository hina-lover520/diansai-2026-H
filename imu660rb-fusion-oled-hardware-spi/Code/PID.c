/**
 * @file    PID.c
 * @brief   位置式 PID 控制器实现
 *
 * 移植自 STM32F103 项目 "6-4 PWM驱动舵机" 的 PID.c
 * 特性:
 *   - 标准位置式 PID
 *   - Ki==0 时自动清零积分项 (支持 PD/P 模式无扰切换)
 *   - 输出饱和时冻结积分 (积分抗饱和)
 *   - 输出限幅
 */

#include "PID.h"

/*===========================================================================
 * PID_Init — 初始化 PID 结构体
 *
 * @param  p:       PID 结构体指针
 * @param  Kp:      比例系数
 * @param  Ki:      积分系数
 * @param  Kd:      微分系数
 * @param  OutMax:  输出上限
 * @param  OutMin:  输出下限
 *===========================================================================*/
void PID_Init(PID_t *p, float Kp, float Ki, float Kd, float OutMax, float OutMin)
{
    p->Target   = 0.0f;
    p->Actual   = 0.0f;
    p->Out      = 0.0f;

    p->Kp       = Kp;
    p->Ki       = Ki;
    p->Kd       = Kd;

    p->Error0   = 0.0f;
    p->Error1   = 0.0f;
    p->ErrorInt = 0.0f;

    p->OutMax   = OutMax;
    p->OutMin   = OutMin;
}

/*===========================================================================
 * PID_Update — PID 计算 (需定时调用, 周期需与参数匹配)
 *
 * 调用前需先更新 p->Target 和 p->Actual.
 *
 * 公式: Out = Kp*e(k) + Ki*Σe(k) + Kd*(e(k)-e(k-1))
 *===========================================================================*/
void PID_Update(PID_t *p)
{
    p->Error1 = p->Error0;
    p->Error0 = p->Target - p->Actual;

    /* 积分项: Ki==0 时清零, 防止积分累加后切回PD时跳变 */
    if (p->Ki != 0.0f)
    {
        p->ErrorInt += p->Error0;

        /* 积分分离: 输出饱和时不累加, 防止积分饱和 (anti-windup) */
        if (p->Out >= p->OutMax || p->Out <= p->OutMin)
        {
            p->ErrorInt -= p->Error0;   /* 回退本次累加 */
        }
    }
    else
    {
        p->ErrorInt = 0.0f;
    }

    /* PID 计算 */
    p->Out = p->Kp * p->Error0
           + p->Ki * p->ErrorInt
           + p->Kd * (p->Error0 - p->Error1);

    /* 输出限幅 */
    if (p->Out > p->OutMax) { p->Out = p->OutMax; }
    if (p->Out < p->OutMin) { p->Out = p->OutMin; }
}

/*===========================================================================
 * PID_Reset — PID 复位 (清零所有误差和积分项, 用于丢球恢复)
 *===========================================================================*/
void PID_Reset(PID_t *p)
{
    p->Error0   = 0.0f;
    p->Error1   = 0.0f;
    p->ErrorInt = 0.0f;
    p->Out      = 0.0f;
}
