#include <Adafruit_Sensor.h>
#include <DHT_U.h>
#include <WiFiMulti.h>
#include <InfluxDbClient.h> //https://github.com/tobiasschuerg/InfluxDB-Client-for-Arduino.git
#include <InfluxDbCloud.h>
#include <PubSubClient.h>
#include <ESP32Servo.h>

// WiFi configuration
WiFiMulti wifiMulti;

#define DEVICE "livingroom"  //change device name

const char* WIFI_SSID = "SSID"; // SSID
const char* WIFI_PASSWORD = "Password"; //PASSWORD

// InfluxDB configuration
const char* INFLUXDB_URL = "ClusterURL"; // cluster URL
const char* INFLUXDB_TOKEN = "API Token";
const char* INFLUXDB_ORG = "org ID"; //organization ID
const char* INFLUXDB_BUCKET = "Home data"; //bucket name

// Define pins
const int DHTPIN = 12;
const int LDR = 13;
const int motionsensor = 14;

// DHT sensor parameters
#define DHTTYPE DHT22 // DHT 11
DHT_Unified dht(DHTPIN, DHTTYPE);
uint32_t delayMS;

// Servo setup
Servo doorLock;
int servoPosition = 90; // Initial position

// MQTT settings
const char *mqttServer = "broker.hivemq.com";
const char *clientID = "client_ID";
const char *topic = "/door/lock";

WiFiClient espClient;
PubSubClient mqttclient(espClient);

// Time zone information
const char *TZ_INFO = "UTC1";

// InfluxDB client instance
InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);

// Data point
Point dbdata("House_data");

void setupWifi()
{
  WiFi.mode(WIFI_STA);
  wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to Wi-Fi");
  while (wifiMulti.run() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(100);
  }
  Serial.println();
}

void setupSensors()
{
  dht.begin();
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  dht.humidity().getSensor(&sensor);

  pinMode(LDR, INPUT);
  pinMode(motionsensor, INPUT);

  // Sync time
  timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");

  if (client.validateConnection())
  {
    Serial.print("Connected to InfluxDB: ");
    Serial.println(client.getServerUrl());
  }
  else
  {
    Serial.print("InfluxDB connection failed: ");
    Serial.println(client.getLastErrorMessage());
  }
  dbdata.addTag("device", DEVICE);
  dbdata.addTag("SSID", WiFi.SSID());

    // Servo setup
  doorLock.attach(15); // Attach the servo to pin D15
  doorLock.write(servoPosition); // Set initial position
}

void readSensors()
{
  dbdata.clearFields();

  sensors_event_t event;
  dht.temperature().getEvent(&event);
  dbdata.addField("temperature", event.temperature);
  dht.humidity().getEvent(&event);
  dbdata.addField("humidity", event.relative_humidity);
  dbdata.addField("rssi", WiFi.RSSI());
  dbdata.addField("motion_sensor", digitalRead(motionsensor));
  dbdata.addField("light", digitalRead(LDR));
}

void writeToInfluxDB()
{
  Serial.print("Writing: ");
  Serial.println(dbdata.toLineProtocol());

  if (!client.writePoint(dbdata))
  {
    Serial.print("InfluxDB write failed: ");
    Serial.println(client.getLastErrorMessage());
  }

  Serial.println("Waiting 1 second");
  
}

void callback(char *topic, byte *payload, unsigned int length)
{
   Serial.print("Received message on topic: ");
  Serial.println(topic);

  Serial.print("Message payload: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  if (strcmp(topic, "/door/lock") == 0)
  {
    if (payload[0] == '1')
    {
      // Open the door lock
      doorLock.write(0); // Adjust the servo position for the open state
      Serial.println("door opened");
      delay(500);       // Keep it open for 5 seconds
      // Close the door lock
      doorLock.write(servoPosition); // Return to the initial position
    }
  }
}

void reconnect()
{
  while (!mqttclient.connected())
  {
    if (mqttclient.connect(clientID))
    {
      mqttclient.subscribe(topic);
    }
    else
    {
      delay(5000);
    }
  }
}

void mqttsetup()
{
  // MQTT setup
  mqttclient.setServer(mqttServer, 1883);
  mqttclient.setCallback(callback);
  mqttclient.setKeepAlive(15);

  if (mqttclient.connect(clientID))
  {
    mqttclient.subscribe(topic);
  }
}

void mqttTask(void *pvParameters) {
  // MQTT setup code here

  while (1) {
    // MQTT loop code here
    if (!mqttclient.connected()) {
      // Reconnect to MQTT broker
     reconnect();
    }
    mqttclient.loop();
    // Add any other MQTT-related logic here
  }
}

void setup()
{
  Serial.begin(115200);
  setupWifi();
  setupSensors();
  mqttsetup();
  xTaskCreatePinnedToCore(mqttTask, "MQTT Task", 8192, NULL, 1, NULL, 0);

}

void loop()
{
  readSensors();
  writeToInfluxDB();
}
