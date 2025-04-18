#define BLYNK_TEMPLATE_ID "TMPL6DXcTCaWc"
#define BLYNK_TEMPLATE_NAME "Gas Leaks"
#define BLYNK_AUTH_TOKEN "c7Y3Vjsd-2isqnOFm6t5owzqKUre8rF3"

#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <FirebaseESP8266.h>

// 🔹 WiFi Credentials
#define WIFI_SSID "Redmi Note 10 Pro"
#define WIFI_PASSWORD "21022003"

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
SoftwareSerial espSerial(RX_PIN, TX_PIN);

// 🔹 GPS kết nối qua SoftwareSerial (D7 = RX, D8 = TX)
#define GPS_RX 15  // D7
#define GPS_TX 13  // D8
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
  if (WiFi.status() != WL_CONNECTED) {
        Serial.println("🚨 WiFi bị mất, không gửi Firebase!");
        return;
    }
    jsonData.clear();
    jsonData.set("datetime", datetime);
    jsonData.set("latitude", lat);
    jsonData.set("longitude", lon);

    int groupIndex = (gpsIndex < 10) ? 0 : (gpsIndex / 100) + 1;
    String path = "/ToaDo/T" + String(groupIndex) + "/GPS" + String(gpsIndex);

    if (Firebase.updateNode(firebaseData, path, jsonData)) {
        Serial.print("✅ Dữ liệu đã gửi lên Firebase tại: ");
        Serial.println(path);
        gpsIndex++;
    } else {
        Serial.print("❌ Lỗi Firebase: ");
        Serial.println(firebaseData.errorReason());
    }

}

// 🔹 Gửi dữ liệu GPS lên Blynk khi phát hiện khí gas vượt ngưỡng
void sendGPSToBlynk() {
    if (gps.location.isValid() && gps.date.isValid() && gps.time.isValid()) {
        float latitude = gps.location.lat();
        float longitude = gps.location.lng();
        String datetime = getVietnamTime();

        String gpsData = datetime + "|" + String(latitude, 6) + "," + String(longitude, 6);

        Serial.println("🚀 Gửi GPS lên Blynk: " + gpsData);
        Blynk.virtualWrite(V5, gpsData);
    } else {
        Serial.println("🚨 Không có tín hiệu GPS để gửi lên Blynk!");
        // Blynk.virtualWrite(V11, gpsData);
    }
}

// 🔹 Gửi dữ liệu GPS lên Firebase mỗi 5 giây
void sendGPSToFirebase() {
    if (gps.location.isValid()) {
        float latitude = gps.location.lat();
        float longitude = gps.location.lng();
        String datetime = getVietnamTime();

        Serial.print("🛰 Gửi GPS lên Firebase: ");
        Serial.println(datetime + " | " + String(latitude, 6) + ", " + String(longitude, 6));

        sendToFirebase(latitude, longitude, datetime);
    } else {
        Serial.println("🚨 Chưa có dữ liệu GPS để gửi lên Firebase!");
        // sendToFirebase(156.238, 253.265, "16/03/2025 15:30:45");
    }
}

void setup() {
    Serial.begin(115200);
    espSerial.begin(9600);   // Giao tiếp với Arduino
    gpsSerial.begin(9600);  // Giao tiếp với GPS

    espSerial.setTimeout(200);  // 🔹 Tránh bị treo khi không có \n

    connectWiFi();
    connectFirebase();
    Blynk.begin(BLYNK_AUTH_TOKEN, WIFI_SSID, WIFI_PASSWORD);

    Serial.println("ESP8266 kết nối Blynk...");

    // Cứ 7 giây gửi GPS lên Firebase
    timer.setInterval(7000L, sendGPSToFirebase);
    Blynk.virtualWrite(V5, " ");
}

void handleSerialInput() {
  if (espSerial.available()) {
    String data = espSerial.readStringUntil('\n');

    if (data.length() == 0) return;  // Không làm gì nếu chuỗi rỗng

    if (data.startsWith("Gas:")) {
      int giatri_gas = data.substring(4).toInt();
      Blynk.virtualWrite(V4, data.substring(4).toInt());
      Serial.println("Gas: " +  String(giatri_gas));

      if (giatri_gas >= 500) {
            Serial.println("🚨 Phát hiện khí gas vượt ngưỡng!");
            Blynk.virtualWrite(V7, "🚨 Phát hiện vị trí rò rỉ!");
            Blynk.virtualWrite(V6, 1);

            // Gửi GPS lên Blynk nếu phát hiện khí gas
            sendGPSToBlynk();
      } else if(giatri_gas < 500 && giatri_gas >= 300) {
            Blynk.virtualWrite(V7, "⚠️ Khí gas nguy hiểm");
            Blynk.virtualWrite(V6, 0);
      } else {
            Blynk.virtualWrite(V7, "✅ Bình thường");
            Blynk.virtualWrite(V6, 0);
      }

    } else if (data.startsWith("V8:")) {
      Blynk.virtualWrite(V8, data.substring(3).toInt());
      int mode = data.substring(3).toInt();
      if(mode == 0){
        Serial.println("Mode Auto");
      } else{
        Serial.println("Mode Thủ Công");
      } 
    }
  }
}

void loop() {
    if (gpsSerial.available()) {
        gps.encode(gpsSerial.read());
    }
    Blynk.run();
    timer.run();
    handleSerialInput();
    }

//🔹 Gửi lệnh đến Arduino nếu trạng thái thay đổi
void guiLenh(int lenh) {
    if (trangThai != lenh) {
        trangThai = lenh;
        espSerial.println("V:" + String(lenh));
        Serial.print("Gửi lệnh: ");
        Serial.println(lenh);
    }else {
        Serial.print("⚠️ Lệnh không thay đổi, không gửi: ");
        Serial.println(lenh);
    }
}

// 🔹 Nhận lệnh từ Blynk và gửi đến Arduino
BLYNK_WRITE(V8) { 
  int value = param.asInt(); 
  if(value == 1){
    espSerial.println("V:5");
    Serial.print("Gửi lệnh: 5" );
    
  }
  else{
     espSerial.println("V:6");
     Serial.print("Gửi lệnh: 6" );
  }                                     
}
BLYNK_WRITE(V0) { int value = param.asInt(); Serial.println("Giá trị nhận được từ V0: " + String(value)); guiLenh(value == 1 ? '7' : '4'); }
BLYNK_WRITE(V1) { int value = param.asInt(); Serial.println("Giá trị nhận được từ V0: " + String(value)); guiLenh(value == 1 ? '8' : '4'); }
BLYNK_WRITE(V2) { int value = param.asInt(); Serial.println("Giá trị nhận được từ V0: " + String(value)); guiLenh(value == 1 ? '2' : '4'); }
BLYNK_WRITE(V3) { int value = param.asInt(); Serial.println("Giá trị nhận được từ V0: " + String(value)); guiLenh(value == 1 ? '3' : '4'); }

