#include <WiFi.h> 
#include <HTTPClient.h> 
#include <Arduino_JSON.h> 
#include <Adafruit_GFX.h> 
#include <Adafruit_SSD1306.h> 
#include <Wire.h> 

#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 
#define OLED_RESET  -1 

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); 

const char* wifiSSID = "WIFI SSID"; 
const char* wifiPassword = "WIFI PASSWORD"; 

String openWeatherMapApiKey = "Your API Key"; 
String city = "London"; //change city name 

String jsonBuffer; 

void setup() { 

 Serial.begin(115200); 
 WiFi.begin(wifiSSID, wifiPassword); 
 Serial.println("Connecting to WiFi..."); 

 while (WiFi.status() != WL_CONNECTED) { 
  delay(500); 
  Serial.print("."); 

 } 

 Serial.println("\nConnected to WiFi network. IP Address: " + WiFi.localIP().toString()); 

 if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
  Serial.println(F("SSD1306 allocation failed")); 
  for (;;); 
 } 

 display.clearDisplay(); 

} 

void loop() {  

  if (WiFi.status() == WL_CONNECTED) { 
   String serverPath = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&APPID=" + openWeatherMapApiKey + "&units=metric";   
   jsonBuffer = httpGETRequest(serverPath.c_str()); 
   Serial.println(jsonBuffer); 
   JSONVar weatherData = JSON.parse(jsonBuffer); 

   if (JSON.typeof(weatherData) == "undefined") { 
    Serial.println("Parsing input failed!"); 
    return; 
   } 

   Serial.print("JSON object = "); 
   Serial.println(weatherData); 
   Serial.print("Temperature: "); 
   Serial.println(weatherData["main"]["temp"]); 
   Serial.print("Pressure: "); 
   Serial.println(weatherData["main"]["pressure"]); 
   Serial.print("Humidity: "); 
   Serial.println(weatherData["main"]["humidity"]); 
   Serial.print("Wind Speed: "); 
   Serial.println(weatherData["wind"]["speed"]); 

   display.clearDisplay(); 
   display.setTextSize(1); 
   display.setTextColor(SSD1306_WHITE); 
   display.setCursor(0, 0); 
   display.println("City: " + city); 
   display.print("\nTemperature: "); 
   display.println(weatherData["main"]["temp"]); 
   display.print("\nPressure: "); 
   display.println(weatherData["main"]["pressure"]); 
   display.print("\nHumidity: "); 
   display.println(weatherData["main"]["humidity"]); 
   display.print("\nWind Speed: "); 
   display.println(weatherData["wind"]["speed"]); 
   display.display(); 

  } else { 

   Serial.println("WiFi Disconnected"); 
  } 

  delay(1000); 

 } 

String httpGETRequest(const char* serverName) { 

 WiFiClient client; 
 HTTPClient http; 

 http.begin(client, serverName); 

 int httpResponseCode = http.GET(); 

 String payload = "{}";  

 if (httpResponseCode > 0) { 
  Serial.print("HTTP Response code: "); 
  Serial.println(httpResponseCode); 

  payload = http.getString(); 

 } else { 

  Serial.print("HTTP Request failed. Error code: "); 

  Serial.println(httpResponseCode); 

 } 

 http.end(); 

 return payload; 

} 