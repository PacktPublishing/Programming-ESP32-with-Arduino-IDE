#include <Adafruit_Sensor.h>
#include <DHT_U.h>
#include <WiFiMulti.h>
#include <InfluxDbClient.h> //https://github.com/tobiasschuerg/InfluxDB-Client-for-Arduino.git
#include <InfluxDbCloud.h>

// WiFi configuration
WiFiMulti wifiMulti;

#define DEVICE "kitchen"  //change device name

const char* WIFI_SSID = "SSID"; // SSID
const char* WIFI_PASSWORD = "Password"; //PASSWORD

// InfluxDB configuration
const char* INFLUXDB_URL = "ClusterURL"; // cluster URL
const char* INFLUXDB_TOKEN = "API Token” //api token
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

// Time zone information
const char* TZ_INFO = "UTC1";

// InfluxDB client instance
InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);

// Data point
Point dbdata("House_data");

void setupWifi() {
  WiFi.mode(WIFI_STA);
  wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);
  
  Serial.print("Connecting to Wi-Fi");
  while (wifiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  Serial.println();
}

void setupSensors() {
  dht.begin();
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  dht.humidity().getSensor(&sensor);
  
  pinMode(LDR, INPUT);
  pinMode(motionsensor, INPUT);
  
  // Sync time
  timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");
  
  if (client.validateConnection()) {
    Serial.print("Connected to InfluxDB: ");
    Serial.println(client.getServerUrl());
  } else {
    Serial.print("InfluxDB connection failed: ");
    Serial.println(client.getLastErrorMessage());
  }
  dbdata.addTag("device", DEVICE);
  dbdata.addTag("SSID", WiFi.SSID());
}

void readSensors() {
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

void writeToInfluxDB() {
  Serial.print("Writing: ");
  Serial.println(dbdata.toLineProtocol());

  if (!client.writePoint(dbdata)) {
    Serial.print("InfluxDB write failed: ");
    Serial.println(client.getLastErrorMessage());
  }

  Serial.println("Waiting 1 second");
  delay(5000);
}

void setup() {
  Serial.begin(115200);
  setupWifi();
  setupSensors();
}

void loop() {
  readSensors();
  writeToInfluxDB();
delay(1000);
}

