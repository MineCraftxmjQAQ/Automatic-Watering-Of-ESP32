#include "Ctrl.h"

CtrlData_Typdef CtrlData;           //控制数据结构体变量
SensorData_Typdef SensorData;       //传感器数据结构体变量

nvs_handle ctrl_nvs_handle;         //NVS操作句柄

uint8_t ModeSwitch;                 //自动浇花模式与手动浇花模式状态切换标志,0状态自动浇花模式,1状态手动浇花模式
uint8_t WateringStatus;             //浇花状态,0(LOW)状态未在浇花,1(HIGH)状态正在浇花
uint8_t LastWateringStatus;         //上一次浇花状态,0(LOW)状态未在浇花,1(HIGH)状态正在浇花
uint8_t VoiceStatus;                //浇花语音状态,0状态不播报(暂稳态),1状态播报(稳态)

uint8_t CTRLKeyStatus[3];	          //实际按键按下状态数组
uint8_t ButtonStatus[3];		        //按键状态机切换数组

/**
 * @brief     获取传感器数据
 * @param     无
 * @return    无
 * @exception 无
 * */
void Get_SensorData(void)
{
  GetDHT20Data(&SensorData.AmbientTemp, &SensorData.AmbientHumi);   //获取环境温湿度
  SensorData.SoilMoisture = GetSMSData(30);                         //获取土壤湿度
}

/**
 * @brief     读取浇花配置数据
 * @param     无
 * @return    无
 * @exception 无
 * */
void Read_CtrlData(void)
{
  esp_err_t ret = nvs_flash_init();                                                 //NVS初始化
  if(ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
  {
    nvs_flash_erase();                                                              //擦除NVS
    nvs_flash_init();                                                               //尝试再次初始化
  }

  if(nvs_open("Ctrl_Config", NVS_READWRITE, &ctrl_nvs_handle) == ESP_OK)            //打开命名空间"Ctrl_Config"
  {
    uint8_t ReadTime;                                                               //浇花时间临时接收变量

    /* 读取浇花模式 */
    nvs_get_u8(ctrl_nvs_handle, "Watering_Model", &CtrlData.Watering_Model);

    /* 读取设定一星期的浇花天 */
    nvs_get_u8(ctrl_nvs_handle, "WeekdayEnable_0", &CtrlData.WeekdayEnable[0]);
    nvs_get_u8(ctrl_nvs_handle, "WeekdayEnable_1", &CtrlData.WeekdayEnable[1]);
    nvs_get_u8(ctrl_nvs_handle, "WeekdayEnable_2", &CtrlData.WeekdayEnable[2]);
    nvs_get_u8(ctrl_nvs_handle, "WeekdayEnable_3", &CtrlData.WeekdayEnable[3]);
    nvs_get_u8(ctrl_nvs_handle, "WeekdayEnable_4", &CtrlData.WeekdayEnable[4]);
    nvs_get_u8(ctrl_nvs_handle, "WeekdayEnable_5", &CtrlData.WeekdayEnable[5]);
    nvs_get_u8(ctrl_nvs_handle, "WeekdayEnable_6", &CtrlData.WeekdayEnable[6]);

    /* 读取一天内第一次浇花开始时间 */
    nvs_get_u8(ctrl_nvs_handle, "FirstStart_h", &ReadTime);
    CtrlData.FirstWatering_Start.tm_hour = ReadTime;
    nvs_get_u8(ctrl_nvs_handle, "FirstStart_m", &ReadTime);
    CtrlData.FirstWatering_Start.tm_min = ReadTime;
    nvs_get_u8(ctrl_nvs_handle, "FirstStart_s", &ReadTime);
    CtrlData.FirstWatering_Start.tm_sec = ReadTime;

    /* 读取一天内第一次浇花结束时间 */
    nvs_get_u8(ctrl_nvs_handle, "FirstStop_h", &ReadTime);
    CtrlData.FirstWatering_Stop.tm_hour = ReadTime;
    nvs_get_u8(ctrl_nvs_handle, "FirstStop_m", &ReadTime);
    CtrlData.FirstWatering_Stop.tm_min = ReadTime;
    nvs_get_u8(ctrl_nvs_handle, "FirstStop_s", &ReadTime);
    CtrlData.FirstWatering_Stop.tm_sec = ReadTime;

    /* 读取一天内第二次浇花开始时间 */
    nvs_get_u8(ctrl_nvs_handle, "SecondStart_h", &ReadTime);
    CtrlData.SecondWatering_Start.tm_hour = ReadTime;
    nvs_get_u8(ctrl_nvs_handle, "SecondStart_m", &ReadTime);
    CtrlData.SecondWatering_Start.tm_min = ReadTime;
    nvs_get_u8(ctrl_nvs_handle, "SecondStart_s", &ReadTime);
    CtrlData.SecondWatering_Start.tm_sec = ReadTime;

    /* 读取一天内第二次浇花结束时间 */
    nvs_get_u8(ctrl_nvs_handle, "SecondStop_h", &ReadTime);
    CtrlData.SecondWatering_Stop.tm_hour = ReadTime;
    nvs_get_u8(ctrl_nvs_handle, "SecondStop_m", &ReadTime);
    CtrlData.SecondWatering_Stop.tm_min = ReadTime;
    nvs_get_u8(ctrl_nvs_handle, "SecondStop_s", &ReadTime);
    CtrlData.SecondWatering_Stop.tm_sec = ReadTime;

    /* 读取浇传感器模式浇花湿度下限 */
    nvs_get_u8(ctrl_nvs_handle, "LowerLimit", &CtrlData.SoilMoisture_LowerLimit);

    /* 读取浇传感器模式浇花湿度上限 */
    nvs_get_u8(ctrl_nvs_handle, "UpperLimit", &CtrlData.SoilMoisture_UpperLimit);

    nvs_close(ctrl_nvs_handle);                                                     //关闭命名空间"Ctrl_Config"
  }
}

/**
 * @brief     保存浇花配置数据
 * @param     无
 * @return    无
 * @exception 无
 * */
void Save_CtrlData(void)
{
  esp_err_t ret = nvs_flash_init();
  if(ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
  {
    nvs_flash_erase();                                                              //擦除NVS
    nvs_flash_init();                                                               //重新尝试初始化
  }

  if(nvs_open("Ctrl_Config", NVS_READWRITE, &ctrl_nvs_handle) == ESP_OK)            //打开命名空间"Ctrl_Config"
  {
    /* 保存浇花模式 */
    nvs_set_u8(ctrl_nvs_handle, "Watering_Model", CtrlData.Watering_Model);

    /* 保存设定一星期的浇花天 */
    nvs_set_u8(ctrl_nvs_handle, "WeekdayEnable_0", CtrlData.WeekdayEnable[0]);
    nvs_set_u8(ctrl_nvs_handle, "WeekdayEnable_1", CtrlData.WeekdayEnable[1]);
    nvs_set_u8(ctrl_nvs_handle, "WeekdayEnable_2", CtrlData.WeekdayEnable[2]);
    nvs_set_u8(ctrl_nvs_handle, "WeekdayEnable_3", CtrlData.WeekdayEnable[3]);
    nvs_set_u8(ctrl_nvs_handle, "WeekdayEnable_4", CtrlData.WeekdayEnable[4]);
    nvs_set_u8(ctrl_nvs_handle, "WeekdayEnable_5", CtrlData.WeekdayEnable[5]);
    nvs_set_u8(ctrl_nvs_handle, "WeekdayEnable_6", CtrlData.WeekdayEnable[6]);

    /* 保存一天内第一次浇花开始时间 */
    nvs_set_u8(ctrl_nvs_handle, "FirstStart_h", CtrlData.FirstWatering_Start.tm_hour);
    nvs_set_u8(ctrl_nvs_handle, "FirstStart_m", CtrlData.FirstWatering_Start.tm_min);
    nvs_set_u8(ctrl_nvs_handle, "FirstStart_s", CtrlData.FirstWatering_Start.tm_sec);

    /* 保存一天内第一次浇花结束时间 */
    nvs_set_u8(ctrl_nvs_handle, "FirstStop_h", CtrlData.FirstWatering_Stop.tm_hour);
    nvs_set_u8(ctrl_nvs_handle, "FirstStop_m", CtrlData.FirstWatering_Stop.tm_min);
    nvs_set_u8(ctrl_nvs_handle, "FirstStop_s", CtrlData.FirstWatering_Stop.tm_sec);

    /* 保存一天内第二次浇花开始时间 */
    nvs_set_u8(ctrl_nvs_handle, "SecondStart_h", CtrlData.SecondWatering_Start.tm_hour);
    nvs_set_u8(ctrl_nvs_handle, "SecondStart_m", CtrlData.SecondWatering_Start.tm_min);
    nvs_set_u8(ctrl_nvs_handle, "SecondStart_s", CtrlData.SecondWatering_Start.tm_sec);

    /* 保存一天内第二次浇花结束时间 */
    nvs_set_u8(ctrl_nvs_handle, "SecondStop_h", CtrlData.SecondWatering_Stop.tm_hour);
    nvs_set_u8(ctrl_nvs_handle, "SecondStop_m", CtrlData.SecondWatering_Stop.tm_min);
    nvs_set_u8(ctrl_nvs_handle, "SecondStop_s", CtrlData.SecondWatering_Stop.tm_sec);

    /* 保存浇传感器模式浇花湿度下限 */
    nvs_set_u8(ctrl_nvs_handle, "LowerLimit", CtrlData.SoilMoisture_LowerLimit);

    /* 保存浇传感器模式浇花湿度上限 */
    nvs_set_u8(ctrl_nvs_handle, "UpperLimit", CtrlData.SoilMoisture_UpperLimit);

    nvs_commit(ctrl_nvs_handle);                                                    //提交修改
    nvs_close(ctrl_nvs_handle);                                                     //关闭命名空间"Ctrl_Config"
  }
}

/**
 * @brief     控制按键初始化
 * @param     无
 * @return    无
 * @exception 无
 * */
void CTRLKey_Init(void)
{
  /* 初始时按键均为未触发状态 */
  ButtonStatus[0] = 0;
  ButtonStatus[1] = 0;
  ButtonStatus[2] = 0;

  /* 初始化按键GPIO为上拉输入 */
  pinMode(CTRLKey_1, INPUT_PULLUP);
  pinMode(CTRLKey_2, INPUT_PULLUP);
  pinMode(CTRLKey_3, INPUT_PULLUP);
}

/**
 * @brief     控制按键响应
 * @param     无
 * @return    无
 * @exception 无
 * */
void CTRLKey_Response(void)
{
  LastWateringStatus = WateringStatus;              //在更新浇花状态前备份当前浇花状态

  /* 按键状态刷新 */
  CTRLKeyStatus[0] = digitalRead(CTRLKey_1);
  CTRLKeyStatus[1] = digitalRead(CTRLKey_2);
  CTRLKeyStatus[2] = digitalRead(CTRLKey_3);

  /* 当按键按下且按键状态机处于未触发状态则置按键状态机为触发状态 */
  if(CTRLKeyStatus[0] == LOW && ButtonStatus[0] == 0)
  {
    ButtonStatus[0] = 1;
  }
  if(CTRLKeyStatus[1] == LOW && ButtonStatus[1] == 0)
  {
    ButtonStatus[1] = 1;
  }
  if(CTRLKeyStatus[2] == LOW && ButtonStatus[2] == 0)
  {
    ButtonStatus[2] = 1;
  }

  /* 当按键松开时则更改按键状态机为未触发状态 */
  if(CTRLKeyStatus[0] == HIGH)
  {
    ButtonStatus[0] = 0;
  }
  if(CTRLKeyStatus[1] == HIGH)
  {
    ButtonStatus[1] = 0;
  }
  if(CTRLKeyStatus[2] == HIGH)
  {
  ButtonStatus[2] = 0;
  }

  /* 当按键状态机为触发状态时进行对应的响应并置按键状态机为未触发状态 */
  /* 按键1按下:立即向前更改OLED显示内容 */
  if(ButtonStatus[0] == 1 && ButtonStatus[1] != 1 && ButtonStatus[2] != 1)
  {
    ButtonStatus[0] = 2;

    OLED_Show(0);
  }
  /* 按键2按下:切换浇花模式 */
  else if(ButtonStatus[0] != 1 && ButtonStatus[1] == 1 && ButtonStatus[2] != 1)
  {
    ButtonStatus[1] = 2;

    if(ModeSwitch == 0)
    {
      ModeSwitch = 1;

      VoiceModule_Play(MANUALLYWATERING);           //播报语音:已切换至手动浇花模式
    }
    else
    {
      ModeSwitch = 0;

      VoiceModule_Play(AUTOMATICWATERING);          //播报语音:已切换至自动浇花模式
    }
  }
  /* 按键3按下:处于自动浇花模式时立即向后更改OLED显示内容;处于手动浇花模式时开始和停止浇花 */
  else if(ButtonStatus[0] != 1 && ButtonStatus[1] != 1 && ButtonStatus[2] == 1)
  {
    ButtonStatus[2] = 2;

    if(ModeSwitch == 0)
    {
      OLED_Show(1);
    }
    else if(ModeSwitch == 1)
    {
      if(WateringStatus == LOW)
      {
        WateringStatus = HIGH;
      }
      else
      {
        WateringStatus = LOW;
      }
    }
  }
  /* 三个键全部按下:恢复缺省配置并重启 */
  else if(ButtonStatus[0] == 1 && ButtonStatus[1] == 1 && ButtonStatus[2] == 1)
  {
    ButtonStatus[0] = 2;
    ButtonStatus[1] = 2;
    ButtonStatus[2] = 2;

    nvs_flash_erase();                              //擦除NVS
    VoiceModule_Play(RESETANDRESTART);              //播报语音:已恢复缺省配置，控制板即将重启
    OLED_SystemShow(4);                             //播放关闭动画
    ESP.restart();                                  //重启ESP32
  }
}

/**
 * @brief     浇花控制初始化
 * @param     无
 * @return    无
 * @exception 无
 * */
void WateringCtrl_Init(void)
{
  ModeSwitch = 0;                                   //上电时初始化为自动浇花模式
  WateringStatus = LOW;                             //上电时初始化浇花状态为未在浇花
  VoiceStatus = 0;                                  //上电时初始化浇花语音状态为不播报

  digitalWrite(WATERING_OPUPUT, WateringStatus);    //配置引脚模式前先输出低电平防止电平跳变
  pinMode(WATERING_OPUPUT, OUTPUT);                 //配置引脚为输出模式
}

/**
 * @brief     浇花控制
 * @param     无
 * @return    无
 * @exception 无
 * */
void WateringCtrl(void)
{
  uint8_t Weekday_Conversion;                       //星期转换

  time_t time_FirstWatering_Start = mktime(&CtrlData.FirstWatering_Start);    //第一次浇花开始时间戳
  time_t time_FirstWatering_Stop = mktime(&CtrlData.FirstWatering_Stop);      //第一次浇花结束时间戳
  time_t time_SecondWatering_Start = mktime(&CtrlData.SecondWatering_Start);  //第二次浇花开始时间戳
  time_t time_SecondWatering_Stop = mktime(&CtrlData.SecondWatering_Stop);    //第二次浇花结束时间戳

  /* 转换存星期数组索引至星期结构体规则 */
  Weekday_Conversion = TimeData.tm_wday;
  if(Weekday_Conversion == 0)
  {
    Weekday_Conversion = 6;
  }
  else
  {
    Weekday_Conversion--;
  }

  /* 处于自动浇花定时模式且处于浇水天 */
  if(ModeSwitch == 0 && CtrlData.Watering_Model == 0 && CtrlData.WeekdayEnable[Weekday_Conversion] == 1)
  {
    /* 如果时间段被设定且正处于时间段 */
    if(
        (
          time_FirstWatering_Start != time_FirstWatering_Stop
          &&
          time_FirstWatering_Start <= NOW
          &&
          NOW < time_FirstWatering_Stop
        ) || (
          time_SecondWatering_Start != time_SecondWatering_Stop
          &&
          time_SecondWatering_Start <= NOW
          &&
          NOW < time_SecondWatering_Stop
        )
      )
    {
      WateringStatus = HIGH;
    }
    else
    {
      WateringStatus = LOW;
    }
  }
  /* 处于自动浇花传感器模式 */
  else if(ModeSwitch == 0 && CtrlData.Watering_Model == 1)
  {
    /* 浇花阈值限制已配置(理论上不存在浇花阈值限制未配置却处于传感器模式的状态) */
    if(CtrlData.SoilMoisture_LowerLimit != CtrlData.SoilMoisture_UpperLimit)
    {
      /* 土壤湿度到达浇花阈值下限 */
      if(SensorData.SoilMoisture <= CtrlData.SoilMoisture_LowerLimit)
      {
        WateringStatus = HIGH;
      }
      
      /* 土壤湿度到达浇花阈值上限 */
      if(CtrlData.SoilMoisture_UpperLimit <= SensorData.SoilMoisture)
      {
        WateringStatus = LOW;
      }
    }
  }

  digitalWrite(WATERING_OPUPUT, WateringStatus);    //驱动水泵浇花

  /* 当浇花状态变化时置浇花语音状态为播报(捕获边沿、过滤电平) */
  if(LastWateringStatus != WateringStatus)
  {
    VoiceStatus = 1;
  }
  else if(LastWateringStatus == WateringStatus)
  {
    VoiceStatus = 0;
  }

  /* 当浇花语音状态为播报时播报对应语音 */
  if(VoiceStatus == 1)
  {
    if(WateringStatus == HIGH)
    {
      VoiceModule_Play(WATERING);                   //播报语音:当前正在浇花
    }
    else
    {
      VoiceModule_Play(NOTWATERING);                //播报语音:当前未在浇花
    }
    VoiceStatus = 0;                                //复位浇花语音状态为不播报
  }
}