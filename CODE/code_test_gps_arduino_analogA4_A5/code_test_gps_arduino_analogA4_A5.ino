#include <SoftwareSerial.h>
#include <TinyGPS++.h>

static const int RXPin = A4, TXPin = A5; // Sử dụng chân Analog A4, A5 làm Serial
static const uint32_t GPSBaud = 9600;

TinyGPSPlus gps;
SoftwareSerial gpsSerial(RXPin, TXPin);

void setup() {
    Serial.begin(115200);
    gpsSerial.begin(GPSBaud);
    Serial.println("Đang khởi động GPS...");
}

void loop() {
    while (gpsSerial.available() > 0) {
        gps.encode(gpsSerial.read());
    }

    if (gps.location.isUpdated()) {
        Serial.print("Vĩ độ: ");
        Serial.println(gps.location.lat(), 6);
        Serial.print("Kinh độ: ");
        Serial.println(gps.location.lng(), 6);
    } else {
        Serial.println("Đang tìm tín hiệu GPS...");
    }
    delay(1000);
}
