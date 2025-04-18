#define BLYNK_TEMPLATE_ID "TMPL6VC7gGXKO"
#define BLYNK_TEMPLATE_NAME "Gas Leaks"
#define BLYNK_AUTH "FHAgamNLVV3VsL2dcw0aH8knJ7jdr0QV"  // 🔹 Chỉ khai báo một lần

#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <FirebaseESP8266.h>

// 🔹 Thông tin WiFi
char ssid[] = "100k 1 luot";
char pass[] = "20082002";

// 🔹 Thông tin Firebase
#define FIREBASE_HOST "toado-xetuhanh-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "5C0AJ1ZI4CaJaYYjhECgcaoumCDse8vyX5siEICN"

// 🔹 Cấu hình Firebase
FirebaseConfig config;
FirebaseAuth auth;
FirebaseData firebaseData;

// 🔹 Cấu hình Serial
#define RX_PIN 4   // D2 - Nhận dữ liệu từ Arduino
#define TX_PIN 5   // D1 - Gửi dữ liệu đến Arduino
SoftwareSerial mySerial(RX_PIN, TX_PIN);

// 🔹 GPS Serial
#define GPS_RX 13  // D7 - TX từ GPS
#define GPS_TX 15  // D8 - RX từ GPS
SoftwareSerial gpsSerial(GPS_RX, GPS_TX);
TinyGPSPlus gps;

BlynkTimer timer;

void sendGPSData() {
    if (gps.location.isValid()) {
        float lat = gps.location.lat();
        float lng = gps.location.lng();

        int hourVN = gps.time.hour() + 7;
        if (hourVN >= 24) hourVN -= 24;

        String gpsData = "Lat: " + String(lat, 6) + ", Lng: " + String(lng, 6) + 
                         " | " + String(hourVN) + ":" + String(gps.time.minute());

        Serial.println("GPS Data: " + gpsData);

        Blynk.virtualWrite(V6, gpsData);
        Firebase.setFloat(firebaseData, "/gps/latitude", lat);
        Firebase.setFloat(firebaseData, "/gps/longitude", lng);
    } else {
        Serial.println("⏳ Đang tìm tín hiệu GPS...");
        Blynk.virtualWrite(V6, "GPS Lost...");
    }
}

void setup() {
    Serial.begin(115200);
    mySerial.begin(9600);
    gpsSerial.begin(9600);

    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\n✅ Kết nối WiFi thành công!");

    Blynk.begin(BLYNK_AUTH, ssid, pass);
    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);

    timer.setInterval(5000L, sendGPSData);
}

void loop() {
    while (gpsSerial.available()) {
        gps.encode(gpsSerial.read());
    }

    Blynk.run();
    timer.run();
}
