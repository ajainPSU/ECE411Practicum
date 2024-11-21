# ECE411Practicum

## Team 12 Members:
- Felix Moss
- Davyd Gamza
- Alex Jain
- Anthony Le

### Project: Fingerprint + RFID Lockbox

This repository contains code and resources for the **Fingerprint + RFID Lockbox** project developed as part of the ECE 411 practicum. The project utilizes an **ESP32 Wroom** microcontroller to integrate fingerprint and RFID authentication for a secure lockbox system. The system is capable of reading RFID tags and verifying fingerprints to grant or deny access, with an optional relay mechanism to unlock the box.

Note* ESP32_Feather_Wifi_Database No Longer Needed
---

### Key Features:

- **Fingerprint Authentication:** Using an **Adafruit Fingerprint Sensor** to match user fingerprints.
- **RFID Authentication:** Using the **MFRC522 RFID module** to scan RFID tags and validate the user.
- **TFT Display:** Displays system status and instructions on a **TFT ILI9163C display**.
- **Relay Control:** Unlocks the lockbox for a set duration upon successful authentication.
- **ESP32 Wroom:** The system is built on the **ESP32 Wroom** module, allowing for robust processing and connectivity.

---

### Repository Overview:

- **Bill_of_Materials:** List of components used in the project.
- **Display_Module:** Code for testing the TFT display with the ESP32 Wroom.
- **ESP32_Feather_Wifi_Database (No Longer Needed):** Previously used for WiFi connectivity and database management.
- **ESP32_Practicum:** Updated version of the main project file.
- **Fingerprint Sensor Module:** Code for testing and using the fingerprint sensor.
- **Fully Integrated Module:** Complete system code integrating RFID, fingerprint sensor, and relay.
- **Functional_Decomp:** Decomposition of the system into functional parts for easier development and testing.
- **LockTest:** Testing the lock mechanism functionality.
- **RFID Module:** Code for working with the MFRC522 RFID module.
- **Team_Meeting_Notes:** Notes from team meetings and discussions.
- **Temporary Files:** Placeholder for temporary or discarded files.
- **WroomTest:** Code for testing ESP32 Wroom functionality with serial print statements.

---

### Notes:
- The **ESP32_Feather_Wifi_Database** folder is no longer necessary and has been removed from the active project files.
- Make sure to check the **Fingerprint Sensor Module** and **Fully Integrated Module** for the latest updates on the system functionality.

---

### License:
This project is licensed under the [GPL-3.0 license](LICENSE).

---

### Setup Instructions:

1. **Hardware:**
   - **ESP32 Wroom** module
   - **Adafruit Fingerprint Sensor** (UART-based)
   - **MFRC522 RFID module**
   - **TFT ILI9163C display**
   - **Relay for lock mechanism**

2. **Software:**
   - Install the necessary libraries:
     - **Adafruit Fingerprint Sensor Library**
     - **Adafruit GFX Library**
     - **MFRC522 RFID Library**
     - **TFT ILI9163C Library**

3. **Connections:**
   - Refer to the individual files for pin connections and hardware setup.

---

### Getting Started:
1. Clone the repository to your local machine:
   ```bash
   git clone https://github.com/ajainPSU/ECE411Practicum.git
