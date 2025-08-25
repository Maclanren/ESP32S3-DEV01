
FPC-OPT3001&lt;style&gt; img{ width: 400px; } &lt;/style&gt;![c726eb04ae9926ffa07f0175c9dddd5a.png](en-resource://database/4824:1)
简介这是一款基于 OPT3001DNPR 芯片的柔性FPC光照传感器，支持 I2C 接口，默认地址为 0x44。传感器可精准检测环境光强，响应范围广，适用于自动亮度调节、光照监测等应用。 采用 FPC-05F-6PH20 连接器，接口定义包括 VCC、NC、INT、SCL、SDA、GND，其中 SCL/SDA 为I2C数据与时钟线，INT 可用作中断输出。 柔性FPC设计方便在狭小空间内安装，适合智能设备、可穿戴设备及嵌入式应用。产品参数工作电压：1.6~3.6V工作电流：&lt;2.5uA测量范围：0.01 Lux 至 83,000 Lux通信接口：FPC 0.5mm-6Pin I2C外形尺寸：30mm(W) * 3.6mm(H)FPC焊盘尺寸：0.3mm(W) * 4mm(H)FPC焊盘间距：0.5mm引脚说明![.png](https://github.com/lbuque/docs/raw/master/source/_static/fpc-series/fpc-opt3001/img/OPT3001_pinmap.png)
标号名称功能描述1GND电源负极2SDA数据线3SCL时钟线4INT中断5NC未使用6VCC电源正极使用教程arduino准备硬件一块支持 Arduino IDE 的开发板接线图待补充样例代码1 - 连续转换模式将 OPT3001 配置为自动量程、连续转换模式#include &lt;Wire.h&gt;

#define I2C_SDA 8
#define I2C_SCL 10

#define OPT3001_ADDRESS 0x44  // OPT3001 的 I2C 地址，根据 ADDR 引脚连接 GND// #define OPT3001_ADDRESS 0x45    //  OPT3001 的 I2C 地址，根据 ADDR 引脚连接 VCC

// 配置寄存器值：自动量程、连续转换模式
#define OPT3001_CONFIG 0xCE10

void setup() {
  Serial.begin(115200);
  Wire.begin(I2C_SDA,I2C_SCL,100000);     // SDA SCL frequency
  // 配置 OPT3001
  Wire.beginTransmission(OPT3001_ADDRESS);
  Wire.write(0x01);  // 寄存器地址：配置寄存器
  Wire.write(OPT3001_CONFIG &gt;&gt; 8);  // 高字节
  Wire.write(OPT3001_CONFIG &amp; 0xFF);  // 低字节
  Wire.endTransmission();
}

void loop() {
  float lux = readLux();
  Serial.print(&quot;光照强度: &quot;);
  Serial.print(lux);
  Serial.println(&quot; lx&quot;);

  delay(1000);  // 每秒读取一次
}

float readLux() {
  uint16_t result;

  // 读取结果寄存器
  Wire.beginTransmission(OPT3001_ADDRESS);
  Wire.write(0x00);  // 寄存器地址：结果寄存器
  Wire.endTransmission();

  Wire.requestFrom(OPT3001_ADDRESS, 2);  // 读取 2 字节
  if (Wire.available() == 2) {
    uint8_t high = Wire.read();
    uint8_t low = Wire.read();
    result = (high &lt;&lt; 8) | low;
  }

  // 计算光照强度
  uint8_t exponent = (result &gt;&gt; 12) &amp; 0x0F;  // 指数部分
  uint16_t mantissa = result &amp; 0x0FFF;      // 尾数部分
  float lux = 0.01 * pow(2, exponent) * mantissa;
  return lux;
}样例代码2 - OLED屏幕上实时显示测量值在 OLED 屏幕上实时显示测量值您需要使用 Arduino 库管理器安装&nbsp;Adafruit_SSD1306&nbsp;库:启动 Arduino IDE，然后通过以下方式打开库管理器：&nbsp;Sketch&nbsp;➔&nbsp;Include Library&nbsp;➔&nbsp;Manage Libraries...在&nbsp;Filter your search...&nbsp;字段中搜索&nbsp;Adafruit_SSD1306&nbsp;库，然后点击&nbsp;install&nbsp;按钮进行安装。#include &lt;Wire.h&gt;
#include &lt;Adafruit_GFX.h&gt;
#include &lt;Adafruit_SSD1306.h&gt;

#define I2C_SDA 8
#define I2C_SCL 10

#define OPT3001_ADDRESS 0x44  // OPT3001 的 I2C 地址，根据 ADDR 引脚连接 GND// #define OPT3001_ADDRESS 0x45    //  OPT3001 的 I2C 地址，根据 ADDR 引脚连接 VCC// 配置寄存器值：自动量程、连续转换模式
#define OPT3001_CONFIG 0xCE10


#define SCREEN_WIDTH 128      // OLED display width, in pixels
#define SCREEN_HEIGHT 64      // OLED display height, in pixels
#define OLED_RESET -1         // Reset pin # (or -1 if sharing Arduino reset pin)
#define SSD1306_ADDRESS 0x3C  ///&lt; See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &amp;Wire, OLED_RESET);

void setup() {
  Serial.begin(115200);
  Wire.begin(I2C_SDA, I2C_SCL, 100000);  // SDA SCL frequency
  // 配置 OPT3001
  Wire.beginTransmission(OPT3001_ADDRESS);
  Wire.write(0x01);                   // 寄存器地址：配置寄存器
  Wire.write(OPT3001_CONFIG &gt;&gt; 8);    // 高字节
  Wire.write(OPT3001_CONFIG &amp; 0xFF);  // 低字节
  Wire.endTransmission();

  // while (!Serial)
  //   delay(10);  // will pause Zero, Leonardo, etc until serial console opens
  // // 初始化 OLED 显示屏
  if (!display.begin(SSD1306_SWITCHCAPVCC, SSD1306_ADDRESS)) {  // 0x3C 是 OLED 的 I2C 地址
    Serial.println(F(&quot;SSD1306 allocation failed&quot;));
    for (;;)
      ;  // 进入无限循环
  }
  display.display();  // 显示启动画面
  delay(1000);
  display.setTextSize(1);   // Draw 1X-scale text
  display.setTextColor(SSD1306_WHITE); // Set OLED Text Color is White
}

void loop() {
  float lux = readLux();
  Serial.print(&quot;光照强度: &quot;);
  Serial.print(lux);
  Serial.println(&quot; lx&quot;);

  // Display data on OLED
  display.clearDisplay();   // Clear OLED Display
  display.setTextSize(2);   // Set Text Size
  display.setCursor(0, 0);  // Set Text Cursor (x,y) x=width y=hight
  display.printf(&quot;OPT3x 0x%02x\n&quot;,OPT3001_ADDRESS);  // 格式化为两位十六进制数
  display.setTextSize(2);                   // Set Text Size
  display.setCursor(0, 20);
  display.println(&quot;Light:&quot;);
  // display.printf(&quot;\n&quot;);
  display.setCursor(0, 25);
        display.printf(&quot;\n%.1f lux\n&quot;,lux);  
  display.display();
  delay(3000);  // 每秒读取一次
}

float readLux() {
  uint16_t result;

  // 读取结果寄存器
  Wire.beginTransmission(OPT3001_ADDRESS);
  Wire.write(0x00);  // 寄存器地址：结果寄存器
  Wire.endTransmission();

  Wire.requestFrom(OPT3001_ADDRESS, 2);  // 读取 2 字节
  if (Wire.available() == 2) {
    uint8_t high = Wire.read();
    uint8_t low = Wire.read();
    result = (high &lt;&lt; 8) | low;
  }

  // 计算光照强度
  uint8_t exponent = (result &gt;&gt; 12) &amp; 0x0F;  // 指数部分
  uint16_t mantissa = result &amp; 0x0FFF;       // 尾数部分
  float lux = 0.01 * pow(2, exponent) * mantissa;
  return lux;
}
常见问题还没有客户对此产品有任何问题，欢迎通过qq或者论坛联系我们！包装信息更多

原理图.pdf
尺寸图.pdfTI-OP3001.pdf
