#include <DHT.h>
//Sensors interfacing & parameters
#define DHTPIN 13          // DHT22 data pin
#define DHTTYPE DHT22       // DHT22 sensor model
#define MoistureSensor 34   // Moisture pin
DHT dht(DHTPIN, DHTTYPE);
int moisturePin = MoistureSensor;
int moistureThresholds[] = {300, 700};  // Adjust these thresholds for your setup
int tempThreshold = 30;
int humidityThreshold = 40;

void setup() {
  Serial.begin(115200);
  dht.begin();
}
void loop() {
  float temperature = readTemperature();
  float humidity = readHumidity();
  int moistureValue = readMoisture();
  String moistureStatus = getMoistureStatus(moistureValue);
  Serial.println(PlantMessages(temperature, humidity, moistureValue, moistureStatus));
  delay(20000);
}
float readTemperature() {
  return dht.readTemperature();
}
float readHumidity() {
  return dht.readHumidity();
}
int readMoisture() {
  return analogRead(moisturePin);
}
String PlantMessages(float temperature, float humidity, int moistureValue, String moistureStatus) {
  String Message = "";
  Message = "Moisture Status is "+moistureStatus+".";
  if(moistureStatus == "Dry"){
    Message += " I need Water Now.";
  }
  if (temperature > tempThreshold){
      Message += "\nThe weather is hot, which causes the soil to dry out more quickly. I will need water more frequently.";
    }
  if (humidity < humidityThreshold){
      Message += "\nLow Moisture Level, Water evaporates quickly. I will need water Frequently.";
    }
  Message += "\nSummary of Data: \n Moisture Status : " + moistureStatus +"\n Moisture Value : "+ String(moistureValue) + "\n Temperature : "+ String(temperature) +"\n Humidity : "+ String(humidity);
  return Message;
  }
String getMoistureStatus(int value) {
  if (value < moistureThresholds[0]) {
    return "Dry";
  } else if (value >= moistureThresholds[0] && value <= moistureThresholds[1]) {
    return "Ok";
  } else {
    return "Wet";
  }
}
