/*  
Unlock Door Function for ESP32 Wroom  
--------------------------------------
This function is responsible for unlocking the door for a defined amount of time 
(2 seconds in this case), and then automatically locking it again after that duration. 
It provides feedback via the serial monitor.

Hardware: ESP32 Wroom (Feather or Dev Kit)
Relay Pin: GPIO 32 (or another GPIO pin of your choice)

Author: Anthony Le <anthle@pdx.edu>  
Date: Nov 19th, 2024  
Version: 0.5  
*/

#define UNLOCK_TIME 2 // Unlock duration in seconds (set to 2 seconds)
#define RELAY_PIN 32  // GPIO pin connected to the relay (adjust as per your wiring)

bool doorUnlocked = false;  // Flag to ensure the door unlocks only once

void setup() {
  // Start serial communication for feedback
  Serial.begin(115200);   // Ensure the baud rate matches your serial monitor
  pinMode(RELAY_PIN, OUTPUT);  // Set the relay pin as output
  digitalWrite(RELAY_PIN, LOW); // Ensure the door is locked initially
}

void loop() {
  // Check if the door has been unlocked
  if (!doorUnlocked) {
    unlockDoor();  // Unlock the door
    doorUnlocked = true;  // Set the flag to true so the door doesn't unlock again
  }

  // Optional: Wait some time before ending the loop or doing other tasks
  delay(5000);  // Wait 5 seconds before the loop ends or other actions happen
}

/*  
Unlock Door Function  
---------------------
This function unlocks the door for a defined time (2 seconds) 
and then locks it back.

1. Unlocks the door by activating the relay.
2. Waits for the specified unlock duration.
3. Locks the door by deactivating the relay.
*/

void unlockDoor() {
  // Start of the unlocking process
  Serial.println("Unlocking door..."); // Print to serial monitor that the door is unlocking
  digitalWrite(RELAY_PIN, HIGH); // Activate relay to unlock the door

  // Notify that the door is now unlocked
  Serial.println("Door unlocked."); // Print door status to serial monitor

  // Wait for the specified unlock duration (2 seconds)
  delay(UNLOCK_TIME * 1000); // Delay in milliseconds (UNLOCK_TIME * 1000 to convert to ms)

  // Start of the locking process after the unlock time has elapsed
  Serial.println("Locking door..."); // Print to serial monitor that the door is locking
  digitalWrite(RELAY_PIN, LOW); // Deactivate relay to lock the door

  // Notify that the door is now locked
  Serial.println("Door locked."); // Print door status to serial monitor
  delay(2000);                   // Wait for 2 seconds before the function ends (additional time to confirm lock)
}
