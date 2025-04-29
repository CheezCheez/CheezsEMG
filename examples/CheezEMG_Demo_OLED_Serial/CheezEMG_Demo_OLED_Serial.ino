/**
******************************************************************************
* @file   : CheezEMG_Demo_OLED_Serial.ino
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
* @brief  : 输出：串口 @波特率 115200
            采样率：500Hz 
            串口输出内容(ASCII码)：
                  原始数据，滤波后的肌电信号,包络信号,佩戴检测信号

            0.96'OLED显示波形
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

CheezsEMG sEMG(INPUT_PIN, DETECT_PIN, 500);  

void setup() 
{ 
  Serial.begin(BAUD_RATE);

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
    filteredValue = sEMG.getFilteredSignal(); // 滤波数据
    envelopValue = sEMG.getEnvelopeSignal();    // 包络数据
    detectValue = sEMG.getDetectSignal();       // 佩戴检测信号
     
    Serial.print(filteredValue);
    Serial.print(",");
    Serial.println(envelopValue); 
    Count++;
  } 
  
  if(Count == 3) // 每隔3点取1点用于绘制
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
      display.fillRect(0, 0, 128, 64, SSD1306_BLACK);
    } 
  } 
}

//******************************************************************
 