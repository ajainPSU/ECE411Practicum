/*

  ESP32 (Feather/Wroom)
  Checks if RFID key provided is correct & unlocks if yes.
  Author: Alex Jain <ajain@pdx.edu>
  Date: Nov 11th, 2024
  Version: 0.1

 */

#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN    23  // PIN23 
#define RST_PIN   13 // PIN13 

MFRC522 rfid(SS_PIN, RST_PIN);

byte keyTagUID[4] = {0xF3, 0x88, 0x17, 0x2A};  // Note: White Card Alex is holding onto.

void setup() {
  Serial.begin(9600);
  SPI.begin(); // init SPI bus
  rfid.PCD_Init(); // init MFRC522

  Serial.println("Tap an RFID/NFC tag on the RFID-RC522 reader");
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
        delay(2000);
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
}
