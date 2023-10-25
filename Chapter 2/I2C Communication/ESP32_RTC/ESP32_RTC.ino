#include <wire.h> 

#include <RTClib.h> 

RTC_DS1307 rtc; 

void setup() { 

 Serial.begin(9600); 

 Wire.begin(); 

 // Uncomment the following line if the RTC has not been initialized 

 // rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); 

 if (!rtc.begin()) { 

  Serial.println("Couldn't find RTC"); 

  while (1); 

 } 

 if (!rtc.isrunning()) { 

  Serial.println("RTC is not running!"); 

  // Uncomment the following line to set the RTC to the date and time at the moment of uploading the code 

  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); 

 } 

} 

void loop() { 

 DateTime now = rtc.now(); 

 Serial.print("Current Date and Time: "); 

 Serial.print(now.year(), DEC); 

 Serial.print('/'); 

 Serial.print(now.month(), DEC); 

 Serial.print('/'); 

 Serial.print(now.day(), DEC); 

 Serial.print(' '); 

 Serial.print(now.hour(), DEC); 

 Serial.print(':'); 

 Serial.print(now.minute(), DEC); 

 Serial.print(':'); 

 Serial.print(now.second(), DEC); 

 Serial.println();  

 delay(1000);  

} 