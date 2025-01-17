#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>

hd44780_I2Cexp lcd;
Servo myservo;

#define IR_1 13       // Sensor for car entry
#define IR_2 14       // Sensor for car exit
#define IR_SLOT_1 34  // Parking slot 1
#define IR_SLOT_2 35  // Parking slot 2
#define IR_SLOT_3 32  // Parking slot 3
#define IR_SLOT_4 33  // Parking slot 4

// Green LEDs
#define LED_GREEN_1 19
#define LED_GREEN_2 18
#define LED_GREEN_3 5
#define LED_GREEN_4 17

// Red LEDs
#define LED_RED_1 23
#define LED_RED_2 16
#define LED_RED_3 4
#define LED_RED_4 27

const int SERVO_PIN = 26;
const int LCD_COLS = 16;
const int LCD_ROWS = 2;

const char* ssid = "";
const char* password = "";
const String webAppURL = "";

int spaceAvailable = 4;
int spaceAvailableTotal = 4;
bool sensor1 = false;
bool sensor2 = false;
bool in = false;
bool out = false;

int dailyCount = 0;  // ตัวแปรสำหรับนับจำนวนรถที่เข้าจอดทั้งวัน

bool parkingSlots[4] = { false, false, false, false };  // Parking slot status
String pendingData = "";                                // Buffer for unsent events

unsigned long lastUpdateTime = 0;
const unsigned long updateInterval = 30000;  // Update every 30 seconds

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 25200, 60000);  // GMT+7 for Thailand

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Connecting to WiFi");

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
    lcd.setCursor(0, 1);
    lcd.print("Please wait...");
  }

  Serial.println("Connected to WiFi");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi Connected");
  delay(1000);
  lcd.clear();
  lcdUpdate();

  timeClient.begin();
  timeClient.update();

  pinMode(IR_1, INPUT);
  pinMode(IR_2, INPUT);
  pinMode(IR_SLOT_1, INPUT);
  pinMode(IR_SLOT_2, INPUT);
  pinMode(IR_SLOT_3, INPUT);
  pinMode(IR_SLOT_4, INPUT);

  pinMode(LED_GREEN_1, OUTPUT);
  pinMode(LED_GREEN_2, OUTPUT);
  pinMode(LED_GREEN_3, OUTPUT);
  pinMode(LED_GREEN_4, OUTPUT);

  pinMode(LED_RED_1, OUTPUT);
  pinMode(LED_RED_2, OUTPUT);
  pinMode(LED_RED_3, OUTPUT);
  pinMode(LED_RED_4, OUTPUT);

  myservo.attach(SERVO_PIN);
  myservo.write(90);
}

void loop() {
  checkIn();
  checkOut();
  updateParkingStatus();

  if (millis() - lastUpdateTime >= updateInterval) {
    updateDataToSheet();
    lastUpdateTime = millis();
  }

  if (spaceAvailable == 0) {
    ParkingFull();
  }
}

void checkIn() {
  if (digitalRead(IR_1) == LOW && digitalRead(IR_2) == HIGH && !in ) {
    myservo.write(0);
    if (!sensor1 && !sensor2) {
      sensor1 = true;
      in = true;
    }
  }
  if (digitalRead(IR_1) == HIGH && digitalRead(IR_2) == LOW) {
    if (sensor1 && !sensor2) {
      sensor2 = true;
    }
  }
  if (digitalRead(IR_1) == HIGH && digitalRead(IR_2) == HIGH && sensor1 && sensor2 && in) {
    doorClose();
    Serial.println("Car entered.");
  }
}

void checkOut() {
  if (digitalRead(IR_1) == HIGH && digitalRead(IR_2) == LOW) {
    myservo.write(0);
    if (!sensor1 && !sensor2) {
      sensor2 = true;
      out = true;
    }
  }
  if (digitalRead(IR_1) == LOW && digitalRead(IR_2) == HIGH) {
    if (!sensor1 && sensor2) {
      sensor1 = true;
    }
  }
  if (digitalRead(IR_1) == HIGH && digitalRead(IR_2) == HIGH && sensor1 && sensor2 && out) {
    doorClose();
    Serial.println("Car exited.");
  }
}

void updateParkingStatus() {
  bool previousSlots[4] = { parkingSlots[0], parkingSlots[1], parkingSlots[2], parkingSlots[3] };

  parkingSlots[0] = digitalRead(IR_SLOT_1) == LOW;
  parkingSlots[1] = digitalRead(IR_SLOT_2) == LOW;
  parkingSlots[2] = digitalRead(IR_SLOT_3) == LOW;
  parkingSlots[3] = digitalRead(IR_SLOT_4) == LOW;

  for (int i = 0; i < 4; i++) {
    if (previousSlots[i] != parkingSlots[i]) {
      if (parkingSlots[i]) {
        spaceAvailable--;
        dailyCount++;
        logEvent("IN", i + 1);  // ส่ง action และ slot
      } else {
        spaceAvailable++;
        logEvent("OUT", i + 1);  // ส่ง action และ slot
      }
      lcdUpdate();  // อัปเดต LCD
    }
  }

  digitalWrite(LED_GREEN_1, !parkingSlots[0]);
  digitalWrite(LED_RED_1, parkingSlots[0]);
  digitalWrite(LED_GREEN_2, !parkingSlots[1]);
  digitalWrite(LED_RED_2, parkingSlots[1]);
  digitalWrite(LED_GREEN_3, !parkingSlots[2]);
  digitalWrite(LED_RED_3, parkingSlots[2]);
  digitalWrite(LED_GREEN_4, !parkingSlots[3]);
  digitalWrite(LED_RED_4, parkingSlots[3]);
}

void logEvent(String action, int slot) {
  timeClient.update();                                  // อัปเดตเวลา
  unsigned long epochTime = timeClient.getEpochTime();  // เวลาปัจจุบันในรูปแบบ Unix timestamp

  // คำนวณวันที่
  int currentYear = 1970 + epochTime / 31556926;                // จำนวนปี
  int currentMonth = (epochTime % 31556926) / 2629743 + 1;      // จำนวนเดือน
  int currentDay = ((epochTime % 31556926) % 2629743) / 86400;  // จำนวนวัน

  String date = String(currentYear) + "-" + String(currentMonth) + "-" + String(currentDay);  // วันที่ในรูปแบบ yyyy-MM-dd
  String time = timeClient.getFormattedTime();                                                // เวลาในรูปแบบ HH:mm:ss

  // เพิ่มวันที่และเวลาใน JSON
  String eventJson = "{\"action\":\"" + action + "\", \"slot\":" + String(slot) + ", \"date\":\"" + date + "\", \"time\":\"" + time + "\"}";

  // เพิ่ม event ลงใน buffer
  pendingData += (pendingData == "" ? "" : ",") + eventJson;

  // แสดงผลใน Serial Monitor
  Serial.println("Event logged: " + action + " at slot " + String(slot) + " on " + date + " at " + time);
}

void updateDataToSheet() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // สร้าง JSON ที่รวมข้อมูล dailyCount และ events
    String jsonData = "{\"dailyCount\":" + String(dailyCount) + ", \"events\":[" + pendingData + "]}";
    Serial.println(jsonData);

    http.begin(webAppURL);                               // URL ของ Google Apps Script
    http.addHeader("Content-Type", "application/json");  // ตั้งค่า header เป็น JSON

    // แสดงข้อความบนจอ LCD ว่ากำลังส่งข้อมูล
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Sending data...");
    lcd.setCursor(0, 1);
    lcd.print("Please wait.");

    int httpResponseCode = http.POST(jsonData);

    if (httpResponseCode > 0) {
      Serial.println("Data sent to Google Sheets:");
      Serial.println(http.getString());

      // แสดงข้อความว่า "ส่งข้อมูลสำเร็จ" บนจอ LCD
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Data sent!");
      lcd.setCursor(0, 1);
      lcd.print("Success.");

      delay(2000);  // แสดงข้อความนี้เป็นเวลา 2 วินาที

      pendingData = "";  // ล้าง buffer ของ events
    } else {
      Serial.print("Error sending data: ");
      Serial.println(httpResponseCode);

      // แสดงข้อความว่า "ส่งข้อมูลล้มเหลว" บนจอ LCD
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Data failed!");
      lcd.setCursor(0, 1);
      lcd.print("Retry later.");

      delay(2000);  // แสดงข้อความนี้เป็นเวลา 2 วินาที
    }

    http.end();  // ปิดการเชื่อมต่อ

    // กลับมาที่หน้าจอแสดงสถานะปกติ
    lcdUpdate();
  } else {
    Serial.println("WiFi not connected");

    // แสดงข้อความว่า "WiFi ไม่เชื่อมต่อ" บนจอ LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi failed!");
    lcd.setCursor(0, 1);
    lcd.print("Retry later.");

    delay(2000);  // แสดงข้อความนี้เป็นเวลา 2 วินาที

    // กลับมาที่หน้าจอแสดงสถานะปกติ
    lcdUpdate();
  }
}




void lcdUpdate() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Spaces Available");
  lcd.setCursor(0, 1);
  lcd.print("Level[1]: ");
  lcd.setCursor(10, 1);
  lcd.print(spaceAvailable);
}

void doorClose() {
  delay(100);
  myservo.write(90);
  sensor1 = false;
  sensor2 = false;
  in = false;
  out = false;
  Serial.println("Door closed");
}

void ParkingFull() {
  lcd.setCursor(10, 1);
  lcd.print("Full");
}
