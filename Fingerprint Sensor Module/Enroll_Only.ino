/***************************************************
  Adafruit Fingerprint Sensor Enrollment Example
  for ESP32 WROOM (using HardwareSerial with custom RX/TX pins)

  This code demonstrates how to enroll a fingerprint 
  in the Adafruit Fingerprint Sensor using the ESP32 WROOM 
  board with the Adafruit_Fingerprint library.

  Wiring:
  - TX of Fingerprint sensor to RX (GPIO16) on ESP32
  - RX of Fingerprint sensor to TX (GPIO17) on ESP32

  Written by [Your Name], [Date]
  BSD license, all text above must be included in any redistribution
 ***************************************************/

// Include the necessary libraries for fingerprint sensor
#include <Adafruit_Fingerprint.h>

// Define RX and TX pins for ESP32 WROOM (GPIO16 for RX, GPIO17 for TX)
#define RX_PIN 16  // RX pin for ESP32 (connected to TX of fingerprint sensor)
#define TX_PIN 17  // TX pin for ESP32 (connected to RX of fingerprint sensor)

// Use HardwareSerial (Serial1) for communication on ESP32
HardwareSerial mySerial(1);  // Use Serial1 for ESP32

// Create an instance of the fingerprint sensor
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

// Variable to store the user ID (1-127) for enrollment
uint8_t id;

void setup() {
  // Start the Serial Monitor for debugging and user interaction
  Serial.begin(9600);
  while (!Serial);  // For ESP32, wait until the Serial monitor is ready
  delay(100);  // Short delay for the serial monitor to initialize
  Serial.println("\n\nAdafruit Fingerprint sensor enrollment");

  // Initialize the serial communication with the fingerprint sensor
  mySerial.begin(57600, SERIAL_8N1, RX_PIN, TX_PIN);  // Set baud rate to 57600 for fingerprint sensor

  // Verify if the fingerprint sensor is connected and working
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }  // Halt the program if no sensor is found
  }

  // Print sensor parameters for debugging
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

// Function to read the ID number from the user input (ID range: 1-127)
uint8_t readnumber(void) {
  uint8_t num = 0;

  while (num == 0) {
    while (! Serial.available());  // Wait until user enters a value
    num = Serial.parseInt();  // Read the entered number
  }
  return num;
}

void loop() {
  // Prompt the user to input the ID number to enroll a fingerprint
  Serial.println("Ready to enroll a fingerprint!");
  Serial.println("Please type in the ID # (from 1 to 127) you want to save this finger as...");
  id = readnumber();  // Read the ID number entered by the user
  if (id == 0) {  // ID #0 is not allowed, prompt the user again
    return;
  }
  Serial.print("Enrolling ID #");
  Serial.println(id);

  // Start the fingerprint enrollment process
  while (!getFingerprintEnroll());
}

// Function to handle the fingerprint enrollment process
uint8_t getFingerprintEnroll() {
  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #"); Serial.println(id);
  
  // Wait for the user to place a finger on the sensor
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();  // Capture an image of the fingerprint
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        Serial.print(".");  // No finger detected, continue waiting
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        break;
      default:
        Serial.println("Unknown error");
        break;
    }
  }

  // Convert the captured image to a template
  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // Ask the user to remove their finger
  Serial.println("Remove finger");
  delay(2000);  // Wait for 2 seconds
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();  // Wait until the finger is removed
  }
  
  Serial.print("ID "); Serial.println(id);
  p = -1;
  Serial.println("Place same finger again");

  // Wait for the user to place the same finger again
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();  // Capture the second image of the fingerprint
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        Serial.print(".");  // No finger detected, continue waiting
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        break;
      default:
        Serial.println("Unknown error");
        break;
    }
  }

  // Convert the second image to a template
  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // Create a fingerprint model from the two captured images
  Serial.print("Creating model for #");  Serial.println(id);

  p = finger.createModel();  // Create a fingerprint model from the two images
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  // Store the fingerprint model in the specified location
  Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);  // Store the fingerprint model in the sensor's memory
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  return true;  // Successful enrollment
}
