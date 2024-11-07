/*
   ESP32/Huzzah32 Feather w/ Fingerprint Sensor

   Reads from fingerprint scanner and then displays valid or not

   by Anthony Le
   modified 1 Nov 2024
*/
/*
  Notes*
  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  Fingerprint Sensor
    - Default Baudrate - 57600
    - Transmit Format - 10 bits
    - Uses UART

    System Status Register:
    - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    | Bit     | 15-4     | 3                                | 2                         |  1                            | 0                        |
    - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    | Purpose | Reserved | ImgBufStat                       | PWD                       |  Pass                         | Busy                     |                                            |
    |         |          | Set to 1 means there is a        | Set to 1 means the        |  Set to 1 indicate that       | Set to 1 to indicate     |
    |         |          | valid fingerprint image in the   | device handshake password |  fingerprint veritification   | that system is executing |
    |         |          | fingerprint image buffer.        | is verified.              |  has passed.                  | commands.                |
    - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

*/
/*
#include <Adafruit_Fingerprint.h>

// Define RX and TX pins for ESP32 Feather
#define RX_PIN 16   // Green wire from sensor (Data Out - TD)
#define TX_PIN 17   // White wire from sensor (Data In - RD)

// Set up Hardware Serial on RX and TX pins
HardwareSerial mySerial(1);  // Using Serial1 for ESP32 HardwareSerial
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
uint8_t id;

////////////////////////////////////////////////////////////////////////////////////////////
// SETUP

void setup() {
  Serial.begin(9600);
  delay(100);
  Serial.println("\n\nAdafruit Fingerprint sensor Enrollment");

  // Initialize Serial1 on the defined RX and TX pins with a baud rate of 57600
  mySerial.begin(57600, SERIAL_8N1, RX_PIN, TX_PIN);
  finger.begin(57600);

  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }

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

uint8_t readnumber(void) {
  uint8_t num = 0;

  while (num == 0) {
    while (! Serial.available());
    num = Serial.parseInt();
  }
  return num;
}

////////////////////////////////////////////////////////////////////////////////////////////
// MAIN LOOP
void loop() {
  Serial.println("Ready to enroll a fingerprint!");
  Serial.println("Please type in the ID # (from 1 to 127) you want to save this finger as...");
  id = readnumber();
  if (id == 0) { // ID #0 not allowed, try again!
    return;
  }
  Serial.print("Enrolling ID #");
  Serial.println(id);

  while (! getFingerprintEnroll() );
  Serial.println("Finger Enrollment Finished!");


  
}

////////////////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS 

// Obtain Fingerprint and adds it
uint8_t getFingerprintEnroll() {
  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #"); Serial.println(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
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

  // OK success!

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

  Serial.println("Remove finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id);
  p = -1;
  Serial.println("Place same finger again");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
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

  // OK success!

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

  // OK converted!
  Serial.print("Creating model for #");  Serial.println(id);

  p = finger.createModel();
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

  Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
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

  return true;
}
*/
#include <Adafruit_Fingerprint.h>

// Define RX and TX pins for ESP32 Feather
#define RX_PIN 16   // Green wire from sensor (Data Out - TD)
#define TX_PIN 17   // White wire from sensor (Data In - RD)

// Set up Hardware Serial on RX and TX pins
HardwareSerial mySerial(1);  // Using Serial1 for ESP32 HardwareSerial
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
uint8_t id;

// Function prototypes
void controlLEDs();
int getFingerprintIDez();
uint8_t getFingerprintEnroll();
uint8_t readnumber(void);

void setup() {
  Serial.begin(9600);
  delay(100);
  Serial.println("\n\nAdafruit Fingerprint sensor Enrollment");

  // Initialize Serial1 on the defined RX and TX pins with a baud rate of 57600
  mySerial.begin(57600, SERIAL_8N1, RX_PIN, TX_PIN);
  finger.begin(57600);

  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }

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

void loop() {
  controlLEDs(); // Control LEDs to indicate status

  // Display the menu
  Serial.println("\nMenu:");
  Serial.println("1: Enroll Fingerprint");
  Serial.println("2: Verify Fingerprint");
  Serial.println("3: Exit");
  Serial.print("Choose an option: ");
  
  while (!Serial.available()); // Wait for user input
  char option = Serial.read(); // Read user input
  Serial.println(option);

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
      Serial.println("Exiting...");
      while (1); // Halt the program
      break;
    default:
      break;
  }

  delay(100); // Delay to avoid overwhelming the sensor
}

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
  Serial.print("Waiting for finger to verify... ");
  // Turn on the fingerprint sensor LED
  finger.LEDcontrol(FINGERPRINT_LED_ON, 0, FINGERPRINT_LED_RED);

  int fingerID = -1; // Initialize with an invalid ID
  while (fingerID < 0) { // Keep polling until a valid finger is detected
    fingerID = getFingerprintIDez();
    if (fingerID < 0) {
      Serial.print("No finger detected or no match found. Retrying...");
      delay(500); // Wait a bit before trying again
    }
  }

  // If we get here, a valid finger was detected
  Serial.print("Detected fingerprint ID: ");
  Serial.println(fingerID);
  Serial.println("User is authorized.");
}

// Get the fingerprint ID
int getFingerprintIDez() {
  int p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      return -1; // No finger detected
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return -1; // Error in communication
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return -1; // Error taking image
    default:
      Serial.println("Unknown error");
      return -1; // Unknown error
  }

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Could not convert image");
      return -1; // Error converting image
    case FINGERPRINT_NOTFOUND:
      Serial.println("No matching fingerprint found");
      return -1; // No matching fingerprint
    default:
      Serial.println("Unknown error");
      return -1; // Unknown error
  }

  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.print("Found a print match! ID: ");
    return finger.fingerID; // Return the matched fingerprint ID
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    return -1; // No match found
  } else {
    Serial.println("Unknown error");
    return -1; // Unknown error
  }
}

// Function to read a number from Serial
uint8_t readnumber(void) {
  uint8_t num = 0;

  while (num == 0) {
    while (!Serial.available());
    num = Serial.parseInt();
  }
  return num;
}

// Obtain fingerprint and adds it
uint8_t getFingerprintEnroll() {
  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #"); Serial.println(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
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

  // OK success!
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

  Serial.println("Remove finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id);
  p = -1;
  Serial.println("Place same finger again");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
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

  // OK success!
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

  Serial.print("Creating model for #"); Serial.println(id);
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Fingerprint model created");
  } else {
    Serial.print("Model could not be created. Error: "); Serial.println(p);
    return p;
  }

  Serial.print("Storing model #"); Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Fingerprint stored!");
  } else {
    Serial.print("Error storing fingerprint. Error: "); Serial.println(p);
    return p;
  }

  return 0; // Indicate success
}

