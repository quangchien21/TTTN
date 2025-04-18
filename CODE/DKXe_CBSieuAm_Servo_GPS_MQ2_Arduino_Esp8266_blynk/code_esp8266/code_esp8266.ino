#define BLYNK_TEMPLATE_ID "TMPL6VC7gGXKO"
#define BLYNK_TEMPLATE_NAME "Gas Leaks"
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char auth[] = "FHAgamNLVV3VsL2dcw0aH8knJ7jdr0QV"; // Thay bằng mã Blynk của bạn
char ssid[] = "LOCAL COFFEE - TRONG NHA";  // Thay bằng tên WiFi của bạn
char pass[] = "88888888";  // Thay bằng mật khẩu WiFi

BlynkTimer timer;

void handleSerialInput() {
  if (Serial.available()) {
    String data = Serial.readStringUntil('\n');

    if (data.startsWith("V0:")) {
      Blynk.virtualWrite(V0, data.substring(3).toInt());
    } else if (data.startsWith("V1:")) {
      Blynk.virtualWrite(V1, data.substring(3).toInt());
    } else if (data.startsWith("V2:")) {
      Blynk.virtualWrite(V2, data.substring(3).toInt());
    } else if (data.startsWith("V3:")) {
      Blynk.virtualWrite(V3, data.substring(3).toInt());
    } else if (data.startsWith("V4:")) {
      Blynk.virtualWrite(V4, data.substring(3));
    } else if (data.startsWith("V5:")) {
      int mode = data.substring(3).toInt();
      Blynk.virtualWrite(V5, mode);
    // } else if (data.startsWith("V6:")) {
    //   Blynk.virtualWrite(V6, data.substring(3)); // Tọa độ GPS
    // } else if (data.startsWith("V7:")) {
    //   int gasValue = data.substring(3).toInt();
    //   Blynk.virtualWrite(V7, gasValue);
    // }
  }
}
}

void sendCommandToArduino(String command) {
  Serial.println(command);
}

BLYNK_WRITE(V0) {
  sendCommandToArduino("V0:" + String(param.asInt()));
}

BLYNK_WRITE(V1) {
  sendCommandToArduino("V1:" + String(param.asInt()));
}

BLYNK_WRITE(V2) {
  sendCommandToArduino("V2:" + String(param.asInt()));
}

BLYNK_WRITE(V3) {
  sendCommandToArduino("V3:" + String(param.asInt()));
}

BLYNK_WRITE(V4) {
  sendCommandToArduino("V4:" + String(param.asInt()));
}

BLYNK_WRITE(V5) {
  sendCommandToArduino("V5:" + String(param.asInt()));
}

void setup() {
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
  timer.setInterval(500L, handleSerialInput);
}

void loop() {
  Blynk.run();
  timer.run();
}
