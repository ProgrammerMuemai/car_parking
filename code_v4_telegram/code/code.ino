#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <HTTPClient.h>
#include "time.h"

// ---------- CONFIG ----------
#define DEBUG true
#define TIMEOUT_MS 10000
#define TOTAL_SLOTS 4

// ---------- TELEGRAM ----------
String botToken = "";  // TOKEN จาก BotFather
String chatID = "";  // CHAT ID จาก @userinfobot

// ---------- LCD ----------
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ---------- SERVO ----------
Servo gateServo;
#define SERVO_PIN 13

// ---------- SENSORS ----------
#define SENSOR1_PIN 25
#define SENSOR2_PIN 26

// ---------- VARIABLES ----------
int availableSlots = TOTAL_SLOTS;
bool carInProgress = false;
bool directionIn = false;
bool s1WasLow = false;
bool s2WasLow = false;
unsigned long gateOpenTime = 0;

// ---------- DEBUG ----------
#define DPRINT(x) if (DEBUG) Serial.print(x)
#define DPRINTLN(x) if (DEBUG) Serial.println(x)

// ---------- URL Encode ----------
String urlencode(String str) {
  String enc = "";
  char c;
  for (int i = 0; i < str.length(); i++) {
    c = str.charAt(i);
    if (isalnum(c)) enc += c;
    else {
      enc += '%';
      enc += String("0123456789ABCDEF"[(c >> 4) & 0xF]);
      enc += String("0123456789ABCDEF"[c & 0xF]);
    }
  }
  return enc;
}

// ---------- ส่งข้อความ Telegram ----------
void sendToTelegram(String event, int slots) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    time_t now;
    struct tm timeinfo;

    // ตั้งเวลาไทย
    if (!getLocalTime(&timeinfo)) {
      configTime(7 * 3600, 0, "pool.ntp.org");
      getLocalTime(&timeinfo);
    }

    char dateBuf[15];
    char timeBuf[15];
    strftime(dateBuf, sizeof(dateBuf), "%Y-%m-%d", &timeinfo);
    strftime(timeBuf, sizeof(timeBuf), "%H:%M:%S", &timeinfo);

    // ข้อความที่ส่งไป Telegram
    String message = event +
                     "\n🅿️ ช่องว่างที่เหลือ: " + String(slots) + " คัน" +
                     "\n📅 วันที่: " + String(dateBuf) +
                     "\n🕒 เวลา: " + String(timeBuf) +
                     "\n📡 ระบบ: Smart Parking";

    String url = "https://api.telegram.org/bot" + botToken +
                 "/sendMessage?chat_id=" + chatID +
                 "&text=" + urlencode(message);

    http.begin(url);
    int httpCode = http.GET();
    DPRINT("Telegram Response: "); DPRINTLN(httpCode);
    http.end();
  }
}

// ---------- SETUP ----------
void setup() {
  if (DEBUG) Serial.begin(115200);
  pinMode(SENSOR1_PIN, INPUT);
  pinMode(SENSOR2_PIN, INPUT);

  gateServo.attach(SERVO_PIN);
  gateServo.write(180);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Smart Parking");
  updateLCD();

  // ตั้งค่า WiFi
  WiFiManager wm;
  lcd.clear(); lcd.print("Connect WiFi...");
  if (!wm.autoConnect("ESP32-ParkingSetup")) {
    lcd.clear(); lcd.print("WiFi Failed!");
    delay(2000);
    ESP.restart();
  }

  configTime(7 * 3600, 0, "pool.ntp.org");
  lcd.clear();
  lcd.print("Smart Parking");
  updateLCD();
  DPRINTLN("System Ready ✅");
}

// ---------- LOOP ----------
void loop() {
  int s1 = digitalRead(SENSOR1_PIN);
  int s2 = digitalRead(SENSOR2_PIN);
  unsigned long now = millis();

  // DEBUG
  DPRINT("S1: "); DPRINT(s1 == LOW ? "BLOCK" : "CLEAR");
  DPRINT(" | S2: "); DPRINT(s2 == LOW ? "BLOCK" : "CLEAR");
  DPRINT(" | Slots: "); DPRINTLN(availableSlots);

  // รถเข้า
  if (!carInProgress && s1 == LOW && s2 == HIGH && availableSlots > 0) {
    carInProgress = true; directionIn = true;
    openGate();
    gateOpenTime = now;
    lcd.clear(); lcd.print("Car IN ->");
  }

  if (carInProgress && directionIn && s2 == LOW) s2WasLow = true;

  if (carInProgress && directionIn && s2WasLow && s1 == HIGH && s2 == HIGH) {
    availableSlots--; if (availableSlots < 0) availableSlots = 0;
    closeGate(); updateLCD();
    sendToTelegram("🚗 รถเข้า", availableSlots);
    carInProgress = false; s2WasLow = false;
  }

  // รถออก
  if (!carInProgress && s2 == LOW && s1 == HIGH && availableSlots < TOTAL_SLOTS) {
    carInProgress = true; directionIn = false;
    openGate();
    gateOpenTime = now;
    lcd.clear(); lcd.print("<- Car OUT");
  }

  if (carInProgress && !directionIn && s1 == LOW) s1WasLow = true;

  if (carInProgress && !directionIn && s1WasLow && s1 == HIGH && s2 == HIGH) {
    availableSlots++; if (availableSlots > TOTAL_SLOTS) availableSlots = TOTAL_SLOTS;
    closeGate(); updateLCD();
    sendToTelegram("🚙 รถออก", availableSlots);
    carInProgress = false; s1WasLow = false;
  }

  // Timeout ปิดไม้กั้น
  if (carInProgress && (now - gateOpenTime > TIMEOUT_MS)) {
    closeGate(); updateLCD();
    carInProgress = false; s1WasLow = s2WasLow = false;
  }

  delay(100);
}

// ---------- Servo ----------
void openGate() {
  gateServo.write(90);
  DPRINTLN("Gate opened");
}
void closeGate() {
  gateServo.write(180);
  DPRINTLN("Gate closed");
}

// ---------- LCD ----------
void updateLCD() {
  lcd.setCursor(0, 1);
  if (availableSlots == 0) lcd.print("Parking FULL!   ");
  else {
    lcd.print("Slot left: ");
    lcd.print(availableSlots);
    lcd.print("   ");
  }
}
