#include <Servo.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

Servo myservo;
SoftwareSerial ss(A1, A2);
SoftwareSerial mySerial(12, 13);  // Serial giao tiếp ESP8266
TinyGPSPlus gps;

const int trig = 7, echo = 4, enbA = 3, in1 = 5, in2 = 6, in3 = 9, in4 = 10, enbB = 11, dongcoservo = 8;
#define MQ2pin A0
#define Threshold 400

int mode = 0;
bool canhbao = false;
int gioihan = 30;
unsigned long lastGasCheck = 0;
unsigned long lastGPSUpdate = 0;

void setup() {
  myservo.attach(dongcoservo);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(enbA, OUTPUT);
  pinMode(enbB, OUTPUT);
  myservo.write(90);
  Serial.begin(9600);
  mySerial.begin(9600);
  ss.begin(9600);
}

void loop() {
  // if (millis() - lastGasCheck > 2000) {
  //   sendSensorData();
  //   lastGasCheck = millis();
  // }

  // if (millis() - lastGPSUpdate > 4000) {
  //   toado();
  //   lastGPSUpdate = millis();
  // }

  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    handleCommand(command);
  }

  autoo(mode);
}

void handleCommand(String command) {
  int c = command.substring(3).toInt();
  if (command.startsWith("V0:")) { if (c == 0) dithang(); }
  else if (command.startsWith("V1:")) { if (c == 1) dilui(); }
  else if (command.startsWith("V2:")) { if (c == 2) disangphai(); }
  else if (command.startsWith("V3:")) { if (c == 3) disangtrai(); }
  else if (command.startsWith("V4:")) { if (c == 4) dunglai(); }
  else if (command.startsWith("V5:")) { mode = c; }
  // sendSensorData();
}

void autoo(int mode) {
  if (mode == 1) {
    int khoangcach = dokhoangcach();
    if (khoangcach > gioihan || khoangcach == 0) {
      dithang();
    } else {
      dunglai(); delay(300);
      quaycbsangtrai(); int khoangcachtrai = dokhoangcach();
      quaycbsangphai(); int khoangcachphai = dokhoangcach();
      resetservo();
      if (khoangcachphai >= gioihan || khoangcachtrai >= gioihan) {
        if (khoangcachphai > khoangcachtrai) disangphai();
        else disangtrai();
      } else {
        dilui(); delay(500); dunglai();
        mode = 0; //toado();
      }
      delay(400); dunglai(); delay(300);
    }
  }
}

void dithang() { motorControl(100, 100, 0, 1, 0, 1); }
void dunglai() { motorControl(0, 0, 0, 0, 0, 0); }
void disangphai() { motorControl(100, 50, 0, 0, 1, 0); }
void disangtrai() { motorControl(50, 100, 1, 0, 0, 0); }
void dilui() { motorControl(100, 100, 1, 0, 1, 0); }

void motorControl(int speedA, int speedB, int a1, int a2, int b1, int b2) {
  analogWrite(enbA, speedA);
  analogWrite(enbB, speedB);
  digitalWrite(in1, a1);
  digitalWrite(in2, a2);
  digitalWrite(in3, b1);
  digitalWrite(in4, b2);
}

int dokhoangcach() {
  digitalWrite(trig, LOW); delayMicroseconds(2);
  digitalWrite(trig, HIGH); delayMicroseconds(10);
  digitalWrite(trig, LOW);
  return pulseIn(echo, HIGH) / 58;
}

void quaycbsangtrai() { myservo.write(150); delay(1000); }
void quaycbsangphai() { myservo.write(30); delay(1000); }
void resetservo() { myservo.write(90); delay(500); }

// void sendSensorData() {
//   float sensorValue = analogRead(MQ2pin);
//   Serial.println("V7:" + String(sensorValue));
//   if (sensorValue > Threshold) {
//     canhbao = true;
//     Serial.println(" | Smoke detected!");
//   } else {
//     canhbao = false;
//   }
// }

// void toado() {
//   while (ss.available() > 0) gps.encode(ss.read());
//   if (gps.location.isValid()) {
//     float latitude = gps.location.lat();
//     float longitude = gps.location.lng();
//     Serial.println("V6:" + String(latitude, 6) + " " + String(longitude, 6));
//   }
// }
