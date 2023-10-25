#include <Adafruit_Sensor.h> 
#include <DHT_U.h> 
#include <WiFi.h> 
#include <PubSubClient.h> 
#include <Servo.h> 

#define DHTPIN 12 
#define LED 26 
#define SERVO_PIN 2 
#define DHTTYPE    DHT22 

DHT_Unified dht(DHTPIN, DHTTYPE); 

uint32_t delayMS; 

Servo servo; 

const char* ssid = "WiFi_SSID"; 
const char* password = "WiFi_Password"; 

const char* mqttServer = "broker.hivemq.com"; 
const char* clientID = "client_ID"; 
const char* topic = "Tempdata"; 

unsigned long previousMillis = 0; 

const long interval = 1000; 

String msgStr = ""; 

float temp, hum; 

WiFiClient espClient; 

PubSubClient client(espClient); 

void setup_wifi() { 

  delay(10); 
  WiFi.begin(ssid, password); 

  while (WiFi.status() != WL_CONNECTED) { 
    delay(500); 
  } 

} 

 

void reconnect() { 

  while (!client.connected()) { 

    if (client.connect(clientID)) {  
      Serial.println("MQTT connected"); 
      client.subscribe("lights"); 
      client.subscribe("servo"); 
    } 

    else { 
      delay(5000); 
    } 

  } 

} 

 

void callback(char* topic, byte* payload, unsigned int length) { 

  String data = ""; 

  for (int i = 0; i < length; i++) { 
    data += (char)payload[i]; 
  } 

  if (String(topic) == "lights") { 
    if (data == "ON") { 
      digitalWrite(LED, HIGH); 
    } 

    else { 
      digitalWrite(LED, LOW); 
    } 

  } 

  else if (String(topic) == "servo") { 
    int degree = data.toInt(); 
    servo.write(degree); 
  } 

} 

 

void setup() { 
  Serial.begin(115200); 
  dht.begin(); 
  sensor_t sensor; 

  dht.temperature().getSensor(&sensor); 
  dht.humidity().getSensor(&sensor); 

  pinMode(LED, OUTPUT); 
  digitalWrite(LED, LOW); 
  servo.attach(SERVO_PIN); 
  servo.write(0); 
  setup_wifi(); 
  client.setServer(mqttServer, 1883); 
  client.setCallback(callback); 

} 

 

void loop() { 

  if (!client.connected()) { 
    reconnect(); 
  } 

  client.loop(); 

  unsigned long currentMillis = millis(); 

  if (currentMillis - previousMillis >= interval) { 
    previousMillis = currentMillis; 
    sensors_event_t event; 

    dht.temperature().getEvent(&event); 

    if (isnan(event.temperature)) { 

      Serial.println(F("Error reading temperature!")); 

    } 

    else { 

      temp = event.temperature; 

    } 

    dht.humidity().getEvent(&event); 

    if (isnan(event.relative_humidity)) { 

      Serial.println(F("Error reading humidity!")); 

    } 

    else { 

      hum = event.relative_humidity; 

    } 

    msgStr = String(temp) + "," + String(hum) + ","; 

    byte arrSize = msgStr.length() + 1; 

    char msg[arrSize]; 

    msgStr.toCharArray(msg, arrSize); 

    client.publish(topic, msg); 

    msgStr = ""; 

    delay(1); 

  } 

} 