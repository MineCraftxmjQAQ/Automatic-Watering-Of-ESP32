#ifndef __CTRL_H
#define __CTRL_H

#include <stdint.h>
#include <nvs_flash.h>

#include "MyDHT20.h"
#include "MyWiFi.h"
#include "OLED.h"
#include "SoilMoistureSensor.h"
#include "VoiceModule.h"

#define WATERING_OPUPUT       4         //浇花水泵控制线(GPIO4)

#define CTRLKey_1             33        //控制按钮1(GPIO33)
#define CTRLKey_2             26        //控制按钮2(GPIO26)
#define CTRLKey_3             27        //控制按钮3(GPIO27)

/* 控制数据结构体 */
typedef struct
{
  uint8_t Watering_Model;               //浇花模式,0为定时模式,1为传感器模式

  uint8_t WeekdayEnable[7];             //设定一周的浇花天

  struct tm FirstWatering_Start;        //一天内第一次浇花开始时间
  struct tm FirstWatering_Stop;         //一天内第一次浇花结束时间

  struct tm SecondWatering_Start;       //一天内第二次浇花开始时间
  struct tm SecondWatering_Stop;        //一天内第二次浇花结束时间

  uint8_t SoilMoisture_LowerLimit;      //传感器模式浇花湿度下限
  uint8_t SoilMoisture_UpperLimit;      //传感器模式浇花湿度上限
}CtrlData_Typdef;

/* 传感器数据结构体 */
typedef struct
{
  float AmbientTemp;                    //环境温度数据
  float AmbientHumi;                    //环境湿度数据
  float SoilMoisture;                   //土壤湿度数据
}SensorData_Typdef;

/* 全局核心变量 */
extern CtrlData_Typdef CtrlData;        //外部调用声明
extern SensorData_Typdef SensorData;    //外部调用声明
extern uint8_t ModeSwitch;              //外部调用声明
extern uint8_t WateringStatus;          //外部调用声明

/* 外部可调用函数 */
void Get_SensorData(void);
void Read_CtrlData(void);
void Save_CtrlData(void);
void CTRLKey_Init(void);
void CTRLKey_Response(void);
void WateringCtrl_Init(void);
void WateringCtrl(void);

#endif