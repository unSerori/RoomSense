/*
  RoomSense
  Date : 2026/06/30
  Author : IE4A 井上 明日可
*/

// #include文
#include <Arduino.h>

// ピン番号をマクロで定義
#define LED_PIN 15         // 赤色LED
#define SW1_PIN 17         // タクトスイッチ
#define SW2_PIN 0          // タクトスイッチ
#define MOSFET_GATE_PIN 25 // MOSFETのゲート
#define DHTPIN 5           // DHT11データ
#define BATT_PIN 34        // バッテリー電圧アナログ入力

#define BUZZER_PIN 23 // ブザーを接続するピン
#define BUZZER_CH 0   // ブザーを接続するチャンネル

// 変数宣言
float batteryValue;     // バッテリ電圧[V]
float temperatureValue; // 環境温度[℃]
float humidityValue;    // 環境湿度[%]

// オブジェクト作成

void beep(int freq, int time)
{
  ledcWriteTone(BUZZER_CH, freq); // freqHzの音を出す
  delay(time);
  ledcWriteTone(BUZZER_CH, 0); // 消音する
}

/**
   LEDを指定したミリ秒分点滅させる
 * @param ledPin LEDを接続しているGPIOピン番号
 * @param delayTime LEDを点灯させておく時間
 * @return なし
*/
void ledFlash(int ledPin, int delayTime)
{
  digitalWrite(ledPin, HIGH);
  delay(delayTime);
  digitalWrite(ledPin, LOW);
  delay(delayTime);
}

/**
   ビープ音の初期設定を行う
 * @param pin LEDCのGPIOピン番号
 * @param freq freqPWMの周波数
 * @param resolution LEDCチャンネルの解像度（範囲は1~14bit、ESP32の場合は1~20bit）
 * @param channel LEDCチャネル
 * @return 設定が完了したらなら真
*/
bool ledcAttachChannel(uint8_t pin, uint32_t freq, uint8_t resolution, int8_t channel)
{
  // 簡易的なバリデーション
  if (!digitalPinIsValid(pin))
  {
    return false;
  }
  if (resolution < 1 || resolution > SOC_LEDC_TIMER_BIT_WIDE_NUM)
  {
    return false;
  }
  if (channel < 0 || channel >= SOC_LEDC_CHANNEL_NUM)
  {
    return false;
  }

  uint32_t actualFreq = ledcSetup(channel, freq, resolution);
  if (actualFreq == 0)
  {
    return false;
  }

  ledcAttachPin(pin, channel);
  return true;
}

void setup()
{
  Serial.begin(115200);

  // ピンモードを設定
  pinMode(MOSFET_GATE_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(SW1_PIN, INPUT_PULLUP);
  pinMode(SW2_PIN, INPUT_PULLUP);
  pinMode(BATT_PIN, INPUT);

  // MOSFETをオンにして電源を供給する
  digitalWrite(MOSFET_GATE_PIN, HIGH); // T2（NPN）がONになり、Pch MOSFETのゲートがLOWに引き下げられてONになる。

  // ビープ音初期設定
  ledcAttachChannel(BUZZER_PIN, 5000, 8, BUZZER_CH);

  // 起動ビープ音（ピポッ）
  beep(500, 150);  // 500[Hz], 150[ms]
  beep(1000, 150); // 1000[Hz], 150[ms]

  // 起動完了時LED点滅表示
  for (int i = 0; i < 4; i++)
  {
    ledFlash(LED_PIN, 100);
  }

  // ハードウェアの安定待ち
  delay(2000);
}

void loop()
{
  // センサーデータを取得する処理を記述する

  // 計測値を表示する
  Serial.printf("\n------計測値------\n");


  // チャタリング防止
  delay(10);
}
