#include <WiFi.h>

const char* ssid = "MyESP32Direct";
const char* password = "password123";

void setup() {

  Serial.begin(115200);

  WiFi.begin(ssid, password);



  Serial.print("Connecting to WiFi Direct Group Owner...");

  while (WiFi.status() != WL_CONNECTED) {

    delay(500);

    Serial.print(".");
  }



  Serial.println();

  Serial.print("Connected to WiFi Direct Group Owner with IP address: ");

  Serial.println(WiFi.localIP());
}



void loop() {

  // Check if there is a connection to the server

  if (WiFi.status() == WL_CONNECTED) {

    WiFiClient client;



    // Connect to the server on the Group Owner's IP address and port 80

    if (client.connect("192.168.4.1", 80)) {

      // Read and print the message from the Group Owner

      while (client.connected()) {

        if (client.available()) {

          String message = client.readStringUntil('\n');

          Serial.print("Received message: ");

          Serial.println(message);



          // Send a response message to the Group Owner

          client.println("Hello from Client!");
        }
      }

      client.stop();

    } else {

      Serial.println("Connection to server failed.");
    }

  } else {

    Serial.println("WiFi connection failed.");
  }
}