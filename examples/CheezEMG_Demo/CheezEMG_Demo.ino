/**
******************************************************************************
* @file   : CheezEMG_Demo.ino
* @brief  ： <!!> 需安装Arduino库 CheezsEMG
            ==================================================
            引脚连接：
                  开发板  |   传感器
                  (D)2    |   L(黄色线)
                  A0      |   S(白色线)
                  5V      |   V(红色线)
                  GND     |   G(黑色线) 
            ==================================================
* @brief  : 输出：串口 @波特率 115200
            采样率：500Hz 
            串口输出内容(ASCII码)：
                  原始数据，滤波后的肌电信号,包络信号,佩戴检测信号
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
#include "CheezsEMG.h"

#define INPUT_PIN A0           // 信号输入(白色)
#define DETECT_PIN 2           // 检测输入(黄色) 
#define BAUD_RATE 115200       // 串口波特率
 
CheezsEMG sEMG(INPUT_PIN, DETECT_PIN, 500);  

void setup() 
{
  Serial.begin(BAUD_RATE);
  sEMG.begin();  
}  

void loop() 
{   
  if(sEMG.checkSampleInterval()) // 500Hz定时器
  {
    sEMG.processSignal();  
    int rawValue = sEMG.getRawSignal();             // 原始数据
    float filteredValue = sEMG.getFilteredSignal(); // 滤波数据
    int envelopValue = sEMG.getEnvelopeSignal();    // 包络数据
    int detectValue = sEMG.getDetectSignal();       // 佩戴检测信号
 
    Serial.println
    (
      String(rawValue)      + "," + // 原始数据
      String(filteredValue) + "," + // 滤波数据
      String(envelopValue) + "," +  // 包络数据
      String(detectValue)           // 佩戴情况
    ); 
  }
} 
