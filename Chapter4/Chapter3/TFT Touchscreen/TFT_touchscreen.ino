#include <Adafruit_ILI9341.h> 

#include <Adafruit_GFX.h> 

#include <Adafruit_FT6206.h> 

#define TFT_CS 15 

#define TFT_DC 2 

#define SCREEN_WIDTH 320 

#define SCREEN_HEIGHT 240 

 

#define BACKGROUND_COLOR ILI9341_BLACK 

#define TEXT_COLOR ILI9341_WHITE 

#define HIGHLIGHT_COLOR ILI9341_YELLOW 

 

#define TS_MINX 0    // Replace with the minimum X touch value 

#define TS_MAXX 240  // Replace with the maximum X touch value 

#define TS_MINY 0    // Replace with the minimum Y touch value 

#define TS_MAXY 320  // Replace with the maximum Y touch value 

 

Adafruit_FT6206 ts = Adafruit_FT6206(); 

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC); 

 

const char* menuOptions[] = { 

  "Toggle LED", 

  "Show Temp", 

  "Change Background", 

  "Rotate Screen", 

  "Restart ESP32" 

}; 

int currentOption = 0; 

int numOptions = sizeof(menuOptions) / sizeof(menuOptions[0]); 

 

#define LED 5 

int LED_state = 0; 

 

void setup() { 

  tft.begin(); 

  tft.setRotation(0); 

  tft.fillScreen(BACKGROUND_COLOR); 

  tft.setTextSize(2); 

  tft.setTextColor(TEXT_COLOR); 

  pinMode(LED, OUTPUT); 

 

  if (!ts.begin(40)) { 

    while (1); 

  } 

  drawMenu(); 

} 

 

void loop() { 

  if (ts.touched()) { 

    TS_Point touchPoint = ts.getPoint(); 

    int16_t screenX = map(touchPoint.x, TS_MINX, TS_MAXX, SCREEN_WIDTH - 1, 0); 

    int16_t screenY = map(touchPoint.y, TS_MINY, TS_MAXY, SCREEN_HEIGHT - 1, 0); 

 

    if (screenY >= 0 && screenY < numOptions * 30) { 

      int selectedOption = screenY / 30; 

      drawMenu(); 

      tft.setTextColor(HIGHLIGHT_COLOR); 

      tft.setCursor(10, selectedOption * 30); 

      tft.println(menuOptions[selectedOption]); 

      tft.setTextColor(TEXT_COLOR); 

      switch (selectedOption) { 

        case 0: 

          Toggle_LED(); 

          break; 

        case 1: 

          showTemp(); 

          break; 

        case 2: 

          tft.fillScreen(random(0xFFFF)); 

          break; 

        case 3: 

          tft.setRotation((tft.getRotation() + 1) % 4); 

          break; 

        case 4: 

          ESP.restart(); 

          break; 

        default: 

          drawMenu(); 

          break; 

      } 

    } 

  } 

} 

void drawMenu() { 

  tft.fillScreen(BACKGROUND_COLOR); 

  for (int i = 0; i < numOptions; i++) { 

    tft.setCursor(10, i * 30); 

    if (i == currentOption) { 

      tft.setTextColor(HIGHLIGHT_COLOR); 

    } else { 

      tft.setTextColor(TEXT_COLOR); 

    } 

    tft.println(menuOptions[i]); 

  } 

} 

void Toggle_LED() { 

  LED_state = !LED_state; 

  digitalWrite(LED, LED_state); 

} 

void showTemp() { 

  float temperature = 25.5; 

  tft.fillScreen(BACKGROUND_COLOR); 

  tft.setTextSize(3); 

  tft.setCursor(0, 30); 

  tft.setTextColor(ILI9341_GREEN); 

  tft.print("Temperature"); 

  tft.setTextSize(5); 

  tft.setCursor(30, 100); 

  tft.setTextColor(TEXT_COLOR); 

  tft.print(temperature, 1); 

  tft.setTextSize(3); 

  tft.setCursor(200, 105); 

  tft.print("C"); 

  tft.setTextSize(2); 

} 
