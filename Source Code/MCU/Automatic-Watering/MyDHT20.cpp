#include "MyDHT20.h"

DHT20 DHT;                                //创建DHT20温湿度传感器类实例

/**
 * @brief     DHT20温湿度传感器初始化
 * @param     无
 * @return    无
 * @exception 无
 * */
void DHT20_Init(void)
{
  Wire.begin();                           //将控制板作为主机加入I2C总线
  DHT.begin();                            //DHT20温湿度传感器类实例初始化

  delay(1000);                            //延迟1s以等待DHT20就绪
}

/**
 * @brief     获取DHT20温湿度传感器数据
 * @param     单精度浮点型指针温度值,单精度浮点型指针湿度值
 * @return    布尔型 返回true表示状态正常,返回false表示状态有误
 * @exception 无
 * */
boolean GetDHT20Data(float* temp, float* humi)
{
  if(millis() - DHT.lastRead() >= 1000)   //相邻数据获取需要间隔1s以上
  {
    *temp = DHT.getTemperature();         //读出温度值
    *humi = DHT.getHumidity();            //读出湿度值
    if(DHT.read() == DHT20_OK)            //判断设备状态
    {
      return true;
    }
  }
  return false;
}