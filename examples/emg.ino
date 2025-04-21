/**
******************************************************************************
* @file   : EMGEnvelop.ino
* @brief  : 输出：串口,波特率 115200
            采样率：500Hz 
            引脚连接：
                  单片机  |   传感器
                  D2      |   L(黄色线)
                  A0      |   S(白色线)
                  5V      |   V(红色线)
                  GND     |   GND(黑色线)

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

#define SAMPLE_RATE 500        // 采样率
#define BAUD_RATE 115200       // 串口波特率
#define INPUT_PIN A0           // 信号输入(白色)
#define DETECT_PIN 2           // 检测输入(黄色)
 
// 使用默认配置  
CheezsEMG sEMG(INPUT_PIN, DETECT_PIN, SAMPLE_RATE);  

void setup() 
{
  Serial.begin(BAUD_RATE);
  sEMG.begin();  
}  

void loop() 
{   
  if(sEMG.checkSampleInterval())
  {
    sEMG.processSignal();  

    Serial.println(
        String(sEMG.getRawSignal()) + "," +      // 原始数据
        String(sEMG.getFilteredSignal()) + "," + // 滤波数据
        String(sEMG.getEnvelopeSignal()) + "," + // 包络数据
        String(sEMG.getDetectSignal())           // 佩戴情况
    );
  }
} 
