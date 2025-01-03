#include <rtc_wdt.h>

#include "Ctrl.h"

uint8_t WiFiConnectStatus;                      //WiFi连接状态,0状态WiFi未连接,1状态WiFi已连接
uint8_t LastWiFiConnectStatus;                  //上一次WiFi状态,0状态WiFi未连接,1状态WiFi已连接
uint8_t WiFiConnectVoiceStatus;                 //WiFi连接语音状态,0状态不播报(暂稳态),1状态播报(稳态)

uint8_t OLED_Refresh;                           //定义OLED刷新状态

hw_timer_t *timer = NULL;                       //定义定时器
volatile SemaphoreHandle_t timerSemaphore;      //定义二值信号量

/* 定时器中断函数 */
void ARDUINO_ISR_ATTR onTimer()
{
  xSemaphoreGiveFromISR(timerSemaphore, NULL);  //给出可观测的信号量
}

/* 主函数初始化部分 */
void setup()
{
  /* ----------全组件初始化开始---------- */
  /* 显示模块初始化 */
  OLED_Init();                                  //OLED初始化
  OLED_Refresh = 0;                             //OLED初始刷新状态为0

  /* 语音模块初始化 */
  VoiceModule_Init();                           //语音模块初始化
  VoiceModule_Play(INITIALIZING);               //播报语音:正在初始化,请稍后
  OLED_SystemShow(0);                           //播放启动动画一阶段

  /* 控制模块初始化 */
  Read_CtrlData();                              //启动时最先读取上次保存的配置信息
  CTRLKey_Init();                               //控制按键初始化
  WateringCtrl_Init();                          //浇花控制初始化

  /* 温湿度模块初始化 */
  DHT20_Init();                                 //DHT20温湿度传感器初始化

  /* 土壤湿度模块初始化 */
  SMS_Init();                                   //土壤湿度传感器初始化

  /* 网络模块初始化 */
  VoiceModule_Play(WIFICONNECTING);             //播报语音:正在连接WiFi
  OLED_SystemShow(1);                           //播放启动动画二阶段
  WiFiConnectStatus = 0;                        //初始化时WiFi未连接
  if(!Internet_Init())                          //如果网络连接初始化失败(即直接连接失败)
  {
    WiFi_Connect();                             //尝试重连,若仍失败则进入配网流程
    while(1)
    {
      Check_Request();
      if(Check_Connect(true))                   //配网成功
      {
        break;
      }
    }
  }
  VoiceModule_Play(WIFICONNECTED);              //播报语音:WiFi连接成功
  OLED_SystemShow(2);                           //播放启动动画三阶段
  WiFiConnectStatus = 1;                        //WiFi已连接
  Time_Init();                                  //时间校准初始化
  UDP_Init();                                   //UDP初始化
  Control_WebServer_Init();                     //控制服务器初始化

  /* 定时器模块初始化 */
  timerSemaphore = xSemaphoreCreateBinary();    //设置定时器中断回调函数
  timer = timerBegin(1000000);                  //以1MHz的计数频率初始化定时器
  timerAttachInterrupt(timer, &onTimer);        //设置定时器中断回调函数
  timerAlarm(timer, 2000000, true, 0);          //设置计数2M次(定时2s),重复定时(true),无限计数(0)

  /* 看门狗模块初始化 */
  rtc_wdt_protect_off();                        //关闭看门狗写保护
  rtc_wdt_enable();                             //启动看门狗
  rtc_wdt_set_time(RTC_WDT_STAGE0, 5000);       //设置看门狗超时5s
  /* ----------全组件初始化完成---------- */

  VoiceModule_Play(INITIALIZED);                //播报语音:初始化完成
  OLED_SystemShow(3);                           //播放启动动画四阶段
}

/* 主函数循环体部分 */
void loop()
{
  LastWiFiConnectStatus = WiFiConnectStatus;    //在更新WiFi连接状态前备份当前WiFi连接状态

  if(WiFi.status() == WL_CONNECTED)             //在线浇花模式
  {
    Get_SensorData();                           //获取传感器数据
    Time_Aligned();                             //联网校准时间
    CTRLKey_Response();                         //控制按键响应
    WateringCtrl();                             //更新浇花状态
    Controlboard_Data_Formatting();             //更新发送向网页的内容
    Check_ClientRequest();                      //检查来自客户端的HTTP请求

    WiFiConnectStatus = 1;
  }
  else                                          //离线浇花模式
  {
    Get_SensorData();                           //获取传感器数据
    CTRLKey_Response();                         //控制按键响应
    WateringCtrl();                             //更新浇花状态

    WiFiConnectStatus = 0; 
  }

  /* 当WiFi连接状态变化时置WiFi连接语音状态为播报(捕获边沿、过滤电平) */
  if(LastWiFiConnectStatus != WiFiConnectStatus)
  {
    WiFiConnectVoiceStatus = 1;
  }
  else
  {
    WiFiConnectVoiceStatus = 0;
  }

  /* 当WiFi连接语音状态为播报时播报对应语音 */
  if(WiFiConnectVoiceStatus == 1)
  {
    if(WiFiConnectStatus == 0)
    {
      VoiceModule_Play(OFFLINEWATERING);        //播报语音:WiFi连接丢失，控制板已进入离线浇花模式
    }
    else
    {
      VoiceModule_Play(ONLINEWATERING);         //播报语音:WiFi连接恢复，控制板已进入在线浇花模式
    }
    WiFiConnectVoiceStatus = 0;                 //复位WiFi连接语音状态为不播报
  }

  /* 检测到信号量变化 */
  if(xSemaphoreTake(timerSemaphore, 0) == pdTRUE)
  {
    UDP_BoadcastPacket();                       //发送UDP广播包(2s一次)
    if(OLED_Refresh == 0)
    {
      OLED_Refresh = 1;
    }
    else                                        //OLED_Refresh变动一次才更新OLED
    {
      OLED_Refresh = 0;
      OLED_Show(1);                             //更新OELD显示内容(4s一次)
    }
  }

  rtc_wdt_feed();                               //主函数循环体部分每次循环末尾执行喂狗
}