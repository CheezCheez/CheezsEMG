/**
******************************************************************************
* @brief  ： <!!> 需安装Arduino库 CheezsEMG、Servo
            ==================================================
            引脚连接：
                  开发板  |   传感器
                  (D)2    |   L(黄色线)
                  A0      |   S(白色线)
                  5V      |   V(红色线)
                  GND     |   G(黑色线)
                  
                  开发板  |   舵机
                  GND     |   GND(黑色线) 
                  5V      |   V(红色线)
                  (D)11   |   S(黄色线)
            ==================================================
* @brief  : 输出：串口 @波特率 115200
            采样率：500Hz 
            串口输出内容(ASCII码)：
                  原始数据，滤波后的肌电信号,包络信号,佩戴检测信号
            
            舵机状态:
                  根据肌肉幅度电量舵机转动相应角度
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
#include <Servo.h>

#define INPUT_PIN A0           // 传感器信号输入(白色)
#define DETECT_PIN 2           // 传感器检测输入(黄色)  
#define SERVO_PIN 11           // 舵机信号引脚（橙色）

#define BAUD_RATE 115200       // 串口波特率 

unsigned long lastTime;
CheezsEMG sEMG(INPUT_PIN, DETECT_PIN, 500);  
Servo sg90;

void setup() 
{
  Serial.begin(BAUD_RATE);
  sEMG.begin();   
  sg90.attach(SERVO_PIN);
  sg90.write(0); 
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
    
    if(millis()-lastTime > 100)
    {
      lastTime = millis();
      if(detectValue == HIGH) 
      {
        int level = getLevel(envelopValue);  
        sg90.write(level); // 舵机转动角度
      }  
    }
  }
} 

//****************************************************************/
 
// EMG 阈值 
#define ServoH 180
#define ServoL 0 
#define ALPH 0.1f // 响应程度

// 获取肌电信号等级  
int getLevel(int value) 
{ 
  static int envelopFilter, enveloplast;
  envelopFilter = (1-ALPH) * enveloplast + ALPH * value;
  enveloplast = envelopFilter;
  envelopFilter = (envelopFilter > ServoH) ? ServoH : envelopFilter;
  envelopFilter = (envelopFilter < ServoL) ? ServoL : envelopFilter;

  return envelopFilter;
}

