#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 5

#define TRIG_PIN A1   // Ultrasonic sensor TRIG pin connected to Arduino pin 7
#define ECHO_PIN A0   // Ultrasonic sensor ECHO pin connected to Arduino pin 6
#define RED_LED A2    // Red LED connected to pin 2

long duration;
float distance;

const int buzzerPin = 2;  //

MFRC522 rfid(SS_PIN, RST_PIN);

// Define authorized UID
byte authorizedUID[] = { 0xF3, 0x01, 0xD6, 0xD9 };
const int authorizedUIDLength = sizeof(authorizedUID) / sizeof(authorizedUID[0]);

void setup() {
  Serial.begin(9600);  // Initialize Serial communication
  SPI.begin();         // Initialize SPI bus
  rfid.PCD_Init();     // Initialize RFID module
  pinMode(buzzerPin, OUTPUT);
 pinMode(RED_LED, OUTPUT);  // Setup RED_LED pin
  pinMode(TRIG_PIN, OUTPUT); // Setup ultrasonic sensor TRIG pin
  pinMode(ECHO_PIN, INPUT);  // Setup ultrasonic sensor ECHO pin
  Serial.println("Slave ready. Waiting for RFID scan...");
}

void loop() {

   digitalWrite(TRIG_PIN, LOW);  // Send a pulse to trigger the ultrasonic sensor
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH);  // Read the echo pulse
  distance = (duration * 0.034) / 2;  // Convert duration to distance (cm)

  // If object is within 7.62 cm, turn on RED LED
  if (distance <= 7.62) {
    digitalWrite(RED_LED, HIGH);  // Turn on RED LED if object detected
  } else {
    digitalWrite(RED_LED, LOW);   // Turn off RED LED if no object detected
  }
  
  if (rfid.PICC_IsNewCardPresent()) {  // Check if a new tag is present
    if (rfid.PICC_ReadCardSerial()) {  // Read the tag's UID
      bool authorized = checkAuthorization(rfid.uid.uidByte, rfid.uid.size);

      if (authorized) {
        Serial.println("AUTHORIZED");  // Send status to master
      } else {
        Serial.println("UNAUTHORIZED");  // Send status to master
      }

      rfid.PICC_HaltA();       // Halt the RFID card
      rfid.PCD_StopCrypto1();  // Stop encryption
    }
  }

  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command == "MOTOR_A") {
      tone(buzzerPin, 500, 500);  // Play buzzer sound for 'A'
    } else if (command == "MOTOR_B") {
      tone(buzzerPin, 500, 500);  // Play buzzer sound for 'B'
    } else if (command == "MOTOR_C") {
      for (int i = 0; i < 3; i++) {
        tone(buzzerPin, 250, 300);  // 250Hz tone for 300ms             // Wait for 350ms before repeating (to separate each tone)
        delay(400);
      }

      // Optionally, stop the tone after it finishes
      noTone(buzzerPin);
    }
  }
}


// Function to check if the UID matches the authorized UID
bool checkAuthorization(byte* uid, byte length) {
  if (length != authorizedUIDLength) {
    return false;  // Length mismatch
  }
  for (int i = 0; i < length; i++) {
    if (uid[i] != authorizedUID[i]) {
      return false;  // UID mismatch
    }
  }
  return true;  // UID matches
}
