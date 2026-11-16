/**
 * @file    Key.c
 * @brief   四路按键扫描 (非阻塞, 带消抖)
 *
 * KEY1=PA28  KEY2=PA31  KEY3=PA8  KEY4=PA9
 * 按下=LOW(0), 内部上拉, 下降沿触发
 */

#include "Key.h"

static uint8_t scan_one(GPIO_Regs *port, uint32_t pin,
                        uint8_t *stable, uint8_t *cnt)
{
    uint8_t cur = (DL_GPIO_readPins(port, pin) != 0) ? 1 : 0;
    if (cur != *stable)
    {
        (*cnt)++;
        if (*cnt >= KEY_DEBOUNCE_CNT)
        {
            *stable = cur;
            *cnt = 0;
            if (cur == 0) return 1;   /* 下降沿 */
        }
    }
    else { *cnt = 0; }
    return 0;
}

uint8_t Key_Scan(void)
{
    static uint8_t s1=1,s2=1,s3=1,s4=1, c1=0,c2=0,c3=0,c4=0;

    if (scan_one(GPIOA, DL_GPIO_PIN_28, &s1, &c1)) return KEY1_PRESS;
    if (scan_one(GPIOA, DL_GPIO_PIN_31, &s2, &c2)) return KEY2_PRESS;
    if (scan_one(GPIOA, DL_GPIO_PIN_8,  &s3, &c3)) return KEY3_PRESS;
    if (scan_one(GPIOA, DL_GPIO_PIN_9,  &s4, &c4)) return KEY4_PRESS;

    return KEY_NONE;
}
