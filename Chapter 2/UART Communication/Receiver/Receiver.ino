void setup() { 

  // Set UART baud rate to 9600 

  Serial.begin(9600); 

} 

 

void loop() { 

  if (Serial.available() > 0) { 

    // If data is available to read 

    String receivedData = Serial.readString(); 

    // Process or use the received data 

    // Example: Print the received data 

    Serial.print("Received Data: "); 

    Serial.println(receivedData); 

  } 

} 