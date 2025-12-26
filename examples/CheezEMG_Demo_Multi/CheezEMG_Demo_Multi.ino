/**
******************************************************************************
* @file   : CheezEMG_Demo_Multi.ino
* @brief  ：多通道EMG传感器同步采样示例
            ==================================================
            引脚连接：
            传感器1          |   传感器2
            D2 - 黄线        |   D3 - 黄线
            A0 - 白线        |   A1 - 白线
            5V  - 红线       |   5V  - 红线
            GND - 黑线       |   GND - 黑线
            ==================================================
* @brief  : 输出：串口 @波特率 115200
            采样率：500Hz
            串口输出内容：双通道同步数据
******************************************************************************
*/
#include "CheezsEMG.h"

#define BAUD_RATE 115200  // 串口波特率
#define SAMPLE_RATE 500   // 采样率

#define NUM_SENSORS 2
CheezsEMG emg[NUM_SENSORS] = {
    CheezsEMG(A0, 2),  // 传感器0
    CheezsEMG(A1, 3)   // 传感器1
};

void setup() {
  Serial.begin(BAUD_RATE);
  CheezsEMG::setSampleRate(SAMPLE_RATE);

  // 初始化传感器
  for (int i = 0; i < NUM_SENSORS; i++) {
    emg[i].begin();
  }

  delay(100);
}

void loop() {
  if (CheezsEMG::checkSampleInterval()) {
    for (int i = 0; i < NUM_SENSORS; i++) {
      emg[i].processSignal();
    }

    // 输出每个传感器的数据
    for (int i = 0; i < NUM_SENSORS; i++) {
      Serial.print(",");
      Serial.print(emg[i].getRawSignal());
      Serial.print(",");
      Serial.print(emg[i].getEnvelopeSignal());
      Serial.print(",");
      Serial.print(emg[i].getDetectSignal());
      if (i < NUM_SENSORS - 1) Serial.print(",");
    }
    Serial.println();
  }
}