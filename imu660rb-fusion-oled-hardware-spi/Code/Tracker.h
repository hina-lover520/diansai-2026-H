/**
 * @file    Tracker.h
 * @brief   八路循迹传感器模块 (8-Channel Line Tracking Sensor Module)
 * @note    适配 MSPM0G3507, 参考 STM32F103 循迹算法
 *
 * 传感器排布 (从左到右, 小车前进方向):
 *   T1 (最左)  -> PA24 (PIN_0)
 *   T2         -> PA25 (PIN_1)
 *   T3         -> PB24 (PIN_2)
 *   T4         -> PB25 (PIN_3)
 *   T5         -> PA22 (PIN_4)
 *   T6         -> PB20 (PIN_5)
 *   T7         -> PA15 (PIN_6)
 *   T8 (最右)  -> PA14 (PIN_7)
 *
 * 读取逻辑:
 *   - 传感器为红外反射式 (TCRT5000 等数字输出模块)
 *   - 检测到黑线: 输出 LOW  (0) — GPIO 读到 0
 *   - 检测到白底: 输出 HIGH (1) — GPIO 读到 1 (内部上拉)
 */

#ifndef __TRACKER_H
#define __TRACKER_H

#include "ti_msp_dl_config.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
 * 传感器引脚宏定义 (复用 SysConfig 的 GPIO_Tracker 定义)
 *===========================================================================*/

/* T1 - 最左侧传感器 */
#define T1_PORT         GPIO_Tracker_PIN_0_PORT   /* GPIOA */
#define T1_PIN          GPIO_Tracker_PIN_0_PIN    /* PA24  */
#define T1_IOMUX        GPIO_Tracker_PIN_0_IOMUX  /* IOMUX_PINCM54 */

/* T2 */
#define T2_PORT         GPIO_Tracker_PIN_1_PORT   /* GPIOA */
#define T2_PIN          GPIO_Tracker_PIN_1_PIN    /* PA25  */
#define T2_IOMUX        GPIO_Tracker_PIN_1_IOMUX  /* IOMUX_PINCM55 */

/* T3 */
#define T3_PORT         GPIO_Tracker_PIN_2_PORT   /* GPIOB */
#define T3_PIN          GPIO_Tracker_PIN_2_PIN    /* PB24  */
#define T3_IOMUX        GPIO_Tracker_PIN_2_IOMUX  /* IOMUX_PINCM52 */

/* T4 */
#define T4_PORT         GPIO_Tracker_PIN_3_PORT   /* GPIOB */
#define T4_PIN          GPIO_Tracker_PIN_3_PIN    /* PB25  */
#define T4_IOMUX        GPIO_Tracker_PIN_3_IOMUX  /* IOMUX_PINCM56 */

/* T5 */
#define T5_PORT         GPIO_Tracker_PIN_4_PORT   /* GPIOA */
#define T5_PIN          GPIO_Tracker_PIN_4_PIN    /* PA22  */
#define T5_IOMUX        GPIO_Tracker_PIN_4_IOMUX  /* IOMUX_PINCM47 */

/* T6 */
#define T6_PORT         GPIO_Tracker_PIN_5_PORT   /* GPIOB */
#define T6_PIN          GPIO_Tracker_PIN_5_PIN    /* PB20  */
#define T6_IOMUX        GPIO_Tracker_PIN_5_IOMUX  /* IOMUX_PINCM48 */

/* T7 */
#define T7_PORT         GPIO_Tracker_PIN_6_PORT   /* GPIOA */
#define T7_PIN          GPIO_Tracker_PIN_6_PIN    /* PA15  */
#define T7_IOMUX        GPIO_Tracker_PIN_6_IOMUX  /* IOMUX_PINCM37 */

/* T8 - 最右侧传感器 */
#define T8_PORT         GPIO_Tracker_PIN_7_PORT   /* GPIOA */
#define T8_PIN          GPIO_Tracker_PIN_7_PIN    /* PA14  */
#define T8_IOMUX        GPIO_Tracker_PIN_7_IOMUX  /* IOMUX_PINCM36 */

/*===========================================================================
 * 循迹状态枚举
 *
 * 赛道: 1.5m 直道 ×2 + 0.5m 半径半圆 ×2 (椭圆形, 顺时针行驶)
 * 全程右弯, 无直角弯, 无十字交叉
 *===========================================================================*/
typedef enum {
    TRACK_STOP = 0,         /* 停车 / 脱线 */
    TRACK_FORWARD,          /* 直行 (居中) */
    TRACK_SMALL_LEFT,       /* 微左调 */
    TRACK_SMALL_RIGHT,      /* 微右调 */
    TRACK_MID_LEFT,         /* 中左转 */
    TRACK_MID_RIGHT,        /* 中右转 (半圆弯道) */
    TRACK_HARD_LEFT,        /* 急左转 (脱线拉回) */
    TRACK_HARD_RIGHT,       /* 急右转 (脱线拉回) */
    TRACK_ALL_WHITE         /* 全白 — 脱线 */
} Track_State;

/*===========================================================================
 * 函数声明
 *===========================================================================*/

/**
 * @brief  初始化八路循迹传感器
 * @note   将 SysConfig 初始化为 OUTPUT 的引脚重新配置为 INPUT + 上拉
 *         调用此函数后即可读取传感器值
 */
void Tracker_Init(void);

/**
 * @brief  读取单个传感器值
 * @param  channel: 传感器通道号 (1~8, 对应 T1~T8)
 * @retval 0 — 检测到黑线; 1 — 检测到白底
 */
uint8_t Tracker_ReadChannel(uint8_t channel);

/**
 * @brief  一次性读取全部 8 路传感器
 * @param  values: 输出数组, values[0..7] 对应 T1..T8
 *         values[i] = 0 表示黑线, 1 表示白底
 */
void Tracker_ReadAll(uint8_t values[8]);

/**
 * @brief  循迹状态决策 (核心算法)
 * @retval Track_State 枚举值
 *
 * 算法: 从中心向外逐级检测, 任一传感器见黑即返回对应状态
 *   (黑线宽 ~1.8cm, 通常只覆盖 1 个传感器, 不需要"同时亮两个")
 *
 * 决策优先级:
 *   1. 全白    → ALL_WHITE     (脱线)
 *   2. T4或T5 → FORWARD       (中心, 直行)
 *   3. T3     → SMALL_RIGHT   (线偏左, 微右调)
 *   4. T6     → SMALL_LEFT    (线偏右, 微左调)
 *   5. T2     → MID_RIGHT     (线偏左较多)
 *   6. T7     → MID_LEFT      (线偏右较多)
 *   7. T1     → HARD_RIGHT    (线在最左)
 *   8. T8     → HARD_LEFT     (线在最右)
 */
Track_State Tracker_GetState(void);

/**
 * @brief  获取当前状态名称字符串 (调试用)
 */
const char* Tracker_StateName(Track_State state);

/**
 * @brief  获取当前 8 路传感器组合值 (调试/显示用)
 * @retval 8-bit 组合值, bit0=T1 ... bit7=T8, 0=黑线 1=白底
 */
uint8_t Tracker_GetRawByte(void);

#ifdef __cplusplus
}
#endif

#endif /* __TRACKER_H */
