/* 
ESP32 (Wroom)
Checks fingerprint from enrollment and other options
Author: Anthony Le <anthle@pdx.edu>
Date: Nov 18th, 2024
Version: 0.2

Necessary Libraries:
1: https://github.com/adafruit/Adafruit-Fingerprint-Sensor-Library
2: Baudrate set at 9600
3: Use ESP32 Dev Module
*/

#include <Adafruit_Fingerprint.h>

// Define RX and TX pins for ESP32-WROOM
#define RX_PIN 16   // Green wire from sensor (Data Out - TD)
#define TX_PIN 17   // White wire from sensor (Data In - RD)

// Set up Hardware Serial on RX and TX pins
HardwareSerial mySerial(1);  // Using Serial1 for ESP32 HardwareSerial
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
uint8_t id;

// Function prototypes
void controlLEDs();
void enrollFingerprint();
void verifyFingerprint();
void deleteFingerprint();
void clearAllFingerprints();
void displayAllFingerprints();
int getFingerprintIDez();
uint8_t getFingerprintEnroll();
uint8_t readnumber(void);

////////////////////////////////////////////////////////////////////////////////////////////
// SETUP
void setup() {
  Serial.begin(9600);
  delay(100);
  Serial.println("\n\nAdafruit Fingerprint Sensor Operations");

  // Initialize Serial1 on the defined RX and TX pins with a baud rate of 57600
  mySerial.begin(57600, SERIAL_8N1, RX_PIN, TX_PIN);
  finger.begin(57600);

  // Check if the fingerprint sensor is found
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }

  // Display sensor parameters
  Serial.println(F("Reading sensor parameters"));
  finger.getParameters();
  Serial.print(F("Status: 0x")); Serial.println(finger.status_reg, HEX);
  Serial.print(F("Sys ID: 0x")); Serial.println(finger.system_id, HEX);
  Serial.print(F("Capacity: ")); Serial.println(finger.capacity);
  Serial.print(F("Security level: ")); Serial.println(finger.security_level);
  Serial.print(F("Device address: ")); Serial.println(finger.device_addr, HEX);
  Serial.print(F("Packet len: ")); Serial.println(finger.packet_len);
  Serial.print(F("Baud rate: ")); Serial.println(finger.baud_rate);
}

////////////////////////////////////////////////////////////////////////////////////////////
// MAIN LOOP
void loop() {
  controlLEDs(); // Control LEDs to indicate status
  delay(1000);
  // Display the menu
  Serial.println("\nMenu:");
  Serial.println("1: Enroll Fingerprint");
  Serial.println("2: Verify Fingerprint");
  Serial.println("3: Delete Fingerprint");
  Serial.println("4: Clear All Fingerprints");
  Serial.println("5: Display All Fingerprints");
  Serial.println("6: Exit");
  Serial.print("Choose an option: ");
  
  while (!Serial.available()); // Wait for user input
  delay(1000);
  char option = Serial.read(); // Read user input
  Serial.println("\nOption chosen is: ");
  Serial.println(option);
  Serial.println("\n");

  switch (option) {
    case '1':
      Serial.println("You selected: Enroll Fingerprint");
      enrollFingerprint();
      break;
    case '2':
      Serial.println("You selected: Verify Fingerprint");
      verifyFingerprint();
      break;
    case '3':
      Serial.println("You selected: Delete Fingerprint");
      deleteFingerprint();
      break;
    case '4':
      Serial.println("You selected: Clear All Fingerprints");
      clearAllFingerprints();
      break;
    case '5':
      Serial.println("You selected: Display All Fingerprints");
      displayAllFingerprints();
      break;
    case '6':
      Serial.println("Exiting...");
      while (1); // Halt the program
      break;
    default:
      Serial.println("Invalid option. Try again.");
      break;
  }

  delay(100); // Delay to avoid overwhelming the sensor
}

////////////////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS

// Control LED indicators
void controlLEDs() {
  // LED fully on
  finger.LEDcontrol(FINGERPRINT_LED_ON, 0, FINGERPRINT_LED_RED);
  delay(250);
  finger.LEDcontrol(FINGERPRINT_LED_ON, 0, FINGERPRINT_LED_BLUE);
  delay(250);
  finger.LEDcontrol(FINGERPRINT_LED_ON, 0, FINGERPRINT_LED_PURPLE);
  delay(250);

  // Flash red LED
  finger.LEDcontrol(FINGERPRINT_LED_FLASHING, 25, FINGERPRINT_LED_RED, 10);
  delay(2000);
  // Breathe blue LED
  finger.LEDcontrol(FINGERPRINT_LED_BREATHING, 100, FINGERPRINT_LED_BLUE);
  delay(3000);
  finger.LEDcontrol(FINGERPRINT_LED_GRADUAL_ON, 200, FINGERPRINT_LED_PURPLE);
  delay(2000);
  finger.LEDcontrol(FINGERPRINT_LED_GRADUAL_OFF, 200, FINGERPRINT_LED_PURPLE);
  delay(2000);
}

// Function to enroll a fingerprint
void enrollFingerprint() {
  Serial.println("Ready to enroll a fingerprint!");
  Serial.println("Please type in the ID # (from 1 to 127) you want to save this finger as...");
  id = readnumber();
  if (id == 0) { // ID #0 not allowed, try again!
    return;
  }
  Serial.print("Enrolling ID #");
  Serial.println(id);

  while (!getFingerprintEnroll());
  Serial.println("Finger Enrollment Finished!");
}

// Function to verify a fingerprint
void verifyFingerprint() {
  Serial.println("Waiting for a finger to verify...");

  int fingerID = -1;
  while (fingerID < 0) {
    fingerID = getFingerprintIDez();
  }

  Serial.print("Fingerprint verified! ID: ");
  Serial.println(fingerID);
}

// Function to delete a fingerprint
void deleteFingerprint() {
  Serial.println("Enter the ID of the fingerprint to delete:");
  id = readnumber();
  if (finger.deleteModel(id) == FINGERPRINT_OK) {
    Serial.println("Fingerprint deleted successfully.");
  } else {
    Serial.println("Failed to delete fingerprint.");
  }
}

// Function to clear all fingerprints
void clearAllFingerprints() {
  if (finger.emptyDatabase() == FINGERPRINT_OK) {
    Serial.println("All fingerprints cleared from database.");
  } else {
    Serial.println("Failed to clear fingerprints.");
  }
}

// Function to display all stored fingerprints
void displayAllFingerprints() {
  Serial.println("Stored Fingerprint IDs:");
  for (uint8_t i = 1; i <= finger.capacity; i++) {
    if (finger.loadModel(i) == FINGERPRINT_OK) {
      Serial.print("Fingerprint ID: ");
      Serial.println(i);
    }
  }
}

// Get the fingerprint ID quickly
int getFingerprintIDez() {
  int p = -1;

  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    if (p == FINGERPRINT_NOFINGER) {
      delay(100); // Wait a bit and retry
      continue;   // Keep polling
    } else if (p == FINGERPRINT_OK) {
      Serial.println("Image captured.");
    } else {
      Serial.println("Error capturing image.");
      return -1; // Exit on an unexpected error
    }
  }

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) {
    Serial.println("Failed to convert image.");
    return -1;
  }

  p = finger.fingerSearch();
  if (p != FINGERPRINT_OK) {
    Serial.println("No match found.");
    return -1;
  }

  return finger.fingerID;
}

// Read a number from the Serial monitor
uint8_t readnumber(void) {
  uint8_t num = 0;
  while (num == 0) {
    while (!Serial.available());
    num = Serial.parseInt();
  }
  return num;
}

uint8_t getFingerprintEnroll() {
  int p = -1;
  unsigned long startMillis = millis();  // Start a timer
  unsigned long timeout = 30000;  // 30 seconds timeout

  // Step 1: Capture first image
  Serial.print("Waiting for valid finger to enroll as #");
  Serial.println(id);

  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    if (p == FINGERPRINT_NOFINGER) {
      delay(100); // Wait a bit and retry
      if (millis() - startMillis > timeout) {  // Timeout check
        Serial.println("Timeout! No finger detected.");
        return -1;  // Timeout occurred
      }
      continue;
    } else if (p == FINGERPRINT_OK) {
      Serial.println("\nImage captured.");
    } else {
      Serial.println("Error capturing image.");
      return p;
    }
  }

  // Step 2: Wait for second image
  Serial.println("Remove finger and place it again...");
  delay(2000);
  while ((p = finger.getImage()) != FINGERPRINT_NOFINGER);

  Serial.println("Place the same finger again...");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    if (p == FINGERPRINT_NOFINGER) {
      delay(100); // Wait and retry
      if (millis() - startMillis > timeout) {  // Timeout check
        Serial.println("Timeout! No second finger detected.");
        return -1;  // Timeout occurred
      }
      continue;
    } else if (p == FINGERPRINT_OK) {
      Serial.println("Image captured.");
    } else {
      Serial.println("Error capturing image.");
      return p;
    }
  }

  p = finger.image2Tz(2);
  if (p != FINGERPRINT_OK) {
    Serial.println("Failed to convert second image to template.");
    return p;
  }

  // Step 3: Create and store the model
  if (finger.createModel() != FINGERPRINT_OK) {
    Serial.println("Failed to create fingerprint model.");
    return -1;
  }

  if (finger.storeModel(id) == FINGERPRINT_OK) {
    Serial.println("Fingerprint enrolled successfully.");
    return FINGERPRINT_OK;
  } else {
    Serial.println("Failed to store fingerprint.");
    return -1;
  }
}
