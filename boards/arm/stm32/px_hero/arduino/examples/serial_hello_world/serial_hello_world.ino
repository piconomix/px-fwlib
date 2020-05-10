// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
}

// the loop routine runs over and over again forever:
void loop() {
  Serial.println("Hello World!");
  delay(1000);        // delay in between reads for stability
}
