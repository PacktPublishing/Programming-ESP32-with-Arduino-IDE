#include <WiFi.h> 
const char* ssid = "MyESP32AP"; 
const char* password = "Password123"; 

void setup() { 
  Serial.begin(115200); 
  WiFi.begin(ssid, password); 

  Serial.print("Connecting to WiFi..."); 

  while (WiFi.status() != WL_CONNECTED) { 

    delay(500); 
    Serial.print("."); 
  } 

  Serial.println(); 
  Serial.print("Connected to WiFi network with IP address: "); 
  Serial.println(WiFi.localIP()); 
} 

 

void loop() { 
  // Your code goes here 
} 