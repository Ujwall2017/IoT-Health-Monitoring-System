// ---------------------------------------------------------
// Project: Heartbeat + Temperature & Humidity Monitor
// Board: Arduino UNO
// Sensors: Pulse Sensor (SEN11574) + DHT11
// Display: 16x2 LCD
// Author: Ujwal Lekurwale
// ---------------------------------------------------------

#include <LiquidCrystal.h>
#include "DHT.h"

#define DHTPIN 7          // DHT11 data pin connected to digital pin 7
#define DHTTYPE DHT11
#define PULSE_PIN A0      // Pulse sensor connected to analog pin A0
#define BUZZER 8          // Buzzer on digital pin 8 (optional alert)
#define LED 9              // LED indicator for heartbeat

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal lcd(12, 11, 5, 4, 3, 2); // RS, E, D4, D5, D6, D7

// Variables for heart rate
int signal;                 
int threshold = 550;        
bool beat = false;
unsigned long lastBeatTime = 0;
int bpm = 0;
int beatCount = 0;
unsigned long startTime = 0;

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  dht.begin();

  pinMode(BUZZER, OUTPUT);
  pinMode(LED, OUTPUT);

  lcd.setCursor(0, 0);
  lcd.print("Health Monitor");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  delay(2000);
  lcd.clear();

  startTime = millis();
}

void loop() {
  // --- Heartbeat Sensor ---
  signal = analogRead(PULSE_PIN);

  if (signal > threshold && !beat) {
    beat = true;
    digitalWrite(LED, HIGH);
    beatCount++;
    tone(BUZZER, 1000, 50); // short beep
  }

  if (signal < threshold) {
    beat = false;
    digitalWrite(LED, LOW);
  }

  // Calculate BPM every 15 seconds
  if (millis() - startTime >= 15000) {
    bpm = (beatCount * 4); // beats in 15s * 4 = BPM
    beatCount = 0;
    startTime = millis();
  }

  // --- Temperature & Humidity ---
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // --- Display on LCD ---
  lcd.setCursor(0, 0);
  lcd.print("BPM:");
  lcd.print(bpm);
  lcd.print("   ");

  lcd.setCursor(0, 1);
  lcd.print("T:");
  lcd.print(temperature, 1);
  lcd.print("C ");
  lcd.print("H:");
  lcd.print(humidity, 0);
  lcd.print("%");

  // --- Serial Monitor ---
  Serial.print("BPM: "); Serial.print(bpm);
  Serial.print(" | Temp: "); Serial.print(temperature);
  Serial.print("°C | Humidity: "); Serial.print(humidity);
  Serial.println("%");

  delay(500);
}
