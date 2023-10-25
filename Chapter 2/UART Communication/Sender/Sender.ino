void setup() { 

  // Set UART baud rate to 9600 

  Serial.begin(9600); 

} 

 

void loop() { 

  // Send the string "IoT" over UART 

  Serial.println("IoT"); 

 

  // Wait for a moment before sending again 

  delay(1000); 

} 