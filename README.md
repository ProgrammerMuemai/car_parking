# car_parking
ดูคลิปวีดีโอได้ที่
https://www.youtube.com/watch?v=NU-3OrfX1fA&t=1s

หรือสอบถามเพิ่มเติมได้ที่
https://www.facebook.com/mengxdiy
https://www.facebook.com/groups/578677129876152

# 🚗 Smart Parking System – Arduino KIT  
**(IR Sensor + Servo + LCD I2C)**

ชุด **Smart Parking System Arduino KIT**  
เหมาะสำหรับการเรียนรู้และสร้างระบบ **ลานจอดรถอัตโนมัติแบบ DIY**  
ใช้งานได้จริง เหมาะทั้งงานเรียน งานสาธิต และโปรเจกต์ส่วนตัว

Designed and developed by **MENGDIY**

---

## 🇹🇭 ภาษาไทย

### 🔹 จุดเด่นของชุด KIT

- ตรวจจับรถ **เข้า–ออกอัตโนมัติ**
- ใช้ IR Sensor 2 ตัว แยกทิศทาง IN / OUT
- ควบคุมไม้กั้นด้วย Servo
- แสดงจำนวนช่องจอดว่างบนจอ LCD I2C (16x2)
- แสดงสถานะ **Full** เมื่อที่จอดเต็ม
- ป้องกันการนับซ้ำ
- เหมาะสำหรับผู้เริ่มต้นจนถึงระดับกลาง
- สามารถต่อยอดเป็นระบบจริงได้

---

### 📦 อุปกรณ์ในชุด

- 2 × IR Sensor  
- 1 × Arduino Uno  
- 1 × Servo SG90  
- 1 × บอร์ดทดลอง (Breadboard)  
- 1 × ชุด Part จากเครื่องพิมพ์ 3 มิติ (โครง / ฐาน / ไม้กั้น)  
- สายไฟแบบ ผู้–เมีย  
- สายไฟแบบ ผู้–ผู้  
- 1 × จอ LCD แบบ I2C (16x2)

> ⚠️ หมายเหตุ: ชุด KIT **ไม่รวมอะแดปเตอร์ไฟเลี้ยง**  
> สามารถใช้ไฟจาก USB ของคอมพิวเตอร์ได้

---

### ⚙️ หลักการทำงานของระบบ

**รถเข้า**
1. IR Sensor ตัวที่ 1 ตรวจจับก่อน  
2. IR Sensor ตัวที่ 2 ตรวจจับถัดมา  
3. Servo เปิดไม้กั้น  
4. จำนวนช่องจอดว่างลดลง

**รถออก**
1. IR Sensor ตัวที่ 2 ตรวจจับก่อน  
2. IR Sensor ตัวที่ 1 ตรวจจับถัดมา  
3. Servo เปิดไม้กั้น  
4. จำนวนช่องจอดว่างเพิ่มขึ้น

**ที่จอดเต็ม**
- เมื่อช่องจอดว่าง = 0  
- จอ LCD แสดงคำว่า **Full**
- ระบบไม่เปิดไม้กั้นฝั่งเข้า

---

### 👨‍🎓 เหมาะสำหรับใคร

- นักเรียน / นักศึกษา (STEM, Arduino, IoT)
- ผู้เริ่มต้นเรียน Arduino
- ครู / อาจารย์ (ใช้สาธิตการสอน)
- Maker / DIY
- ผู้ที่ต้องการต้นแบบระบบจอดรถอัจฉริยะ

---

### 🔧 การต่อยอดเพิ่มเติม

- ระบบจอดรถหลายชั้น
- เชื่อมต่อ WiFi / IoT
- บันทึกข้อมูลขึ้น Cloud
- เพิ่มระบบ RFID / บัตรจอดรถ
- ใช้เป็น Mini Project / Final Project

---

### 📌 ข้อควรทราบ

- IR Sensor เป็นแบบ **Active LOW**
- แนะนำให้ใช้ไฟเลี้ยงที่เสถียรหากใช้งานต่อเนื่อง
- ชิ้นส่วน 3D Print ออกแบบเพื่อการเรียนรู้และสาธิต

---

## 🇺🇸 English

### 🔹 Key Features

- Automatic **vehicle entry & exit detection**
- Uses **2 IR sensors** to determine IN / OUT direction
- Servo motor controlled parking gate
- Displays available parking spaces on **LCD I2C (16x2)**
- Shows **“Full”** status when parking is full
- Prevents double counting
- Beginner to intermediate friendly
- Expandable to real-world applications

---

### 📦 KIT Contents

- 2 × IR Sensors  
- 1 × Arduino Uno  
- 1 × Servo SG90  
- 1 × Breadboard  
- 1 × 3D Printed Parts Set (gate, base, structure)  
- Jumper wires (Male–Female)  
- Jumper wires (Male–Male)  
- 1 × LCD I2C Display (16x2)

> ⚠️ Note: Power adapter is **not included**  
> The system can be powered via USB

---

### ⚙️ System Working Principle

**Vehicle Entry**
1. IR Sensor 1 is triggered first  
2. IR Sensor 2 is triggered next  
3. Servo opens the gate  
4. Available parking spaces decrease

**Vehicle Exit**
1. IR Sensor 2 is triggered first  
2. IR Sensor 1 is triggered next  
3. Servo opens the gate  
4. Available parking spaces increase

**Parking Full**
- When available spaces reach **0**
- LCD displays **“Full”**
- Entry gate will not open

---

### 👨‍🎓 Who Is This KIT For?

- Students (STEM / Arduino / IoT)
- Arduino beginners
- Teachers & instructors
- Makers & DIY enthusiasts
- Smart parking system prototyping

---

### 🔧 Possible Extensions

- Multi-level parking system
- WiFi / IoT integration
- Cloud data logging
- RFID access control
- Final project or prototype use

---

### 📌 Important Notes

- IR sensors are **Active LOW**
- Stable power supply recommended for continuous use
- 3D printed parts are designed for learning & demonstration

---

## 🧾 License & Usage

- Free for learning and DIY projects
- Commercial redistribution of the code is not allowed without permission

---

## 🏷️ Brand

**MENGDIY**  
DIY • Arduino • IoT • Smart Systems
