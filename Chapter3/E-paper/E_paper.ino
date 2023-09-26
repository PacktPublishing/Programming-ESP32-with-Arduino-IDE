include <GxEPD2_BW.h> 

#include <Adafruit_GFX.h>   

#include <Fonts/FreeMonoBold12pt7b.h> 

#include <Fonts/FreeSerif9pt7b.h> 

 

GxEPD2_BW<GxEPD2_290, GxEPD2_290::HEIGHT> display(GxEPD2_290(/*CS=*/ 5, /*DC=*/ 17, /*RST=*/ 16, /*BUSY=*/ 4)); 

 

void setup() { 

  Serial.begin(115200); 

  display.init(); 

  display.setRotation(1); 

} 

void loop() { 

  display.fillScreen(GxEPD_WHITE); 

  display.setFont(&FreeMonoBold12pt7b); 

  display.setTextColor(GxEPD_BLACK); 

  display.setCursor((display.width() - 11) / 2, display.height() / 2); 

  display.println("Hello World"); 

  display.display(); 

  delay(10000); 

} 
