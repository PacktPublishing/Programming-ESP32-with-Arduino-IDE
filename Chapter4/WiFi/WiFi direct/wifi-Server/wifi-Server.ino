#include <WiFi.h>

WiFiServer server(80);
const char* ssid = "MyESP32Direct";
const char* password = "password123";

void setup() {
  Serial.begin(115200);
  WiFi.softAP(ssid, password);

  IPAddress apIP(192, 168, 4, 1);

  IPAddress subnet(255, 255, 255, 0);

  WiFi.softAPConfig(apIP, apIP, subnet);

  server.begin();

  Serial.print("WiFi Direct Group Owner IP Address: ");

  Serial.println(WiFi.softAPIP());
}

void loop() {
  WiFiClient client = server.available();

  if (client) {
    Serial.println("Client connected.");
    client.println("Hello from Group Owner!");
    while (client.connected()) {

      if (client.available()) {

        String message = client.readStringUntil('\n');

        Serial.print("Received message: ");

        Serial.println(message);
      }
    }

    client.stop();

    Serial.println("Client disconnected.");
  }
}