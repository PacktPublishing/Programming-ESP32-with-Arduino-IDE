#include <Wire.h> 

#include <LiquidCrystal_I2C.h> 

const int LCD_ADDRESS = 0x27; 

const int LCD_COLS = 16; 

const int LCD_ROWS = 2; 

LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLS, LCD_ROWS); 

void setup() { 

  lcd.init(); 

  lcd.backlight(); 

  lcd.setCursor(0, 0); 

  lcd.print("Hello, ESP32!"); 

  lcd.setCursor(0, 1); 

  lcd.print("LCD Example"); 

  delay(3000); 

  lcd.clear(); 

  lcd.print("Scrolling Text:"); 

  lcd.autoscroll(); 

  for (int i = 0; i < 16; i++) { 

    lcd.scrollDisplayLeft(); 

    delay(500); 

  } 

  lcd.noAutoscroll(); 

  lcd.clear(); 

  byte heart[8] = { 

    B00000, 

    B01010, 

    B11111, 

    B11111, 

    B01110, 

    B00100, 

    B00000, 

  }; 

  lcd.createChar(0, heart); 

  lcd.setCursor(0, 0); 

  lcd.print("I "); 

  lcd.write(byte(0)); 

  lcd.print(" ESP32!"); 

} 

void loop() { 

  // Do nothing in the loop 

} 
