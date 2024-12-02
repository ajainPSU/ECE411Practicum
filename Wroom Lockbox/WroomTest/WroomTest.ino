/*

  ESP32 (Wroom)
  Test Code that blinks LED on WROOM to make sure it works.
  Note* the LED should be red when plugged in signaling the power is on.
  Author: Anthony Le <anthle@pdx.edu>
  Date: Nov 18th, 2024
  Version: 0.1

  - Used ESP32 Library and ESP32 Dev Module as board
 */

#define LED_BUILTIN 2 // Replace to whatever the onboard LED Pin is

void setup() {
  // Start the Serial Monitor for debugging and user interaction
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("LED ON");
  delay(500);
  digitalWrite(LED_BUILTIN,LOW);
  Serial.println("LED OFF");
  delay(500);
}
