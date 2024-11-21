/*  
  ESP32 TFT Display - Hello World Example
  
  This simple program demonstrates how to display "Hello World" on an ILI9163C TFT display connected to an ESP32.
  The message is printed to the screen when the ESP32 is powered on.
  
  Author: Anthony Le <anthle@pdx.edu>  
  Date: Nov 19th, 2024  
  Version: 1.0  
  
  Necessary Libraries:
  1. Adafruit GFX Library: https://github.com/adafruit/Adafruit-GFX-Library
  2. TFT ILI9163C Library: https://github.com/sumotoy/TFT_ILI9163C
  
  Hardware Requirements:
  1. ESP32 Wroom/Feather Dev Module
  2. TFT ILI9163C Display
*/

#include <SPI.h>                // Include the SPI library for communication with TFT
#include <Adafruit_GFX.h>        // Include the Adafruit GFX library for graphics functions
#include <TFT_ILI9163C.h>        // Include the TFT ILI9163C library for the specific TFT display

// Pin Definitions for TFT display
#define __CS 15     // Chip Select for TFT as GPIO 15
#define __DC 2      // Data/Command for TFT as GPIO 2
#define A0 4        // Reset for TFT as GPIO 4
#define SCK 18      // Clock for TFT as GPIO 18

// Color definitions for TFT
// Define color in 16-bit RGB format
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

// Instantiate TFT object using the defined pin connections
TFT_ILI9163C tft(__CS, __DC, A0);  // Create a TFT display object

void setup() {
  // Initialize Serial communication for debugging (115200 baud rate)
  Serial.begin(115200);

  // Initialize the TFT display
  tft.begin();                    // Start the TFT display
  tft.setRotation(2);              // Set display rotation (adjust as needed)
  tft.fillScreen(BLACK);           // Fill the screen with a black background
  tft.setTextColor(WHITE);         // Set the text color to white
  tft.setTextSize(2);              // Set text size (2 is medium size)

  // Display "Hello World" message at position (10, 30) on the TFT screen
  tft.setCursor(10, 30);           // Set the cursor to the (10, 30) position
  tft.println("Hello \n World");      // Print "Hello World" to the TFT screen
}

void loop() {
  // Nothing needs to happen in the loop for this example
  // The "Hello World" message is displayed once on the screen during setup
}
