#include "Arduino.h"
#include "rotary.h"

using rotary::Rotary;

#define CLK 2
#define DT 3

Rotary rotary1 = Rotary(CLK, DT);

void isr();

void setup() {
  Serial.begin(9600);
  Serial.println("setup begin");

  attachInterrupt(digitalPinToInterrupt(CLK), isr, CHANGE);
  attachInterrupt(digitalPinToInterrupt(DT), isr, CHANGE);

  Serial.println("setup finish");
}

void loop() {}

void isr() {
  Rotary::Direction direction = rotary1.processInputs();
  switch (direction) {
    case Rotary::Direction::CLOCKWISE:
      Serial.println("u");
      break;
    case Rotary::Direction::ANTI_CLOCKWISE:
      Serial.println("d");
      break;
  }
}
