/**
 * @file    Serial.c
 * @brief   串口通信模块实现 (UART0 中断接收 + 环形缓冲区 + 回调)
 */

#include "Serial.h"
#include "ti_msp_dl_config.h"
#include <string.h>
#include <stdio.h>

/*===========================================================================
 * 环形接收缓冲区
 *===========================================================================*/
static uint8_t  rx_buf[SERIAL_RX_BUF_SIZE];
static volatile uint16_t rx_head = 0;   /* 写入位置 (ISR 写入) */
static volatile uint16_t rx_tail = 0;   /* 读取位置 (主循环读取) */

/*===========================================================================
 * 行缓冲区 (逐行回调用)
 *===========================================================================*/
static char     line_buf[SERIAL_LINE_BUF_SIZE];
static uint16_t line_idx = 0;

/*===========================================================================
 * 回调函数指针
 *===========================================================================*/
static Serial_RxByteCallback rx_byte_cb = NULL;
static Serial_RxLineCallback rx_line_cb = NULL;

/*===========================================================================
 * 内部辅助
 *===========================================================================*/

/* 环形缓冲区是否为空 */
static inline int rx_buf_empty(void)
{
    return (rx_head == rx_tail);
}

/* 环形缓冲区已用字节数 */
static inline uint16_t rx_buf_used(void)
{
    return (rx_head - rx_tail) & (SERIAL_RX_BUF_SIZE - 1);
}

/* 环形缓冲区剩余空间 */
static inline uint16_t rx_buf_free(void)
{
    return (SERIAL_RX_BUF_SIZE - 1) - rx_buf_used();
}

/*===========================================================================
 * Serial_Init — 使能 UART0 接收中断
 *===========================================================================*/
void Serial_Init(void)
{
    /* 清空缓冲区 */
    rx_head  = 0;
    rx_tail  = 0;
    line_idx = 0;

    /* 清空回调 */
    rx_byte_cb = NULL;
    rx_line_cb = NULL;

    /*
     * 重新配置波特率为 115200 (SysConfig 默认 9600)
     * 40MHz / (16 × 115200) = 21.7013...
     * IBRD=21, FBRD=round(0.7013×64)=45
     */
    DL_UART_Main_setBaudRateDivisor(UART_0_INST, 21, 45);

    /* 使能 UART0 接收中断 (TX 中断不需要, 发送用阻塞方式) */
    DL_UART_enableInterrupt(UART_0_INST, DL_UART_INTERRUPT_RX);
    NVIC_EnableIRQ(UART_0_INST_INT_IRQN);
}

/*===========================================================================
 * 发送接口 — 阻塞式
 *===========================================================================*/

void Serial_SendByte(uint8_t byte)
{
    /* 等待 TX FIFO 有空位 */
    while (DL_UART_isTXFIFOFull(UART_0_INST)) {}
    DL_UART_transmitData(UART_0_INST, byte);
}

void Serial_SendString(const char *str)
{
    while (*str)
    {
        Serial_SendByte((uint8_t)*str++);
    }
}

void Serial_SendBuffer(const uint8_t *data, uint16_t len)
{
    while (len--)
    {
        Serial_SendByte(*data++);
    }
}

/*===========================================================================
 * Serial_Printf — 格式化打印
 *===========================================================================*/
void Serial_Printf(const char *fmt, ...)
{
    static char printf_buf[256];
    va_list args;
    va_start(args, fmt);
    vsnprintf(printf_buf, sizeof(printf_buf), fmt, args);
    va_end(args);
    Serial_SendString(printf_buf);
}

/*===========================================================================
 * 接收接口
 *===========================================================================*/

uint16_t Serial_Available(void)
{
    return rx_buf_used();
}

int16_t Serial_Read(void)
{
    if (rx_buf_empty())
    {
        return -1;   /* 缓冲区空 */
    }

    uint8_t byte = rx_buf[rx_tail];
    rx_tail = (rx_tail + 1) & (SERIAL_RX_BUF_SIZE - 1);
    return byte;
}

uint16_t Serial_ReadLine(char *buf, uint16_t maxLen)
{
    uint16_t count = 0;

    while (count < maxLen - 1)
    {
        int16_t ch = Serial_Read();
        if (ch < 0) break;       /* 无数据 */

        buf[count++] = (char)ch;

        if (ch == '\n') break;   /* 行尾 */
    }

    buf[count] = '\0';
    return count;
}

void Serial_Flush(void)
{
    /* 原子操作: 清空环形缓冲区 */
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    rx_head = 0;
    rx_tail = 0;
    __set_PRIMASK(primask);
}

/*===========================================================================
 * 回调注册
 *===========================================================================*/

void Serial_SetRxByteCallback(Serial_RxByteCallback callback)
{
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    rx_byte_cb = callback;
    __set_PRIMASK(primask);
}

void Serial_SetRxLineCallback(Serial_RxLineCallback callback)
{
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    rx_line_cb = callback;
    __set_PRIMASK(primask);
}

/*===========================================================================
 * Serial_RxIRQHandler — UART0 接收中断处理
 *
 * 在 UART0_IRQHandler 中调用。
 * 流程:
 *   1. 从 RX FIFO 读取数据
 *   2. 填入环形缓冲区
 *   3. 如果有逐字节回调 → 触发
 *   4. 如果收到 \n 且有逐行回调 → 触发
 *===========================================================================*/
void Serial_RxIRQHandler(void)
{
    /*
     * 循环读取 RX FIFO 直到为空 (一次中断可能收到多个字节)
     */
    while (DL_UART_isRXFIFOEmpty(UART_0_INST) == false)
    {
        uint8_t byte = DL_UART_receiveData(UART_0_INST);

        /* ---- 写入环形缓冲区 (若满则丢弃最旧数据) ---- */
        uint16_t next_head = (rx_head + 1) & (SERIAL_RX_BUF_SIZE - 1);
        if (next_head != rx_tail)   /* 缓冲区未满 */
        {
            rx_buf[rx_head] = byte;
            rx_head = next_head;
        }
        /* else: 缓冲区满, 丢弃此字节 (不阻塞) */

        /* ---- 逐字节回调 ---- */
        if (rx_byte_cb)
        {
            rx_byte_cb(byte);
        }

        /* ---- 逐行回调 ---- */
        if (rx_line_cb)
        {
            /* 忽略 '\r' (回车), 以 '\n' (换行) 作为行分隔符 */
            if (byte == '\n')
            {
                line_buf[line_idx] = '\0';
                rx_line_cb(line_buf);
                line_idx = 0;
            }
            else if (byte != '\r' && line_idx < SERIAL_LINE_BUF_SIZE - 1)
            {
                line_buf[line_idx++] = (char)byte;
            }
        }
    }

    /* 清除中断标志 */
    DL_UART_clearInterruptStatus(UART_0_INST, DL_UART_INTERRUPT_RX);
}
