#include "Arduino.h"
#define EI_ARDUINO_INTERRUPTED_PIN
#include "EnableInterrupt.h"
#include "rotary.h"

using rotary::Rotary;

void rotatedIsr();

Rotary rotaries[]{Rotary(2, 3), Rotary(5, 6)};

void setup() {
  for (auto &rotary : rotaries) {
    enableInterrupt(rotary.getClk(), rotatedIsr, CHANGE);
    enableInterrupt(rotary.getDt(), rotatedIsr, CHANGE);
  }

  Serial.begin(9600);
}

void loop() {}

void rotatedIsr() {
  byte i{0};

  for (auto &rotary : rotaries) {
    if (rotary.isConnectedTo(arduinoInterruptedPin)) {
      Rotary::Direction direction{rotary.processPinState()};

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
