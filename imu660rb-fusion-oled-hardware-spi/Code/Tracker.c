/**
 * @file    Tracker.c
 * @brief   八路循迹传感器模块实现
 * @note    基于 MSPM0G3507 DriverLib, 算法参考 STM32F103 循迹项目
 *
 * 算法说明:
 *   - 优先级梯形式决策 (if-else ladder)
 *   - 红外传感器在白色跑道上输出 HIGH, 黑线上输出 LOW
 *   - 内部上拉, 传感器模块为开漏输出 (检测到黑线拉低)
 */

#include "Tracker.h"
#include "ti_msp_dl_config.h"

/*===========================================================================
 * 直角弯防重入标志
 *===========================================================================*/
/* (椭圆赛道无直角弯, 不需此标志, 保留以备后用) */

/*===========================================================================
 * Tracker_Init — 初始化八路循迹传感器为输入 + 上拉
 *
 * 注意: SysConfig 默认将这些引脚配为 OUTPUT, 此处强制覆盖为 INPUT
 *===========================================================================*/
void Tracker_Init(void)
{
    /*
     * 每路传感器:
     *   - initDigitalInputFeatures: 输入模式 + 内部上拉 + 无迟滞
     *   - enableHiZ: 高阻输入 (GPIO 不驱动, 由外部传感器决定电平)
     */

    /* T1: PA24 */
    DL_GPIO_initDigitalInputFeatures(T1_IOMUX,
        DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
        DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_enableHiZ(T1_IOMUX);

    /* T2: PA25 */
    DL_GPIO_initDigitalInputFeatures(T2_IOMUX,
        DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
        DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_enableHiZ(T2_IOMUX);

    /* T3: PB24 */
    DL_GPIO_initDigitalInputFeatures(T3_IOMUX,
        DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
        DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_enableHiZ(T3_IOMUX);

    /* T4: PB25 */
    DL_GPIO_initDigitalInputFeatures(T4_IOMUX,
        DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
        DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_enableHiZ(T4_IOMUX);

    /* T5: PA22 */
    DL_GPIO_initDigitalInputFeatures(T5_IOMUX,
        DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
        DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_enableHiZ(T5_IOMUX);

    /* T6: PB20 */
    DL_GPIO_initDigitalInputFeatures(T6_IOMUX,
        DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
        DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_enableHiZ(T6_IOMUX);

    /* T7: PA15 */
    DL_GPIO_initDigitalInputFeatures(T7_IOMUX,
        DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
        DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_enableHiZ(T7_IOMUX);

    /* T8: PA14 */
    DL_GPIO_initDigitalInputFeatures(T8_IOMUX,
        DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
        DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_enableHiZ(T8_IOMUX);
}

/*===========================================================================
 * Tracker_ReadChannel — 读取单路传感器
 *===========================================================================*/
uint8_t Tracker_ReadChannel(uint8_t channel)
{
    uint8_t result = 1; /* 默认白底 */

    switch (channel)
    {
    case 1:
        result = (DL_GPIO_readPins(T1_PORT, T1_PIN) != 0) ? 1 : 0;
        break;
    case 2:
        result = (DL_GPIO_readPins(T2_PORT, T2_PIN) != 0) ? 1 : 0;
        break;
    case 3:
        result = (DL_GPIO_readPins(T3_PORT, T3_PIN) != 0) ? 1 : 0;
        break;
    case 4:
        result = (DL_GPIO_readPins(T4_PORT, T4_PIN) != 0) ? 1 : 0;
        break;
    case 5:
        result = (DL_GPIO_readPins(T5_PORT, T5_PIN) != 0) ? 1 : 0;
        break;
    case 6:
        result = (DL_GPIO_readPins(T6_PORT, T6_PIN) != 0) ? 1 : 0;
        break;
    case 7:
        result = (DL_GPIO_readPins(T7_PORT, T7_PIN) != 0) ? 1 : 0;
        break;
    case 8:
        result = (DL_GPIO_readPins(T8_PORT, T8_PIN) != 0) ? 1 : 0;
        break;
    default:
        break;
    }

    return result;
}

/*===========================================================================
 * Tracker_ReadAll — 一次性读取全部 8 路
 *===========================================================================*/
void Tracker_ReadAll(uint8_t values[8])
{
    values[0] = (DL_GPIO_readPins(T1_PORT, T1_PIN) != 0) ? 1 : 0;
    values[1] = (DL_GPIO_readPins(T2_PORT, T2_PIN) != 0) ? 1 : 0;
    values[2] = (DL_GPIO_readPins(T3_PORT, T3_PIN) != 0) ? 1 : 0;
    values[3] = (DL_GPIO_readPins(T4_PORT, T4_PIN) != 0) ? 1 : 0;
    values[4] = (DL_GPIO_readPins(T5_PORT, T5_PIN) != 0) ? 1 : 0;
    values[5] = (DL_GPIO_readPins(T6_PORT, T6_PIN) != 0) ? 1 : 0;
    values[6] = (DL_GPIO_readPins(T7_PORT, T7_PIN) != 0) ? 1 : 0;
    values[7] = (DL_GPIO_readPins(T8_PORT, T8_PIN) != 0) ? 1 : 0;
}

/*===========================================================================
 * Tracker_GetRawByte — 获取 8-bit 组合值
 *===========================================================================*/
uint8_t Tracker_GetRawByte(void)
{
    uint8_t raw = 0;
    uint8_t vals[8];
    Tracker_ReadAll(vals);

    for (uint8_t i = 0; i < 8; i++)
    {
        if (vals[i] == 0)  /* 黑线 = 置位 */
        {
            raw |= (1 << i);
        }
    }

    return raw;
}

/*===========================================================================
 * Tracker_GetState — 循迹状态决策
 *
 * 赛道: 1.5m 直道 ×2 + 0.5m 半径半圆 ×2, 顺时针行驶
 * 黑线宽度: ~1.8cm (通常只覆盖 1 个传感器)
 *
 * 传感器阵列 (俯视图, 车头朝前):
 *   T1    T2    T3    T4    ||    T5    T6    T7    T8
 *  (PA24)(PA25)(PB24)(PB25) ||   (PA22)(PB20)(PA15)(PA14)
 *
 * 算法: 以"黑线相对于车的位置"决策, 从中心向外检查:
 *   优先级: 全白 → 中心(T4/T5) → 内测(T3/T6) → 外侧(T2/T7) → 边缘(T1/T8)
 *
 * 顺时针赛道特征:
 *   直道: 线在 T4/T5 附近 → FORWARD
 *   右弯: 线偏左(T3→T2→T1) → 向右调, 程度递增
 *   出弯/纠偏: 线偏右(T6→T7→T8) → 向左调
 *===========================================================================*/
Track_State Tracker_GetState(void)
{
    uint8_t t1, t2, t3, t4, t5, t6, t7, t8;

    t1 = (DL_GPIO_readPins(T1_PORT, T1_PIN) != 0) ? 1 : 0;
    t2 = (DL_GPIO_readPins(T2_PORT, T2_PIN) != 0) ? 1 : 0;
    t3 = (DL_GPIO_readPins(T3_PORT, T3_PIN) != 0) ? 1 : 0;
    t4 = (DL_GPIO_readPins(T4_PORT, T4_PIN) != 0) ? 1 : 0;
    t5 = (DL_GPIO_readPins(T5_PORT, T5_PIN) != 0) ? 1 : 0;
    t6 = (DL_GPIO_readPins(T6_PORT, T6_PIN) != 0) ? 1 : 0;
    t7 = (DL_GPIO_readPins(T7_PORT, T7_PIN) != 0) ? 1 : 0;
    t8 = (DL_GPIO_readPins(T8_PORT, T8_PIN) != 0) ? 1 : 0;

    /* ---- 1. 全白 → 脱线 ---- */
    if (t1 && t2 && t3 && t4 && t5 && t6 && t7 && t8)
        return TRACK_ALL_WHITE;

    /* ---- 2. 中心区域 (T4 或 T5) 踩线 → 直行 ---- */
    if (!t4 || !t5)
        return TRACK_FORWARD;

    /* ---- 3. 左侧 T3 踩线 → 线偏左, 微右调 ---- */
    if (!t3)
        return TRACK_SMALL_RIGHT;

    /* ---- 4. 右侧 T6 踩线 → 线偏右, 微左调 ---- */
    if (!t6)
        return TRACK_SMALL_LEFT;

    /* ---- 5. 左外侧 T2 踩线 → 线偏左较多, 中右调 ---- */
    if (!t2)
        return TRACK_MID_RIGHT;

    /* ---- 6. 右外侧 T7 踩线 → 线偏右较多, 中左调 ---- */
    if (!t7)
        return TRACK_MID_LEFT;

    /* ---- 7. 最左 T1 踩线 → 线在最左边, 急右调 ---- */
    if (!t1)
        return TRACK_HARD_RIGHT;

    /* ---- 8. 最右 T8 踩线 → 线在最右边, 急左调 ---- */
    if (!t8)
        return TRACK_HARD_LEFT;

    /* ---- 9. 兜底 (理论上不会到这里) ---- */
    return TRACK_FORWARD;
}

/*===========================================================================
 * Tracker_StateName — 状态名称 (调试/OLED 显示用)
 *===========================================================================*/
const char* Tracker_StateName(Track_State state)
{
    switch (state)
    {
    case TRACK_STOP:         return "STOP";
    case TRACK_FORWARD:      return "FORWARD";
    case TRACK_SMALL_LEFT:   return "S_LEFT";
    case TRACK_SMALL_RIGHT:  return "S_RIGHT";
    case TRACK_MID_LEFT:     return "M_LEFT";
    case TRACK_MID_RIGHT:    return "M_RIGHT";
    case TRACK_HARD_LEFT:    return "H_LEFT";
    case TRACK_HARD_RIGHT:   return "H_RIGHT";
    case TRACK_ALL_WHITE:    return "NO_LINE";
    default:                 return "????";
    }
}
