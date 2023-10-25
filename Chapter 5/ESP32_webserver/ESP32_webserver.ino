#include <WiFi.h> 

#include <WebServer.h> 

const char* ssid = "YourWiFiSSID"; 

const char* password = "WIFIPASSWORD"; 

WebServer server(80); 

const int ledPin = 13;  

const int buttonPin = 12;  

bool ledState = false; 

void setup() { 

 Serial.begin(115200); 

 pinMode(ledPin, OUTPUT); 

 pinMode(buttonPin, INPUT_PULLUP); 

 digitalWrite(ledPin, ledState); 

 WiFi.begin(ssid, password); 

 while (WiFi.status() != WL_CONNECTED) { 

  delay(1000); 

  Serial.println("Connecting to WiFi..."); 

 } 

 Serial.println("WiFi connected"); 

 Serial.println("IP address: "); 

 Serial.println(WiFi.localIP()); 

 server.on("/", HTTP_GET, handleRoot); 

 server.on("/toggle", HTTP_GET, handleToggle);  

 server.begin(); 

} 

void loop() { 

 server.handleClient(); 

 int buttonState = digitalRead(buttonPin); 

 if (buttonState == LOW) { 

  ledState = !ledState; 

  digitalWrite(ledPin, ledState); 

 } 

} 

void handleRoot() { 

 String html = "<html><body>"; 

 html += "<h1>ESP32 LED Control</h1>"; 

 html += "<p>LED Status: " + String(ledState) + "</p>"; 

 html += "<form method='get' action='/toggle'>"; 

 html += "<button type='submit'>Toggle LED</button>"; 

 html += "</form>"; 

 html += "</body></html>"; 

 server.send(200, "text/html", html); 

} 

void handleToggle() { 

 ledState = !ledState; 

 digitalWrite(ledPin, ledState); 

 server.send(200, "text/plain", "LED toggled"); 

} 