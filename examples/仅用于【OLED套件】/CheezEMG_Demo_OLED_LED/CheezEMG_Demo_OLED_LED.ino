/**
******************************************************************************
* @file   : CheezEMG_Demo_OLED2.ino
* @brief  ： <!!> 需安装Arduino库 CheezsEMG
                                 Adafruit_SSD1306
            ==================================================
            引脚连接：
                  开发板  |   传感器
                  (D)2    |   L(黄色线)
                  A0      |   S(白色线)
                  5V      |   V(红色线)
                  GND     |   G(黑色线) 

                  开发板  |   0.96'OLED屏幕
                  SDA     |   SDA
                  SCL     |   SCL
                  5V      |   5V 
                  GND     |   GND
            ==================================================
* @brief  : 
            LED状态:
                  根据肌肉幅度电量不同个数的灯珠
            0.96'OLED屏:
                 显示波形
******************************************************************************
* @attention
*
* Copyright (c) 2024 Cheez.
* All rights reserved.
*
* This software is licensed under terms that can be found in the LICENSE file
* in the root directory of this software component.
* If no LICENSE file comes with this software, it is provided AS-IS.
*
******************************************************************************
*/

// TODO 
#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include "CheezsEMG.h"

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64, &Wire);
 
#define INPUT_PIN A0           // 信号输入(白色)
#define DETECT_PIN 2           // 检测输入(黄色) 
#define BAUD_RATE 115200       // 串口波特率

int rawValue;         // 原始数据
float filteredValue;  // 滤波数据
int envelopValue;     // 包络数据
int detectValue;      // 佩戴检测信号


// LED 引脚
const int pinArray[6] = { 3, 5, 6, 9, 10, 11 };   
const int numLeds = sizeof(pinArray) / sizeof(pinArray[0]);

// EMG 阈值
const int thresholds[] = { 10, 15, 20, 25, 30, 35, 40 };
const int numThresholds = sizeof(thresholds) / sizeof(thresholds[0]);

CheezsEMG sEMG(INPUT_PIN, DETECT_PIN, 500);  

void setup() 
{ 
  Serial.begin(BAUD_RATE);
  ledInit();
  
  Wire.begin();
  Wire.setClock(400000L);   
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  
  display.clearDisplay();
  display.display();
}

void loop() 
{ 
  static unsigned long Count ;
  if(sEMG.checkSampleInterval()) // 500Hz定时器
  { 
    sEMG.processSignal();  
  
    rawValue = sEMG.getRawSignal();             // 原始数据
    filteredValue = sEMG.getFilteredSignal();   // 滤波数据
    envelopValue = sEMG.getEnvelopeSignal();    // 包络数据
    detectValue = sEMG.getDetectSignal();       // 佩戴检测信号
     
    Serial.print(filteredValue);
    Serial.print(",");
    Serial.println(envelopValue); 

    
    if(sEMG.getDetectSignal() == HIGH) 
    {
      int level = getLevel(sEMG.getEnvelopeSignal());  
      ledStatus(level, HIGH);  
    } 
    else 
    {  
      ledStatus(numLeds, LOW);  
    } 

    Count++;
  } 
  
  if(Count >= 10) // 每隔3点取1点用于绘制
  { 
    Count = 0;   
    static int8_t sX = 0, x = 0, sY = 60, y = 0;  // 绘图参数
    y = 32-2*map(filteredValue, 0, 1024, 0, 64);
    display.drawLine(sX, sY, x, y, WHITE);
    sX = x;
    sY = y;
    x++;

    if(x%15==1)
    { 
      display.display(); 
    } 
    if (x >= 127) 
    {
      x = 0;
      sX = 0;
      display.display(); 
      display.fillRect(0, 0, 128, 64, SSD1306_BLACK);
    } 
  } 
}
//****************************************************************/

// LED引脚初始化
void ledInit() 
{  
  for (int i = 0; i < numLeds; i++) 
  {  
    pinMode(pinArray[i], OUTPUT);  
    digitalWrite(pinArray[i], HIGH);  
  }    
}

// LED状态控制
void ledStatus(int num, bool status) 
{
  num = (num > numLeds) ? (numLeds) : 
        ((num < 0) ? (0) : num);
 
  for (int i = 0; i < numLeds; i++) 
    digitalWrite(pinArray[i], HIGH);   

  for (int i = 0; i < num; i++)  
    digitalWrite(pinArray[i], !status);   
}

// 获取肌电信号等级  
int getLevel(int value) 
{
  value = abs(value);
  if(value >= thresholds[numThresholds-1])
    return numThresholds;

  if(value <= thresholds[0])
    return 0;

  for (int i = 0; i < numThresholds; i++) 
  {
    if (value <= thresholds[i])  
      return i; 
  } 
}


//******************************************************************
 