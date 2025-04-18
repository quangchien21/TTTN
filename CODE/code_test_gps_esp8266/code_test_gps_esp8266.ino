

#define BLYNK_TEMPLATE_ID "TMPL6VC7gGXKO"
#define BLYNK_TEMPLATE_NAME "Gas Leaks"
#define BLYNK_AUTH_TOKEN "FHAgamNLVV3VsL2dcw0aH8knJ7jdr0QV"

#define WIFI_SSID "WayHome T1-2"
#define WIFI_PASSWORD "88888888"

#define FIREBASE_HOST "toado-xetuhanh-default-rtdb.firebaseio.com"  // Không có "https://"
#define FIREBASE_AUTH "5C0AJ1ZI4CaJaYYjhECgcaoumCDse8vyX5siEICN"

#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <FirebaseESP8266.h>

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = WIFI_SSID;
char pass[] = WIFI_PASSWORD;

static const int RXPin = 13, TXPin = 15; // // D7 = RX, D8 = TX || Sử dụng chân D7 (GPIO13) cho RX và D8 (GPIO15) cho TX
static const uint32_t GPSBaud = 9600;

TinyGPSPlus gps;
SoftwareSerial gpsSerial(RXPin, TXPin);

// Khai báo cấu hình Firebase
FirebaseData firebaseData;
FirebaseAuth firebaseAuth;
FirebaseConfig firebaseConfig;

void setup() {
    Serial.begin(115200);
    gpsSerial.begin(GPSBaud);

    // Kết nối WiFi
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("WiFi Connected!");

    // Kết nối Blynk
    Blynk.begin(auth, ssid, pass);

    // Cấu hình Firebase
    firebaseConfig.host = FIREBASE_HOST;  // Không có "https://"
    firebaseConfig.signer.tokens.legacy_token = FIREBASE_AUTH;
    Firebase.begin(&firebaseConfig, &firebaseAuth);
    Firebase.reconnectWiFi(true);

    Serial.println("GPS Module Initialized");
    Serial.println("Time\tLatitude\tLongitude");
}

    void loop() {
    Blynk.run();
    while (gpsSerial.available() > 0) {
        gps.encode(gpsSerial.read());
    }

    if (gps.location.isUpdated()) {
        String timeStr;
        if (gps.time.isValid()) {
            timeStr = String(gps.time.hour()) + ":" + String(gps.time.minute()) + ":" + String(gps.time.second());
        } else {
            timeStr = "Invalid Time";
        }

        float latitude = gps.location.lat();
        float longitude = gps.location.lng();

        // In ra Serial Monitor trên cùng 1 dòng
        Serial.println(timeStr + "\t" + String(latitude, 6) + "\t" + String(longitude, 6));

        // Gửi dữ liệu lên Blynk dưới dạng 1 chuỗi
        String blynkData = timeStr + " | " + String(latitude, 6) + " | " + String(longitude, 6);
        Blynk.virtualWrite(V6, blynkData);

        // Gửi dữ liệu lên Firebase dưới dạng một dòng
        String firebaseDataString = timeStr + " | " + String(latitude, 6) + " | " + String(longitude, 6);
        Firebase.setString(firebaseData, "/GPS", firebaseDataString);
    }
}


