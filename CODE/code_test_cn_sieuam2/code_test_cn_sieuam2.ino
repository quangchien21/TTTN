// Khai báo chân của cảm biến IOE-SR05
#define TRIG_PIN 7   // Chân TRIG nối với chân số 9 trên Arduino
#define ECHO_PIN 4  // Chân ECHO nối với chân số 10 trên Arduino

void setup() {
    Serial.begin(9600);  // Khởi động giao tiếp Serial với baudrate 9600
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
}

void loop() {
    long duration;
    float distance;
    // Phát xung siêu âm
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    
    // Đo thời gian xung phản hồi
    duration = pulseIn(ECHO_PIN, HIGH);
    
    // Tính toán khoảng cách (tốc độ âm thanh trong không khí khoảng 343 m/s)
    distance = duration * 0.0343 / 2;
    
    // Hiển thị khoảng cách lên Serial Monitor
    Serial.print("Khoảng cách: ");
    Serial.print(distance);
    Serial.println(" cm");
    
    delay(500); // Đợi 500ms trước khi đo tiếp theo
}