// Including the libraries.
#include "WiFi.h"
#include <HTTPClient.h>

// SSID and PASSWORD of your WiFi network.
const char* ssid = "";  //--> Your WiFi SSID
const char* password = ""; //--> Your WiFi password

// Google script Web_App_URL.
String Web_App_URL = "";

String temp = "";
String Person = "";        // Name of the person
String Authorization = ""; // Authorization status
uint8_t image;             // Image ID
int id;                    // Unique ID

// GPIO pin for the onboard LED (change if different)
#define On_Board_LED_PIN 13

//________________________________________________________________________________VOID SETUP()
void setup() {
  Serial.begin(115200);
  Serial.println("\nInitializing...");

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  int connecting_process_timed_out = 40;
  pinMode(On_Board_LED_PIN, OUTPUT);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    digitalWrite(On_Board_LED_PIN, HIGH);
    delay(250);
    digitalWrite(On_Board_LED_PIN, LOW);
    delay(250);

    if (--connecting_process_timed_out == 0) {
      Serial.println("\nConnection timeout, restarting...");
      delay(1000);
      ESP.restart();
    }
  }
  digitalWrite(On_Board_LED_PIN, LOW);
  Serial.println("\nWiFi connected!\nESP32 Is Ready!\n");
  Serial.println("Enter ID: ");
}

//________________________________________________________________________________VOID LOOP()
void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(On_Board_LED_PIN, HIGH);
    Serial.println("Enter ID: ");
    if (Serial.available() > 0) {
      while (Serial.available() == 0) {} 
      id = Serial.readStringUntil('\n').toInt();
      Serial.println("ID: " + String(id));

      Serial.println("Enter Image Number: ");
      while (Serial.available() == 0) {} 
      image = Serial.readStringUntil('\n').toInt();
      Serial.println("Image Number: " + String(image));

      Serial.println("Enter Name: ");
      while (Serial.available() == 0) {} 
      Person = Serial.readStringUntil('\n');
      Serial.println("Name: " + Person);

      Serial.println("Enter Authorization: ");
      while (Serial.available() == 0) {} 
      Authorization = Serial.readStringUntil('\n');
      Serial.println("Authorization: " + Authorization);

      String Send_Data_URL = Web_App_URL + "?sts=write";
      Send_Data_URL += "&id=" + String(id);
      Send_Data_URL += "&image=" + String(image);
      Send_Data_URL += "&person=" + Person;
      Send_Data_URL += "&authorization=" + Authorization;

      Serial.println("-------------");
      Serial.println("Send data to Google Spreadsheet...");
      Serial.print("URL : ");
      Serial.println(Send_Data_URL);

      HTTPClient http;
      http.begin(Send_Data_URL.c_str());
      http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

      int httpCode = http.GET(); 
      Serial.print("HTTP Status Code : ");
      Serial.println(httpCode);

      if (httpCode > 0) {
        String payload = http.getString();
        Serial.println("Payload : " + payload);    
      }
      http.end();

      digitalWrite(On_Board_LED_PIN, LOW);
      Serial.println("-------------");
    }

    delay(10000);

    // Reading data from Google Sheets
    digitalWrite(On_Board_LED_PIN, HIGH);

    String Read_Data_URL = Web_App_URL + "?sts=read";
    Serial.println("-------------");
    Serial.println("Read data from Google Spreadsheet...");
    Serial.print("URL : ");
    Serial.println(Read_Data_URL);

    HTTPClient http;
    http.begin(Read_Data_URL.c_str());
    int httpCode = http.GET();

    Serial.print("HTTP Status Code: ");
    Serial.println(httpCode);

    if (httpCode > 0) {
      String payload = http.getString();
      Serial.println("Response: " + payload);

      image = getValue(payload, ',', 0).toInt();
      id = getValue(payload, ',', 1).toInt();
      Person = getValue(payload, ',', 2);
      Authorization = getValue(payload, ',', 3);
      
      Serial.println("Parsed Data:");
      Serial.println("Image: " + String(image));
      Serial.println("ID: " + String(id));
      Serial.println("Person: " + Person);
      Serial.println("Authorization: " + Authorization);
    }
    http.end();

    digitalWrite(On_Board_LED_PIN, LOW);
    delay(10000);
  }
}

//________________________________________________________________________________getValue()
// Helper function to split a string based on a delimiter
String getValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;
  
  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
