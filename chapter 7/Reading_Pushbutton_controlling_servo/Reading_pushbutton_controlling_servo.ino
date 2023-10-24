#include <ESP32Servo.h>
#define BUTTON 5
Servo myservo;  // Create a Servo object
const int servoPin = 14; 
bool barrier = false;
void setup() {
  Serial.begin(115200);
  pinMode(BUTTON, INPUT_PULLUP);
  myservo.attach(servoPin); // Attaches the servo on the specified pin
}
void loop() {
  if (!barrier) {
    if (!digitalRead(BUTTON)) {
      openServo();
      barrier = true;
      delay(1000);
    }
  }
  if (barrier) {
    if (!digitalRead(BUTTON)) {
      closeServo();
      barrier = false;
    }
  }
}

void openServo() {
  Serial.println("Servo open");
  myservo.write(0);
}
void closeServo() {
  delay(1000);
  Serial.println("Servo closed");
  myservo.write(180);
}
