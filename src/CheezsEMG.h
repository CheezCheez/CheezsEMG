// CheezsEMG.h
#ifndef EMG_SIGNAL_PROCESSOR_H
#define EMG_SIGNAL_PROCESSOR_H

#include <Arduino.h>

class CheezsEMG 
{
public:
    // 构造函数
    CheezsEMG(
        uint8_t inputPin = A0, 
        uint8_t detectPin = 2, 
        uint32_t sampleRate = 500 
    );

    ~CheezsEMG(){};  
 
    void begin();
  
    // 读取并处理肌电信号
    void processSignal(); 
    bool checkSampleInterval();

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
    uint32_t _sampleRate;  
    static const int BUFFER_SIZE = 128; 
 
    // 包络检测缓存
    int _circularBuffer[BUFFER_SIZE];
    int _dataIndex;
    int _bufferSum;

    // 定时相关变量
    unsigned long _pastTime;
    long _timer;

    bool isWear; 
    int sEmgRawValue; 
    int sEmgEnvelopeValue;
    float sEmgfilteredValue;
    
    float butterworthFilter(float input);
    int calculateEnvelope(int abs_emg);
};

#endif  
