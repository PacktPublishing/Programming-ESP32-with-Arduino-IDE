#include <DHT.h>
#include <WiFi.h>
#include <ESP_Mail_Client.h>
#include <HTTPClient.h>
#include <UrlEncode.h>

//Sensors interfacing & parameters
#define DHTPIN 13          // DHT22 data pin
#define DHTTYPE DHT22       // DHT22 sensor model
#define MoistureSensor 34   // Moisture pin
DHT dht(DHTPIN, DHTTYPE);
int moisturePin = MoistureSensor;
int moistureThresholds[] = {300, 700};  // Adjust these thresholds for your setup
int tempThreshold = 30;
int humidityThreshold = 40;

//WiFi Credentitals
#define WIFI_SSID "Your WIFI SSID"
#define WIFI_PASSWORD "WIFI PASSWORD"

//Email Credentials
#define SMTP_server "smtp.gmail.com"
#define SMTP_Port 465
#define sender_email "youremail@gmail.com"
#define sender_password "password that we generated"
#define Recipient_email "receiver_email@gmail.com"
#define Recipient_name "Recipient name"
SMTPSession smtp;

//connectWiFi() Function
void connectWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

//SendEmail(const String& messageText) function
void sendEmail(const String& messageText) {
  ESP_Mail_Session session;
  session.server.host_name = SMTP_server ;
  session.server.port = SMTP_Port;
  session.login.email = sender_email;
  session.login.password = sender_password;
  session.login.user_domain = "";
  SMTP_Message message;
  message.sender.name = "ESP32";
  message.sender.email = sender_email;
  message.subject = "ESP32 Plant Updates";
  message.addRecipient(Recipient_name, Recipient_email);
  // Send simple text message
  message.text.content = messageText.c_str();
  message.text.charSet = "us-ascii";
  message.text.transfer_encoding = Content_Transfer_Encoding::enc_7bit;
  if (!smtp.connect(&session)){
    Serial.println("Email Sent");
    return;
}
  if (!MailClient.sendMail(&smtp, &message)){
    Serial.println("Error sending Email, " + smtp.errorReason());
  }
}

void setup() {
  Serial.begin(115200);
  connectWiFi();
  dht.begin();
}

void loop() {
  float temperature = readTemperature();
  float humidity = readHumidity();
  
  int moistureValue = readMoisture();
  String moistureStatus = getMoistureStatus(moistureValue);
  String msg = PlantMessages(temperature, humidity, moistureValue, moistureStatus);
  Serial.println(msg);
  sendEmail(msg);
  sendWhatsAppMessage(msg);
  sendTelegramMessage(msg);
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

void sendWhatsAppMessage(const String& message) {
  String apiUrl = "https://api.callmebot.com/whatsapp.php";
  String phoneNumber = "Phone Number"; // Replace with the desired phone number
  String apiKey = "API Key"; // Replace with your API key
  String url = apiUrl + "?phone=" + phoneNumber + "&text=" + urlEncode(message) + "&apikey=" + apiKey;
  HTTPClient http;
  http.begin(url);
  int httpResponseCode = http.GET();
  if (httpResponseCode == 200) {
    Serial.println("WhatsApp message sent successfully!");
  } else {
    Serial.print("Error sending WhatsApp message. HTTP code: ");
    Serial.println(httpResponseCode);
  }
  http.end();
}

void sendTelegramMessage(const String& message) {
  String apiUrl = "http://api.callmebot.com/text.php";
  String user = "Photon67000"; // Replace with your user ID
  String url = apiUrl + "?user=" + user + "&text=" + urlEncode(message);
  HTTPClient http;
  http.begin(url);
  int httpResponseCode = http.GET();

  if (httpResponseCode == 200) {
    Serial.println("Telegram message sent successfully!");
  } else {
    Serial.print("Error sending Telegram message. HTTP code: ");
    Serial.println(httpResponseCode);
  }
  http.end();
}
