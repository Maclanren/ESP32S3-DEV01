#include <arduino.h>
#include <Wire.h>

#define LED_ON HIGH
#define LED_OFF LOW
#define led_delay delay

#define TCA6408_I2C_ADDRESS 0x20  // 根据实际连接的ADDR引脚设置地址
#define I2C_SDA 1
#define I2C_SCL 2

// TCA6408ARGTR 寄存器地址
#define INPUT_REG    0x00  // 输入寄存器（只读）
#define OUTPUT_REG   0x01  // 输出寄存器（读写）
#define POLARITY_REG 0x02  // 极性反转寄存器
#define CONFIG_REG   0x03  // 方向配置寄存器（0=输出，1=输入）


// 定义 LED 引脚数组
unsigned int ledPins_A[] = { 4, 5, 6, 7, 15, 16, 17, 18, 8, 19, 20, 3, 46, 9, 10, 11, 12, 13, 14, 21 };
unsigned int ledPins_B[] = { 47, 48, 45, 0, 35, 36, 37, 38, 39, 40, 41, 42 };
unsigned int numLeds_A = sizeof(ledPins_A) / sizeof(ledPins_A[0]);  // LED 数量
unsigned int numLeds_B = sizeof(ledPins_B) / sizeof(ledPins_B[0]);  // LED 数量
unsigned int led_delay_time = 200;

void setup() {
  Serial.begin(115200);
  Serial.printf("numLeds_A=%3d\n", numLeds_A);
  Serial.print("\n");
  Serial.printf("numLeds_B=%3d\n", numLeds_B);
  // 初始化所有 LED 引脚为输出模式
  for (int i = 0; i < numLeds_A; i++) {
    pinMode(ledPins_A[i], OUTPUT);
    digitalWrite(ledPins_A[i], LOW);  // 初始状态为熄灭
  }
  for (int j = 0; j < numLeds_B; j++) {
    pinMode(ledPins_B[j], OUTPUT);
    digitalWrite(ledPins_B[j], LOW);  // 初始状态为熄灭
  }
  Wire.begin(I2C_SDA, I2C_SCL, 100000);  // ESP32S3 SDA=1 SCL=2 100kHz
  led_delay(10);
  // Configure TCA6408ARGTR
  if (!configureGPIO()) {
    Serial.println("TCA6408 initialization failure!");
    while (1);  // Waiting for initializationWait for the initialization to succeed.
  }
  setOutput_LOW();    // 默认关闭8颗灯
}

void loop() {
  // 正向流水灯效果
  for (int i = 0; i < numLeds_A; i++) {
    digitalWrite(ledPins_A[i], HIGH);  // 点亮当前 LED
    led_delay(led_delay_time);         // 延时 100ms
    digitalWrite(ledPins_A[i], LOW);   // 熄灭当前 LED
  }
  
  // 正向流水灯（P0 → P7）
  for (int j = 0; j < 8; j++) {
    setGPIO(j, HIGH); // 点亮当前 LED
    led_delay(led_delay_time);         // 延时 100ms
    setGPIO(j, LOW);  // 熄灭当前 LED
  }

  for (int k = 0; k < numLeds_B; k++) {
    digitalWrite(ledPins_B[k], HIGH);  // 点亮当前 LED
    led_delay(led_delay_time);         // 延时 100ms
    digitalWrite(ledPins_B[k], LOW);   // 熄灭当前 LED
  }

  // 反向流水灯效果
  // B组LED
  for (int k = numLeds_B - 1; k >= 0; k--) {
    digitalWrite(ledPins_B[k], HIGH);  // 点亮当前 LED
    led_delay(led_delay_time);         // 延时 100ms
    digitalWrite(ledPins_B[k], LOW);   // 熄灭当前 LED
  }

  // 反向流水灯（P7 → P0）
  for (int j = 7; j >= 0; j--) {
    setGPIO(j, HIGH); // 点亮当前 LED
    led_delay(led_delay_time);         // 延时 100ms
    setGPIO(j, LOW);  // 熄灭当前 LED
  }

  // A组LED
  for (int i = numLeds_A - 1; i >= 0; i--) {
    digitalWrite(ledPins_A[i], HIGH);  // 点亮当前 LED
    led_delay(led_delay_time);         // 延时 100ms
    digitalWrite(ledPins_A[i], LOW);   // 熄灭当前 LED
  }
}

bool configureGPIO() {
  // 配置TCA6408ARGTR的引脚模式，例如将所有引脚设置为输出模式
  Wire.beginTransmission(TCA6408_I2C_ADDRESS);
  Wire.write(CONFIG_REG); // CONFIGURATION寄存器地址,配置输出模式
  // 1：输入 0：输出
  Wire.write(0x00);       // 0=输出，1=输入（这里设为全输出）
  if (Wire.endTransmission() != 0) {
    return false; // 初始化失败
  }
  Serial.println("TCA6408 configure is success.\n");
  return true;
}

// 设置指定 GPIO 的电平（0-7 对应 P0-P7）
void setGPIO(uint8_t pin, bool state) {
  static uint8_t outputState = 0x00; // 缓存当前输出状态
 
  if (state) {
    outputState |= (1 << pin);  // 置位
    // led_delay(50);         // 延时 50ms
  } else {
    outputState &= ~(1 << pin); // 清零
    // led_delay(50);         // 延时 50ms
  }
 
  Wire.beginTransmission(TCA6408_I2C_ADDRESS);
  Wire.write(OUTPUT_REG); // 输出寄存器
  Wire.write(outputState); // 写入新状态
  Wire.endTransmission();
}

void setOutput_LOW() {
  Wire.beginTransmission(TCA6408_I2C_ADDRESS);
  Wire.write(OUTPUT_REG);  // OUTPUT寄存器地址控制GPIO的电平
  Wire.write(0x00);       //  关闭8颗LED
  Wire.endTransmission();
}