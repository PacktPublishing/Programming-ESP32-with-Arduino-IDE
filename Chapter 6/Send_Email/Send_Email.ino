#include <DHT.h>
#include <WiFi.h>
#include <ESP_Mail_Client.h>

//Sensors interfacing & parameters

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
  delay(10000);
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
