using System.Windows.Forms;
using CefSharp.WinForms;
using CefSharp;

namespace AutomaticWatering
{
    public partial class Form2 : Form
    {
        public Form2()
        {
            InitializeComponent();
        }

        public Form2(string Address, bool InitFlag)    //重载的带入参构造函数,创建窗体即为传入IP地址
        {
            InitializeComponent();

            //Cef.Initialize方法每个进程只能调用一次
            if (InitFlag == true)
            {
                CefSettings settings = new CefSettings();                           //创建Cef设置实例
                Cef.Initialize(settings);                                           //使用默认设置初始化浏览器
            }

            ChromiumWebBrowser chromeBrowser = new ChromiumWebBrowser(Address);     //使用传入IP地址创建浏览器实例
            this.Controls.Add(chromeBrowser);                                       //向窗体添加浏览器
            chromeBrowser.Dock = DockStyle.Fill;                                    //浏览器控件填充窗体
            BrowserSettings browserSettings = new BrowserSettings();                //创建Browser设置实例
            chromeBrowser.BrowserSettings = browserSettings;                        //浏览器使用默认设置
        }
    }
}