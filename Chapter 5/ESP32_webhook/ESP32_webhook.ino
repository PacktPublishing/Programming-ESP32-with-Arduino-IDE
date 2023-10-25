#include <WiFi.h> 

#include <HTTPClient.h> 

const char* ssid = "WiFi_SSID"; 

const char* password = "WiFi_Password"; 

const char* webhookURL = "Your_Unique_URL"; 

const int buttonPin = 35; 

int buttonState = HIGH; 

int lastButtonState = HIGH; 

unsigned long lastDebounceTime = 0; 

unsigned long debounceDelay = 50; 

 

void setup() { 

  Serial.begin(115200); 

  pinMode(buttonPin, INPUT_PULLUP); 

  digitalWrite(buttonPin, HIGH); 

  WiFi.begin(ssid, password); 

  while (WiFi.status() != WL_CONNECTED) { 

    delay(1000); 

  } 

} 

 

void loop() { 

  int reading = digitalRead(buttonPin); 

  if (reading != lastButtonState) { 

    lastDebounceTime = millis(); 

  } 

  if ((millis() - lastDebounceTime) > debounceDelay) { 

    if (reading != buttonState) { 

      buttonState = reading;  

      if (buttonState == LOW) { 

        sendWebhookRequest(buttonState); 

      } 

    } 

  } 

  lastButtonState = reading; 

} 

 

void sendWebhookRequest(int switchStatus) { 

  HTTPClient http; 

  String url = String(webhookURL) + "?random=" + String(random(30)); 

  http.begin(url); 

  int httpResponseCode = http.GET(); 

  if (httpResponseCode > 0) { 

    Serial.print("Webhook request sent. Response code: "); 

    Serial.println(httpResponseCode); 

  } else { 

    Serial.print("Error sending webhook request. HTTP response code: "); 

    Serial.println(httpResponseCode); 

  } 

  http.end(); 

} 