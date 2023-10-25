// Digital input and output pin definitions 

#define BUTTON_PIN 12 

#define LED_PIN 13 

void setup() { 

 // Set the button pin as input 

 pinMode(BUTTON_PIN, INPUT_PULLUP); 

 // Set the LED pin as output 

 pinMode(LED_PIN, OUTPUT); 

} 

void loop() { 

 // Read the state of the button 

 int buttonState = digitalRead(BUTTON_PIN); 

 // If the button is pressed (LOW state), turn on the LED 

 if (buttonState == LOW) { 

  digitalWrite(LED_PIN, HIGH); 

 } else { 

  // Otherwise, turn off the LED 

  digitalWrite(LED_PIN, LOW); 

 } 

} 