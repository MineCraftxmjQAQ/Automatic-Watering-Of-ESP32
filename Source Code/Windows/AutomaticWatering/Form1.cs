using System;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Windows.Forms;
using System.Text.RegularExpressions;

namespace AutomaticWatering
{
    public partial class Form1 : Form
    {
        UdpClient receiveUdpClient;                         //创建UDP客户端实例

        IPAddress ip = IPAddress.Parse("0.0.0.0");          //指定监听地址
        const int port = 1145;                              //指定监听端口

        bool InitFlag = true;                               //初次打开控制页面标识

        delegate void ShowMessageDelegate(ListBox listbox, string text);        //通过委托回调机制显示消息内容

        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)     //窗体加载即为接收开始
        {
            Thread ReceiveThread = new Thread(ReceiveMsg);  //接收广播包使用子线程,防止阻塞UI
            ReceiveThread.IsBackground = true;              //指示接收线程为后台线程
            ReceiveThread.Start();                          //启动线程,开始从0.0.0.0:1145端口监听广播
        }

        private void ReceiveMsg()
        {
            IPEndPoint local = new IPEndPoint(ip, port);    //用监听地址和监听端口初始化IPEndPoint类的新实例
            receiveUdpClient = new UdpClient(local);        //初始化一个UdpClient的新实例并绑定到本地
            while (true)
            {
                //使用try...catch...结构防止出现致命错误时程序终止
                try
                {
                    byte[] receiveBytes = receiveUdpClient.Receive(ref local);                                  //接收UDP广播数据包
                    string receivemsg = Encoding.UTF8.GetString(receiveBytes, 0, receiveBytes.Length);          //将字节数组格式化为UTF-8字符串
                    if (receivemsg.IndexOf("This is Automatic Watering Server") >= 0)                           //判断数据包是否来自目标服务端
                    {
                        Regex Server_IPAddress_Regex = new Regex(@"\b\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}\b");    //定义用于描述IPv4地址的正则表达式
                        MatchCollection MatchResult = Server_IPAddress_Regex.Matches(receivemsg);               //使用正则表达式提取IPv4地址
                        ShowMessage(listBox1, MatchResult[0].Groups[0].Value);                                  //使用消息向列表框发送地址
                    }
                }
                catch
                {
                    MessageBox.Show("UDP数据包接收错误,程序即将终止!", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    Application.Exit();
                }
            }
        }

        private void ShowMessage(ListBox listbox, string text)
        {
            if (listbox.InvokeRequired)
            {
                ShowMessageDelegate d = ShowMessage;
                listbox.Invoke(d, new object[] { listbox, text });             //委托发送
            }
            else if (listbox.FindString(text) < 0)                          //仅显示未列表的IP地址
            {
                listbox.Items.Add(text);
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            listBox1.Items.Clear();         //清空列表
        }

        private void button2_Click(object sender, EventArgs e)
        {
            if (InitFlag == true)
            {
                InitFlag = false;
            }

            //判断开启控制页面时文本框是否为空
            if (!string.IsNullOrEmpty(textBox1.Text))
            {
                if (textBox1.Text.IndexOf("http://") < 0 || textBox1.Text.IndexOf("https://") < 0)
                {
                    Form2 form2 = new Form2("http://" + textBox1.Text, InitFlag);       //为IP添加http前缀
                    form2.ShowDialog();     //以模式对话框方式显示控制页面,在控制页面打开的状态下主窗体不可操作
                }
            }
            else
            {
                MessageBox.Show("尚未键入需要控制服务端的IP地址", "提示", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }

        private void listBox1_SelectedIndexChanged(object sender, EventArgs e)  //列表框点击事件
        {
            if (listBox1.SelectedIndex > -1)
            {
                textBox1.Text = listBox1.SelectedItem.ToString();               //点击列表项时发送IP字符串至文本框
            }
        }
    }
}