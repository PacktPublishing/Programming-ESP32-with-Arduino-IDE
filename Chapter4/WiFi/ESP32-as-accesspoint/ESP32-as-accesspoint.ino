#include <WiFi.h>
const char* ssid = "MyESP32AP";
const char* password = "password123";

void setup() {
  Serial.begin(115200);
  WiFi.softAP(ssid, password);
  IPAddress apIP(192, 168, 4, 1);
  IPAddress subnet(255, 255, 255, 0);
  WiFi.softAPConfig(apIP, apIP, subnet);
  Serial.print("Access Point IP Address: ");
  Serial.println(WiFi.softAPIP());
}

void loop() {
  // Your code goes here
}