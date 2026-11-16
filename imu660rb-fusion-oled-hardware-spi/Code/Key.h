/**
 * @file    Key.h
 * @brief   四路按键 (非阻塞, 带消抖)
 *
 * KEY1=PA28: 发车+计时
 * KEY2=PA31: 停车时间+5s
 * KEY3=PA8:  停车时间-5s
 * KEY4=PA9:  急停/复位
 */

#ifndef __KEY_H
#define __KEY_H

#include "ti_msp_dl_config.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define KEY_DEBOUNCE_CNT    4

#define KEY_NONE            0
#define KEY1_PRESS          1    /* PA28 */
#define KEY2_PRESS          2    /* PA31 */
#define KEY3_PRESS          3    /* PA8  */
#define KEY4_PRESS          4    /* PA9  */

uint8_t Key_Scan(void);         /* 返回 KEY_NONE / KEY1~4_PRESS */

#ifdef __cplusplus
}
#endif

#endif
