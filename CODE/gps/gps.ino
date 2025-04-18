#include <SoftwareSerial.h>
#include <TinyGPS++.h>

// Khai báo UART cho GPS
#define RXPin 13  // D7 (GPIO13) - GPS TX
#define TXPin 15  // D8 (GPIO12) - GPS RX
#define GPSBaud 9600

TinyGPSPlus gps;
SoftwareSerial gpsSerial(RXPin, TXPin);

void setup() {
  Serial.begin(115200);
  gpsSerial.begin(GPSBaud);
  Serial.println("Đang khởi động GPS...");
}

void loop() {
  while (gpsSerial.available()) {
    gps.encode(gpsSerial.read());  // Giải mã dữ liệu GPS
  }

  if (gps.location.isUpdated()) {
    Serial.print("Tọa độ: ");
    Serial.print(gps.location.lat(), 6);
    Serial.print(", ");
    Serial.println(gps.location.lng(), 6);
  }
}
