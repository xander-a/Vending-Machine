#include <Stepper.h>
#include <Keypad.h>
#include <LiquidCrystal.h>
#include <LiquidCrystal_I2C.h>

// Stepper motor configuration
const int stepsPerRevolution1 = 200;
const int stepsPerRevolution2 = 200;
Stepper myStepper1(stepsPerRevolution1, A0, A1, A2, A3);
Stepper myStepper2(stepsPerRevolution2, 2, 3, 4, 5 );

// Keypad configuration
const byte numRows = 4, numCols = 4;
char keymap[numRows][numCols] = {
  {'1', '2', '3', 'A'}, {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'}, {'*', '0', '#', 'D'}
};
byte rowPins[numRows] = {13,12,11,10};
byte colPins[numCols] = {9,8,7,6};
Keypad keypad = Keypad(makeKeymap(keymap), rowPins, colPins, numRows, numCols);

// LCD configuration
LiquidCrystal_I2C lcd(0x27, 16, 2);

bool isAuthorized = false;

void setup() {
  myStepper1.setSpeed(70);
  myStepper2.setSpeed(70);
  lcd.init(); // Correct initialization for LiquidCrystal_I2C
  lcd.backlight(); // Turn on the backlight
  Serial.begin(9600); // Initialize Serial communication with the slave

  lcd.print("Waiting for RFID");
  Serial.println("Master ready. Waiting for RFID authorization...");
}

void loop() {

  // Check for authorization status from slave
  if (Serial.available() > 0) {
    String message = Serial.readStringUntil('\n'); // Read data from slave
    message.trim(); // Remove any extra spaces or newlines

    if (message == "AUTHORIZED") {
      isAuthorized = true;
      lcd.clear();
      lcd.print("Access Granted!");
      delay(2000);
      lcd.clear();
      lcd.print("Use keypad...");
    } else if (message == "UNAUTHORIZED") {
      isAuthorized = false;
      lcd.clear();
      lcd.print("Access Denied!");
      delay(2000);
      lcd.clear();
      lcd.print("Waiting for RFID");
    }
  }

  // If authorized, allow keypad input
  if (isAuthorized) {
    char key = keypad.getKey();

    if (key) { // If a key is pressed
      lcd.clear();
      lcd.print("Key Pressed: ");
      lcd.print(key);

      if (key == 'A') {
        Serial.println("MOTOR_A");
        lcd.setCursor(0, 1);
        lcd.print("Motor 1 Rotating");
        unsigned long startTime = millis(); // Record the start time
  while (millis() - startTime < 10000) { // Run for 5 seconds
    myStepper1.step(1); // Rotate motor step by step
  }
        // myStepper1.step(stepsPerRevolution1);   // Rotate Motor 1 forward // Rotate Motor 1 backward
        isAuthorized = false; // Reset authorization after action
        lcd.clear();
        lcd.print("Re-scan RFID");
      } else if (key == 'B') {
        Serial.println("MOTOR_B");
        lcd.setCursor(0, 1);
        lcd.print("Motor 2 Rotating");
        // myStepper2.step(stepsPerRevolution2);   // Rotate Motor 2 forward  // Rotate Motor 2 backward
          unsigned long startTime = millis(); // Record the start time
  while (millis() - startTime < 10000) { // Run for 5 seconds
    myStepper2.step(1); // Rotate motor step by step
  }
        isAuthorized = false; // Reset authorization after action
        lcd.clear();
        lcd.print("Re-scan RFID");
      } else {
        Serial.println("MOTOR_C");
      }
    
      delay(2000); // Wait 2 seconds before resetting the LCD
      if (!isAuthorized) {
        lcd.clear();
        lcd.print("Waiting for RFID");
      }
    }
  }
}
