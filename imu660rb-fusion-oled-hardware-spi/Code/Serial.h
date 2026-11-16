/**
 * @file    Serial.h
 * @brief   串口通信模块 (UART0: TX=PB0, RX=PB1, 9600bps)
 *
 * 功能:
 *   - 阻塞式发送 (单字节 / 字符串 / 二进制数据)
 *   - 格式化打印 (Serial_Printf)
 *   - 中断接收 + 环形缓冲区
 *   - 接收回调函数 (逐字节回调 / 逐行回调)
 */

#ifndef __SERIAL_H
#define __SERIAL_H

#include "ti_msp_dl_config.h"
#include <stdint.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
 * 环形缓冲区大小
 *===========================================================================*/
#define SERIAL_RX_BUF_SIZE      256     /* 接收环形缓冲区 */
#define SERIAL_LINE_BUF_SIZE    128     /* 行缓冲区 (用于逐行回调) */

/*===========================================================================
 * 回调函数类型
 *===========================================================================*/

/** 逐字节回调: 每收到一个字节就调用 */
typedef void (*Serial_RxByteCallback)(uint8_t byte);

/** 逐行回调: 收到换行符 \n 时调用, line 以 '\0' 结尾 */
typedef void (*Serial_RxLineCallback)(const char *line);

/*===========================================================================
 * 函数声明
 *===========================================================================*/

/**
 * @brief  初始化串口
 * @note   使能 UART0 接收中断, 注册到 NVIC
 *         UART0 已在 SYSCFG_DL_init() 中完成时钟和引脚初始化,
 *         本函数只配置中断相关功能
 */
void Serial_Init(void);

/*===========================================================================
 * 发送接口
 *===========================================================================*/

/** @brief  发送单个字节 (阻塞) */
void Serial_SendByte(uint8_t byte);

/** @brief  发送字符串 (阻塞) */
void Serial_SendString(const char *str);

/** @brief  发送二进制数据 (阻塞) */
void Serial_SendBuffer(const uint8_t *data, uint16_t len);

/** @brief  格式化打印 (类似 printf, 阻塞)
 *  @note   支持 %d %u %x %X %s %c %f %% 等基本格式
 *          浮点打印需要链接时启用 -u _printf_float
 */
void Serial_Printf(const char *fmt, ...);

/*===========================================================================
 * 接收接口
 *===========================================================================*/

/** @brief  检查接收缓冲区中是否有数据 */
uint16_t Serial_Available(void);

/** @brief  从接收缓冲区读取一个字节 (非阻塞)
 *  @retval 0~255: 读到的字节; -1: 缓冲区空
 */
int16_t Serial_Read(void);

/** @brief  读取一行 (遇到 \n 或缓冲区满时返回)
 *  @param  buf: 存放字符串的缓冲区
 *  @param  maxLen: 缓冲区最大长度
 *  @retval 实际读取的字节数 (不含结尾 '\0'); 0 表示无数据
 */
uint16_t Serial_ReadLine(char *buf, uint16_t maxLen);

/** @brief  清空接收缓冲区 */
void Serial_Flush(void);

/*===========================================================================
 * 回调函数注册
 *===========================================================================*/

/** @brief  注册逐字节接收回调
 *  @param  callback: 回调函数指针, 传入 NULL 取消回调
 *  @note   回调在 UART0 中断上下文中执行, 不要做耗时操作
 */
void Serial_SetRxByteCallback(Serial_RxByteCallback callback);

/** @brief  注册逐行接收回调
 *  @param  callback: 回调函数指针, 传入 NULL 取消回调
 *  @note   收到 '\n' 时触发, 回调在中断上下文中执行
 */
void Serial_SetRxLineCallback(Serial_RxLineCallback callback);

/*===========================================================================
 * 中断处理 (由 interrupt.c 的 UART0_IRQHandler 调用)
 *===========================================================================*/

/** @brief  串口接收中断处理 (非阻塞, 填入环形缓冲区)
 *  @note   应在 UART0_IRQHandler 中调用此函数
 */
void Serial_RxIRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif /* __SERIAL_H */
