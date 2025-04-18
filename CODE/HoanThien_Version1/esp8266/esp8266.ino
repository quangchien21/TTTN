#define BLYNK_TEMPLATE_ID "TMPL6VC7gGXKO"
#define BLYNK_TEMPLATE_NAME "Gas Leaks"
#define BLYNK_AUTH_TOKEN "FHAgamNLVV3VsL2dcw0aH8knJ7jdr0QV"

#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <FirebaseESP8266.h>

// 🔹 WiFi Credentials
#define WIFI_SSID "WayHome T1-2"
#define WIFI_PASSWORD "88888888"

// 🔹 Firebase Credentials
#define FIREBASE_HOST "https://toado-xetuhanh-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "5C0AJ1ZI4CaJaYYjhECgcaoumCDse8vyX5siEICN"

// 🔹 Firebase Config
FirebaseData firebaseData;
FirebaseConfig firebaseConfig;
FirebaseAuth firebaseAuth;
FirebaseJson jsonData;

// 🔹 Serial giữa ESP8266 & Arduino
#define RX_PIN 4  // D2
#define TX_PIN 5  // D1
SoftwareSerial mySerial(RX_PIN, TX_PIN);

// 🔹 GPS kết nối qua SoftwareSerial (D7 = RX, D8 = TX)
#define GPS_RX 13  // D7
#define GPS_TX 15  // D8
SoftwareSerial gpsSerial(GPS_RX, GPS_TX);
TinyGPSPlus gps;

// 🔹 Biến trạng thái
char trangThai = '4'; // Mặc định là DỪNG
BlynkTimer timer;
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

// 🔹 Gửi dữ liệu GPS lên Firebase theo nhóm (T0, T1, T2...)
void sendToFirebase(float lat, float lon, String datetime) {
    jsonData.clear();
    jsonData.set("datetime", datetime);
    jsonData.set("latitude", lat);
    jsonData.set("longitude", lon);

    int groupIndex = (gpsIndex < 10) ? 0 : (gpsIndex / 100) + 1;
    String path = "/ToaDo/T" + String(groupIndex) + "/GPS" + String(gpsIndex);

    if (Firebase.updateNode(firebaseData, path, jsonData)) {
        Serial.print("✅ Dữ liệu đã gửi lên Firebase tại: ");
        Serial.println(path);
    } else {
        Serial.print("❌ Lỗi Firebase: ");
        Serial.println(firebaseData.errorReason());
    }

    gpsIndex++;
}

// 🔹 Gửi dữ liệu GPS lên Blynk khi phát hiện khí gas vượt ngưỡng
void sendGPSToBlynk() {
    if (gps.location.isValid() && gps.date.isValid() && gps.time.isValid()) {
        float latitude = gps.location.lat();
        float longitude = gps.location.lng();
        String datetime = getVietnamTime();

        String gpsData = datetime + " | " + String(latitude, 6) + ", " + String(longitude, 6);

        Serial.println("Gửi GPS lên Blynk: " + gpsData);
        Blynk.virtualWrite(V11, gpsData);
    } else {
        Serial.println("🚨 Không có tín hiệu GPS để gửi lên Blynk!");
    }
}

// 🔹 Gửi dữ liệu GPS lên Firebase mỗi 5 giây
void sendGPSToFirebase() {
    if (gps.location.isValid()) {
        float latitude = gps.location.lat();
        float longitude = gps.location.lng();
        String datetime = getVietnamTime();

        Serial.print("Gửi GPS lên Firebase: ");
        Serial.println(datetime + " | " + String(latitude, 6) + ", " + String(longitude, 6));

        sendToFirebase(latitude, longitude, datetime);
        
    } else {
        Serial.println("🚨 Chưa có dữ liệu GPS để gửi lên Firebase!");
    }
}

void setup() {
    Serial.begin(115200);
    mySerial.begin(9600);   // Giao tiếp với Arduino
    gpsSerial.begin(19200);  // Giao tiếp với GPS

    connectWiFi();
    connectFirebase();
    Blynk.begin(BLYNK_AUTH_TOKEN, WIFI_SSID, WIFI_PASSWORD);

    Serial.println("ESP8266 kết nối Blynk...");

    // Cứ 5 giây gửi GPS lên Firebase
    timer.setInterval(5000L, sendGPSToFirebase);
    
    // Gửi lệnh DỪNG ban đầu
    mySerial.write('4');
    Serial.println("Gửi DUNG đến Arduino");
}

void loop() {
    while (gpsSerial.available()) {
        gps.encode(gpsSerial.read());
    }

    Blynk.run();
    timer.run();

    // Kiểm tra dữ liệu từ Arduino
    if (mySerial.available()) {
        String data = mySerial.readStringUntil('\n'); // Đọc dữ liệu từ Arduino
        int giatri_gas = data.toInt(); // Chuyển đổi sang số nguyên

        Serial.print("🌡 Nhận từ Arduino giá trị khí gas: ");
        Serial.println(giatri_gas);

        // Đẩy dữ liệu lên Blynk (Virtual Pin V9)
        Blynk.virtualWrite(V12, giatri_gas);

        if (giatri_gas > 400) {
            Serial.println("🚨 Phát hiện khí gas vượt ngưỡng!");
            Blynk.virtualWrite(V8, "⚠️ Phát hiện khí gas nguy hiểm!");
            Blynk.virtualWrite(V10, 1);

            // Gửi GPS lên Blynk nếu phát hiện khí gas
            sendGPSToBlynk();
        } else {
            Blynk.virtualWrite(V8, "Bình thường");
            Blynk.virtualWrite(V10, 0);
        }
    }
}

// 🔹 Gửi lệnh đến Arduino nếu trạng thái thay đổi
void guiLenh(char lenh) {
    if (trangThai != lenh) {
        trangThai = lenh;
        mySerial.write(lenh);
        Serial.print("Gửi lệnh: ");
        Serial.println(lenh);
    }
}

// 🔹 Nhận lệnh từ Blynk và gửi đến Arduino
BLYNK_WRITE(V0) { int value = param.asInt(); guiLenh(value == 1 ? '0' : '4'); }
BLYNK_WRITE(V1) { int value = param.asInt(); guiLenh(value == 1 ? '1' : '4'); }
BLYNK_WRITE(V3) { int value = param.asInt(); guiLenh(value == 1 ? '2' : '4'); }
BLYNK_WRITE(V2) { int value = param.asInt(); guiLenh(value == 1 ? '3' : '4'); }
BLYNK_WRITE(V5) { int value = param.asInt(); guiLenh(value == 1 ? '5' : '6'); }
