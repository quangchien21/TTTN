#include <Servo.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>

Servo myservo;
SoftwareSerial mySerial(12, 13); // RX, TX

// SoftwareSerial ss(A1, A2);
// TinyGPSPlus gps;

const int trig = 7;
const int echo = 4;
int enbA = 3;
int in1 = 5;
int in2 = 6;
int in3 = 9;
int in4 = 10;
int enbB = 11;
int dongcoservo = 8;

// Cảm biến khí gas
int gas_sensor = A0; // Cảm biến khí gas nối chân A0
int giatri_gas;
const int nguong_gas = 400; // Ngưỡng phát hiện khí gas

int gioihan = 12;
unsigned long thoigian;
int khoangcach;
int khoangcachtrai;
int khoangcachphai;
int mode = 1;  // 1 - tu dong; 0 - thu cong

void dokhoangcach();
void dithang();
void disangtrai();
void disangphai();
void dilui();
void dunglai();
void quaycbsangphai();
void quaycbsangtrai();
void resetservo();

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
    pinMode(gas_sensor, INPUT);

    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
    digitalWrite(enbA, LOW);
    digitalWrite(enbB, LOW);
    myservo.write(90);
    delay(500);
    Serial.begin(115200);
    mySerial.begin(9600);
    // ss.begin(9600);
}

void loop() {
    giatri_gas = analogRead(gas_sensor); // Đọc giá trị cảm biến khí gas
    Serial.print("Phát hiện khí gas: ");
    Serial.println(giatri_gas);
    mySerial.println(giatri_gas);
    delay(100);

    if (giatri_gas > nguong_gas) { 
        Serial.println("CẢNH BÁO! Phát hiện khí gas nguy hiểm!");
        dunglai(); // Dừng xe lại
        // toado(); // gửi tọa độ
        delay(2000);
    } else {

        if (mySerial.available()) {
            char c = mySerial.read();
            Serial.println(c);
             // Kiểm tra chuyển mode
            if (c == '5') {
                mode = 1; // Chế độ tự động
                Serial.println("Chuyển sang chế độ TỰ ĐỘNG");

                khoangcach = 0;
                dokhoangcach();
                Serial.println(khoangcach);
                if (khoangcach > gioihan || khoangcach == 0)
                {
                  dithang();
                  Serial.println("Di toi");
                }
                else
                {
                  dunglai(); delay(100);
                  quaycbsangtrai(); dokhoangcach(); khoangcachtrai = khoangcach;
                  delay(100);
                  quaycbsangphai(); dokhoangcach(); khoangcachphai = khoangcach;
                  delay(100);
                  resetservo();
                  
                  if (khoangcachphai >= gioihan && khoangcachtrai >= gioihan) {
                    if (khoangcachphai > khoangcachtrai) {
                      disangphai();
                      Serial.println("Di sang phai");
                    } else {
                      disangtrai();
                      Serial.println("Di sang trai");
                    }
                  } else if (khoangcachphai >= gioihan) {
                    disangphai();
                    Serial.println("Di sang phai");
                  } else if (khoangcachtrai >= gioihan) {
                    disangtrai();
                    Serial.println("Di sang trai");
                  } else {
                    dilui(); delay(500); dunglai();
                    Serial.println("Di lui do khong co duong di");
                    // Gửi vị trí về app và chuyển sang chế độ điều khiển thủ công qua app : Chưa có
                  }
                  delay(400); dunglai(); delay(300);
                }
            } else if (c == '6') {
                mode = 0; // Chế độ thủ công
                Serial.println("Chuyển sang chế độ THỦ CÔNG");
          
            }
             if (mode == 0) {
            switch (c) {
                case '0': dithang(); Serial.println("Đi tới"); break;
                case '1': dilui(); Serial.println("Đi lùi"); break;
                case '2': disangtrai(); Serial.println("Đi sang trái"); break;
                case '3': disangphai(); Serial.println("Đi sang phải"); break;
                case '4': dunglai(); Serial.println("Dừng lại"); break;
                case '5': case '6': dunglai(); break; // Chỉ đổi mode, không điều khiển
                default: Serial.println("Lệnh không hợp lệ"); break;
            }
        }
        }
    }


    delay(200);
}

void dithang() {
    analogWrite(enbA, 100);
    analogWrite(enbB, 100);
    digitalWrite(in1, 0);
    digitalWrite(in2, 1);
    digitalWrite(in3, 0);
    digitalWrite(in4, 1);
}

void dunglai() {
    analogWrite(enbA, 0);
    analogWrite(enbB, 0);
    digitalWrite(in1, 0);
    digitalWrite(in2, 0);
    digitalWrite(in3, 0);
    digitalWrite(in4, 0);
}

void disangphai() {
    analogWrite(enbA, 100);
    analogWrite(enbB, 50);
    digitalWrite(in1, 0);
    digitalWrite(in2, 0);
    digitalWrite(in3, 1);
    digitalWrite(in4, 0);
}

void disangtrai() {
    analogWrite(enbA, 50);
    analogWrite(enbB, 100);
    digitalWrite(in1, 1);
digitalWrite(in2, 0);
    digitalWrite(in3, 0);
    digitalWrite(in4, 0);
}

void dilui() {
    analogWrite(enbA, 100);
    analogWrite(enbB, 100);
    digitalWrite(in1, 1);
    digitalWrite(in2, 0);
    digitalWrite(in3, 1);
    digitalWrite(in4, 0);
}

void dokhoangcach() {
    digitalWrite(trig, LOW);
    delayMicroseconds(2);
    digitalWrite(trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig, LOW);
    thoigian = pulseIn(echo, HIGH);
    khoangcach = thoigian / 2 / 29.412;
}

void quaycbsangtrai() {
    myservo.write(150);
    delay(1000);
}

void quaycbsangphai() {
    myservo.write(30);
    delay(1000);
}

void resetservo() {
    myservo.write(90);
    delay(500);
}



// void toado() {
//   while (ss.available() > 0) gps.encode(ss.read());
//   if (gps.location.isValid()) {
//     float latitude = gps.location.lat();
//     float longitude = gps.location.lng();
//     Serial.println("Vĩ độ: " + String(latitude, 6) + "  Kinh độ: " + String(longitude, 6));
//   }
// }
