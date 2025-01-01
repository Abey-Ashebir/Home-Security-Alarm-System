#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const int calibrationTime = 10;
const int buttonPin = 2;
const int pirPin = 3;
const int buzPin = 4;
const int ledPin = 13;

int buttonState = LOW;
int lastButtonState = LOW;
int ledState = LOW;
boolean lockLow = true;

unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;
unsigned long previousMillis = 0;
const unsigned long interval = 1000;

LiquidCrystal_I2C lcd(0x20, 16, 2);

void setup() {
  pinMode(pirPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(buzPin, OUTPUT);
  pinMode(buttonPin, INPUT);
  digitalWrite(pirPin, LOW);

  lcd.init();
  lcd.backlight();
  showStartupMessage();
  calibrateSensor();
  showStatusMessage("System Activated", "");
}

void loop() {
  if (digitalRead(pirPin) == HIGH) {
    handleMotionDetected();
  } else if (digitalRead(pirPin) == LOW && lockLow == false) {
    // Motion has stopped, reset LED and buzzer
    lockLow = true; // Prevent re-triggering
    digitalWrite(ledPin, LOW);
    digitalWrite(buzPin, LOW);

    // Display "Intruder is Gone"
    showStatusMessage("Intruder is Gone", "");
    delay(2000); // Display message for 2 seconds
    showStatusMessage("System Activated", "");
  }

  if (digitalRead(buttonPin) == HIGH && digitalRead(pirPin) == LOW) {
    resetAlarm();
  }
}


void showStartupMessage() {
  lcd.setCursor(0, 0); // Start at column 0
  lcd.print("Home Security");
  lcd.setCursor(2, 1); // Adjust column to fit text
  lcd.print("Alarm System");
  delay(1500);
  lcd.clear();
  lcd.setCursor(5, 0);
  lcd.print("LOADING");
}


void calibrateSensor() {
  for (int i = 0; i < calibrationTime; i++) {
    lcd.setCursor(i + 3, 1);
    lcd.print("*");
    delay(100);
  }
  lcd.clear();
}

void showStatusMessage(String line1, String line2) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1);
  if (line2 != "") {
    lcd.setCursor(0, 1);
    lcd.print(line2);
  }
}

void handleMotionDetected() {
  showStatusMessage("Motion Detected", "Alarm Triggered");

  while (lockLow) {
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      ledState = !ledState;
      digitalWrite(ledPin, ledState);
      digitalWrite(buzPin, ledState);
    }

    int reading = digitalRead(buttonPin);
    if (reading != lastButtonState) {
      lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > debounceDelay && reading != buttonState) {
      buttonState = reading;
      if (buttonState == HIGH) {
        lockLow = false;
        digitalWrite(ledPin, LOW);
        digitalWrite(buzPin, LOW);
        delay(50);
      }
    }
    lastButtonState = reading;
  }
}

void resetAlarm() {
  showStatusMessage("Alarm Reset", "");
  delay(1000);
  showStatusMessage("System Activated", "");
  lockLow = true;
}
