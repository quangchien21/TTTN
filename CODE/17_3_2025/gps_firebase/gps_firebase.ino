#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>

// 🔹 WiFi Credentials
#define WIFI_SSID "TrieuNgan"
#define WIFI_PASSWORD "19801981"

// 🔹 Firebase Credentials
#define FIREBASE_HOST "https://toado-xetuhanh-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "5C0AJ1ZI4CaJaYYjhECgcaoumCDse8vyX5siEICN"

// 🔹 Firebase Config
FirebaseData firebaseData;
FirebaseConfig firebaseConfig;
FirebaseAuth firebaseAuth;
FirebaseJson jsonData;

// 🔹 GPS UART Pins (D7 = RX, D8 = TX)
#define RX_PIN 13  // D7 trên NodeMCU
#define TX_PIN 15  // D8 trên NodeMCU

SoftwareSerial gpsSerial(RX_PIN, TX_PIN);
TinyGPSPlus gps;

// 🔹 Biến đếm GPS
int gpsIndex = 0; // Bắt đầu từ GPS0

// 🔹 Kết nối WiFi
void connectWiFi() {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Đang kết nối WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    Serial.println("\n✅ WiFi đã kết nối!");
}

// 🔹 Kết nối Firebase
void connectFirebase() {
    firebaseConfig.host = FIREBASE_HOST;
    firebaseConfig.signer.tokens.legacy_token = FIREBASE_AUTH;

    Firebase.begin(&firebaseConfig, &firebaseAuth);
    Firebase.reconnectWiFi(true);

    Serial.println("✅ Firebase đã kết nối!");
}

// 🔹 Gửi dữ liệu GPS lên Firebase theo nhóm (T0, T1, T2...)
void sendToFirebase(float lat, float lon, String datetime) {
    jsonData.clear();
    jsonData.set("datetime", datetime);
    jsonData.set("latitude", lat);
    jsonData.set("longitude", lon);

    // Xác định groupIndex
    int groupIndex = (gpsIndex < 10) ? 0 : (gpsIndex / 100) + 1;

    // Tạo đường dẫn động: "/ToaDo/T0/GPS0", "/ToaDo/T1/GPS10", ...
    String path = "/ToaDo/T" + String(groupIndex) + "/GPS" + String(gpsIndex);

    if (Firebase.updateNode(firebaseData, path, jsonData)) {
        Serial.print("✅ Dữ liệu đã gửi lên Firebase tại: ");
        Serial.println(path);
    } else {
        Serial.print("❌ Lỗi Firebase: ");
        Serial.println(firebaseData.errorReason());
    }

    gpsIndex++;  // Tăng index để lưu dữ liệu mới
}


// 🔹 Lấy thời gian từ GPS và chuyển sang giờ Việt Nam
String getVietnamTime() {
    if (gps.time.isValid() && gps.date.isValid()) {
        int hourVN = gps.time.hour() + 7;
        if (hourVN >= 24) hourVN -= 24;

        char datetime[25];
        sprintf(datetime, "%02d/%02d/%04d %02d:%02d:%02d",
                gps.date.day(), gps.date.month(), gps.date.year(),
                hourVN, gps.time.minute(), gps.time.second());

        return String(datetime);
    } else {
        return "Chưa có thời gian GPS";
    }
}

void setup() {
    Serial.begin(115200);
    gpsSerial.begin(9600);

    connectWiFi();
    connectFirebase();
}

void loop() {
    while (gpsSerial.available()) {
        gps.encode(gpsSerial.read());
    }

    if (gps.location.isUpdated()) {
        float latitude = gps.location.lat();
        float longitude = gps.location.lng();
        String datetime = getVietnamTime();

        Serial.print("🛰 Vĩ độ: ");
        Serial.print(latitude, 6);
        Serial.print(" | Kinh độ: ");
        Serial.print(longitude, 6);
        Serial.print(" | Thời gian: ");
        Serial.println(datetime);

        sendToFirebase(latitude, longitude, datetime);
    }

    // delay(5000); // Gửi dữ liệu mỗi 5 giây
    delay(300);
}
