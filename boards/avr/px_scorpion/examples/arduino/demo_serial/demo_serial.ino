/*
 demo_serial

 Output the string "Hello World!" to the serial monitor. Echo back any received
 characters to serial monitor.

 This example code is in the public domain.
 */

int incomingByte = 0;   // for incoming serial data

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 115200 bits per second:
  Serial.begin(115200);
  Serial.println("Hello World!");
}

// the loop routine runs over and over again forever:
void loop() {
  // Received any data?
  if (Serial.available() > 0) {
    // read the incoming byte:
    incomingByte = Serial.read();
    // echo received byte
    Serial.write(incomingByte);
  }
}



