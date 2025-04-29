/**
******************************************************************************
* @file   : CheezEMG_Demo_LED.ino
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
            
            LED状态:
                  根据肌肉幅度电量不同个数的灯珠
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
  sEMG.begin();  
  ledInit();
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

    if(detectValue==0)
    {
      Serial.println("0,0,0,0" ); 
    }
    else
    {
      Serial.println
      (
        String(rawValue)      + "," + // 原始数据
        String(filteredValue) + "," + // 滤波数据
        String(envelopValue) + "," +  // 包络数据
        String(detectValue)           // 佩戴情况
      ); 
    }
  
    if(sEMG.getDetectSignal() == HIGH) 
    {
      int level = getLevel(sEMG.getEnvelopeSignal());  
      ledStatus(level, HIGH);  
    } 
    else 
    {  
      ledStatus(numLeds, LOW);  
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

