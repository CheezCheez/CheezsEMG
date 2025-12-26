#define SAMPLE_RATE 500
#define CHANNEL_COUNT 6

const uint8_t emgPins[CHANNEL_COUNT] = {A0, A1, A2, A3, A4, A5};
const uint8_t leadPins[CHANNEL_COUNT] = {2, 3, 4, 5, 6, 7};

float filter_z1[CHANNEL_COUNT][4];
float filter_z2[CHANNEL_COUNT][4];

// 预计算的包络检测状态
int envelope_buffer[CHANNEL_COUNT][16];
int envelope_index[CHANNEL_COUNT];
int envelope_sum[CHANNEL_COUNT];

// 输出缓冲区预分配
byte output_buf[16] = {0xCC, 0xCC, 0x01, 0x0C};

// 模式变量
volatile byte current_mode = 1;

void buttonISR() {
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();

  if (interrupt_time - last_interrupt_time > 50) {
    current_mode = (current_mode + 1) % 3;
  }
  last_interrupt_time = interrupt_time;
}

inline float ultrafastFilter(int channel, float input) {
  float output = input;
  float x;

  x = output - (-0.55195385 * filter_z1[channel][0]) -
      (0.60461714 * filter_z2[channel][0]);
  output = 0.00223489 * x + (0.00446978 * filter_z1[channel][0]) +
           (0.00223489 * filter_z2[channel][0]);
  filter_z2[channel][0] = filter_z1[channel][0];
  filter_z1[channel][0] = x;

  x = output - (-0.86036562 * filter_z1[channel][1]) -
      (0.63511954 * filter_z2[channel][1]);
  output = 1.00000000 * x + (2.00000000 * filter_z1[channel][1]) +
           (1.00000000 * filter_z2[channel][1]);
  filter_z2[channel][1] = filter_z1[channel][1];
  filter_z1[channel][1] = x;

  x = output - (-0.37367240 * filter_z1[channel][2]) -
      (0.81248708 * filter_z2[channel][2]);
  output = 1.00000000 * x + (-2.00000000 * filter_z1[channel][2]) +
           (1.00000000 * filter_z2[channel][2]);
  filter_z2[channel][2] = filter_z1[channel][2];
  filter_z1[channel][2] = x;

  x = output - (-1.15601175 * filter_z1[channel][3]) -
      (0.84761589 * filter_z2[channel][3]);
  output = 1.00000000 * x + (-2.00000000 * filter_z1[channel][3]) +
           (1.00000000 * filter_z2[channel][3]);
  filter_z2[channel][3] = filter_z1[channel][3];
  filter_z1[channel][3] = x;

  return output;
}

inline int ultrafastEnvelope(int channel, int abs_val) {
  int idx = envelope_index[channel];
  int old_val = envelope_buffer[channel][idx];

  envelope_sum[channel] -= old_val;
  envelope_sum[channel] += abs_val;
  envelope_buffer[channel][idx] = abs_val;

  idx++;
  if (idx >= 16) idx = 0;
  envelope_index[channel] = idx;

  return (envelope_sum[channel] * 2) >> 4;  
}

void setup() {
  Serial.begin(115200);

// 设置ADC预分频器为16 
#if defined(ADCSRA)
  ADCSRA = (ADCSRA & 0xF8) | 0x04;
#endif
 
  for (int i = 0; i < CHANNEL_COUNT; i++) {
    pinMode(leadPins[i], INPUT);
  }
  pinMode(8, INPUT);
 
  attachInterrupt(digitalPinToInterrupt(8), buttonISR, FALLING);
 
  memset(filter_z1, 0, sizeof(filter_z1));
  memset(filter_z2, 0, sizeof(filter_z2));
  memset(envelope_buffer, 0, sizeof(envelope_buffer));
  memset(envelope_sum, 0, sizeof(envelope_sum));
}

void loop() { 
  static unsigned long last_sample_time = 0;
  unsigned long now = micros();

  if (now - last_sample_time >= 2000) {  // 500Hz = 2000us间隔
    last_sample_time = now;
 
    for (int ch = 0; ch < CHANNEL_COUNT; ch++) { 
      int raw = analogRead(emgPins[ch]); 
      if (digitalRead(leadPins[ch]) == LOW) {
        raw = 0;
      }

      int result;

      switch (current_mode) {
        case 0:  // 原始数据
          result = raw;
          break;

        case 1:  // 滤波数据
          result = (int)ultrafastFilter(ch, (float)raw);
          break;

        case 2:  // 包络数据
          float filtered = ultrafastFilter(ch, (float)raw);
          int abs_val = filtered >= 0 ? (int)filtered : -(int)filtered;
          result = ultrafastEnvelope(ch, abs_val);
          break;
      }

      // 直接填充输出缓冲区
      int buf_idx = 4 + ch * 2;
      output_buf[buf_idx] = (byte)((result >> 8) & 0xFF);
      output_buf[buf_idx + 1] = (byte)(result & 0xFF);
    }

    // 发送数据
    Serial.write(output_buf, 16);
  }
}