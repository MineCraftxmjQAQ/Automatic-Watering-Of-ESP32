#ifndef __OLED_H
#define __OLED_H

#include <Wire.h>
#include <Adafruit_GFX.h>

/* 1.3寸使用头文件(SH1106驱动芯片) */
#include <Adafruit_SH110X.h>

/* 0.96寸使用头文件(SSD1306驱动芯片) */
//#include <Adafruit_SSD1306.h>

#include "Ctrl.h"
#include "MyWiFi.h"

/** 默认I2C_SCL为GPIO22
 *  默认I2C_SDA为GPIO21
 * */
#define OLED_I2C_ADDRESS        0x3c        //OLED屏幕I2C地址

/* 本项目使用1.3寸或0.96寸128*64白色OLED屏幕 */
#define SCREEN_WIDTH            128         //长128像素
#define SCREEN_HEIGHT           64          //宽64像素
#define OLED_RESET              -1          //没有重置位-1,有重置位4

/* 外部可调用函数 */
void OLED_Init(void);
void OLED_SystemShow(uint8_t num);
void OLED_Show(uint8_t dir);

#endif