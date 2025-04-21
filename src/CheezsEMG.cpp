#include "CheezsEMG.h"

// 构造函数
CheezsEMG::CheezsEMG( uint8_t emgPin, uint8_t detectPin, 
                      unsigned long sampleRate): 
    _emgInputPin(emgPin),
    _detectPin(detectPin),
    _sampleRate(sampleRate),
    _dataIndex(0),
    _bufferSum(0),
    _pastTime(0),
    _timer(0)
{
    // 初始化滤波器状态 
    memset(_circularBuffer, 0, sizeof(_circularBuffer));
}

// 初始化方法
void CheezsEMG::begin() {
    pinMode(_detectPin, INPUT);
    pinMode(_emgInputPin, INPUT);
}

// 处理信号的主方法
void CheezsEMG::processSignal() 
{ 
  sEmgRawValue = analogRead(_emgInputPin);
  sEmgfilteredValue = butterworthFilter(sEmgRawValue);
  sEmgEnvelopeValue = calculateEnvelope(abs(sEmgfilteredValue));
  isWear = digitalRead(_detectPin);  
}

// 定时检查方法
bool CheezsEMG::checkSampleInterval() 
{
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

// Butterworth带通滤波器
float CheezsEMG::butterworthFilter(float input)  {
  float output = input;
  {
    static float z1, z2; // filter section state
    float x = output - (-0.55195385 * z1) - (0.60461714 * z2);
    output = 0.00223489 * x + (0.00446978 * z1) + (0.00223489 * z2);
    z2 = z1;
    z1 = x;
  }

  {
    static float z1, z2; // filter section state
    float x = output - (-0.86036562 * z1) - (0.63511954 * z2);
    output = 1.00000000 * x + (2.00000000 * z1) + (1.00000000 * z2);
    z2 = z1;
    z1 = x;
  }

  {
    static float z1, z2; // filter section state
    float x = output - (-0.37367240 * z1) - (0.81248708 * z2);
    output = 1.00000000 * x + (-2.00000000 * z1) + (1.00000000 * z2);
    z2 = z1;
    z1 = x;
  }

  {
    static float z1, z2; // filter section state
    float x = output - (-1.15601175 * z1) - (0.84761589 * z2);
    output = 1.00000000 * x + (-2.00000000 * z1) + (1.00000000 * z2);
    z2 = z1;
    z1 = x;
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



// 获取佩戴检测信号
int CheezsEMG::getRawSignal() { 
    return sEmgRawValue;
}

// 获取滤波后的信号
float CheezsEMG::getFilteredSignal() { 
    return sEmgfilteredValue;
}

// 获取包络信号
int CheezsEMG::getEnvelopeSignal() { 
    return sEmgEnvelopeValue;
}

// 获取佩戴检测信号
int CheezsEMG::getDetectSignal() {
    return isWear;
}
