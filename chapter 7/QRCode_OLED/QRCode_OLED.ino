#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "qrcode.h" //https://github.com/ricmoo/QRCode.git

#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 
#define OLED_RESET -1    

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
QRCode qrcode;
String paypalLink = "https://paypal.me/username"; //insert your paypal link from here

void setup()
{
  Serial.begin(115200);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  showScantoPay();
}

void loop()
{
}

void showScantoPay(void) {
  display.clearDisplay();
  display.setTextSize(2); // Change the font size to 2
  display.setTextColor(WHITE);
  uint8_t qrcodeData[qrcode_getBufferSize(3)];
  qrcode_initText(&qrcode, qrcodeData, 3, 0, paypalLink.c_str() );
  int scale = 2; // Change this for different sizes
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
  display.setCursor(65, 5); // Adjust the position as needed
  display.println("Scan");
  display.setCursor(65, 25); // Adjust the position as needed
  display.println("to");
  display.setCursor(65, 45); // Adjust the position as needed
  display.println("Open.");

  display.display();
}
