#ifndef _MAIN_H_
#define _MAIN_H_

#include "clock.h"
#include "interrupt.h"

#include "mpu6050.h"
/* 软件 I2C OLED: PB2=SCL, PB3=SDA (复用原硬件 I2C 引脚) */
#define GPIO_OLED_PORT                  GPIOB
#define GPIO_OLED_PIN_OLED_SCL_PIN      DL_GPIO_PIN_2
#define GPIO_OLED_PIN_OLED_SCL_IOMUX    (IOMUX_PINCM15)
#define GPIO_OLED_PIN_OLED_SDA_PIN      DL_GPIO_PIN_3
#define GPIO_OLED_PIN_OLED_SDA_IOMUX    (IOMUX_PINCM16)

//#include "oled_hardware_i2c.h"
#include "oled_software_i2c.h"
//#include "oled_software_i2c.h"
//#include "oled_software_spi.h"
//#include "oled_hardware_spi.h"
#include "ultrasonic_capture.h"
#include "ultrasonic_gpio.h"
#include "bno08x_uart_rvc.h"
#include "wit.h"
#include "vl53l0x.h"
#include "lsm6dsv16x.h"
#include "imu660rb.h"
#include "Tracker.h"
#include "Motor.h"
#include "Key.h"
#include "Servo.h"
#include "PID.h"
#include "Encoder.h"
#include "Serial.h"

#endif  /* #ifndef _MAIN_H_ */
