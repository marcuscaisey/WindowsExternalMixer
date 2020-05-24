#include "Arduino.h"
#define EI_ARDUINO_INTERRUPTED_PIN
#include "EnableInterrupt.h"
#include "rotary.h"

using rotary::Rotary;

void rotatedIsr();
void clickedIsr();

Rotary rotaries[]{Rotary(2, 3, 4), Rotary(5, 6, 7)};

void setup() {
  Serial.begin(9600);

  for (auto &rotary : rotaries) {
    rotary.setupInputs();
    enableInterrupt(rotary.getClk(), rotatedIsr, CHANGE);
    enableInterrupt(rotary.getDt(), rotatedIsr, CHANGE);
    enableInterrupt(rotary.getSw(), clickedIsr, FALLING);
  }
}

void loop() {}

void rotatedIsr() {
  byte i{0};

  for (auto &rotary : rotaries) {
    if (rotary.isConnectedTo(arduinoInterruptedPin)) {
      Rotary::Direction direction{rotary.processRotation()};

      switch (direction) {
        case Rotary::CLOCKWISE:
          Serial.println(String(i) + "cw");
          break;
        case Rotary::ANTI_CLOCKWISE:
          Serial.println(String(i) + "acw");
          break;
      }

      break;
    }
    i++;
  }
}

void clickedIsr() {
  byte i{0};

  for (auto &rotary : rotaries) {
    if (rotary.isConnectedTo(arduinoInterruptedPin)) {
      bool clicked{rotary.processClick()};
      if (clicked) {
        Serial.println(String(i) + "s");
      }

      break;
    }
    i++;
  }
}
