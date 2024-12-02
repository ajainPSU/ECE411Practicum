/*
  ESP32 TFT Display - Hello World Example
  
  This simple program demonstrates how to display "Hello World" on an ILI9163C TFT display connected to an ESP32.
  The program cycles through all available colors on the screen and prints the respective color name.
  
  Author: Anthony Le <anthle@pdx.edu>  
  Date: Dec 1st, 2024  
  Version: 1.3  
  
  Necessary Libraries:
  1. Adafruit GFX Library: https://github.com/adafruit/Adafruit-GFX-Library
  2. TFT ILI9163C Library: https://github.com/sumotoy/TFT_ILI9163C
  
  Hardware Requirements:
  1. ESP32 Wroom/Feather Dev Module
  2. TFT ILI9163C Display
*/

#include <SPI.h>                 // SPI library for communication with the TFT
#include <Adafruit_GFX.h>        // Adafruit GFX library for graphics functions
#include <TFT_ILI9163C.h>        // TFT ILI9163C library for the specific display

// Pin Definitions for TFT Display
#define __CS  15  // Chip Select pin
#define __DC  2   // Data/Command pin
#define A0    4   // Reset pin
#define SCK   18  // Clock pin

// Color Definitions (16-bit RGB format)
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#define TIME 5  // Delay time in seconds

// Create a TFT display object using the defined pin connections
TFT_ILI9163C tft(__CS, __DC, A0);

void setup() {
  // Initialize Serial communication for debugging (115200 baud rate)
  Serial.begin(115200);
  Serial.println("Booting up Display Program...\n");

  // Initialize the TFT display
  tft.begin();
  tft.setRotation(2);          // Adjust the screen orientation
  tft.fillScreen(BLACK);       // Set the initial background color to black
  tft.setTextColor(WHITE);     // Set the text color to white
  tft.setTextSize(2);          // Set the text size to medium

  // Display "Hello World" message
  tft.setCursor(10, 30);       // Set cursor position (x=10, y=30)
  tft.println("Hello\nWorld"); // Print "Hello World" on the TFT screen
  delay(TIME * 1000);

  // Cycle through and display all available colors
  displayColor("Black", BLACK, WHITE);
  displayColor("Blue", BLUE, WHITE);
  displayColor("Red", RED, WHITE);
  displayColor("Green", GREEN, WHITE);
  displayColor("Cyan", CYAN, WHITE);
  displayColor("Magenta", MAGENTA, WHITE);
  displayColor("Yellow", YELLOW, WHITE);
  displayColor("White", WHITE, BLACK);
}

void loop() {
  // No operations in the loop for this example.
  // All actions are performed once during setup.
}

/**
 * @brief Displays a specified color on the TFT screen.
 * 
 * @param colorName Name of the color to display (e.g., "Red").
 * @param bgColor Background color to fill the screen.
 * @param textColor Text color to use for the label.
 */
void displayColor(const char* colorName, uint16_t bgColor, uint16_t textColor) {
  Serial.printf("%s.\n", colorName); // Print color name to Serial Monitor
  tft.fillScreen(bgColor);           // Fill the screen with the specified background color
  tft.setTextColor(textColor);       // Set the text color
  tft.setCursor(10, 30);             // Position the cursor for text display
  tft.println(colorName);            // Print the color name on the screen
  delay(TIME * 1000);                // Pause for the specified duration
}
