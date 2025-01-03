#ifndef __MYWIFI_H__
#define __MYWIFI_H__

#include <string.h>
#include <nvs_flash.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <ESPmDNS.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <NTPClient.h>

#include "Ctrl.h"
#include "VoiceModule.h"

extern IPAddress hostIP;      //外部调用声明
extern time_t NOW;            //外部调用声明
extern struct tm TimeData;    //外部调用声明

/* 外部可调用函数 */
boolean Internet_Init(void);
void Check_Request(void);
void WiFi_Connect(void);
boolean Check_Connect(boolean reConnect);
void UDP_Init(void);
void UDP_BoadcastPacket(void);
void Controlboard_Data_Formatting(void);
void Control_WebServer_Init(void);
void Check_ClientRequest(void);
void Time_Init(void);
void Time_Aligned(void);

/* 内部函数 */
//void Handle_Root(void);
//void Handle_Config(void);
//void APMode_Init(void);
//void DNS_Init(void);
//void WebServer_Init(void);
//boolean WiFi_Scan(void);
//boolean UDP_RecvPacket(void);
//void UDP_Stop(void);
//void Control_WebServer_Callback(void);
//void Control_WebServer_SendData_Callback(void);
//void Ctrl_Config(void);

#endif