#ifndef __MYDHT20_H
#define __MYDHT20_H

#include <DHT20.h>

/* 外部可调用函数 */
void DHT20_Init(void);
boolean GetDHT20Data(float* temp, float* humi);

#endif