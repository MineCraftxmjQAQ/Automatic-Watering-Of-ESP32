#include "OLED.h"

int8_t LoopShow;                                            //用于控制OLED显示内容的状态变量

/* 1.3寸使用构造函数 */
Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);           //用上述配置作构造函数入参创建实例

/* 0.96寸使用构造函数 */
//Adafruit_SSD1306 display(Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire));     //用上述配置作构造函数入参创建实例

/**
 * @brief     OLED显示屏初始化
 * @param     无
 * @return    无
 * @exception 无
 * */
void OLED_Init(void)
{
  LoopShow = 5;                                             //初始化时设置第0项为OLED显示内容(第5项在自增后变为第0项)

  /* 1.3寸使用初始化函数 */
  display.begin(OLED_I2C_ADDRESS, true);                    //于对应芯片的默认I2C引脚上初始化OELD屏幕

  /* 0.96寸使用初始化函数 */
  //display.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDRESS);    //于对应芯片的默认I2C引脚上初始化OELD屏幕

  display.clearDisplay();                                   //清空缓冲区
  display.display();                                        //送显
}

/**
 * @brief     OLED系统动画
 * @param     无符号8位整型 表示选择显示的动画
 * @return    无
 * @exception 无
 * */
void OLED_SystemShow(uint8_t num)
{
  uint8_t i;

  if(num == 0)                  //启动动画一阶段
  {
    for(i = 0; i < 10; i++)
    {
      display.clearDisplay();
      display.drawCircle(63, 31, i, SH110X_WHITE);
      display.display();
      delay(100);
    }
    for(i = 0; i < 10; i++)
    {
      display.clearDisplay();
      display.drawCircle(63, 31, 10 - i, SH110X_WHITE);
      display.display();
      delay(100);
    }
  }
  else if(num == 1)             //启动动画二阶段
  {
    for(i = 0; i < 20; i++)
    {
      display.clearDisplay();
      display.drawCircle(63, 31, i, SH110X_WHITE);
      display.display();
      delay(50);
    }
    for(i = 0; i < 20; i++)
    {
      display.clearDisplay();
      display.drawCircle(63, 31, 20 - i, SH110X_WHITE);
      display.display();
      delay(50);
    }
  }
  else if(num == 2)             //启动动画三阶段
  {
    for(i = 0; i < 31; i++)
    {
      display.clearDisplay();
      display.drawCircle(63, 31, i, SH110X_WHITE);
      display.display();
      delay(32);
    }
    for(i = 0; i < 31; i++)
    {
      display.clearDisplay();
      display.drawCircle(63, 31, 31 - i, SH110X_WHITE);
      display.display();
      delay(32);
    }
  }
  else if(num == 3)             //启动动画四阶段
  {
    display.clearDisplay();
    for(i = 0; i < 80; i++)
    {
      display.fillCircle(63, 31, i, SH110X_WHITE);
      display.display();
      delay(10);
    }
  }
  else                          //关闭动画
  {
    for(i = 0; i < 80; i++)
    {
      display.fillCircle(63, 31, i, SH110X_WHITE);
      display.display();
    }
    for(i = 0; i < 80; i++)
    {
      display.clearDisplay();
      display.fillCircle(63, 31, 80 - i, SH110X_WHITE);
      display.display();
    }
    display.clearDisplay();
    display.display();
  }
}

/**
 * @brief     OLED显示内容
 * @param     无符号8位整型 表示OLED显示内容的方向
 * @return    无
 * @exception 无
 * */
void OLED_Show(uint8_t dir)
{
  time_t compare_time_start;                                //比较时间戳(开始)
  time_t compare_time_stop;                                 //比较时间戳(结束)

  char format_time[20];                                     //定义格式化时间字符数组
  strftime(format_time, 20, "%Y/%m/%e %H:%M:%S", &TimeData);//格式化时间字符串

  String ShowStr;                                           //定义显示字符串

  /* OLED显示内容方向控制 */
  if(dir == 0)
  {
    LoopShow--;
  }
  else
  {
    LoopShow++;
  }

  /* 状态变量越界回环 */
  if(LoopShow > 5)
  {
    LoopShow = 0;
  }
  else if(LoopShow < 0)
  {
    LoopShow = 5;
  }

  /* 通过状态变量更改OLED显示内容 */
  if(LoopShow == 0)                                         //显示控制板时间和控制板IP地址
  {
    ShowStr = "LocalTime:\n\n" + String(format_time) + "\n\nLocalIP:\n\n" + hostIP.toString();
  }
  else if(LoopShow == 1)                                    //显示实时传感器数据
  {
    ShowStr = "\nAmbientTemp :" + String(SensorData.AmbientTemp) +
              "C\n\nAmbientHumi :" + String(SensorData.AmbientHumi) +
              "%\n\nSoilMoisture:" + String(SensorData.SoilMoisture) +
              "%";
  }
  else if(LoopShow == 2)                                    //显示浇水模式
  {
    if(ModeSwitch == 0 && CtrlData.Watering_Model == 0)     //0未选中,1选中
    {
      ShowStr = "Watering_Model\n\n[TimingModel]\n\n SensorModel\n\n ManuallyModel";
    }
    else if(ModeSwitch == 0 && CtrlData.Watering_Model == 1)
    {
      ShowStr = "Watering_Model\n\n TimingModel\n\n[SensorModel]\n\n ManuallyModel";
    }
    else
    {
      ShowStr = "Watering_Model\n\n TimingModel\n\n SensorModel\n\n[ManuallyModel]";
    }
  }
  else if(LoopShow == 3)                                    //显示设定一星期的浇水天
  {
    ShowStr = "Watering_Weekday\n\nSelected:\n\n";
    for(uint8_t i = 0; i < 7; i++)
    {
      if(CtrlData.WeekdayEnable[i] == 0)                    //0未选中,1选中
      {
        ShowStr += " " + String(i + 1) + " ";
      }
      else
      {
        ShowStr += "[" + String(i + 1) + "]";
      }
    }
  }
  else if(LoopShow == 4)                                    //显示设定一天内浇水时段
  {
    char format_time[9];                                    //定义格式化时间字符数组

    ShowStr = "Watering_First:\n\n";
    compare_time_start = mktime(&CtrlData.FirstWatering_Start);
    compare_time_stop = mktime(&CtrlData.FirstWatering_Stop);
    if(compare_time_start == compare_time_stop)
    {
      ShowStr += "Not Configured!";
    }
    else
    {
      strftime(format_time, 9, "%H:%M:%S", &CtrlData.FirstWatering_Start);
      ShowStr += String(format_time) + " --> ";
      strftime(format_time, 9, "%H:%M:%S", &CtrlData.FirstWatering_Stop);
      ShowStr += String(format_time);
    }
    ShowStr += "\n\nWatering_Second:\n\n";
    compare_time_start = mktime(&CtrlData.SecondWatering_Start);
    compare_time_stop = mktime(&CtrlData.SecondWatering_Stop);
    if(compare_time_start == compare_time_stop)
    {
      ShowStr += "Not Configured!";
    }
    else
    {
      strftime(format_time, 9, "%H:%M:%S", &CtrlData.SecondWatering_Start);
      ShowStr += String(format_time) + " --> ";
      strftime(format_time, 9, "%H:%M:%S", &CtrlData.SecondWatering_Stop);
      ShowStr += String(format_time);
    }
  }
  else if(LoopShow == 5)                                    //显示设定浇水阈值限制
  {
    ShowStr = "SoilMoisture_Limit:\n\n";
    if(CtrlData.SoilMoisture_LowerLimit == CtrlData.SoilMoisture_UpperLimit)
    {
      ShowStr += "LowerLimit:Not Configured!\n\n";
      ShowStr += "UpperLimit:Not Configured!";
    }
    else
    {
      ShowStr += "LowerLimit:" + String(CtrlData.SoilMoisture_LowerLimit) + "%\n\n";
      ShowStr += "UpperLimit:" + String(CtrlData.SoilMoisture_UpperLimit) + "%";
    }
  }
  else                                                      //不应存在的状态,设置为状态0
  {
    LoopShow = 0;
  }

  display.clearDisplay();                                   //清空缓冲区
  display.setCursor(0, 0);                                  //设置开始坐标0,0
  display.setTextSize(1);                                   //设置字号

  /* 1.3寸使用颜色关键字 */
  display.setTextColor(SH110X_WHITE, SH110X_BLACK);         //设置为黑底白字

  /* 0.96寸使用颜色关键字 */
  //display.setTextColor(WHITE, BLACK);                       //设置为黑底白字

  display.print(ShowStr);                                   //将字符串送入缓冲区
  display.display();                                        //送显
}