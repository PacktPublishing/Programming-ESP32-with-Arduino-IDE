#define PWM_PIN 13 

void setup() { 

 pinMode(PWM_PIN, OUTPUT); 

} 

void loop() { 

 for (int dutyCycle = 0; dutyCycle <= 255; dutyCycle++) { 

  analogWrite(PWM_PIN, dutyCycle); 

  delay(10); 

 } 

} 