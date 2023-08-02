#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

#define IR_1 2
#define IR_2 3
#define LED_R 11
#define LED_G 12
Servo myservo;


int spaceAvailable = 4;
int spaceAvailableTotal = 4;
bool sensor1 = false;
bool sensor2 = false;
bool in = false;
bool out = false;


void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print(" PARKING SYSTEM ");
  //lcd.setCursor(0, 1);
  //lcd.print("     MENGDIY     ");
  delay(2000);
  lcd.clear();
  lcdUpdate();

  pinMode(IR_1, INPUT);
  pinMode(IR_2, INPUT);
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  digitalWrite(LED_R, HIGH);
  digitalWrite(LED_G, HIGH);

  myservo.attach(9);
  myservo.write(90);
}

void loop() {
  checkIn();
  checkOut();
  if (spaceAvailable == 0) {
    ParkingFull();
  }
  if (spaceAvailable >= 1) {
    digitalWrite(LED_R, HIGH);
    digitalWrite(LED_G, LOW);
  }
  delay(10);
}

void checkIn() {
  if (digitalRead(IR_1) == LOW && digitalRead(IR_2) == HIGH && in == false && spaceAvailable > 0) {
    myservo.write(0);
    if (sensor1 == false && sensor2 == false) {
      sensor1 = true;
      in = true;
      Serial.println("checkIn 1");
    }
  }
  if (digitalRead(IR_1) == HIGH && digitalRead(IR_2) == LOW) {
    if (sensor1 == true && sensor2 == false) {
      sensor2 = true;
      Serial.println("checkIn 2");
    }
  }
  if (digitalRead(IR_1) == HIGH && digitalRead(IR_2) == HIGH && sensor1 && sensor2 && in) {
    doorClose();
    spaceAvailable--;
    lcdUpdate();
    Serial.println("spaceAvailable--");
  }
}

void checkOut() {
  if (digitalRead(IR_1) == HIGH && digitalRead(IR_2) == LOW && spaceAvailableTotal - spaceAvailable != 0) {
    myservo.write(0);
    if (sensor1 == false && sensor2 == false) {
      sensor2 = true;
      out = true;
      Serial.println("checkOut 1");
    }
  }
  if (digitalRead(IR_1) == LOW && digitalRead(IR_2) == HIGH) {
    if (sensor1 == false && sensor2 == true) {
      sensor1 = true;
      Serial.println("checkOut 2");
    }
  }
  if (digitalRead(IR_1) == HIGH && digitalRead(IR_2) == HIGH && sensor1 && sensor2 && out) {
    doorClose();
    spaceAvailable++;
    lcdUpdate();
    Serial.println("spaceAvailable++");
  }
}

void doorClose() {
  myservo.write(90);
  delay(500);
  sensor1 = false;
  sensor2 = false;
  in = false;
  out = false;
  lcdUpdate();
}

void ParkingFull() {
  lcd.setCursor(10, 1);
  lcd.print("Full");
  digitalWrite(LED_R, LOW);
  digitalWrite(LED_G, HIGH);
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