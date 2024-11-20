/*  
ESP32 (Wroom) - RFID and Fingerprint Access Control System  
Combines RFID and fingerprint authentication to unlock for a set duration.  
Checks RFID tag first, followed by fingerprint matching.  
If both pass, it unlocks for a defined amount of time.  

Author: Alex Jain <ajain@pdx.edu>, Anthony Le <anthle@pdx.edu>  
Date: Nov 19th, 2024  
Version: 0.4  

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
#define UNLOCK_TIME 2

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

// Not sure how to make this look like actual fingerprint
// const uint8_t fingerprintBitmap[] PROGMEM = {
//   // 128x128 Fingerprint Image Bitmap (each byte represents 8 pixels)
//   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Row 1
//   0x00, 0x1C, 0x3E, 0x3E, 0x1C, 0x00, 0x00, 0x00, // Row 2
//   0x00, 0x3E, 0x7F, 0x7F, 0x3E, 0x00, 0x00, 0x00, // Row 3
//   0x00, 0x3E, 0x7F, 0x77, 0x3E, 0x1C, 0x00, 0x00, // Row 4
//   0x00, 0x1C, 0x3E, 0x77, 0x77, 0x3E, 0x00, 0x00, // Row 5
//   0x00, 0x00, 0x1C, 0x3E, 0x77, 0x3E, 0x1C, 0x00, // Row 6
//   0x00, 0x00, 0x00, 0x1C, 0x3E, 0x3E, 0x3E, 0x00, // Row 7
//   0x00, 0x00, 0x00, 0x00, 0x1C, 0x3E, 0x1C, 0x00, // Row 8
//   // Repeat pattern or add unique details for the remaining rows
// };


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
  tft.setRotation(2); // Adjust rotation for your specific setup
  tft.fillScreen(BLACK);
  tft.setTextColor(WHITE);
  tft.setTextSize(1);
  tft.setCursor(0, 0);

  // Boot-up message with animation
  for (int i = 1; i <= 3; i++) {
    tft.fillScreen(BLACK); // Clear the screen for each frame
    tft.setCursor(10, 30);
    tft.println("Booting up");

    // Add dots based on the current iteration
    if (i == 1) {
      tft.println(" System.");
    } else if (i == 2) {
      tft.println(" System..");
    } else if (i == 3) {
      tft.println(" System...");
    }
    delay(1000); // Wait for 1 second per frame
  }
  delay(1000); // Additional delay before moving on

  // Display fingerprint image
  tft.fillScreen(BLACK);
  tft.setTextSize(1);
  tft.setCursor(10, 30);
  tft.println("Initializing");
  tft.setCursor(10, 50);
  tft.println("Fingerprint Sensor...");
  delay(1000);

  // Just for fun if we can get it to work
  // Draw the fingerprint image (use a 128x128 bitmap)
  // tft.drawBitmap(0, 0, fingerprintBitmap, 128, 128, WHITE); // Centered on display
  // delay(3000); // Wait for 3 seconds

  // Display system message
  tft.fillScreen(BLACK);
  tft.setTextSize(2);
  tft.setCursor(10, 20);
  tft.println("Access");
  tft.setCursor(10, 60);
  tft.println("Control");
  tft.setCursor(10, 100);
  tft.println("System");
  delay(3000); // Wait for 3 seconds

  // Display startup message
  tft.println(" \n\n Access\n Control\n System");
  delay(2000);

  // Initialize RFID reader
  SPI.begin();
  rfid.PCD_Init();

  // Initialize fingerprint sensor
  if (finger.verifyPassword()) {
    tft.fillScreen(BLACK);
    tft.println(" \n\n Finger\n Sensor\n Ready");
    delay(2000);
  } else {
    tft.fillScreen(RED);
    tft.println(" \n Finger\n Sensor\n Error");
    while (1); // Halt if fingerprint sensor fails to initialize
  }

  // Configure relay pin
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); // Ensure lock is locked initially
}

void loop() {
  // Step 1: Prompt user to scan RFID tag
  tft.fillScreen(BLACK);
  tft.setCursor(0, 0);
  tft.println(" \n\n Scan\n RFID...");
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    if (verifyRFID()) {
      // RFID verified, prompt for fingerprint
      tft.fillScreen(GREEN);
      tft.setCursor(0, 0);
      tft.println(" \n\n RFID \n Valid\n ");
      delay(2000);
      tft.fillScreen(BLACK);
      tft.setCursor(0, 0);
      tft.println(" \n\n Place\n Finger\n ");
      
      // Step 2: Scan fingerprint
      int id = getFingerprintID();
      if (id != -1) {
        // Fingerprint matched, grant access
        tft.fillScreen(GREEN);
        tft.setCursor(0, 0);
        tft.print(" \n\n Access\n Granted\n ID: ");
        tft.println(id);
        
        // Unlock for the defined duration
        unlockDoor();
      } else {
        // Fingerprint mismatch
        tft.fillScreen(RED);
        tft.setCursor(0, 0);
        tft.println(" \n\n Finger\n No Match");
        delay(2000);
      }
    } else {
      // RFID mismatch
      tft.fillScreen(RED);
      tft.setCursor(0, 0);
      tft.println(" \n\n RFID\n Denied");
      delay(2000);
    }
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  } else {
    // No RFID detected
    delay(1000); // Short delay before re-prompting
  }
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
  // Displays message opening 
  delay(2000);
  tft.println(" Unlocking \n ");
  tft.fillScreen(BLACK);
  delay(UNLOCK_TIME * 1000);    // Keep unlocked for the set duration
  digitalWrite(RELAY_PIN, LOW); // Relock the door
}
