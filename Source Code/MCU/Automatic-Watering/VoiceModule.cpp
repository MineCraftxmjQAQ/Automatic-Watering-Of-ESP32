#include "VoiceModule.h"

HardwareSerial VoiceModule_Serial(2);     //使用Serial2创建串口实例

/**
 * @brief     语音模块初始化
 * @param     无
 * @return    无
 * @exception 无
 * */
void VoiceModule_Init(void)
{
  /* 初始化配置为:波特率9600,1位起始位,8位数据位,1位停止位,无校验位,指定的RX与DX */
  VoiceModule_Serial.begin(9600, SERIAL_8N1, VOICEMODULE_SERIAL_RX, VOICEMODULE_SERIAL_TX);
}

/**
 * @brief     指定语音发声
 * @param     无符号8位整型 表示指定语音的序号
 * @return    无
 * @exception 无
 * */
void VoiceModule_Play(uint8_t num)
{
  VoiceModule_Serial.write(0xAA);         //发送固定格式
  VoiceModule_Serial.write(0x07);         //发送固定格式
  VoiceModule_Serial.write(0x02);         //发送固定格式
  VoiceModule_Serial.write(0x00);         //发送固定格式
  VoiceModule_Serial.write(num);          //发送歌曲序数
  VoiceModule_Serial.write(0xB3 + num);   //发送校验和
}