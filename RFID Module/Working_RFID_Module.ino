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

byte keyTagUID[4] = {0xF3, 0x88, 0x17, 0x2A};  // Note: White Card Alex is holding onto.

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
  if (rfid.PICC_IsNewCardPresent()) { // new tag is available
    if (rfid.PICC_ReadCardSerial()) { // NUID has been readed
      MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);

      if (rfid.uid.uidByte[0] == keyTagUID[0] &&
          rfid.uid.uidByte[1] == keyTagUID[1] &&
          rfid.uid.uidByte[2] == keyTagUID[2] &&
          rfid.uid.uidByte[3] == keyTagUID[3] ) {
        Serial.println("Access is granted");
        digitalWrite(RELAY_PIN, HIGH); // Unlock the lock
        delay(2000);
        digitalWrite(RELAY_PIN, LOW); // Relock the lock
      }
      else
      {
        Serial.print("Access denied, UID:");
        for (int i = 0; i < rfid.uid.size; i++) {
          Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
          Serial.print(rfid.uid.uidByte[i], HEX);
        }
        Serial.println();
      }

      rfid.PICC_HaltA(); // halt PICC
      rfid.PCD_StopCrypto1(); // stop encryption on PCD

    }
  }
  rfid.PCD_Init();
}
