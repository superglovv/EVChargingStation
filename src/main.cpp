#include <Arduino.h>

#define LED1 10
#define LED2 9
#define LED3 8
#define LED4 7
#define RED_LED 4
#define GREEN_LED 6
#define BLUE_LED 5
#define INTERVAL 3000
#define BTNSTART 3
#define BTNSTOP 2
#define BLINK_INTERVAL 200

int lastIntervalTime = 0;
int battery = 0;
int charging = 0;
int buttonPressDuration = 0;
int lastBlinkTime = 0;
int blinkState = LOW;
int previousMillis = 0;

void setup() {
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  pinMode(BTNSTART, INPUT_PULLUP);
  pinMode(BTNSTOP, INPUT_PULLUP);

  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(BLUE_LED, LOW);
}

void blinkLED(int led) {
  int currentMillis = millis();
  
  if (currentMillis - lastBlinkTime >= BLINK_INTERVAL) {
    lastBlinkTime = currentMillis;
    blinkState = !blinkState;
    digitalWrite(led, blinkState);
  }
}

void resetStation() {
  charging = 0;
  battery = 0;
  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(BLUE_LED, LOW);

  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);
  digitalWrite(LED4, LOW);
}

void loop() {
  int start_button = digitalRead(BTNSTART);

  if (start_button == LOW && charging == 0) {
    delay(20);
    charging = 1;
    previousMillis = millis();
    digitalWrite(RED_LED, HIGH);
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(BLUE_LED, LOW);
  }

  int stop_button = digitalRead(BTNSTOP);

  if (stop_button == LOW) {
    if (buttonPressDuration == 0) {
      buttonPressDuration = millis();
    }
    if (millis() - buttonPressDuration >= 2000) {
      resetStation();
      return;
    }
  } else {
    buttonPressDuration = 0;
  }

  if (charging) {
    int currentMillis = millis();

    if (currentMillis - previousMillis >= INTERVAL) {
      previousMillis = currentMillis;
      battery++;

      if (battery > 4) {
        battery = 0;
        charging = 0;

        for (int i = 0; i < 3; i++) {
          digitalWrite(LED1, HIGH);
          digitalWrite(LED2, HIGH);
          digitalWrite(LED3, HIGH);
          digitalWrite(LED4, HIGH);
          delay(30);
          digitalWrite(LED1, LOW);
          digitalWrite(LED2, LOW);
          digitalWrite(LED3, LOW);
          digitalWrite(LED4, LOW);
          delay(30);
        }

        resetStation();
        return;
      }
    }

    switch (battery) {
      case 0:
        blinkLED(LED1);
        break;
      case 1:
        digitalWrite(LED1, HIGH);
        blinkLED(LED2);
        break;
      case 2:
        digitalWrite(LED2, HIGH);
        blinkLED(LED3);
        break;
      case 3:
        digitalWrite(LED3, HIGH);
        blinkLED(LED4);
        break;
      case 4:
        digitalWrite(LED4, HIGH);
        break;
      default:
        break;
    }
  }
}
