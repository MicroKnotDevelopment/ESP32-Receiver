#include <ESP32Servo.h>
Servo myServo;
int pos;

#define SERVO_PIN 13

void setup() {
  Serial.begin(115200);
  myServo.attach(SERVO_PIN);
  myServo.write(90);
}
void loop() {
  Serial.println("Enter position: ");
  while(Serial.available() == 0){};
  pos = Serial.readString().toInt();
  if (pos >= 0 && pos <= 180){
    myServo.write(pos);
    Serial.print("Turned to: ");
    Serial.println(pos);
  }
  else {
    Serial.println("Invalid position!");
  }
}
