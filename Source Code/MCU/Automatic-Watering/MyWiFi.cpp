#include "MyWiFi.h"

/* 配网网页元素 */
const char ROOT_HTML[] = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
	<title> 办公室盆栽自动浇水仪配网页面 </title>
	<meta name="viewport" content="width=device-width, initial-scale=1">
</head>
<style type="text/css">
	.input {
	    display: block;
	    margin-top: 10px;
	}

	.input span {
	    width: 100px;
	    float: left;
	    float: left;
	    height: 36px;
	    line-height: 36px;
	}

	.input input {
	    height: 30px;
	    width: 200px;
	}

	.btn {
	    width: 120px;
	    height: 35px;
	    background-color: #000000;
	    border: 0px;
	    color: #ffffff;
	    margin-top: 15px;
	    margin-left: 100px;
	}
</style>
<body>
	<form method="POST" action="configwifi"><label class="input"><span>WiFi SSID</span><input style="border-radius:10px" type="text" name="ssid" value=""></label><label class="input"><span>WiFi PASS</span><input style="border-radius:10px" type="text" name="pass"></label><input
			style="border-radius:10px" class="btn" type="submit" name="submit" value="Submit">
		<p><span>Nearby wifi:</span></p>
	</form>
</body>
</html>
)rawliteral";

/* 控制网页元素 */
const char CTRL_HTML[] = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
	<meta charset="UTF-8">
	<title> 办公室盆栽自动浇水仪控制页面 </title>
	<h1 style="text-align: center;"> 办公室盆栽自动浇水仪控制页面 </h1>
	<script>
		setInterval(function() {
		        var xhttp = new XMLHttpRequest();
		        xhttp.onreadystatechange = function() {
		            if (this.readyState == 4 && this.status == 200) {
		                document.getElementById("Controlboard_Data").innerHTML = this.responseText;
		            }
		        };
		        xhttp.open("GET", "/Controlboard_Data", true);
		        xhttp.send();
		    },
		    1000)
	</script>
</head>
<body>
	<h2 style="text-align: center;"> 控制板下行数据: </h2>
	<fieldset style="border-radius:10px">
		<div style="text-align: center;">
			<b id="Controlboard_Data">
			</b>
		</div>
	</fieldset>
	<h2 style="text-align: center;"> 控制板上行数据: </h2>
	<form style="text-align: center;" method="POST" action="ConfigCtrl">
		<fieldset style="border-radius:10px">
			<legend>选择自动浇花装置的工作模式</legend>
			<label>浇花模式:</label>
			<input type="radio" id="TimingModel" name="model" value="TimingModel" checked>
			<label>定时模式</label>
			<input type="radio" id="SensorModel" name="model" value="SensorModel">
			<label>传感器模式</label>
			<br>
		</fieldset>
		<fieldset style="border-radius:10px">
			<legend>选择一周的浇花天(定时模式)</legend>
			<div>
				<input type="checkbox" id="Monday" name="Monday" value="Monday">
				<label for="Monday">星期一</label>
				<input type="checkbox" id="Tuesday" name="Tuesday" value="Tuesday">
				<label for="Tuesday">星期二</label>
				<input type="checkbox" id="Wednesday" name="Wednesday" value="Wednesday">
				<label for="Wednesday">星期三</label>
			</div>
			<div>
				<input type="checkbox" id="Thursday" name="Thursday" value="Thursday">
				<label for="Thursday">星期四</label>
				<input type="checkbox" id="Friday" name="Friday" value="Friday">
				<label for="Friday">星期五</label>
				<input type="checkbox" id="Saturday" name="Saturday" value="Saturday">
				<label for="Saturday">星期六</label>
			</div>
			<input type="checkbox" id="Sunday" name="Sunday" value="Sunday">
			<label for="Sunday">星期日</label>
		</fieldset>
		<fieldset style="border-radius:10px">
			<legend>设定一天内浇花时段(定时模式)</legend>
			<label>第一次浇花:</label>
			<input type="time" name="startTime1" id="startTime1" style="border-radius:10px" />
			<br>
			<br>
			<label>浇花时长:</label>
			<input type="text" name="startTime1_Length" style="width:120px;border-radius:10px" oninput="value=value.replace(/[^\d]/g,'')">
			<label>S</label>
			<br>
			<br>
			<label>第二次浇花:</label>
			<input type="time" name="startTime2" id="startTime2" style="border-radius:10px" />
			<br>
			<br>
			<label>浇花时长:</label>
			<input type="text" name="startTime2_Length" style="width:120px;border-radius:10px" oninput="value=value.replace(/[^\d]/g,'')">
			<label>S</label>
		</fieldset>
		<fieldset style="border-radius:10px">
			<legend>设定浇花阈值限制(传感器模式)</legend>
			<label>土壤湿度下限</label>
			<input type="range" id="soilMoistureLower" name="soilMoistureLower" min="0" max="100" step="1" value="50" onchange="document.getElementById('soilMoistureLower_Show').innerHTML=value" />
			<span id="soilMoistureLower_Show">50</span>
			<span>%</span>
			<br>
			<br>
			<label>土壤湿度上限</label>
			<input type="range" id="soilMoistureUpper" name="soilMoistureUpper" min="0" max="100" step="1" value="70" onchange="document.getElementById('soilMoistureUpper_Show').innerHTML=value" />
			<span id="soilMoistureUpper_Show">70</span>
			<span>%</span>
		</fieldset>
		<br>
		<input type="submit" value="提交更改完成的设定" style="font-size:25px;width:250px;height:40px;border-radius:10px">
	</form>
</body>
</html>
)rawliteral";

/* 提交完成网页元素 */
const char COMMIT_HTML[] = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
	<meta charset="UTF-8" http-equiv="refresh" content="5;url=/">
	<title> 配置信息提交完成页面 </title>
	<h1 style="text-align: center;"> 控制板已接收并写入新的配置信息 <br>
		<br> 5秒后跳转回控制页面:)
	</h1>
</head>
</html>
)rawliteral";

const char* AP_SSID  = "Automatic-Watering_Config"; //控制板AP模式WiFi标识符
const char* HOST_NAME = "Automatic-Watering_ESP32"; //控制板主机名

const byte dnsPort = 53;                            //DNS端口号
const int webPort = 80;                             //Web端口号
const int udpPort = 1145;                           //UDP监听端口号

const char* ntpServer = "pool.ntp.org";             //网络时间服务器
const long gmtOffset_sec = 8 * 3600;                //设置时区为东八区

IPAddress apIP(192, 168, 1, 1);                     //AP模式控制板IP地址
IPAddress hostIP;                                   //控制板获取的IP地址

String scanNetworksID = "";                         //储存扫描到的WiFi标识符
String ssid = "";                                   //测试与出厂的WiFi标识符(根据需要填写)
String password = "";                               //测试与出厂的WiFi密码(根据需要填写)
String save_ssid;                                   //用于写入NVS的WiFi标识符
String save_password;                               //用于写入NVS的WiFi密码

DNSServer dnsServer;                                //创建DnsServer实例

WebServer webServer(webPort);                       //创建WebServer实例(80端口号可不输入直接访问)
String Controlboard_Data;                           //储存送往控制页面的数据

WiFiUDP MytUDP;                                     //创建用于接收广播的UDP实例
IPAddress udpIP(0, 0, 0, 255);                      //UDP广播地址(小型网络通常为C类IP地址,最后一字节为主机号)
char recvUDP[255];                                  //用于存储UDP客户端发送的数据

WiFiUDP ntpUDP;                                     //创建用于NTP校时的UDP实例
NTPClient timeClient(ntpUDP, ntpServer, gmtOffset_sec, 60000);    //创建NTP校时客户端实例
time_t NOW;                                         //控制板实时时间的时间戳
struct tm TimeData;                                 //控制板实时时间的时间结构体

nvs_handle wifi_nvs_handle;                         //NVS操作句柄

/**
 * @brief     网络连接初始化
 * @param     无
 * @return    布尔型 返回true表示WiFi连接成功,返回false表示WiFi连接超时
 * @exception 无
 * */
boolean Internet_Init(void)
{
  unsigned char Connect_Timeout = 0;                                                //超时计数

  String ssid_temp;                                                                 //临时WiFi标识符
  String password_temp;                                                             //临时WiFi密码

  char ssid_nvs_temp[33];                                                           //WiFi标识符长度范围0~32
  char password_nvs_temp[64];                                                       //AES和TKIP加密的WiFi密码最大长度63

  size_t ssid_size = 32;                                                            //WiFi标识符长度
  size_t password_size = 63;                                                        //WiFi密码长度

  esp_err_t ret = nvs_flash_init();                                                 //NVS初始化
  if(ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)      //当NVS不包含空页或NVS包含新格式的数据
  {
    nvs_flash_erase();                                                              //擦除NVS
    nvs_flash_init();                                                               //尝试再次初始化
  }

  if(nvs_open("WiFi_Config", NVS_READWRITE, &wifi_nvs_handle) == ESP_OK)            //打开命名空间"WiFi_Config"
  {                                
    nvs_get_str(wifi_nvs_handle, "ssid", ssid_nvs_temp, &ssid_size);                //获取当前命名空间中的键名为"ssid"的值
    nvs_get_str(wifi_nvs_handle, "password", password_nvs_temp, &password_size);    //获取当前命名空间中的键名为"password"的值

    ssid_temp = ssid_nvs_temp;
    password_temp = password_nvs_temp;

    if(ssid_temp.isEmpty() == false && password_temp.isEmpty() == false)            //如果存储的ssid和password均不为空,则使用其尝试连接WiFi,否则使用默认ssid和password
    {
      ssid = ssid_temp;
      password = password_temp;
    }
    nvs_close(wifi_nvs_handle);                                                     //关闭命名空间"WiFi_Config"
  }

  WiFi.begin(ssid, password);                                                       //开始连接WiFi

  while(WiFi.status() != WL_CONNECTED)                                              //WiFi连接状态不为WL_CONNECTED(已连接)时
  {
    delay(1000);
    Connect_Timeout++;

    if(Connect_Timeout > 9)
    {
      VoiceModule_Play(WIFICONNECTIONFAILED);                                       //播报语音:WiFi连接超时,正在尝试重连
      return false;                                                                 //一秒检查一次网络连接状态,10秒未成功连接则判定超时
    }
  }

  if(WiFi.status() == WL_CONNECTED)                                                 //WiFi连接状态为WL_CONNECTED(已连接)
  {
    hostIP = WiFi.localIP();
    return true;
  }
  return false;
}

/**
 * @brief     AP配网页面发送扫描的附近WiFi
 * @param     无
 * @return    无
 * @exception 无
 * */
void Handle_Root(void)
{
  if(webServer.hasArg("selectSSID"))
  {
    webServer.send(200, "text/html", ROOT_HTML + scanNetworksID + "</body></html>");
  }
  else
  {
    webServer.send(200, "text/html", ROOT_HTML + scanNetworksID + "</body></html>");
  }
}

/**
 * @brief     提交报表响应(配网网页)
 * @param     无
 * @return    无
 * @exception 无
 * */
void Handle_Config(void)
{
  if(webServer.hasArg("ssid"))
  {
    ssid = webServer.arg("ssid");
    save_ssid = ssid;                     //在WiFi_Connect函数中可能会清除ssid的值,因此在从网页获取报表数据后立即转存
  }
  else
  {
    webServer.send(200, "text/html", "<meta charset='UTF-8'>error, not found ssid");
    return;
  }

  if(webServer.hasArg("pass"))
  {
    password = webServer.arg("pass");
    save_password = password;             //在WiFi_Connect函数中可能会清除password的值,因此在从网页获取报表数据后立即转存
  }
  else
  {
    webServer.send(200, "text/html", "<meta charset='UTF-8'>error, not found password");
    return;
  }

  /* 提交表单后的响应 */
  webServer.send(200, "text/html", "<meta charset='UTF-8'>SSID:" + ssid + "<br />password:" + password + "<br />已获得WiFi信息,正在尝试连接......");
  delay(2000);                            //安卓系统自带的登录界面在WiFi连接丢失后会立即关闭登录界面,此处延时2s使用户得以看清
  WiFi.softAPdisconnect(true);            //关闭AP模式
  webServer.close();                      //关闭服务器
  WiFi.softAPdisconnect();                //空参调用,清除AP模式的网络名和密码

  if(WiFi.status() != WL_CONNECTED)
  {
    WiFi_Connect();
  }
}

/**
 * @brief     处理DNS请求和检查来自客户端的HTTP请求
 * @param     无
 * @return    无
 * @exception 无
 * */
void Check_Request(void)
{
  dnsServer.processNextRequest();
  webServer.handleClient();
}

/**
 * @brief     进入AP模式
 * @param     无
 * @return    无
 * @exception 无
 * */
void APMode_Init(void)
{
  WiFi.mode(WIFI_AP);                                           //设置WiFi为AP模式       
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));   //配置AP主机IP及子网掩码

  if(WiFi.softAP(AP_SSID) == 0)                                 //无法以AP模式启动热点则重启esp32
  {
    ESP.restart();
  }
}

/**
 * @brief     开启DNS服务器
 * @param     无
 * @return    无
 * @exception 无
 * */
void DNS_Init(void)
{
  if(dnsServer.start(dnsPort, "*", apIP) == 0)          //无法启动DNS服务器则重启esp32
  {
    ESP.restart();
  }
}

/**
 * @brief     初始化DNS服务器
 * @param     无
 * @return    无
 * @exception 无
 * */
void WebServer_Init(void)
{
  webServer.on("/", HTTP_GET, Handle_Root);             //设置配网网页根目录回调函数
  webServer.on("/configwifi", HTTP_POST, Handle_Config);//设置配网网页上行数据回调函数
  webServer.onNotFound(Handle_Root);                    //设置服务器收到HTTP请求无效地址的回调函数

  webServer.begin();                                    //初始化HTTP服务器
}

/**
 * @brief     扫描附近WiFi并显示
 * @param     无
 * @return    布尔型 返回true表示已扫描到WiFi,返回false表示未扫描到WiFi
 * @exception 无
 * */
boolean WiFi_Scan(void)
{
  int n = WiFi.scanNetworks();

  if(n == 0)
  {
    scanNetworksID = "no networks found";
    return false;
  }
  else
  {
    for(int i = 0; i < n; ++i)
    {
      scanNetworksID += "<P>" + WiFi.SSID(i) + "</P>";
      delay(10);
    }
    return true;
  }
}

/**
 * @brief     连接WiFi
 * @param     无
 * @return    无
 * @exception 无
 * */
void WiFi_Connect(void)
{
  unsigned char Connect_Timeout = 0;                //超时计数

  WiFi.hostname(HOST_NAME);                         //设置控制板主机名
  WiFi.mode(WIFI_STA);                              //切换至STA模式
  WiFi.setSleep(false);                             //关闭WiFi休眠以提高响应速度

  if(ssid != "")                                    //如果WiFi标识符不为空(可以没有密码)
  {
    WiFi.begin(ssid.c_str(), password.c_str());
    ssid = "";
    password = "";
  }
  else                                              //WiFi标识符为空则空参调用,尝试使用最后一次正常连接的参数连接WiFi
  {
    WiFi.begin();
  }

  while(WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Connect_Timeout++;

    if(Connect_Timeout > 9)                         //一秒检查一次网络连接状态,10秒未成功连接则判定超时
    {
      VoiceModule_Play(DISTRIBUTIONNETWORK);        //播报语音:语音:WiFi重连超时,控制板已切换至AP模式,请连接控制板热点执行配网流程
      APMode_Init();                                //初始化AP模式
      DNS_Init();                                   //初始化DNS
      WebServer_Init();                             //初始化网络服务器
      WiFi_Scan();                                  //扫描附近WiFi
      return;                                       //判断超时后退出循环,等待网页填写报表
    }
  }

  if(WiFi.status() == WL_CONNECTED)
  {
    webServer.stop();                               //终止服务器
  }
}

/**
 * @brief     检查WiFi是否连接
 * @param     布尔型 传入true表示需要重连,传入false表示不需要重连
 * @return    布尔型 返回true表示WiFi连接成功,返回false表示WiFi连接失败
 * @exception 无
 * */
boolean Check_Connect(boolean reConnect)
{
  if(WiFi.status() != WL_CONNECTED)                                               //WiFi连接失败
  {
    if(reConnect == true && WiFi.getMode() != WIFI_AP && WiFi.getMode() != WIFI_AP_STA) 
    {
      return false;
    }
  }
  else                                                                            //WiFi连接成功
  {
    hostIP = WiFi.localIP();

    esp_err_t ret = nvs_flash_init();
    if(ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
      nvs_flash_erase();                                                          //擦除NVS
      nvs_flash_init();                                                           //重新尝试初始化
    }

    if(nvs_open("WiFi_Config", NVS_READWRITE, &wifi_nvs_handle) == ESP_OK)        //打开命名空间"WiFi_Config"
    {
      nvs_set_str(wifi_nvs_handle, "ssid", save_ssid.c_str());                    //写入当前命名空间中的键名为"ssid"的值
      nvs_set_str(wifi_nvs_handle, "password", save_password.c_str());            //写入当前命名空间中的键名为"password"的值
      nvs_commit(wifi_nvs_handle);                                                //提交修改
      nvs_close(wifi_nvs_handle);                                                 //关闭命名空间"WiFi_Config"
      return true;
    }
  }
  return false;
}

/**
 * @brief     UDP通讯初始化
 * @param     无
 * @return    无
 * @exception 无
 * */
void UDP_Init(void)
{
  MytUDP.begin(udpPort);
}

/**
 * @brief     UDP通讯广播数据包
 * @param     无
 * @return    无
 * @exception 无
 * */
void UDP_BoadcastPacket(void)
{
  MytUDP.beginPacket(hostIP | udpIP, 1145);                                       //准备发送数据到直接广播地址的目标端口
  MytUDP.print("This is Automatic Watering Server:" + hostIP.toString());         //将自身身份识别与自身IP地址放入发送的缓冲区
  MytUDP.endPacket();                                                             //广播UDP数据包
}

/**
 * @brief     UDP通讯接收数据包
 * @param     无
 * @return    布尔型 返回true表示已接收到UDP数据包,返回false表示未接收到UDP数据包
 * @exception 无
 * */
boolean UDP_RecvPacket(void)
{
  int UDPReceiveData_Length = MytUDP.parsePacket();

  if(UDPReceiveData_Length)                                                       //获取UDP接收数据包长度
  {
    int temp = MytUDP.read(recvUDP, 255);                                         //读取数据,将数据保存在recvUDP数组中

    if(temp > 0)
    {
      recvUDP[temp] = 0;
    }
    return true;
  }
  return false;
}

/**
 * @brief     UDP通讯终止
 * @param     无
 * @return    无
 * @exception 无
 * */
void UDP_Stop(void)
{
  MytUDP.stop();
}

/**
 * @brief     下行数据字符串格式化
 * @param     无
 * @return    无
 * @exception 无
 * */
void Controlboard_Data_Formatting(void)
{
  String ShowStr;                                                 //定义显示字符串

  time_t compare_time_start;                                      //比较时间戳(开始)
  time_t compare_time_stop;                                       //比较时间戳(结束)

  char format_time[20];                                           //定义格式化时间字符数组
  strftime(format_time, 20, "%Y/%m/%e %H:%M:%S", &TimeData);      //格式化时间字符串

  /* ----------实时数据段落---------- */
  Controlboard_Data = "<p>";
  //控制板时间
  Controlboard_Data += "<b>控制板时间:</b>";
  Controlboard_Data += String(format_time);
  Controlboard_Data += "<br>";
  //控制板IP地址
  Controlboard_Data += "<b>控制板IP地址:</b>";
  Controlboard_Data += hostIP.toString();
  Controlboard_Data += "<br>";
  //室内温度
  Controlboard_Data += "<b>室内温度:</b>";
  Controlboard_Data += String(SensorData.AmbientTemp);
  Controlboard_Data += "℃";
  Controlboard_Data += "<br>";
  //室内湿度
  Controlboard_Data += "<b>室内湿度:</b>";
  Controlboard_Data += String(SensorData.AmbientHumi);
  Controlboard_Data += "%";
  Controlboard_Data += "<br>";
  //土壤湿度
  Controlboard_Data += "<b>土壤湿度:</b>";
  Controlboard_Data += String(SensorData.SoilMoisture);
  Controlboard_Data += "%";
  Controlboard_Data += "</p>";

  /* ----------浇花配置数据段落---------- */
  Controlboard_Data += "<p>";
  //当前浇花模式
  Controlboard_Data += "<b>当前浇花模式:</b>";
  if(ModeSwitch == 0 && CtrlData.Watering_Model == 0)
  {
    ShowStr = "自动浇花定时模式";
  }
  else if(ModeSwitch == 0 && CtrlData.Watering_Model == 1)
  {
    ShowStr = "自动浇花传感器模式";
  }
  else
  {
    ShowStr = "手动浇花模式";
  }
  Controlboard_Data +=  ShowStr;
  Controlboard_Data += "<br>";
  //当前一周的浇花天
  Controlboard_Data += "<b>当前一周的浇花天:</b>";
  ShowStr = "";
  for(uint8_t i = 0; i < 7; i++)
  {
    if(CtrlData.WeekdayEnable[i] == 0)
    {
      ShowStr += " " + String(i + 1) + " ";
    }
    else
    {
      ShowStr += "[" + String(i + 1) + "]";
    }
  }
  Controlboard_Data +=  ShowStr;
  Controlboard_Data += "<br>";
  //一天内第一次浇花
  Controlboard_Data += "<b>一天内第一次浇花:</b>";
  compare_time_start = mktime(&CtrlData.FirstWatering_Start);
  compare_time_stop = mktime(&CtrlData.FirstWatering_Stop);
  if(compare_time_start == compare_time_stop)
  {
    ShowStr = "该浇花时间段未设定!";
  }
  else
  {
    strftime(format_time, 9, "%H:%M:%S", &CtrlData.FirstWatering_Start);
    ShowStr = String(format_time) + " --> ";
    strftime(format_time, 9, "%H:%M:%S", &CtrlData.FirstWatering_Stop);
    ShowStr += String(format_time);
  }
  Controlboard_Data +=  ShowStr;
  Controlboard_Data += "<br>";
  //一天内第一次浇花
  Controlboard_Data += "<b>一天内第二次浇花:</b>";
  compare_time_start = mktime(&CtrlData.SecondWatering_Start);
  compare_time_stop = mktime(&CtrlData.SecondWatering_Stop);
  if(compare_time_start == compare_time_stop)
  {
    ShowStr = "该浇花时间段未设定!";
  }
  else
  {
    strftime(format_time, 9, "%H:%M:%S", &CtrlData.SecondWatering_Start);
    ShowStr = String(format_time) + " --> ";
    strftime(format_time, 9, "%H:%M:%S", &CtrlData.SecondWatering_Stop);
    ShowStr += String(format_time);
  }
  Controlboard_Data +=  ShowStr;
  Controlboard_Data += "<br>";
  //浇花阈值限制
  Controlboard_Data += "<b>浇花阈值限制:</b>";
  if(CtrlData.SoilMoisture_LowerLimit == CtrlData.SoilMoisture_UpperLimit)
  {
    ShowStr = "浇花阈值限制未设定!";
  }
  else
  {
    ShowStr = String(CtrlData.SoilMoisture_LowerLimit) + "%";
    ShowStr += "<--->";
    ShowStr += String(CtrlData.SoilMoisture_UpperLimit) + "%";
  }
  Controlboard_Data +=  ShowStr;
  Controlboard_Data += "<br>";
  //当前浇花状态
  Controlboard_Data += "<b>当前浇花状态:</b>";
  if(WateringStatus == LOW)
  {
    ShowStr = "未在浇花";
  }
  else
  {
    ShowStr = "正在浇花";
  }
  Controlboard_Data +=  ShowStr;
  Controlboard_Data += "</p>";
}

/**
 * @brief     网页控制服务器回调函数(发送HTML页面内容)
 * @param     无
 * @return    无
 * @exception 无
 * */
void Control_WebServer_Callback(void)
{
  webServer.send(200, "text/html", CTRL_HTML);            //向客户端发送HTML页面内容
}

/**
 * @brief     网页控制服务器回调函数(发送控制板数据)
 * @param     无
 * @return    无
 * @exception 无
 * */
void Control_WebServer_SendData_Callback(void)
{
  webServer.send(200, "text/plain", Controlboard_Data);   //向客户端发送控制板数据
}

/**
 * @brief     提交报表响应(控制网页)
 * @param     无
 * @return    无
 * @exception 无
 * */
void Ctrl_Config(void)
{
  String format_time;                           //用于存储格式化时间字符串
  time_t target_time;                           //用以存储转换目标的时间戳

  time_t time_FirstWatering_Start;              //第一次浇花开始时间戳
  time_t time_FirstWatering_Stop;               //第一次浇花结束时间戳
  time_t time_SecondWatering_Start;             //第二次浇花开始时间戳
  time_t time_SecondWatering_Stop;              //第二次浇花结束时间戳

  /* 星期全部重置为未选中 */
  CtrlData.WeekdayEnable[0] = 0;
  CtrlData.WeekdayEnable[1] = 0;
  CtrlData.WeekdayEnable[2] = 0;
  CtrlData.WeekdayEnable[3] = 0;
  CtrlData.WeekdayEnable[4] = 0;
  CtrlData.WeekdayEnable[5] = 0;
  CtrlData.WeekdayEnable[6] = 0;

  /* 设定第一次浇花起止的年月日为更新当天 */
  CtrlData.FirstWatering_Start = *localtime(&NOW);
  CtrlData.FirstWatering_Stop = *localtime(&NOW);

  /* 设定第二次浇花起止的年月日为更新当天 */
  CtrlData.SecondWatering_Start = *localtime(&NOW);
  CtrlData.SecondWatering_Stop = *localtime(&NOW);

  /* 设定第一次浇花起止的时分秒为全0 */
  CtrlData.FirstWatering_Start.tm_hour = 0;
  CtrlData.FirstWatering_Start.tm_min = 0;
  CtrlData.FirstWatering_Start.tm_sec = 0;

  /* 设定第二次浇花起止的时分秒为全0 */
  CtrlData.SecondWatering_Start.tm_hour = 0;
  CtrlData.SecondWatering_Start.tm_min = 0;
  CtrlData.SecondWatering_Start.tm_sec = 0;

  if(webServer.hasArg("model"))                 //判断设置的浇水模式
  {
    if(webServer.arg("model").compareTo("TimingModel") == 0)
    {
      CtrlData.Watering_Model = 0;
    }
    else
    {
      CtrlData.Watering_Model = 1;
    }
  }

  if(webServer.hasArg("Monday"))                //若星期一选中则更改对应结构体数据
  {
    if(webServer.arg("Monday").compareTo("Monday") == 0)
    {
      CtrlData.WeekdayEnable[0] = 1;
    }
  }

  if(webServer.hasArg("Tuesday"))               //若星期二选中则更改对应结构体数据
  {
    if(webServer.arg("Tuesday").compareTo("Tuesday") == 0)
    {
      CtrlData.WeekdayEnable[1] = 1;
    }
  }

  if(webServer.hasArg("Wednesday"))             //若星期三选中则更改对应结构体数据
  {
    if(webServer.arg("Wednesday").compareTo("Wednesday") == 0)
    {
      CtrlData.WeekdayEnable[2] = 1;
    }
  }

  if(webServer.hasArg("Thursday"))              //若星期四选中则更改对应结构体数据
  {
    if(webServer.arg("Thursday").compareTo("Thursday") == 0)
    {
      CtrlData.WeekdayEnable[3] = 1;
    }
  }

  if(webServer.hasArg("Friday"))                //若星期五选中则更改对应结构体数据
  {
    if(webServer.arg("Friday").compareTo("Friday") == 0)
    {
      CtrlData.WeekdayEnable[4] = 1;
    }
  }

  if(webServer.hasArg("Saturday"))              //若星期六选中则更改对应结构体数据
  {
    if(webServer.arg("Saturday").compareTo("Saturday") == 0)
    {
      CtrlData.WeekdayEnable[5] = 1;
    }
  }

  if(webServer.hasArg("Sunday"))                //若星期日选中则更改对应结构体数据
  {
    if(webServer.arg("Sunday").compareTo("Sunday") == 0)
    {
      CtrlData.WeekdayEnable[6] = 1;
    }
  }

  if(webServer.hasArg("startTime1"))            //保存一天内第一次浇花开始时间
  {
    format_time = webServer.arg("startTime1");
    strptime(format_time.c_str(), "%H:%M", &CtrlData.FirstWatering_Start);
  }

  if(webServer.hasArg("startTime1_Length"))     //保存一天内第一次浇花结束时间(开始时间 + 浇花时长)
  {
    target_time = mktime(&CtrlData.FirstWatering_Start) + webServer.arg("startTime1_Length").toInt();
    CtrlData.FirstWatering_Stop = *localtime(&target_time);

    /* 如果第一次浇花结束时间达到第二天,强制设置结束时间为23:59:59 */
    if(CtrlData.FirstWatering_Start.tm_mday != CtrlData.FirstWatering_Stop.tm_mday)
    {
      CtrlData.FirstWatering_Stop.tm_mday = CtrlData.FirstWatering_Start.tm_mday;
      CtrlData.FirstWatering_Stop.tm_hour = 23;
      CtrlData.FirstWatering_Stop.tm_min  = 59;
      CtrlData.FirstWatering_Stop.tm_sec  = 59;
    }
  }

  if(webServer.hasArg("startTime2"))            //保存一天内第二次浇花开始时间
  {
    format_time = webServer.arg("startTime2");
    strptime(format_time.c_str(), "%H:%M", &CtrlData.SecondWatering_Start);
  }

  if(webServer.hasArg("startTime2_Length"))     //保存一天内第二次浇花结束时间(开始时间 + 浇花时长)
  {
    target_time = mktime(&CtrlData.SecondWatering_Start) + webServer.arg("startTime2_Length").toInt();
    CtrlData.SecondWatering_Stop = *localtime(&target_time);

    /* 如果第二次浇花结束时间达到第二天,强制设置结束时间为23:59:59 */
    if(CtrlData.SecondWatering_Start.tm_mday != CtrlData.SecondWatering_Stop.tm_mday)
    {
      CtrlData.SecondWatering_Stop.tm_mday = CtrlData.SecondWatering_Start.tm_mday;
      CtrlData.SecondWatering_Stop.tm_hour = 23;
      CtrlData.SecondWatering_Stop.tm_min  = 59;
      CtrlData.SecondWatering_Stop.tm_sec  = 59;
    }
  }

  time_FirstWatering_Start = mktime(&CtrlData.FirstWatering_Start);
  time_FirstWatering_Stop = mktime(&CtrlData.FirstWatering_Stop);
  time_SecondWatering_Start = mktime(&CtrlData.SecondWatering_Start);
  time_SecondWatering_Stop = mktime(&CtrlData.SecondWatering_Stop);

  /* 以下多分支用于处理两个时间段为0或重叠的情况 */
  /* 两个浇花时间段均不为0 */
  if(time_FirstWatering_Start != time_FirstWatering_Stop
  && time_SecondWatering_Start != time_SecondWatering_Stop)
  {
    /* 第二次浇花时间段被包裹在第一次浇花时间段 */
    if(time_FirstWatering_Start <= time_SecondWatering_Start
    && time_SecondWatering_Stop <= time_FirstWatering_Stop)
    {
      CtrlData.SecondWatering_Start.tm_hour = 0;
      CtrlData.SecondWatering_Start.tm_min = 0;
      CtrlData.SecondWatering_Start.tm_sec = 0;
      CtrlData.SecondWatering_Stop.tm_hour = 0;
      CtrlData.SecondWatering_Stop.tm_min = 0;
      CtrlData.SecondWatering_Stop.tm_sec = 0;
    }
    /* 第一次浇花时间段被包裹在第二次浇花时间段 */
    else if(time_SecondWatering_Start <= time_FirstWatering_Start
    && time_FirstWatering_Stop <= time_SecondWatering_Stop)
    {
      CtrlData.FirstWatering_Start = CtrlData.SecondWatering_Start;
      CtrlData.FirstWatering_Stop = CtrlData.SecondWatering_Stop;

      CtrlData.SecondWatering_Start.tm_hour = 0;
      CtrlData.SecondWatering_Start.tm_min = 0;
      CtrlData.SecondWatering_Start.tm_sec = 0;
      CtrlData.SecondWatering_Stop.tm_hour = 0;
      CtrlData.SecondWatering_Stop.tm_min = 0;
      CtrlData.SecondWatering_Stop.tm_sec = 0;
    }
    /* 第一次浇花时间段超前且不包裹第二次浇花时间段 */
    else if(time_FirstWatering_Start <= time_SecondWatering_Start
    && time_SecondWatering_Start <= time_FirstWatering_Stop
    && time_FirstWatering_Stop <= time_SecondWatering_Stop)
    {
      CtrlData.FirstWatering_Stop = CtrlData.SecondWatering_Stop;

      CtrlData.SecondWatering_Start.tm_hour = 0;
      CtrlData.SecondWatering_Start.tm_min = 0;
      CtrlData.SecondWatering_Start.tm_sec = 0;
      CtrlData.SecondWatering_Stop.tm_hour = 0;
      CtrlData.SecondWatering_Stop.tm_min = 0;
      CtrlData.SecondWatering_Stop.tm_sec = 0;
    }
    /* 第一次浇花时间段落后且不包裹第二次浇花时间段 */
    else if(time_SecondWatering_Start <= time_FirstWatering_Start
    && time_FirstWatering_Start <= time_SecondWatering_Stop
    && time_SecondWatering_Stop <= time_FirstWatering_Stop)
    {
      CtrlData.FirstWatering_Start = CtrlData.SecondWatering_Start;

      CtrlData.SecondWatering_Start.tm_hour = 0;
      CtrlData.SecondWatering_Start.tm_min = 0;
      CtrlData.SecondWatering_Start.tm_sec = 0;
      CtrlData.SecondWatering_Stop.tm_hour = 0;
      CtrlData.SecondWatering_Stop.tm_min = 0;
      CtrlData.SecondWatering_Stop.tm_sec = 0;
    }
  }
  /* 第一次浇花时长为0,第二次浇花时长不为0 */
  else if(time_FirstWatering_Start == time_FirstWatering_Stop && time_SecondWatering_Start != time_SecondWatering_Stop)
  {
    CtrlData.FirstWatering_Start = CtrlData.SecondWatering_Start;
    CtrlData.FirstWatering_Stop = CtrlData.SecondWatering_Stop;

    CtrlData.SecondWatering_Start.tm_hour = 0;
    CtrlData.SecondWatering_Start.tm_min = 0;
    CtrlData.SecondWatering_Start.tm_sec = 0;
    CtrlData.SecondWatering_Stop.tm_hour = 0;
    CtrlData.SecondWatering_Stop.tm_min = 0;
    CtrlData.SecondWatering_Stop.tm_sec = 0;
  }

  /* 第一次浇花时长为0时设定起止为0时0分0秒 */
  if(time_FirstWatering_Start == time_FirstWatering_Stop)
  {
    CtrlData.FirstWatering_Start.tm_hour = 0;
    CtrlData.FirstWatering_Start.tm_min = 0;
    CtrlData.FirstWatering_Start.tm_sec = 0;
    CtrlData.FirstWatering_Stop.tm_hour = 0;
    CtrlData.FirstWatering_Stop.tm_min = 0;
    CtrlData.FirstWatering_Stop.tm_sec = 0;
  }

  /* 第二次浇花时长为0时设定起止为0时0分0秒 */
  if(time_SecondWatering_Start == time_SecondWatering_Stop)
  {
    CtrlData.SecondWatering_Start.tm_hour = 0;
    CtrlData.SecondWatering_Start.tm_min = 0;
    CtrlData.SecondWatering_Start.tm_sec = 0;
    CtrlData.SecondWatering_Stop.tm_hour = 0;
    CtrlData.SecondWatering_Stop.tm_min = 0;
    CtrlData.SecondWatering_Stop.tm_sec = 0;
  }

  if(webServer.hasArg("soilMoistureLower"))     //保存传感器模式浇花湿度下限
  {
    CtrlData.SoilMoisture_LowerLimit = webServer.arg("soilMoistureLower").toInt();
  }

  if(webServer.hasArg("soilMoistureUpper"))     //保存传感器模式浇花湿度上限
  {
    CtrlData.SoilMoisture_UpperLimit = webServer.arg("soilMoistureUpper").toInt();

    /* 如果下限湿度大于上限湿度则交换上下限数值 */
    if(CtrlData.SoilMoisture_LowerLimit > CtrlData.SoilMoisture_UpperLimit)
    {
      CtrlData.SoilMoisture_LowerLimit = CtrlData.SoilMoisture_LowerLimit + CtrlData.SoilMoisture_UpperLimit;
      CtrlData.SoilMoisture_UpperLimit = CtrlData.SoilMoisture_LowerLimit - CtrlData.SoilMoisture_UpperLimit;
      CtrlData.SoilMoisture_LowerLimit = CtrlData.SoilMoisture_LowerLimit - CtrlData.SoilMoisture_UpperLimit;
    }

    /* 如果上下限湿度差值小于10% */
    if(CtrlData.SoilMoisture_UpperLimit - CtrlData.SoilMoisture_LowerLimit < 10)
    {
      CtrlData.SoilMoisture_UpperLimit = CtrlData.SoilMoisture_LowerLimit + 10;
    }

    /* 如果上限湿度值大于100% */
    if(CtrlData.SoilMoisture_UpperLimit > 100)
    {
      CtrlData.SoilMoisture_LowerLimit = 90;
      CtrlData.SoilMoisture_UpperLimit = 100;
    }
  }

  Save_CtrlData();                              //转存结构体至控制板Flash上,防止掉电数据丢失
  Read_CtrlData();                              //从控制板Flash读取至结构体覆盖旧数据

  VoiceModule_Play(CONFIGURATIONUPDATED);       //播报语音:已保存并应用新的浇花配置

  webServer.send(200, "text/html", COMMIT_HTML);//发送数据接收确认页面
}

/**
 * @brief     网页控制服务器初始化
 * @param     无
 * @return    无
 * @exception 无
 * */
void Control_WebServer_Init(void)
{
  webServer.removeRoute("/", HTTP_GET);                                             //移除配网网页根目录路径

  webServer.on("/", Control_WebServer_Callback);                                    //设置控制网页根目录回调函数
  webServer.on("/Controlboard_Data", HTTP_GET, Control_WebServer_SendData_Callback);//设置控制网页下行数据回调函数
  webServer.on("/ConfigCtrl", Ctrl_Config);                                         //设置控制网页上行数据回调函数(需要上下行因而未填写HTTP方法)
  webServer.onNotFound(Control_WebServer_Callback);                                 //设置服务器收到HTTP请求无效地址的回调函数

  webServer.begin();                                                                //初始化HTTP服务器
}

/**
 * @brief     检查来自客户端的HTTP请求
 * @param     无
 * @return    无
 * @exception 无
 * */
void Check_ClientRequest(void)
{
  webServer.handleClient();
}

/**
 * @brief     时间校准初始化
 * @param     无
 * @return    无
 * @exception 无
 * */
void Time_Init(void)
{
  timeClient.begin();                                               //使用NTP校时客户端实例启动校时服务
}

/**
 * @brief     联网校准时间
 * @param     无
 * @return    无
 * @exception 无
 * */
void Time_Aligned(void)
{
  timeClient.update();                                              //更新NTP客户端
  NOW = timeClient.getEpochTime();                                  //更新控制板计时时间戳
  TimeData = *localtime(&NOW);                                      //更新控制板计时时间结构体

  /* 如果实时时间的日期与浇花时间段的日期不一致则进行更正 */
  if(
      TimeData.tm_year != CtrlData.FirstWatering_Start.tm_year      //判断实时时间与浇花时间段的年份是否一致
      ||
      TimeData.tm_yday != CtrlData.FirstWatering_Start.tm_yday      //判断实时时间与浇花时间段的位于该年份的天数是否一致
    )
  {
    CtrlData.FirstWatering_Start.tm_mday = TimeData.tm_mday;
    CtrlData.FirstWatering_Start.tm_mon = TimeData.tm_mon;
    CtrlData.FirstWatering_Start.tm_year = TimeData.tm_year;
    CtrlData.FirstWatering_Start.tm_wday = TimeData.tm_wday;
    CtrlData.FirstWatering_Start.tm_yday = TimeData.tm_yday;
    CtrlData.FirstWatering_Start.tm_isdst = TimeData.tm_isdst;

    CtrlData.FirstWatering_Stop.tm_mday = TimeData.tm_mday;
    CtrlData.FirstWatering_Stop.tm_mon = TimeData.tm_mon;
    CtrlData.FirstWatering_Stop.tm_year = TimeData.tm_year;
    CtrlData.FirstWatering_Stop.tm_wday = TimeData.tm_wday;
    CtrlData.FirstWatering_Stop.tm_yday = TimeData.tm_yday;
    CtrlData.FirstWatering_Stop.tm_isdst = TimeData.tm_isdst;

    CtrlData.SecondWatering_Start.tm_mday = TimeData.tm_mday;
    CtrlData.SecondWatering_Start.tm_mon = TimeData.tm_mon;
    CtrlData.SecondWatering_Start.tm_year = TimeData.tm_year;
    CtrlData.SecondWatering_Start.tm_wday = TimeData.tm_wday;
    CtrlData.SecondWatering_Start.tm_yday = TimeData.tm_yday;
    CtrlData.SecondWatering_Start.tm_isdst = TimeData.tm_isdst;

    CtrlData.SecondWatering_Stop.tm_mday = TimeData.tm_mday;
    CtrlData.SecondWatering_Stop.tm_mon = TimeData.tm_mon;
    CtrlData.SecondWatering_Stop.tm_year = TimeData.tm_year;
    CtrlData.SecondWatering_Stop.tm_wday = TimeData.tm_wday;
    CtrlData.SecondWatering_Stop.tm_yday = TimeData.tm_yday;
    CtrlData.SecondWatering_Stop.tm_isdst = TimeData.tm_isdst;
  }
}