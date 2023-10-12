#include <SoftwareSerial.h>

SoftwareSerial BG95Serial(2, 3);  // RX, TX pins for BG95 Shield

const int baudRate = 9600;

void setup() {

  Serial.begin(115200);

  BG95Serial.begin(baudRate);



  // Wait for the BG95 module to initialize

  delay(1000);



  Serial.println("Initializing BG95...");

  sendATCommand("AT");

  sendATCommand("AT+CPIN?");
}

void loop() {

  // Your code goes here
}

void sendATCommand(String command) {

  BG95Serial.println(command);

  delay(500);

  while (BG95Serial.available()) {

    Serial.write(BG95Serial.read());
  }
}