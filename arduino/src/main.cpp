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

/**
 * Describes an action performed on an encoder.
 */
enum Action : byte {
  CLOCKWISE_TURN,
  ANTI_CLOCKWISE_TURN,
  CLICK,
};

/**
 * Return the action message to be sent over the serial port when an action is
 * performed on an encoder.
 * @param encoderId The ID of the encoder.
 * @param action The action performed.
 * @return A hex byte where the 16 digit is the encoder ID and the 1 digit is
 * the action performed.
 */
byte actionMessage(byte encoderId, Action action) {
  return 0x10 * encoderId + action;
}

void rotatedIsr() {
  byte i{0};

  for (auto &rotary : rotaries) {
    if (rotary.isConnectedTo(arduinoInterruptedPin)) {
      Rotary::Direction direction{rotary.processRotation()};

      switch (direction) {
        case Rotary::CLOCKWISE:
          Serial.print(actionMessage(i, CLOCKWISE_TURN));
          break;
        case Rotary::ANTI_CLOCKWISE:
          Serial.print(actionMessage(i, ANTI_CLOCKWISE_TURN));
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
        Serial.print(actionMessage(i, CLICK));
      }

      break;
    }
    i++;
  }
}
