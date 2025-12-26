// CheezsEMG.cpp
#include "CheezsEMG.h"

// 初始化静态成员变量
uint32_t CheezsEMG::_sampleRate = 500;
unsigned long CheezsEMG::_pastTime = 0;
long CheezsEMG::_timer = 0;

// 构造函数1：带采样率参数
CheezsEMG::CheezsEMG(uint8_t emgPin, uint8_t detectPin, uint32_t sampleRate)
    : _emgInputPin(emgPin),
      _detectPin(detectPin),
      _dataIndex(0),
      _bufferSum(0) {
  // 初始化缓冲区和滤波器状态
  memset(_circularBuffer, 0, sizeof(_circularBuffer));
  memset(_z1, 0, sizeof(_z1));
  memset(_z2, 0, sizeof(_z2));
  setSampleRate(sampleRate);
}

// 构造函数2：不带采样率参数
CheezsEMG::CheezsEMG(uint8_t emgPin, uint8_t detectPin)
    : _emgInputPin(emgPin),
      _detectPin(detectPin),
      _dataIndex(0),
      _bufferSum(0) {
  // 初始化缓冲区和滤波器状态
  memset(_circularBuffer, 0, sizeof(_circularBuffer));
  memset(_z1, 0, sizeof(_z1));
  memset(_z2, 0, sizeof(_z2)); 
}

// 初始化方法
void CheezsEMG::begin() {
  pinMode(_detectPin, INPUT); 
}

// 定时检查方法（静态）
bool CheezsEMG::checkSampleInterval() {
  unsigned long present = micros();
  unsigned long interval = present - _pastTime;
  _pastTime = present;
  _timer -= interval;

  if (_timer < 0) {
    _timer += 1000000 / _sampleRate;
    return true;
  }
  return false;
}

// 设置采样率（静态）
void CheezsEMG::setSampleRate(uint32_t sampleRate) {
  _sampleRate = sampleRate;
  _pastTime = micros();
  _timer = 0;
}

// 处理信号的主方法
void CheezsEMG::processSignal() {
  sEmgRawValue = analogRead(_emgInputPin);
  sEmgfilteredValue = butterworthFilter(sEmgRawValue);
  sEmgEnvelopeValue = calculateEnvelope(abs(sEmgfilteredValue));
  isWear = digitalRead(_detectPin);
}

// Butterworth带通滤波器
float CheezsEMG::butterworthFilter(float input) {
  float output = input;
  {
    float x = output - (-0.55195385 * _z1[0]) - (0.60461714 * _z2[0]);
    output = 0.00223489 * x + (0.00446978 * _z1[0]) + (0.00223489 * _z2[0]);
    _z2[0] = _z1[0];
    _z1[0] = x;
  }

  {
    float x = output - (-0.86036562 * _z1[1]) - (0.63511954 * _z2[1]);
    output = 1.00000000 * x + (2.00000000 * _z1[1]) + (1.00000000 * _z2[1]);
    _z2[1] = _z1[1];
    _z1[1] = x;
  }

  {
    float x = output - (-0.37367240 * _z1[2]) - (0.81248708 * _z2[2]);
    output = 1.00000000 * x + (-2.00000000 * _z1[2]) + (1.00000000 * _z2[2]);
    _z2[2] = _z1[2];
    _z1[2] = x;
  }

  {
    float x = output - (-1.15601175 * _z1[3]) - (0.84761589 * _z2[3]);
    output = 1.00000000 * x + (-2.00000000 * _z1[3]) + (1.00000000 * _z2[3]);
    _z2[3] = _z1[3];
    _z1[3] = x;
  }

  return output;
}

// 包络信号计算
int CheezsEMG::calculateEnvelope(int abs_emg) {
  _bufferSum -= _circularBuffer[_dataIndex];
  _bufferSum += abs_emg;
  _circularBuffer[_dataIndex] = abs_emg;
  _dataIndex = (_dataIndex + 1) % BUFFER_SIZE;

  return (_bufferSum / BUFFER_SIZE) * 2;
}

// 获取原始信号
int CheezsEMG::getRawSignal() { return sEmgRawValue; }

// 获取滤波后的信号
float CheezsEMG::getFilteredSignal() { return sEmgfilteredValue; }

// 获取包络信号
int CheezsEMG::getEnvelopeSignal() { return sEmgEnvelopeValue; }

// 获取佩戴检测信号
int CheezsEMG::getDetectSignal() { return isWear; }