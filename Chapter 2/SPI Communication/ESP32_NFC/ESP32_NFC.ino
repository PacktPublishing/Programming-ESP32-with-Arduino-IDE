#include <SPI.h> 

#include <Adafruit_PN532.h> 

#define PN532_SCK  (14) 

#define PN532_MOSI (13) 

#define PN532_SS   (15) 

#define PN532_MISO (12) 

Adafruit_PN532 nfc(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS); 

void setup(void) { 

 Serial.begin(115200); 

 nfc.begin(); 

 uint32_t versiondata = nfc.getFirmwareVersion(); 

 if (! versiondata) { 

  Serial.print("Didn't find PN53x board"); 

  while (1); // halt 

 } 

 // Got ok data, print it out! 

 Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 

 Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 

 Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC); 

 Serial.println("Waiting for a Card ..."); 

} 

void loop(void) { 

 uint8_t success; 

 uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  

 uint8_t uidLength;             

 success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength); 

 

 if (success) { 

  Serial.println("Found a card"); 

  Serial.print(" UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes"); 

  Serial.print(" UID Value: "); 

  nfc.PrintHex(uid, uidLength); 

  if (uidLength == 4) 

  { 

   // We probably have a Mifare Classic card ... 

   uint32_t cardid = uid[0]; 

   cardid <<= 8; 

   cardid |= uid[1]; 

   cardid <<= 8; 

   cardid |= uid[2]; 

   cardid <<= 8; 

   cardid |= uid[3]; 

   Serial.print("Seems to be a Mifare Classic card #"); 

   Serial.println(cardid); 

  } 

  Serial.println(""); 

 } 

} 