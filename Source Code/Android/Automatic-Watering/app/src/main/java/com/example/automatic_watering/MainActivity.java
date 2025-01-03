package com.example.automatic_watering;

import static android.view.View.GONE;
import static android.view.View.VISIBLE;

import android.content.Context;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;
import android.view.View;
import android.webkit.RenderProcessGoneDetail;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Spinner;
import android.widget.Toast;

import androidx.activity.EdgeToEdge;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.graphics.Insets;
import androidx.core.view.ViewCompat;
import androidx.core.view.WindowInsetsCompat;

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.util.ArrayList;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class MainActivity extends AppCompatActivity {
    private EditText show;         //创建输入框(仅展示)实例
    private EditText input;         //创建输入框实例
    private Spinner ipSpinner;      //创建列表选择框实例
    private Button enter;           //创建按钮(打开控制页面)实例
    private Button clear;           //创建按钮(清空接收列表)实例
    private WebView ctrl;           //创建网页浏览器实例
    ArrayList<String> final_serverIPAddress = new ArrayList<>();    //创建用于接收IP地址的数组(字符串泛型)

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        EdgeToEdge.enable(this);
        setContentView(R.layout.activity_main);
        ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.main), (v, insets) -> {
            Insets systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars());
            v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom);
            return insets;
        });
        initView();     //UI控件初始化
        WifiManager manager = (WifiManager) this.getSystemService(Context.WIFI_SERVICE);
        WifiManager.MulticastLock lock = manager.createMulticastLock("unLock");     //解开设备组播锁
        lock.acquire();     //锁定多播状态
        udpReceiveMsg();    //启动UDP广播监听

        //设置列表选择框监听事件(列表选择事件)
        ipSpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                input.setText(ipSpinner.getSelectedItem().toString());      //选中列表项时,将对应项值发送至输入框
            }

            @Override
            public void onNothingSelected(AdapterView<?> parent) {
                input.setText("");      //未选中时输入框空值
            }
        });

        enter.setOnClickListener(this::enterWeb);       //设置按钮点击监听事件(打开控制页面)
        clear.setOnClickListener(this::clearList);      //设置按钮点击监听事件(清空接收列表)
    }

    //UI控件绑定与初始化方法
    private void initView() {
        show = findViewById(R.id.showText);             //绑定输入框(仅展示)
        show.setEnabled(false);                         //设置输入框(仅展示)为不可用
        input = findViewById(R.id.editText);            //绑定输入框
        ipSpinner = findViewById(R.id.spinner);         //绑定列表选择框
        enter = findViewById(R.id.button1);             //绑定按钮(打开控制页面)
        clear = findViewById(R.id.button2);             //绑定按钮(清空接收列表)
        ctrl = findViewById(R.id.webView);              //绑定网页浏览器
        ctrl.getSettings().setJavaScriptEnabled(true);  //启用网页浏览器对JavaScript的支持
    }

    //打开控制页面方法
    public void enterWeb(View v) {
        //如果输入框不为空则打开页面
        if (!input.getText().toString().isEmpty()) {
            show.setVisibility(GONE);                   //设置输入框(仅展示)不可见,且不占据空间
            input.setVisibility(GONE);                  //设置输入框不可见,且不占据空间
            ipSpinner.setVisibility(GONE);              //设置列表选择框不可见,且不占据空间
            enter.setVisibility(GONE);                  //设置按钮(打开控制页面)不可见,且不占据空间
            clear.setVisibility(GONE);                  //设置按钮(清空接收列表)不可见,且不占据空间
            ctrl.setVisibility(VISIBLE);                //设置网页浏览器可见
            webViewCtrl(input.getText().toString());    //使用输入框内的IP地址文本打开网页
        } else {
            //如果输入框为空则弹出Toast提示信息
            Toast.makeText(this, "尚未键入需要控制服务端的IP地址", Toast.LENGTH_SHORT).show();
        }
    }

    //关闭控制页面方法
    private long exitTime = 0;

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        //如果监听到按键事件为退返回键且网页浏览器处于可见(正在显示控制页面)状态
        if (keyCode == KeyEvent.KEYCODE_BACK && ctrl.getVisibility() == VISIBLE) {
            //如果两次按键事件时间间隔超过2秒则认为两次事件独立,否则执行返回
            if ((System.currentTimeMillis() - exitTime) > 2000) {
                Toast.makeText(getApplicationContext(), "再滑一次退出控制页面", Toast.LENGTH_SHORT).show();
                exitTime = System.currentTimeMillis();
            } else {
                show.setVisibility(VISIBLE);                //设置输入框(仅展示)可见
                input.setVisibility(VISIBLE);               //设置输入框可见
                ipSpinner.setVisibility(VISIBLE);           //设置列表选择框可见
                enter.setVisibility(VISIBLE);               //设置按钮(打开控制页面)可见
                clear.setVisibility(VISIBLE);               //设置按钮(清空接收列表)可见
                ctrl.setVisibility(GONE);                   //设置网页浏览器不可见,且不占据空间
            }
            return true;
        } else {
            return super.onKeyDown(keyCode, event);
        }
    }

    //清空接收列表方法
    public void clearList(View v) {
        final_serverIPAddress.clear();                  //清空接收IP地址数组(清除未提交的缓存)
        //创建列表选择框适配器
        ArrayAdapter<String> arrayAdapter = new ArrayAdapter<String>(this, android.R.layout.simple_spinner_dropdown_item, new ArrayList<>());
        ipSpinner.setAdapter(arrayAdapter);             //指定其为控件的适配器
        arrayAdapter.clear();                           //清空已提交列表
        arrayAdapter.notifyDataSetChanged();            //向控件提交更改
    }

    //打开网页方法
    public void webViewCtrl(String Url) {
        WebView webView = findViewById(R.id.webView);   //创建网页视图实例并绑定对应UI
        webView.loadUrl(Url);                           //加载URL所对应的网址
        webView.setWebViewClient(new WebViewClient() {
            @Override
            public boolean shouldOverrideUrlLoading(WebView view, String url) {
                view.loadUrl(url);
                return false;
            }

            @Override
            public boolean onRenderProcessGone(WebView view, RenderProcessGoneDetail detail) {
                return true;
            }
        });
    }

    //UDP广播监听方法
    private void udpReceiveMsg() {
        //使用Lambda表达式开启新线程并绑定到对应的匿名方法
        new Thread(() -> {
            //使用try...catch...结构防止出现致命错误时程序终止
            try {
                final int PORT = 1145;      //定义UDP广播接收端口号
                DatagramSocket socket = new DatagramSocket(PORT);   //使用UDP广播接收端口号创建数据包接收实例
                byte[] buffer = new byte[1024];     //创建接收缓冲区字节数组
                //定义用于描述IPv4地址的正则表达式
                String regex_serverIPAddress = "((2(5[0-5]|[0-4]\\d))|[0-1]?\\d{1,2})(\\.((2(5[0-5]|[0-4]\\d))|[0-1]?\\d{1,2})){3}";
                String serverIPAddress;     //定义实时接收的服务端IP地址
                //死循环,永远保持接收态
                while (true) {
                    DatagramPacket packet = new DatagramPacket(buffer, buffer.length);  //绑定接收缓冲区字节数组为数据包接收对象并创建实例
                    socket.receive(packet);     //接收数据包,该方法为阻塞运行
                    String receivedData = new String(packet.getData(), 0, packet.getLength());  //将接收的数据转为字符串
                    //判断数据包是否来自目标服务端
                    if (receivedData.contains("This is Automatic Watering Server:")) {
                        Pattern pattern = Pattern.compile(regex_serverIPAddress);
                        Matcher matcher = pattern.matcher(receivedData);        //使用正则表达式提取IPv4地址
                        //如果存在提取结果(找到字符串中存在IP地址)
                        if (matcher.find()) {
                            serverIPAddress = matcher.group(0);     //获取实时接收的服务端IP地址
                            //如果在已列表的IP地址中不存在新接收到的IP地址则更新列表
                            if (!final_serverIPAddress.contains(serverIPAddress)) {
                                final_serverIPAddress.add(serverIPAddress);     //向列表添加新IP地址项
                                //使用runOnUiThread的Lambda表达式,在子线程向主线程UI发起更新
                                runOnUiThread(() -> {
                                    //创建列表选择框适配器
                                    ArrayAdapter<String> arrayAdapter = new ArrayAdapter<String>(this, android.R.layout.simple_spinner_dropdown_item, final_serverIPAddress);
                                    ipSpinner.setAdapter(arrayAdapter);     //设置更新后列表为列表选择框适配器
                                    arrayAdapter.notifyDataSetChanged();    //向控件提交更改
                                });
                            }
                        }
                    }
                }
            } catch (Exception e) {
                //异常处理
                Log.e("UDP数据接收失败", "UDP数据接收失败");
            }
        }).start();
    }
}