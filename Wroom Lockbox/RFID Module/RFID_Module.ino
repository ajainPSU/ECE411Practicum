/*  
  ESP32 RFID Reader - Validity Check Example
  
  This program demonstrates how to use an MFRC522 RFID module with an ESP32.
  It reads an RFID tag and checks if the tag matches a stored UID.
  If the tag is valid, it prints the UID and indicates access is granted.
  If the tag is invalid, it prints the UID and denies access.
  
  Author: Anthony Le <anthle@pdx.edu>  
  Date: Nov 19th, 2024  
  Version: 1.0  
  
  Necessary Libraries:
  1. MFRC522 RFID Library: https://github.com/miguelbalboa/rfid
  
  Hardware Requirements:
  1. ESP32 Wroom/Feather Dev Module
  2. MFRC522 RFID Module
  
  Pin Definitions:
  - Adjust GPIO pins as needed.
*/

#include <SPI.h>              // Include the SPI library for communication with RFID
#include <MFRC522.h>          // Include the MFRC522 library for handling RFID

// Pin Definitions for the MFRC522 RFID reader
#define SS_PIN 5    // Chip Select for RFID as GPIO 5
#define RST_PIN 3   // Reset for RFID as GPIO 3

// RFID UID (replace with your own UID for a valid tag)
byte validTagUID[4] = {0x39, 0xFB, 0xFF, 0x00};  // Example UID

MFRC522 rfid(SS_PIN, RST_PIN);  // Create MFRC522 instance for RFID reader

void setup() {
  // Initialize Serial communication for debugging (115200 baud rate)
  Serial.begin(115200);
  
  // Initialize SPI communication for RFID
  SPI.begin();                 // Start SPI communication
  rfid.PCD_Init();             // Initialize the MFRC522 RFID reader
  
  Serial.println("Scan an RFID tag...");
}

void loop() {
  // Check if a new RFID tag is present
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    // Output the UID of the scanned RFID tag
    Serial.print("UID of the tag: ");
    for (byte i = 0; i < rfid.uid.size; i++) {
      Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
      Serial.print(rfid.uid.uidByte[i], HEX);
    }
    Serial.println();
    
    // Compare scanned RFID UID with the stored valid UID
    if (verifyRFID()) {
      Serial.println("Access Granted");
    } else {
      Serial.println("Access Denied");
    }

    // Halt the current card to prepare for the next one
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  } else {
    // No new RFID tag detected, continue to loop
    delay(100);  // Short delay before checking again
  }
}

bool verifyRFID() {
  // Compare the scanned RFID UID with the stored valid UID
  for (byte i = 0; i < 4; i++) {
    if (rfid.uid.uidByte[i] != validTagUID[i]) {
      return false;  // Return false if the UID doesn't match
    }
  }
  return true;  // Return true if the UID matches
}
