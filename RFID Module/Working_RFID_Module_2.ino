/*

  ESP32 (Feather/Wroom)
  Checks if RFID key provided is correct & unlocks if yes.
  Author: Alex Jain <ajain@pdx.edu>
  Date: Nov 11th, 2024
  Version: 0.1

 */

#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN    5  // GPIO PIN-5  (Pin 34)
#define RST_PIN   3 // GPIO PIN-16 (Pin 25)
#define RELAY_PIN 32 // Connect PIN32 to Relay (Pin 12)

MFRC522 rfid(SS_PIN, RST_PIN);

byte keyTagUID[4] = {0x52, 0x8E, 0xAF, 0x02};  // Note: White Card Alex is holding onto.

void setup() {
  Serial.begin(9600);
  Serial.println("Setting Up now!");
  delay(500); // Setup Delay
  SPI.begin(); // init SPI bus
  rfid.PCD_Init(); // init MFRC522
  pinMode(RELAY_PIN, OUTPUT); // initialize pin as output
  digitalWrite(RELAY_PIN, LOW); // Initialize Lock

  Serial.println("Program is Live");
}

void loop() {
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) { // New tag available and read
    MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);

    if (rfid.uid.uidByte[0] == keyTagUID[0] &&
        rfid.uid.uidByte[1] == keyTagUID[1] &&
        rfid.uid.uidByte[2] == keyTagUID[2] &&
        rfid.uid.uidByte[3] == keyTagUID[3]) {
      Serial.println("Access granted");
      digitalWrite(RELAY_PIN, HIGH); // Unlock
      delay(2000); // Keep unlocked for 2 seconds
      digitalWrite(RELAY_PIN, LOW); // Relock
    } else {
      Serial.print("Access denied, UID:");
      for (int i = 0; i < rfid.uid.size; i++) {
        Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(rfid.uid.uidByte[i], HEX);
      }
      Serial.println();
    }

    rfid.PICC_HaltA();          // Halt PICC
    rfid.PCD_StopCrypto1();     // Stop encryption
  }

  // Optionally reset the reader
  rfid.PCD_Init();
}

