#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "qrcode.h" //https://github.com/ricmoo/QRCode.git
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <time.h>
#include <ESP32Servo.h> 

enum ScreenStatus
{
  SCREEN_PARKING,
  SCREEN_PAYMENT,
  SCREEN_OPEN,
  SCREEN_PAYMENTSUCCESS,
  SCREEN_CLOSE,
  SCREEN_OTHER
};

ScreenStatus currentScreen = SCREEN_PARKING;
ScreenStatus lastScreen = SCREEN_OTHER;
const int servoPin = 14;
Servo myservo; // Create a Servo object
#define BUTTON 5
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
QRCode qrcode;

String paypalLink = "https://paypal.me/username"; // insert your

const int trigPin = 13;
const int echoPin = 12;
const int redPin = 4;
const int greenPin = 2;
const int bluePin = 15;
bool parked = false;
bool barrier = false;
int sensorRange = 15;
const char *ssid = "Wokwi-GUEST";
const char *password = "";
const char* apiKey = "Your API key"; //insert your webhook.site api key
const char* url = "https://webhook.site/token/df23f2ff-1b38-452c-8c4b-928e1c437ce4/request/latest/"; //replace with your webhook.site api key
String lastModifiedHeader = "";

void setup()
{
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }
  myservo.attach(servoPin); // Attaches the servo on the specified pin
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);

  updateDisplay();
}

void loop()
{
  if (!parked)
  {
    while (!barrier)
    {
      currentScreen = SCREEN_PARKING;
      updateDisplay();
      lastScreen = currentScreen;

      if (!digitalRead(BUTTON))
      {
        openServo();
        currentScreen = SCREEN_OPEN;
        updateDisplay();
        lastScreen = currentScreen;
        barrier = true;
        delay(1000);
      }
    }
    while (barrier)
    {
      if (!digitalRead(BUTTON))
      {
        currentScreen = SCREEN_CLOSE;
        updateDisplay();
        lastScreen = currentScreen;
        closeServo();
        barrier = false;
        parked = true;
      }
    }
  }

  while (parked)
  {
    currentScreen = SCREEN_PAYMENT;
    updateDisplay();
    lastScreen = currentScreen;
    int payment = checkPayment();
    if (payment >= 1)
    {
      openServo();
      currentScreen = SCREEN_PAYMENTSUCCESS;
      updateDisplay();
      lastScreen = currentScreen;
      while (getdistance() < sensorRange)
        ;
      currentScreen = SCREEN_CLOSE;
      updateDisplay();
      lastScreen = currentScreen;
      closeServo();
      parked = false;
      barrier = false;
      payment = 0;
    }
    delay(6000);
  }
}

void openServo()
{
  Serial.println("Servo open");
  myservo.write(0);
}
void closeServo()
{
  delay(1000);
  Serial.println("Servo closed");
  myservo.write(180);
}

int getdistance()
{
  long duration;
  int distance;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration / 2) / 29.1;
  Serial.println("Distance: " + String(distance));

  if (distance > 100)
  {
    digitalWrite(redPin, LOW);
    digitalWrite(greenPin, HIGH);
  }
  else
  {
    digitalWrite(greenPin, LOW);
    digitalWrite(redPin, HIGH);
  }
  return distance;
}

int checkPayment()
{
  String amountPaid;
  int timeLimitMinutes = 2;
  timeClient.update();
  time_t now = timeClient.getEpochTime();

  if (WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http;
    http.begin(url);
    http.addHeader("accept", "application/json");
    http.addHeader("api-key", apiKey);

    if (!lastModifiedHeader.isEmpty())
    {
      http.addHeader("If-Modified-Since", lastModifiedHeader);
    }

    int httpCode = http.GET();

    if (httpCode > 0)
    {
      if (httpCode == HTTP_CODE_OK)
      {
        lastModifiedHeader = http.header("Last-Modified");
        String payload = http.getString();

        if (parseJsonData(payload, amountPaid, now, timeLimitMinutes))
        {
          return amountPaid.toInt();
        }
      }
      else if (httpCode == HTTP_CODE_NOT_MODIFIED)
      {
        Serial.println("No new data available.");
      }
      else
      {
        Serial.printf("HTTP error code: %d\n", httpCode);
      }
    }
    else
    {
      Serial.println("Failed to connect to the server.");
    }
    http.end();
  }
  return amountPaid.toInt();
}

bool parseJsonData(String payload, String &amountPaid, time_t now, int timeLimitMinutes)
{
  StaticJsonDocument<4096> jsonDocument;
  DeserializationError error = deserializeJson(jsonDocument, payload);

  if (error)
  {
    Serial.println("JSON parsing error: " + String(error.c_str()));
    return false;
  }

  String timestampStr = jsonDocument["created_at"].as<String>();
  struct tm createdTime;
  if (!parseTimestamp(timestampStr, createdTime))
  {
    return false;
  }

  int minutesSinceData = (now - mktime(&createdTime)) / 60;

  if (minutesSinceData <= timeLimitMinutes)
  {
    String name = jsonDocument["request"]["first_name"].as<String>() + " " + jsonDocument["request"]["last_name"].as<String>();
    String email = jsonDocument["request"]["payer_email"].as<String>();
    amountPaid = jsonDocument["request"]["mc_gross"].as<String>();
    Serial.println("Name: " + name);
    Serial.println("Email: " + email);
    Serial.println("Amount Paid: " + amountPaid);
  }
  else
  {
    Serial.println("Data is too old to process.");
    return false;
  }

  return true;
}

bool parseTimestamp(String timestampStr, struct tm &createdTime)
{
  if (strptime(timestampStr.c_str(), "%Y-%m-%d %H:%M:%S", &createdTime))
  {
    return true;
  }
  else
  {
    Serial.println("Failed to parse timestamp.");
    return false;
  }
}

void updateDisplay()
{
  if (currentScreen != lastScreen)
  {
    uint8_t qrcodeData[qrcode_getBufferSize(3)];
    int scale = 2;
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);

    switch (currentScreen)
    {
    case SCREEN_PARKING:
      display.setTextSize(1);
      display.setCursor(0, 0);
      display.println("Paid parking!\n");
      display.println("Press button to open.\n");
      display.println("Pay while leaving.\n");
      display.println("Using Paypal.");
      break;

    case SCREEN_PAYMENT:
      qrcode_initText(&qrcode, qrcodeData, 3, 0, paypalLink.c_str());

      for (uint8_t y = 0; y < qrcode.size; y++)
      {
        for (uint8_t x = 0; x < qrcode.size; x++)
        {
          if (qrcode_getModule(&qrcode, x, y))
          {
            display.fillRect(x * scale, y * scale, scale, scale, WHITE);
          }
        }
      }
      display.setTextSize(2);
      display.setCursor(65, 5);
      display.println("Scan");
      display.setCursor(65, 25);
      display.println("to");
      display.setCursor(65, 45);
      display.println("Open.");
      break;

    case SCREEN_OPEN:
      display.setTextSize(1);
      display.setCursor(0, 0);
      display.println("Door Opened.\n");
      display.println("Park Car.\n");
      display.println("Press button to Close.\n");
      break;

    case SCREEN_CLOSE:
      display.setTextSize(1);
      display.setCursor(0, 0);
      display.println("Door is Closing\n");
      display.println("Stay away.\n");
      delay(2000);
      break;

    case SCREEN_PAYMENTSUCCESS:
      display.setTextSize(1);
      display.setCursor(0, 0);
      display.println("Payment Successful.\n");
      display.println("Opening door.\n");
      display.println("Take Out car.\n");
      break;

    case SCREEN_OTHER:
      display.setTextSize(1);
      display.println("Some other screen");
      break;
    }
    display.display();
  }
}
