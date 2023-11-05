#include <DHT.h>
#include <WiFi.h>
#include <ESP_Mail_Client.h>
#include <HTTPClient.h>
#include <UrlEncode.h>
#include <WiFiClientSecure.h>
#include "time.h"
#include <TweESP32.h>
#include <TwitterServerCert.h>
#include <UrlEncode.h>
#include <ArduinoJson.h>

// Sensors interfacing & parameters
#define DHTPIN 13         // DHT22 data pin
#define DHTTYPE DHT22     // DHT22 sensor model
#define MoistureSensor 34 // Moisture pin
DHT dht(DHTPIN, DHTTYPE);
int moisturePin = MoistureSensor;
int moistureThresholds[] = {300, 700}; // Adjust these thresholds for your setup
int tempThreshold = 30;
int humidityThreshold = 40;

// WiFi Credentitals
#define WIFI_SSID "WiFi SSID"
#define WIFI_PASSWORD "WIFI Password"

// twitter credentials, update them with your credentials
const char *consumerKey = "j4jpQJNKyxZHFUac";
const char *consumerSecret = "9DnlbfcmRMvruXZEm3pbpHKUF";
const char *accessToken = "1697183233648013312-iRzF";
const char *accessTokenSecret = "yb13u10BqsQ2unYBHR";

WiFiClientSecure client;
TweESP32 twitter(client, consumerKey, consumerSecret, accessToken, accessTokenSecret);
// connectWiFi() Function
void connectWiFi()
{
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");
}

void setup()
{
    Serial.begin(115200);
    connectWiFi();
    dht.begin();
}

void loop()
{
    float temperature = readTemperature();
    float humidity = readHumidity();
    int moistureValue = readMoisture();
    String moistureStatus = getMoistureStatus(moistureValue);
    String msg = PlantMessages(temperature, humidity, moistureValue, moistureStatus);
    Serial.println(msg);
    sendTelegramMessage(msg);
    delay(20000);
}

float readTemperature()
{
    return dht.readTemperature();
}
float readHumidity()
{
    return dht.readHumidity();
}
int readMoisture()
{
    return analogRead(moisturePin);
}

// PlantMessages function
String PlantMessages(float temperature, float humidity, int moistureValue, String moistureStatus)
{
    String Message = "";
    bool NeedWater = false;
    Message = "Moisture Status is " + moistureStatus + ".";
    if (moistureStatus == "Dry")
    {
        Message += " I need Water Now.";
    }
    if (temperature > tempThreshold)
    {
        Message += "The weather is hot, which causes the soil to dry out more quickly. I will need water more frequently.";
    }
    if (humidity < humidityThreshold)
    {
        Message += "Low Moisture Level, Water evaporates quickly. I will need water Frequently.";
    }
    Message += "Summary of Data:  Moisture Status : " + moistureStatus + " Moisture Value : " + String(moistureValue) + " Temperature : " + String(temperature) + " Humidity : " + String(humidity);
    return Message;
}

// getMoistureStatus function
String getMoistureStatus(int value)
{
    if (value < moistureThresholds[0])
    {
        return "Dry";
    }
    else if (value >= moistureThresholds[0] && value <= moistureThresholds[1])
    {
        return "Ok";
    }
    else
    {
        return "Wet";
    }
}

// send Tweet
void sendTweet(const char *tweetText)
{
    twitter.timeConfig();
    client.setCACert(twitter_server_cert);
    bool success = twitter.sendTweet(const_cast<char *>(tweetText));
    if (success)
    {
        Serial.println("Tweet Sent");
    }
}
