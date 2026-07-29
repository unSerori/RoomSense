/*
  RoomSense
  Date : 2026/06/30
  Author : IE4A 井上 明日可
*/

// #include文
#include <Arduino.h>
#include <ST7032_asukiaaa.h>
#include "DHT.h"

// ピン番号をマクロで定義
#define LED_PIN 15         // 赤色LED
#define SW1_PIN 17         // タクトスイッチ
#define SW2_PIN 0          // タクトスイッチ
#define MOSFET_GATE_PIN 25 // MOSFETのゲート
#define DHTPIN 26           // DHT11のDATA
#define DHTTYPE DHT11
#define BATT_PIN 34        // バッテリー電圧アナログ入力
#define BUZZER_PIN 23 // ブザーを接続するピン
#define BUZZER_CH 0   // ブザーを接続するチャンネル
#define BUTTON_DEBOUNCE_MS 20UL
#define SENSOR_INTERVAL_MS 5000UL
#ifndef AUTO_POWER_OFF_MS
#define AUTO_POWER_OFF_MS 30000UL
#endif

// 変数宣言
float temperatureValue; // 環境温度[℃]
float humidityValue;    // 環境湿度[%]
float batteryValue;     // バッテリ電圧[V]
uint32_t lastActivityMillis = 0;

// オブジェクト作成
ST7032_asukiaaa lcd;
DHT dht(DHTPIN, DHTTYPE);

// 初期化

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

void readSensor() {
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  if (isnan(t) || isnan(h))
  {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.printf("Temperature: %f, Humidity: %f\n", t, h);
  temperatureValue = t;
  humidityValue = h;
}

void showSensorData() {
  char line1[9];
  snprintf(line1, sizeof(line1), "T: %4.1fC", temperatureValue);
  char line2[9];
  snprintf(line2, sizeof(line2), "H: %4.1f%%", humidityValue);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(line1);
  lcd.setCursor(0,1);
  lcd.print(line2);
}

/**
  入力操作があった時に呼び出し、グローバルな値を更新する。
 * @return なし
*/
void resetActivityTime(){
  lastActivityMillis = millis();
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

  // st7032 lcdの初期化（AQM0802A-FLW-GBW）
  Wire.begin(21, 22);
  lcd.setWire(&Wire);
  lcd.begin(8, 2);
  lcd.setContrast(25); // 0~40

  // オープニング表示
  lcd.clear();
  char line1[9] = "Room \\";
  char line2[9] = "Sense v1";
  lcd.setCursor(0,0);
  lcd.print(line1);
  lcd.setCursor(0,1);
  lcd.print(line2);

  // データ取得処理の設定
  dht.begin();

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
  // 最初に一回表示
  readSensor();
  showSensorData();
  resetActivityTime();
}

// ループ

/**
  電源を切る
 * @return なし
*/
void powerOff(){
  // UI
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("PwrOff..");
  beep(1000, 150);
  beep(500, 150);
  Serial.println("PwrOff..");

  // 終了
  digitalWrite(MOSFET_GATE_PIN, LOW); // TODO: 実際に起動するか
  while (true) // 待機
  {
    delay(1000);
  }
}

void loop()
{
  // 表示処理
  static uint32_t lastMillis = 0;
  if (millis() - lastMillis > SENSOR_INTERVAL_MS)
  {
    lastMillis = millis();
    readSensor();
    showSensorData();
  }

  // ボタン更新のなさを記録
  Serial.printf("Idle time: %lu\n", millis() - lastActivityMillis);
  if (digitalRead(SW1_PIN) == LOW || digitalRead(SW2_PIN) == LOW) // HACK: もっと綺麗にしたい
  {
    resetActivityTime();
  }

  // 更新がなければオートパワーオフ
  if (millis() - lastActivityMillis > AUTO_POWER_OFF_MS)
  {
    powerOff();
  }

  // チャタリング防止
  delay(BUTTON_DEBOUNCE_MS);
}
