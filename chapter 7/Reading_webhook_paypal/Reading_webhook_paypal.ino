#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <time.h>

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

const char* ssid = "Your SSID";
const char* password = "Your Password";
const char* apiKey = "Your API key"; //insert your webhook.site api key
const char* url = "https://webhook.site/token/df23f2ff-1b38-452c-8c4b-928e1c437ce4/request/latest/"; //replace with your webhook.site api key
String lastModifiedHeader = "";

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  int payment = checkPayment();
  delay(6000);
}

int checkPayment() {
  String amountPaid;
  int timeLimitMinutes = 2;
  timeClient.update();
  time_t now = timeClient.getEpochTime();

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(url);
    http.addHeader("accept", "application/json");
    http.addHeader("api-key", apiKey);

    if (!lastModifiedHeader.isEmpty()) {
      http.addHeader("If-Modified-Since", lastModifiedHeader);
    }

    int httpCode = http.GET();

    if (httpCode > 0) {
      if (httpCode == HTTP_CODE_OK) {
        lastModifiedHeader = http.header("Last-Modified");
        String payload = http.getString();

        if (parseJsonData(payload, amountPaid, now, timeLimitMinutes)) {
          return amountPaid.toInt();
        }
      } else if (httpCode == HTTP_CODE_NOT_MODIFIED) {
        Serial.println("No new data available.");
      } else {
        Serial.printf("HTTP error code: %d\n", httpCode);
      }
    } else {
      Serial.println("Failed to connect to the server.");
    }
    http.end();
  }
  return amountPaid.toInt();
}

bool parseJsonData(String payload, String &amountPaid, time_t now, int timeLimitMinutes) {
  StaticJsonDocument<4096> jsonDocument;
  DeserializationError error = deserializeJson(jsonDocument, payload);

  if (error) {
    Serial.println("JSON parsing error: " + String(error.c_str()));
    return false;
  }

  String timestampStr = jsonDocument["created_at"].as<String>();
  struct tm createdTime;
  if (!parseTimestamp(timestampStr, createdTime)) {
    return false;
  }

  int minutesSinceData = (now - mktime(&createdTime)) / 60;

  if (minutesSinceData <= timeLimitMinutes) {
    String name = jsonDocument["request"]["first_name"].as<String>() + " " + jsonDocument["request"]["last_name"].as<String>();
    String email = jsonDocument["request"]["payer_email"].as<String>();
    amountPaid = jsonDocument["request"]["mc_gross"].as<String>();
    Serial.println("Name: " + name);
    Serial.println("Email: " + email);
    Serial.println("Amount Paid: " + amountPaid);
  } else {
    Serial.println("Data is too old to process.");
    return false;
  }

  return true;
}

bool parseTimestamp(String timestampStr, struct tm &createdTime) {
  if (strptime(timestampStr.c_str(), "%Y-%m-%d %H:%M:%S", &createdTime)) {
    return true;
  } else {
    Serial.println("Failed to parse timestamp.");
    return false;
  }
}
