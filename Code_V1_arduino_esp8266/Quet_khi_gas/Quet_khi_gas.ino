
#include <Servo.h>
#include <SoftwareSerial.h>
Servo myservo;  
int pos = 0;    
int dongcoservo = 8;   

SoftwareSerial espSerial(12, 13); // RX, TX (Kết nối với ESP8266)

// Định nghĩa chân cảm biến khí gas
#define GAS_SENSOR A0  // Chân analog đọc giá trị từ cảm biến khí gas

// Định nghĩa chân điều khiển L298N
#define IN1 5  // Điều khiển chiều quay motor bên trái
#define IN2 6
#define ENA 3 // Điều khiển tốc độ motor bên trái

#define IN3 9  // Điều khiển chiều quay motor bên phải
#define IN4 10
#define ENB 11  // Điều khiển tốc độ motor bên phải

// Tốc độ động cơ
#define MAX_SPEED 255
#define NORMAL_SPEED 70
#define SLOW_SPEED 60

// Ngưỡng phát hiện khí gas
#define GAS_THRESHOLD 250  // Giá trị này cần điều chỉnh dựa trên cảm biến cụ thể
#define GAS_MAX 500  // Giá trị khí gas cảnh báo

// Biến lưu trữ
int gasValue = 0;         // Giá trị đọc từ cảm biến
int previousGasValue = 0; // Giá trị trước đó để so sánh
int maxGasValue = 0;      // Giá trị khí gas cao nhất tìm thấy
int searchState = 0;      // Trạng thái tìm kiếm (0: tìm kiếm, 1: tới gần nguồn, 2: xác định vị trí chính xác)

bool foundLeak = false;  // Đánh dấu nếu đã tìm thấy vị trí rò rỉ
const int trig = 7;  
const int echo = 4; 
int gioihan = 30;
unsigned long thoigian;
int khoangcach, khoangcachtrai, khoangcachphai;

int mode = 0; // 1 - Auto; 0 - Thủ công
int c; // lưu lệnh gửi từ app
unsigned long lastGasSentTime = 0; 

void setup() {
  Serial.begin(115200);
  espSerial.begin(9600); // Kết nối với ESP8266
  myservo.attach(dongcoservo); 

  pinMode(GAS_SENSOR, INPUT);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);
  
  stopMotors();
  
  Serial.println("Robot dò khí gas đã sẵn sàng!");
  delay(1000);
}

void quayServo(int goc) {
  myservo.write(goc);  
  delay(1000);         
}

void dokhoangcach() {
  digitalWrite(trig, LOW); 
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);  
  delayMicroseconds(10); 
  digitalWrite(trig, LOW); 
  thoigian = pulseIn(echo, HIGH, 50000);
  khoangcach = thoigian / 2 / 29.412;
}

void tranh_vat_can () {
  dokhoangcach();
  Serial.println(khoangcach);

  if (khoangcach > gioihan || khoangcach == 0) {
    Serial.println("Di toi");
    moveForward(NORMAL_SPEED);
    return;
  } else {
    di_vong_vat_can();
  }
}

void di_vong_vat_can() {
  // Đo khoảng cách bên phải
  quayServo(30); 
  delay(800);
  dokhoangcach(); 
  khoangcachphai = khoangcach; 
  Serial.print("Khoảng cách phải: "); 
  Serial.println(khoangcachphai); 
  
  
  // Đo khoảng cách bên trái
  quayServo(150); 
  delay(800);
  dokhoangcach(); 
  khoangcachtrai = khoangcach; 
  Serial.print("Khoảng cách trái: "); 
  Serial.println(khoangcachtrai); 
   
  // Quay về vị trí trung tâm
  quayServo(90);

  // Kiểm tra điều kiện tránh vật cản
  if (khoangcachtrai < gioihan && khoangcachphai < gioihan) {
    // Nếu cả hai bên đều không an toàn, lùi lại
    dilui(); mode = 1; espSerial.println("V8:" + String(mode)); // chuyển sang thủ công 
    delay(500);
    myservo.detach();
    Serial.println("Lùi lại do không có khoảng trống. Chuyển sang Thủ Công!");
    return;
  } else if (khoangcachtrai > khoangcachphai && khoangcachtrai > gioihan) {
    // Nếu bên trái an toàn hơn, quay trái
    turnLeft(90); 
    delay(1000); 
    stopMotors();
    delay(1000);
    moveForward(70);

    // Trở lại hướng cũ
    delay(1000);
    turnRight(80);
    delay(1000); 
    stopMotors();
    delay(1000);
    moveForward(70);
    delay(2000);
    turnLeft(80);
    delay(1000);
    stopMotors();
    delay(2000);
    Serial.println("Tro lai huong ban dau");
    moveForward(NORMAL_SPEED);
    
  } else if (khoangcachphai >= khoangcachtrai && khoangcachphai > gioihan) {
    // Nếu bên phải an toàn hơn hoặc bằng bên trái, quay phải
    turnRight(80);
    delay(1000);
    stopMotors();
    delay(2000);
    moveForward(70);

    // Trở lại hướng cũ
    delay(1000);
    turnLeft(80);
    delay(1000); 
    stopMotors();
    delay(2000);
    moveForward(70);
    delay(2000);
    turnRight(75);
    delay(500);
    stopMotors();
    delay(2000); 
    Serial.println("Tro lai huong ban dau");
    moveForward(NORMAL_SPEED);
  }
}

void dilui() {
  analogWrite(ENA, 100);
  analogWrite(ENB, 100);
  digitalWrite(IN1, 1);
  digitalWrite(IN2, 0);
  digitalWrite(IN3, 1);
  digitalWrite(IN4, 0);

  delay(1000);
  stopMotors();
}

void check_mode(){
  if (espSerial.available()) {
    String command = espSerial.readStringUntil('\n');
    if (command.startsWith("V:")) {
      c = command.substring(2).toInt();

      Serial.print("📥 Nhận lệnh từ ESP8266: ");
      Serial.println(c);
    }
  }
  if(c == 5){
    mode = 1; // chế độ tc
  } else if(c == 6){
    mode = 0; // chế độ at
  } else if(c==7 || c== 8 || c==2 || c==3 || c==4 ){
    Serial.println("Lệnh Di Chuyển");
  } else{
    Serial.println("Chưa có lệnh");
  }
}

void loop() {
   if (foundLeak) {
    stopMotors();
    return;  
  }
    
  gasValue = analogRead(GAS_SENSOR);
  
  Serial.print("Giá trị khí gas: ");
  Serial.println(gasValue);
  // Gửi giá trị đến ESP8266 2s 1 lần
  if (millis() - lastGasSentTime >= 2000) {
      espSerial.println("Gas: " + String(gasValue));
      lastGasSentTime = millis();
    } 
  
  check_mode(); // Kiểm tra lệnh từ app

  // mode tự hành - Auto
  if(mode == 0){
    myservo.attach(dongcoservo);
    espSerial.println("V8:" + String(mode)); // đồng bộ dữ liệu mode lên app
    if (gasValue > GAS_THRESHOLD) {
      if (gasValue > maxGasValue) { 
        maxGasValue = gasValue;
        
        if (gasValue > previousGasValue && gasValue < GAS_MAX) {
          searchState = 1;  
        } 
        else if(gasValue >= GAS_MAX) {
          searchState = 2;  
          // espSerial.println("Gas: " + String(gasValue));
        }
      } 
      else {
        searchState = 0;  
      }
    }
  
    previousGasValue = gasValue;
  
    switch (searchState) {
      case 0:  
        searchPattern();
        break;
        
      case 1:  
        tranh_vat_can();
        // moveForward(SLOW_SPEED);
        Serial.println("Tiến về vị trí rò rỉ");
        delay(3000);
        break;
        
      case 2:  
        Serial.println("ĐÃ TÌM THẤY VỊ TRÒ RÒ RỈ KHÍ GAS!");
        stopMotors();
        foundLeak = true;  
        delay(5000);
        break;
    }
  } 
  // mode Thủ công
  else{
    myservo.detach();
    espSerial.println("V8:" + String(mode));
    if (gasValue >= GAS_MAX) { 
        Serial.println("ĐÃ TÌM THẤY VỊ TRÒ RÒ RỈ KHÍ GAS!");
        stopMotors(); // Dừng xe lại
        foundLeak = true; 
        delay(500);
        return;
    }
    switch (c) {
      case '7': move(100,0,1,0,1); Serial.println("Đi tới"); break; delay(2000);
      case '8': move(100,1,0,1,0); Serial.println("Đi lùi"); break; delay(2000);
      case '2': move(100,1,0,0,0); Serial.println("Đi sang trái"); break; delay(2000);
      case '3': move(100,0,0,1,0); Serial.println("Đi sang phải"); break; delay(2000);
      case '4': stopMotors(); break;
      case '5': mode = 0; Serial.println("Chế độ Auto");
      case '6': mode = 1; stopMotors(); Serial.println("Chế độ Thủ Công"); break; // Chỉ đổi mode, không điều khiển
      default: Serial.println("Lệnh không hợp lệ"); break;
    }
  }
  delay(100);
}

void moveForward(int speed) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(ENA, speed);
  
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENB, speed);

  Serial.println("Đang tiến tới");
  delay(500);
  stopMotors();

}

void stopMotors() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 0);
  
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, 0);
  
  Serial.println("Đã dừng");
}

void turnLeft(int speed) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, speed);
  
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENB, speed);
  
  Serial.println("Đang quay trái");
}

void turnRight(int speed) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(ENA, speed);
  
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, speed);
  
  Serial.println("Đang quay phải");
}

void move(int speed, int i1, int i2, int i3, int i4) {
    analogWrite(ENA, speed);
    analogWrite(ENB, speed);
    digitalWrite(IN1, i1);
    digitalWrite(IN2, i2);
    digitalWrite(IN3, i3);
    digitalWrite(IN4, i4);
}

void searchPattern() {
  int frontGas, leftGas, rightGas;

  quayServo(90);
  delay(500);
  frontGas = analogRead(GAS_SENSOR);
  Serial.print("Khí gas phía trước: ");
  Serial.println(frontGas);

  quayServo(150);
  delay(500);
  leftGas = analogRead(GAS_SENSOR);
  Serial.print("Khí gas bên trái: ");
  Serial.println(leftGas);
  

  quayServo(30);
  delay(500);
  rightGas = analogRead(GAS_SENSOR);
  Serial.print("Khí gas bên phải: ");
  Serial.println(rightGas);
  
  quayServo(90); // quay về vị trí ban đầu

  if (frontGas >= leftGas && frontGas >= rightGas) {
    espSerial.println("Gas: " + String(frontGas));
    Serial.println("Tiến thẳng");
    tranh_vat_can();
  } else if (leftGas > rightGas) {
    espSerial.println("Gas: " + String(leftGas));
    Serial.println("Rẽ trái");
    turnLeft(65);
    delay(1000);
    stopMotors();
    delay(2000);
    tranh_vat_can();
  } else {
    espSerial.println("Gas: " + String(rightGas));
    Serial.println("Rẽ phải");
    turnRight(SLOW_SPEED);
    delay(1000);
    stopMotors();
    delay(2000);
    tranh_vat_can();
  }
  
  delay(1000);
}
