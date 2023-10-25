#include <WiFi.h> 

#include <WiFiManager.h> 

 

void setup() { 

 Serial.begin(115200); 

 WiFiManager wifiManager; 

 // Uncomment the following line to reset Wi-Fi settings and enter configuration mode 

 //wifiManager.resetSettings(); 

 wifiManager.autoConnect("ESP32-Config"); // Access point name 

 Serial.println("Connected to Wi-Fi!"); 

 Serial.print("IP Address: "); 

 Serial.println(WiFi.localIP()); 

} 

void loop() { 

 // Your code goes here 

} 