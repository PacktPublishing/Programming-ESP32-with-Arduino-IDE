#include <Adafruit_Sensor.h>
#include <DHT_U.h>

// Define pins
const int DHTPIN = 12;
const int LDR = 13;
const int motionsensor = 14;

// DHT sensor parameters
#define DHTTYPE DHT22 // DHT 11
DHT_Unified dht(DHTPIN, DHTTYPE);
uint32_t delayMS;

void setupSensors()
{
  dht.begin();
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  dht.humidity().getSensor(&sensor);

  pinMode(LDR, INPUT);
  pinMode(motionsensor, INPUT);
}
void readSensors()
{
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  float temperature = event.temperature;
  Serial.println("Temperature : " + String(temperature));
  dht.humidity().getEvent(&event);
  float humidity = event.relative_humidity;
  Serial.println("Humidity : " + String(humidity));
  Serial.println("Motion : " + String(digitalRead(motionsensor)));
  Serial.println("Light : " + String(digitalRead(LDR)));
}
void setup()
{
  Serial.begin(115200);
  setupSensors();
}
void loop()
{
  readSensors();
  delay(1000);
}
