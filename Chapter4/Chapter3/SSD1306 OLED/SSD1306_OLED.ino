#include <Wire.h> 

#include <Adafruit_GFX.h> 

#include <Adafruit_SSD1306.h> 

#define SCREEN_WIDTH 128 

#define SCREEN_HEIGHT 64 

 

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1); 

const unsigned char logo [] PROGMEM = { 

// Paste the generated logo bitmap here 
// you could convert your BMP image to c array by using this online tool: https://javl.github.io/image2cpp/ 

}; 

 

void setup() { 

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 

    Serial.println(F("SSD1306 allocation failed")); 

    for (;;); 

  } 

 

  display.clearDisplay(); 

  display.setTextSize(1); 

  display.setTextColor(SSD1306_WHITE); 

  display.setCursor(0, 0); 

  display.println("Hello, ESP32!"); 

  display.println("OLED Example"); 

  display.display(); 

  delay(2000); 

 

  display.clearDisplay(); 

  display.display(); 

} 

 

void loop() { 

  display.clearDisplay(); 

  display.setTextSize(2); 

  display.setTextColor(SSD1306_WHITE); 

  display.setCursor(0, 10); 

  display.println("ESP32"); 

  display.setTextSize(1); 

  display.setCursor(0, 30); 

  display.println("OLED Display"); 

  display.display(); 

  delay(2000); 

 

  display.clearDisplay(); 

  display.drawRect(10, 10, 50, 30, SSD1306_WHITE); 

  display.display(); 

  delay(2000); 

 

  display.clearDisplay(); 

  display.fillCircle(30, 20, 15, SSD1306_WHITE); 

  display.display(); 

  delay(2000); 
  display.clearDisplay(); 

  display.drawLine(10, 10, 50, 30, SSD1306_WHITE); 

  display.display(); 

  delay(2000); 

 

  display.clearDisplay(); 

  display.drawBitmap(0, 0, logo, 128, 64, SSD1306_WHITE); 

  display.display(); 

  delay(2000); 

} 
