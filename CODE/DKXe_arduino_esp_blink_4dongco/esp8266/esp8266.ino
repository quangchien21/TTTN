#define BLYNK_TEMPLATE_ID "TMPL6VC7gGXKO"
#define BLYNK_TEMPLATE_NAME "Gas Leaks"
#define BLYNK_AUTH_TOKEN "FHAgamNLVV3VsL2dcw0aH8knJ7jdr0QV"

#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SoftwareSerial.h>

// Thông tin WiFi
char auth[] = "FHAgamNLVV3VsL2dcw0aH8knJ7jdr0QV";
char ssid[] = "WayHome T1-2";
char pass[] = "88888888";

// Khai báo các chân RX và TX cho SoftwareSerial
#define RX_PIN 4 // D2
#define TX_PIN 5 // D1
SoftwareSerial mySerial(RX_PIN, TX_PIN);

// Biến lưu trạng thái hiện tại
char trangThai = '4'; // Mặc định là DỪNG

void setup() {
  Serial.begin(115200);   // Giao tiếp Serial với máy tính
  mySerial.begin(9600);   // Giao tiếp với Arduino qua SoftwareSerial
  Blynk.begin(auth, ssid, pass);
  Serial.println("ESP8266 UART Communication with Blynk");

  // Mặc định gửi DỪNG khi khởi động
  mySerial.write('4');
  Serial.println("Gửi DUNG đến Arduino");
}

void loop() {
   Blynk.run();
  // Kiểm tra dữ liệu từ Arduino
    if (mySerial.available()) {
        String data = mySerial.readStringUntil('\n'); // Đọc dữ liệu từ Arduino
        int giatri_gas = data.toInt(); // Chuyển đổi sang số nguyên

        Serial.print("Nhận từ Arduino giá trị cảm biến khí gas: ");
        Serial.println(giatri_gas);

        // Đẩy dữ liệu lên Blynk (Virtual Pin V9)
        Blynk.virtualWrite(V9, giatri_gas);
        if(giatri_gas > 400){
          Blynk.virtualWrite(V8, "Phát hiện khí gas nguy hiểm!");
          Blynk.virtualWrite(V10, 1);
        }
        else if (giatri_gas <= 400){
          Blynk.virtualWrite(V8, "Bình thường");
          Blynk.virtualWrite(V10, 0);
        }
        
        
    }
}

// Hàm gửi lệnh đến Arduino nếu trạng thái thay đổi
void guiLenh(char lenh) {
  if (trangThai != lenh) {
    trangThai = lenh;
    mySerial.write(lenh);
    Serial.print("Gửi lệnh: ");
    Serial.println(lenh);
  }
}

// Nhận lệnh từ Blynk và gửi đến Arduino
BLYNK_WRITE(V0) { // Tiến
  int value = param.asInt();
  if (value == 1) {
    guiLenh('0'); // Gửi lệnh tiến
  } else {
    guiLenh('4');
  }
}

BLYNK_WRITE(V1) { // Lùi
  int value = param.asInt();
  if (value == 1) {
    guiLenh('1'); // Gửi lệnh lùi
  }
  else {
    guiLenh('4');
  }
}

BLYNK_WRITE(V3) { // Sang Trái
  int value = param.asInt();
  if (value == 1) {
    guiLenh('2'); // Gửi lệnh rẽ trái
  } else {
    guiLenh('4');
  }
}

BLYNK_WRITE(V2) { // Sang Phải
  int value = param.asInt();
  if (value == 1) {
    guiLenh('3'); // Gửi lệnh rẽ phải
  } else {
    guiLenh('4');
  }
}

BLYNK_WRITE(V5) { // Sang Phải
  int value = param.asInt();
  if (value == 1) {
    guiLenh('5'); // Gửi lệnh đổi chế độ
  } else {
    guiLenh('6');
  }
}