/*
   Google Sheets to ESP32/Huzzah32 Feather

   Reads from ESP32 and then stores in google sheets

   by Anthony Le
   modified 8 Nov 2024
   Credit: https://www.youtube.com/watch?v=u7TYu61l0t4&t=144s
*/
#include "WiFi.h"
#include <HTTPClient.h>

// WiFi credentials
const char* ssid = "";          // WiFi network name (SSID)
const char* password = "";    // WiFi password

// Google Apps Script Web App URL for data submission
String Web_App_URL = "https://script.google.com/macros/s/AKfycbxqODgmwtgdT65K4eHZ3o8GDwfrviQeMIPJLl4Rtm7evwjQbFujbmngrwrkdEOvbvwT/exec";

// Variables to store data
String Person = "Bob";           // Example person name
String Authorization = "Yes";    // Example authorization status
int image = 5;                   // Example image ID
int id = 2;                      // Example unique ID

#define On_Board_LED_PIN 13      // Onboard LED pin for visual indication

bool dataSent = false;           // Flag to track if data has been sent

// Function to gather data from user input through Serial Monitor
void Getting_Fingerdata() {
  // Prompt user to enter person's name
  Serial.println("Please enter the person's name:");
  while (Serial.available() == 0); // Wait for user input
  Person = Serial.readStringUntil('\n');
  Person.trim(); // Remove any newline characters

  // Prompt user to enter authorization status
  Serial.println("Please enter the authorization status (Yes/No):");
  while (Serial.available() == 0); // Wait for user input
  Authorization = Serial.readStringUntil('\n');
  Authorization.trim(); // Remove any newline characters

  // Prompt user to enter image ID
  Serial.println("Please enter the image ID (integer):");
  while (Serial.available() == 0); // Wait for user input
  image = Serial.parseInt(); // Read integer value

  // Prompt user to enter unique ID
  Serial.println("Please enter the unique ID (integer):");
  while (Serial.available() == 0); // Wait for user input
  id = Serial.parseInt(); // Read integer value

  // Output gathered data to Serial Monitor
  Serial.println();
  Serial.println("-------------");
  Serial.print("Name : ");
  Serial.print(Person);
  Serial.print(" | Authorization : ");
  Serial.print(Authorization);
  Serial.print(" | Image : ");
  Serial.print(image);
  Serial.print(" | ID : ");
  Serial.print(id);
  Serial.println("-------------");
}

void setup() {
  // Initialize Serial communication
  Serial.begin(115200);
  delay(1000);

  // Configure the onboard LED as an output
  pinMode(On_Board_LED_PIN, OUTPUT);

  // Connect to Wi-Fi
  WiFi.mode(WIFI_STA); // Set Wi-Fi to station mode
  WiFi.begin(ssid, password); // Start connection
  Serial.print("Connecting to WiFi");

  int timeout = 40; // 20 seconds timeout (40 * 250 ms)
  while (WiFi.status() != WL_CONNECTED && timeout > 0) {
    delay(250);
    Serial.print(".");
    digitalWrite(On_Board_LED_PIN, HIGH); // Blink LED while connecting
    delay(250);
    digitalWrite(On_Board_LED_PIN, LOW);
    timeout--;
  }

  // Check Wi-Fi connection status
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected");
  } else {
    Serial.println("\nWiFi connection failed, restarting...");
    ESP.restart(); // Restart ESP if connection fails
  }
}

void loop() {
  // Check if data has already been sent
  if (!dataSent) {
    Getting_Fingerdata(); // Gather data from user input

    // Send data if Wi-Fi is connected
    if (WiFi.status() == WL_CONNECTED) {
      digitalWrite(On_Board_LED_PIN, HIGH); // Turn on LED to indicate data sending

      // Form the URL for sending data
      String Send_Data_URL = Web_App_URL + "?sts=write";
      Send_Data_URL += "&id=" + String(id);
      Send_Data_URL += "&image=" + String(image);
      Send_Data_URL += "&person=" + Person;
      Send_Data_URL += "&authorization=" + Authorization;

      // Output the URL to the Serial Monitor for debugging
      Serial.println("\n-------------");
      Serial.println("Sending data to Google Spreadsheet...");
      Serial.print("URL: ");
      Serial.println(Send_Data_URL);

      // Send the HTTP GET request
      HTTPClient http;
      http.begin(Send_Data_URL.c_str());
      int httpCode = http.GET();

      // Handle the response
      if (httpCode > 0) {
        Serial.print("HTTP Status Code: ");
        Serial.println(httpCode);
        String payload = http.getString(); // Read server response
        Serial.println("Payload: " + payload);

        // Set the flag to true after successful data transmission
        dataSent = true;
      } else {
        Serial.print("HTTP request failed: ");
        Serial.println(http.errorToString(httpCode).c_str());
      }

      http.end(); // Close connection
      digitalWrite(On_Board_LED_PIN, LOW); // Turn off LED
      Serial.println("-------------");
    } else {
      Serial.println("WiFi not connected, retrying...");
    }
  }

  // Stop execution after sending data
  if (dataSent) {
    Serial.println("Data sent. Stopping further execution.");
    while (true) {
      delay(1000); // Infinite loop to stop further execution
    }
  }
}
