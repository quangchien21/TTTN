#include <Servo.h> 
 
Servo myservo;  // khởi tạo đối tượng Servo với tên gọi là myservo 
                // bạn có thể tạo tối đa 8 đối tượng Servo 
 
int pos = 0;    // biến pos dùng để lưu tọa độ các Servo
 
void setup() 
{ 
  myservo.attach(8);  // attach (đính) servo ở chân digital 9 
                      // bạn có thể thay bằng bất cứ chân digital/analog nào khác
} 
 
 
void loop() 
{ 
  for(pos = 0; pos < 180; pos += 1)  // cho servo quay từ 0->179 độ
  {                                  // mỗi bước của vòng lặp tăng 1 độ
    myservo.write(pos);              // xuất tọa độ ra cho servo
    delay(15);                       // đợi 15 ms cho servo quay đến góc đó rồi tới bước tiếp theo
  } 
  for(pos = 180; pos>=1; pos-=1)     // cho servo quay từ 179-->0 độ
  {                                
    myservo.write(pos);              // xuất tọa độ ra cho servo
    delay(15);                       // đợi 15 ms cho servo quay đến góc đó rồi tới bước tiếp theo
  } 
}