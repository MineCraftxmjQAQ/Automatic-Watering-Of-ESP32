#ifndef __VOICEMODULE_H
#define __VOICEMODULE_H

#include <HardwareSerial.h>

/* 本项目使用DY-SV8F语音播放模块,模块启用标准串口模式 */
#define VOICEMODULE_SERIAL_RX     16      //语音模块串口接收线(GPIO16)
#define VOICEMODULE_SERIAL_TX     17      //语音模块串口发送线(GPIO17)

#define INITIALIZING              1       //语音:正在初始化,请稍后
#define INITIALIZED               2       //语音:初始化完成

#define RESETANDRESTART           3       //语音:已恢复缺省配置，控制板即将重启

#define WIFICONNECTING            4       //语音:正在连接WiFi
#define WIFICONNECTIONFAILED      5       //语音:WiFi连接超时,正在尝试重连
#define DISTRIBUTIONNETWORK       6       //语音:WiFi重连超时,控制板已切换至AP模式,请连接控制板热点执行配网流程
#define WIFICONNECTED             7       //语音:WiFi连接成功

#define OFFLINEWATERING           8       //语音:WiFi连接丢失，控制板已进入离线浇花模式
#define ONLINEWATERING            9       //语音:WiFi连接恢复，控制板已进入在线浇花模式

#define CONFIGURATIONUPDATED      10      //语音:已保存并应用新的浇花配置

#define AUTOMATICWATERING         11      //语音:已切换至自动浇花模式
#define MANUALLYWATERING          12      //语音:已切换至手动浇花模式

#define NOTWATERING               13      //语音:当前未在浇花
#define WATERING                  14      //语音:当前正在浇花

/* 外部可调用函数 */
void VoiceModule_Init(void);
void VoiceModule_Play(uint8_t num);

#endif