#include "SoilMoistureSensor.h"

/**
 * @brief     土壤湿度传感器初始化
 * @param     无
 * @return    无
 * @exception 无
 * */
void SMS_Init(void)
{
  pinMode(SMS_GET, INPUT);
}

/**
 * @brief     获取土壤湿度传感器数据
 * @param     无符号16位整型 表示单次采样次数
 * @return    单精度浮点型 表示土壤湿度值,范围0% ~ 100%
 * @exception 无
 * */
float GetSMSData(uint16_t sampling_times)
{
  float SMSData_All = 0;

  for(int i = 0; i < sampling_times; i++)
  {
    SMSData_All += analogRead(SMS_GET);
    delay(1);
  }
  return ((4096 - SMSData_All / sampling_times) / 4096 * 100);
}