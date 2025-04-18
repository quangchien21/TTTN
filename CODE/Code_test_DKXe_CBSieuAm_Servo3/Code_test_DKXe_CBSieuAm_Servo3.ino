// #include <Servo.h>
// Servo myservo;  // create servo object to control a servo
// int pos = 0;    // variable to store the servo position

const int trig = 7;  
const int echo = 4;    
int enbA=3;
int in1 = 5;       
int in2 = 6;        
int in3 = 9;        
int in4 = 10;  
int enbB=11;      
// int dongcoservo = 8;   

int gioihan = 30;//khoảng cách nhận biết vật 
unsigned long thoigian;
int khoangcach;          
int khoangcachtrai, khoangcachphai;

void dokhoangcach();
void dithang();
void disangtrai();
void disangphai();
void dilui();
void dunglai();
void quaycbsangphai();
void quaycbsangtrai();
// void resetservo();

void setup() {
  // myservo.attach(dongcoservo); 
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);   
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(enbA, OUTPUT);
  pinMode(enbB, OUTPUT);
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
  digitalWrite(enbA, LOW);
  digitalWrite(enbB, LOW);
  // myservo.write(90);
  delay(500);
  Serial.begin(9600);
  // Kiểm tra xem có khí gas không? : chưa có
  // Gửi vị trí bắt đầu của xe : chưa có
  // Cập nhật thông số về không khí, vị trí ban đầu của xe lên app
}

void loop()
{
  // Kiểm tra xem có khí gas không? : chưa có
  // Nếu có khí gas thì dừng xe và gửi thông số vị trí về app, sau đó tiếp tục di chuyển
  // Liên tục cập nhật thông số về không khí lên app sau mỗi vòng lặp
  // Nếu không có thì mới thực hiện di chuyển

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
    dunglai(); delay(200);
    quaycbsangtrai(); dunglai(); delay(200); dokhoangcach(); khoangcachtrai = khoangcach;
    quaycbsangphai(); dunglai(); delay(200); dokhoangcach(); khoangcachphai = khoangcach;
    // resetservo();
    
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
}

void dithang()
{
  analogWrite(enbA, 60);
  analogWrite(enbB, 60);
  digitalWrite(in1, 0);
  digitalWrite(in2, 1);
  digitalWrite(in3, 0);
  digitalWrite(in4, 1);
}

void dunglai(){
  analogWrite(enbA, 0);
  analogWrite(enbB, 0);
  digitalWrite(in1, 0);
  digitalWrite(in2, 0);
  digitalWrite(in3, 0);
  digitalWrite(in4, 0);
}

void disangphai()
{
  analogWrite(enbA, 100);
  analogWrite(enbB, 50);
  digitalWrite(in1, 0);
  digitalWrite(in2, 0);
  digitalWrite(in3, 1);
  digitalWrite(in4, 0);
}

void disangtrai()
{
  analogWrite(enbA, 50);
  analogWrite(enbB, 100);
  digitalWrite(in1, 1);
  digitalWrite(in2, 0);
  digitalWrite(in3, 0);
  digitalWrite(in4, 0);
}

void dilui()
{
  analogWrite(enbA, 60);
  analogWrite(enbB, 60);
  digitalWrite(in1, 1);
  digitalWrite(in2, 0);
  digitalWrite(in3, 1);
  digitalWrite(in4, 0);
}

void dokhoangcach()
{
  digitalWrite(trig, LOW); 
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);  
  delayMicroseconds(10); 
  digitalWrite(trig, LOW); 
  thoigian = pulseIn(echo, HIGH);
  khoangcach = thoigian / 2 / 29.412;
}

void quaycbsangtrai()
{
  analogWrite(enbA, 50);
  analogWrite(enbB, 100);
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  Serial.println("Quay cảm biến sang trái");
}

void quaycbsangphai()
{
  analogWrite(enbA, 100);
  analogWrite(enbB, 100);
  digitalWrite(in1, 0);
  digitalWrite(in2, 0);
  digitalWrite(in3, 1);
  digitalWrite(in4, 0);
  Serial.println("Quay cảm biến sang phải");
}

void resetservo()
{
  myservo.write(90);
  delay(500);
}
