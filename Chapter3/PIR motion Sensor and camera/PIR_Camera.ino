#include "esp_camera.h" 

#include <SD_MMC.h> 


#define CAMERA_MODEL_AI_THINKER 

#define PWDN_GPIO_NUM    -1 

#define RESET_GPIO_NUM   -1 

#define XCLK_GPIO_NUM    4 

#define SIOD_GPIO_NUM    18 

#define SIOC_GPIO_NUM    23 

#define Y9_GPIO_NUM      36 

#define Y8_GPIO_NUM      37 

#define Y7_GPIO_NUM      38 

#define Y6_GPIO_NUM      39 

#define Y5_GPIO_NUM      35 

#define Y4_GPIO_NUM      14 

#define Y3_GPIO_NUM      13 

#define Y2_GPIO_NUM      34 

#define VSYNC_GPIO_NUM   5 

#define HREF_GPIO_NUM    27 

#define PCLK_GPIO_NUM    25 

// Motion sensor configuration 

#define MOTION_SENSOR_PIN 2 



void setup() { 

  Serial.begin(115200); 

  // Initialize SD card 

  if (!SD_MMC.begin()) { 

    Serial.println("SD card initialization failed"); 

    return; 

  } 

  Serial.println("SD card initialized"); 

  // Initialize the camera 

  camera_config_t config; 

  config.ledc_channel = LEDC_CHANNEL_0; 

  config.ledc_timer = LEDC_TIMER_0; 

  config.pin_d0 = Y2_GPIO_NUM; 

  config.pin_d1 = Y3_GPIO_NUM; 

  config.pin_d2 = Y4_GPIO_NUM; 

  config.pin_d3 = Y5_GPIO_NUM; 

  config.pin_d4 = Y6_GPIO_NUM; 

  config.pin_d5 = Y7_GPIO_NUM; 

  config.pin_d6 = Y8_GPIO_NUM; 

  config.pin_d7 = Y9_GPIO_NUM; 

  config.pin_xclk = XCLK_GPIO_NUM; 

  config.pin_pclk = PCLK_GPIO_NUM; 

  config.pin_vsync = VSYNC_GPIO_NUM; 

  config.pin_href = HREF_GPIO_NUM; 

  config.pin_sscb_sda = SIOD_GPIO_NUM; 

  config.pin_sscb_scl = SIOC_GPIO_NUM; 

  config.pin_pwdn = PWDN_GPIO_NUM; 

  config.pin_reset = RESET_GPIO_NUM; 

  config.xclk_freq_hz = 20000000; 

  config.pixel_format = PIXFORMAT_JPEG; 

  // Initialize camera module 

  if (esp_camera_init(&config) != ESP_OK) { 

    Serial.println("Camera initialization failed"); 

    return; 

  } 

  Serial.println("Camera initialized"); 


   // Configure motion sensor pin as input
  pinMode(MOTION_SENSOR_PIN, INPUT);

} 

 
void loop() { 

  if (digitalRead(MOTION_SENSOR_PIN) == HIGH) {

    // Motion detected, capture image 

    // Capture image 

    camera_fb_t* fb = NULL; 

    fb = esp_camera_fb_get(); 

    if (!fb) { 

      Serial.println("Camera capture failed"); 

      return; 

    } 

    // Create a file on the SD card 

    File file = SD_MMC.open("/image.jpg", FILE_WRITE); 

    if (!file) { 

      Serial.println("Failed to create file"); 

      return; 

    } 
    // Write the image data to the file 

    file.write(fb->buf, fb->len); 

    file.close(); 

    esp_camera_fb_return(fb); 

    Serial.println("Image captured and saved"); 

  } 

  // Delay for a short time before checking for motion again 

  delay(100); 

} 
