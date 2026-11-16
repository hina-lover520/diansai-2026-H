// #include "Encoder.h"
// #include "ti_msp_dl_config.h"

// volatile int32_t L_E1A;
// volatile int32_t R_E2A;
// volatile int32_t Count;

// int Encoder_get(int number)
// {
//     if (number == 1)
//     {
//         Count = L_E1A;
//     }
//     else if (number == 2)
//     {
//         Count = R_E2A;
//     }
//     return Count;
// }

// /*
//  * 编码器方向判断原理（正交编码器 A/B 相信号）：
//  *   正转 A 超前 B 90°：A 上升沿时 B = 0  →  计数+1
//  *   反转 B 超前 A 90°：A 上升沿时 B = 1  →  计数-1
//  * B 相仅作电平读取，不作中断源。
//  * 当前方案使用 A 相上升沿单边沿触发 = 1x 倍频分辨率。
//  */
// void Encoder_ProcessInterrupt(uint32_t iidx)
// {
//     /* 左轮编码器: L_ENA (PB13) 上升沿 → 读 L_ENB (PB12) 电平判断方向 */
//     if (iidx == Motor_L_L_ENA_IIDX)
//     {
//         if (DL_GPIO_readPins(Motor_L_L_ENB_PORT, Motor_L_L_ENB_PIN))
//         {
//             L_E1A--;  /* B=1 → 反转 */
//         }
//         else
//         {
//             L_E1A++;  /* B=0 → 正转 */
//         }
//         DL_GPIO_clearInterruptStatus(Motor_L_L_ENA_PORT, Motor_L_L_ENA_PIN);
//     }
//     /* 右轮编码器: R_ENA (PA14) 上升沿 → 读 R_ENB (PA15) 电平判断方向 */
//     else if (iidx == Motor_R_R_ENA_IIDX)
//     {
//         if (DL_GPIO_readPins(Motor_R_PORT, Motor_R_R_ENB_PIN))
//         {
//             R_E2A--;  /* B=1 → 反转 */
//         }
//         else
//         {
//             R_E2A++;  /* B=0 → 正转 */
//         }
//         DL_GPIO_clearInterruptStatus(Motor_R_PORT, Motor_R_R_ENA_PIN);
//     }
// }
