/**
******************************************************************************
* @file   : CheezEMG_Demo_OLED.ino
* @brief  ： <!!> 需安装Arduino库 Adafruit_SSD1306
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
 
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64, &Wire);
 

typedef struct  // 曲线参数
{
  float Draw_Buf[128];   
  float Draw_Min;       
  float Draw_Max;       
} _DrawCurve;    
static _DrawCurve DrawCurve;

void setup() 
{ 
  Wire.begin();
  Wire.setClock(400000L);   
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();  
}

void loop() {
  static uint16_t value = 0;
  value = analogRead(A0);
 
  PlotDataInput(&DrawCurve, value / 1024.0);  // 传入数据
  display.clearDisplay();
  PlotDataPrint(&DrawCurve, 30, 63, WHITE);  // 打印曲线
  display.display(); 
}

//******************************************************************
void PlotDataInput(_DrawCurve *Draw, float val) 
{
  uint8_t i;
  for (i = 1; i < 128; i++) {
    Draw->Draw_Buf[i - 1] = Draw->Draw_Buf[i];
  }
  Draw->Draw_Buf[127] = val;

  if (Draw->Draw_Buf[127] > 1)
    Draw->Draw_Buf[127] = 1;
  else if (Draw->Draw_Buf[127] < 0)
    Draw->Draw_Buf[127] = 0;

  Draw->Draw_Max = 0;
  Draw->Draw_Min = 1;
  for (i = 0; i < 128; i++)  // 获取最大最小范围
  {
    if (Draw->Draw_Min > (Draw->Draw_Buf[i])) {
      Draw->Draw_Min = Draw->Draw_Buf[i];
      Draw->Draw_Min = (float)((uint8_t)(Draw->Draw_Min * 100) / 5) / 20;

      if (Draw->Draw_Min < 0)
        Draw->Draw_Min = 0;
      if ((Draw->Draw_Max - Draw->Draw_Min) < 0.05)
        Draw->Draw_Max = Draw->Draw_Min + 0.05;
    } else if (Draw->Draw_Max < Draw->Draw_Buf[i]) {
      Draw->Draw_Max = Draw->Draw_Buf[i];
      Draw->Draw_Max = (float)((uint8_t)(Draw->Draw_Max * 100) / 5 + 1) / 20;
      if (Draw->Draw_Max > 1.0)
        Draw->Draw_Max = 1.0;
      if ((Draw->Draw_Max - Draw->Draw_Min) < 0.05)
        Draw->Draw_Min = Draw->Draw_Max - 0.05;
    }
  }
} 

void PlotDataPrint(_DrawCurve *Draw, int fllor, int upper, int lineColor) {

  for (int x = 1; x < 128; x++) {
    float k = (upper - fllor) / (Draw->Draw_Max - Draw->Draw_Min);  
    float last_y = upper - ((Draw->Draw_Buf[x - 1] - Draw->Draw_Min) * k);
    float now_y = upper - ((Draw->Draw_Buf[x] - Draw->Draw_Min) * k);

    display.drawLine(x - 1, last_y, x, now_y, lineColor);  
  }
}