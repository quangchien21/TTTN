 
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

static const int RXPin = A1, TXPin = A2;
static const uint32_t GPSBaud = 9600;

// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

void setup(){
  Serial.begin(9600);
  ss.begin(GPSBaud);  
}

void loop(){
 
  while (ss.available() > 0){
    gps.encode(ss.read()); 
  }
  if (gps.location.isUpdated()){
      Serial.print("Latitude - vĩ độ:  "); 
      Serial.print(gps.location.lat(), 6);
      Serial.print(" Longitude - kinh độ:  "); 
      Serial.println(gps.location.lng(), 6);
    }
}