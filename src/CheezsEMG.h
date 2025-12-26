// CheezsEMG.h
#ifndef EMG_SIGNAL_PROCESSOR_H
#define EMG_SIGNAL_PROCESSOR_H

#include <Arduino.h>

class CheezsEMG {
 public:
  CheezsEMG(uint8_t inputPin, uint8_t detectPin, uint32_t sampleRate);

  CheezsEMG(uint8_t inputPin = A0, uint8_t detectPin = 2);

  ~CheezsEMG() {};

  void begin();

  // 读取并处理肌电信号
  void processSignal();

  // 检查采样间隔(静态)
  static bool checkSampleInterval();

  // 静态方法设置采样率
  static void setSampleRate(uint32_t sampleRate);

  // 获取当前采样率
  static uint32_t getSampleRate() { return _sampleRate; }

  // 获取佩戴检测信号
  int getDetectSignal();

  // 获取原始的信号
  int getRawSignal();

  // 获取滤波后的信号
  float getFilteredSignal();

  // 获取包络信号
  int getEnvelopeSignal();

 private:
  // 私有配置参数
  uint8_t _emgInputPin;
  uint8_t _detectPin;

  // 静态定时器变量（所有实例共享）
  static uint32_t _sampleRate;
  static unsigned long _pastTime;
  static long _timer;

  static const int BUFFER_SIZE = 32;

  // 包络检测缓存
  int _circularBuffer[BUFFER_SIZE];
  int _dataIndex;
  int _bufferSum;

  bool isWear;
  int sEmgRawValue;
  int sEmgEnvelopeValue;
  float sEmgfilteredValue;

  // 滤波器状态变量 (4个阶段)
  float _z1[4], _z2[4];

  float butterworthFilter(float input);
  int calculateEnvelope(int abs_emg);
};

#endif