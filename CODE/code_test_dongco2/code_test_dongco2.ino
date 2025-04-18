const int enbA = 3;
const int in1 = 5;
const int in2 = 6;
const int in3 = 9;
const int in4 = 10;
const int enbB = 11;

void setup() {
  Serial.begin(9600);
  
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(enbA, OUTPUT);
  pinMode(enbB, OUTPUT);
  
  Serial.println("Bat dau kiem tra dong co...");
}

void loop() {
  // Serial.println("Di thang...");
  // dithang();
  // delay(2000);
  // dunglai();
  
  // Serial.println("Di lui...");
  // dilui();
  // delay(2000);
  // dunglai();
  
  for (int i = 0; i < 1; i++) {
    disangtrai();
    delay(500);
     dunglai();
    delay(200); 
  }
  Serial.println("Di sang trai...");
  delay(2000);
 
  for (int i = 0; i < 2; i++) {
    disangphai();
    delay(500);
    dunglai();
    delay(200); 
  }
  Serial.println("Di sang phai...");
  delay(2000);
}

void dithang() {
  analogWrite(enbA, 100);
  analogWrite(enbB, 100);
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
}

void dilui() {
  analogWrite(enbA, 100);
  analogWrite(enbB, 100);
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}

void disangtrai() {
  analogWrite(enbA, 60);
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);

  analogWrite(enbB, 60);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
  delay(333);
}

void disangphai() {
  analogWrite(enbA, 60);
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);

  analogWrite(enbB, 60);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  delay(333);
}

void dunglai() {
  Serial.println("Dung lai...");
  analogWrite(enbA, 0);
  analogWrite(enbB, 0);
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
  delay(1000);
}