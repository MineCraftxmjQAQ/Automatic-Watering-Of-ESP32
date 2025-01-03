#ifndef __SOILMOISTURESENSOR_H
#define __SOILMOISTURESENSOR_H

#include <Arduino.h>

#define SMS_GET     34      //土壤湿度传感器采集线(GPIO34)

/* 外部可调用函数 */
void SMS_Init(void);
float GetSMSData(uint16_t sampling_times);

#endif