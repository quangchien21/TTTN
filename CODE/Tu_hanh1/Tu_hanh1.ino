const int trig = 7;  
const int echo = 4;    
int enbA = 3;
int in1 = 5;       
int in2 = 6;        
int in3 = 9;        
int in4 = 10;  
int enbB = 11;      

int gioihan = 30;
unsigned long thoigian;
int khoangcach, khoangcachtrai, khoangcachphai;
bool huong_tranh = false; // true: phải, false: trái

void dokhoangcach();
void dithang();
void disangtrai();
void disangphai();
void dilui();
void dunglai();
void quaycbsangphai();
void quaycbsangtrai();
void di_vong_vat_can();
void tro_lai_huong_cu();

void setup() {
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);   
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(enbA, OUTPUT);
  pinMode(enbB, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  dokhoangcach();
  Serial.println(khoangcach);

  if (khoangcach > gioihan || khoangcach == 0) {
    dithang();
    Serial.println("Di toi");
  } else {
    dunglai(); delay(1000);
    di_vong_vat_can();
    tro_lai_huong_cu();
  }
}

void di_vong_vat_can() {
  quaycbsangtrai(80); dokhoangcach(); khoangcachtrai = khoangcach; Serial.println(khoangcachtrai); delay(3000); // Kiểm tra khoảng trống bên trái
  quaycbsangphai(80); delay(2000); // quay về trung tâm
  quaycbsangphai(75); dokhoangcach(); khoangcachphai = khoangcach; Serial.println(khoangcachphai); delay(3000); // Kiểm tra khoảng trống bên phải
  // tại đây, xe đang quay về bên phải
  
  if (khoangcachphai > gioihan && khoangcachtrai > gioihan) {
    if (khoangcachphai > khoangcachtrai) {
      huong_tranh = true;
      dithang(); delay(2000); dunglai();
    } else if (khoangcachtrai > khoangcachphai) {
      huong_tranh = false;
      quaycbsangtrai(75); delay(1000); 
      quaycbsangtrai(80); delay(1000);
      
      dithang(); delay(2000); dunglai();
      
    } else { // Nếu bằng nhau, ưu tiên chọn bên phải
        huong_tranh = true; 
        dithang(); delay(2000);
        dunglai();
    }
  } else if (khoangcachphai > gioihan) {
    huong_tranh = true;
    dithang(); delay(2000);
    dunglai();
  } else if (khoangcachtrai > gioihan) {
    huong_tranh = false;
    quaycbsangtrai(75); delay(1000);
    quaycbsangtrai(80); delay(1000);
    dithang(); delay(2000); dunglai();
  } else {
    quaycbsangtrai(60); delay(1000);
    dilui(); delay(500);
  }
  Serial.println(huong_tranh);
}

void tro_lai_huong_cu() {
  if (huong_tranh) {
    delay(1000);
    quaycbsangtrai(100); dithang(); delay(3000); dunglai(); quaycbsangphai(80); delay(2000); 
  } else {
    delay(1000);
    quaycbsangphai(100); dithang(); delay(3000); dunglai(); quaycbsangtrai(80); delay(2000);
  }
  Serial.println("Tro lai huong ban dau");
}

void dithang() {
  analogWrite(enbA, 60);
  analogWrite(enbB, 60);
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

void disangphai(int speed) {
  analogWrite(enbA, speed);
  analogWrite(enbB, speed);
  digitalWrite(in1, 0);
  digitalWrite(in2, 0);
  digitalWrite(in3, 1);
  digitalWrite(in4, 0);
}

void disangtrai(int speed) {
    analogWrite(enbA, speed);
    analogWrite(enbB, speed);
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
    
}

void dilui() {
  analogWrite(enbA, 60);
  analogWrite(enbB, 60);
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

void quaycbsangtrai( int speed) {
  analogWrite(enbA, speed);
  analogWrite(enbB, speed);
  digitalWrite(in1, 1);
  digitalWrite(in2, 0);
  digitalWrite(in3, 0);
  digitalWrite(in4, 0);
  delay(300);
  dunglai();
  Serial.println("Quay xe nhẹ sang trái để đo khoảng cách");
}

void quaycbsangphai( int speed) {
   analogWrite(enbA, speed);
  analogWrite(enbB, speed);
  digitalWrite(in1, 0);
  digitalWrite(in2, 0);
  digitalWrite(in3, 1);
  digitalWrite(in4, 0);
  delay(300);
  dunglai();
  Serial.println("Quay xe nhẹ sang phải để đo khoảng cách");
}



