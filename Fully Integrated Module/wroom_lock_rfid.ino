/*  
ESP32 (Wroom) - RFID and Fingerprint Access Control System  
Combines RFID and fingerprint authentication to unlock for a set duration.  
Checks RFID tag first, followed by fingerprint matching.  
If both pass, it unlocks for a defined amount of time.  

Author: Alex Jain <ajain@pdx.edu>, Anthony Le <anthle@pdx.edu>  
Date: Nov 19th, 2024  
Version: 0.3  

Necessary Libraries:  
1. Adafruit Fingerprint Sensor Library: https://github.com/adafruit/Adafruit-Fingerprint-Sensor-Library  
2. Adafruit GFX Library: https://github.com/adafruit/Adafruit-GFX-Library  
3. TFT ILI9163C Library: https://github.com/sumotoy/TFT_ILI9163C  
4. MFRC522 RFID Library: https://github.com/miguelbalboa/rfid  

Hardware Requirements:  
1. ESP32 Wroom/Feather Dev Module  
2. TFT ILI9163C Display  
3. Adafruit Fingerprint Sensor (UART-based)  
4. MFRC522 RFID Module  
5. Relay for lock mechanism  

Note: Ensure all pin connections match the pin definitions in the code.  
*/

// Library Headers
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <HardwareSerial.h>
#include <Adafruit_Fingerprint.h>
#include <TFT_ILI9163C.h>
#include <MFRC522.h>

// Pin Definitions
#define __CS 15     // Chip Select for TFT as GPIO 15
#define __DC 2      // Data/Command for TFT as GPIO 2
#define A0 4        // Reset for TFT as GPIO 4
#define SCK 18      // Clock for TFT as GPIO 18
#define RX_PIN 16   // RX2 for fingerprint sensor
#define TX_PIN 17   // TX2 for fingerprint sensor

#define SS_PIN 5    // Chip Select for RFID as GPIO 5
#define RST_PIN 3   // Reset for RFID as GPIO 3
#define RELAY_PIN 32 // Relay to control lock mechanism as GPIO 32

// Unlock duration in seconds
#define UNLOCK_TIME 5

// RFID Tag UID (Replace with your own)
byte keyTagUID[4] = {0x39, 0xFB, 0xFF, 0x00};

// Color definitions for TFT
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

// Instantiate objects
TFT_ILI9163C tft(__CS, __DC, A0);       // TFT Display
HardwareSerial mySerial(2);            // Serial2 for fingerprint sensor
Adafruit_Fingerprint finger(&mySerial);
MFRC522 rfid(SS_PIN, RST_PIN);         // RFID Reader

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  mySerial.begin(57600, SERIAL_8N1, RX_PIN, TX_PIN);

  // Initialize TFT display
  tft.begin();
  tft.setRotation(2); // Adjust rotation
  tft.fillScreen(BLACK);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);

  // Display startup message
  tft.println(" \n\n Access\n Control\n System");
  delay(2000);

  // Initialize RFID reader
  SPI.begin();
  rfid.PCD_Init();

  // Initialize fingerprint sensor
  if (finger.verifyPassword()) {
    tft.fillScreen(BLACK);
    tft.println(" Finger\n Sensor\n Ready");
    delay(2000);
  } else {
    tft.fillScreen(RED);
    tft.println(" Finger\n Sensor\n Error");
    while (1); // Halt if fingerprint sensor fails to initialize
  }

  // Configure relay pin
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); // Ensure lock is locked initially
}

void loop() {
  // Step 1: Scan RFID tag
  tft.fillScreen(BLACK);
  tft.println(" \n\n Scan\n RFID...");
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    if (verifyRFID()) {
      tft.fillScreen(GREEN);
      tft.println(" RFID OK\n Place\n Finger");
      delay(2000);

      // Step 2: Scan fingerprint
      int id = getFingerprintID();
      if (id != -1) {
        tft.fillScreen(GREEN);
        tft.print(" Access\n Granted\n ID: ");
        tft.println(id);

        // Unlock for the defined duration
        unlockDoor();
      } else {
        tft.fillScreen(RED);
        tft.println(" Finger\n No Match");
        delay(2000);
      }
    } else {
      tft.fillScreen(RED);
      tft.println(" RFID\n Denied");
      delay(2000);
    }
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }

  delay(3000); // Wait before restarting the loop
}

bool verifyRFID() {
  // Compare scanned RFID tag with the stored UID
  for (byte i = 0; i < 4; i++) {
    if (rfid.uid.uidByte[i] != keyTagUID[i]) return false;
  }
  return true;
}

int getFingerprintID() {
  int result = FINGERPRINT_NOFINGER;
  while (result != FINGERPRINT_OK) {
    result = finger.getImage();
    if (result == FINGERPRINT_OK) break;
    if (result == FINGERPRINT_NOFINGER) continue;
    return -1; // Error or timeout
  }

  result = finger.image2Tz();
  if (result != FINGERPRINT_OK) return -1;

  result = finger.fingerFastSearch();
  if (result != FINGERPRINT_OK) return -1;

  return finger.fingerID; // Return matched ID
}

void unlockDoor() {
  digitalWrite(RELAY_PIN, HIGH); // Unlock the door
  delay(UNLOCK_TIME * 1000);    // Keep unlocked for the set duration
  digitalWrite(RELAY_PIN, LOW); // Relock the door
}
