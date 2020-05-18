#include <PinChangeInt.h>
#include <rotary.h>

using rotary::Rotary;

#define CLK1 2
#define DT1 3
#define SW1 6
#define CLK2 4
#define DT2 5
#define SW2 7

Rotary rotary1 = Rotary(1, 2, 3);
Rotary rotary2 = Rotary(2, 4, 5);

void setup() {
  Serial.begin(9600);
  Serial.println("setup begin");
  
  attachPinChangeInterrupt(CLK1, isr1, CHANGE);
  attachPinChangeInterrupt(DT1, isr1, CHANGE);
  pinMode(SW1, INPUT_PULLUP);
  attachPinChangeInterrupt(SW1, isr1sw, RISING);
  
  attachPinChangeInterrupt(CLK2, isr2, CHANGE);
  attachPinChangeInterrupt(DT2, isr2, CHANGE);
  attachPinChangeInterrupt(SW2, isr2sw, RISING);
  pinMode(SW2, INPUT_PULLUP);
  
  Serial.println("setup finish");
}

void loop() {
  
}

void isr1() {
  Rotary::Direction direction = rotary1.processInputs();
  switch (direction) {
    case Rotary::Direction::CLOCKWISE:
      Serial.print(rotary1.getId());
      Serial.println("u");
      break;
    case Rotary::Direction::ANTI_CLOCKWISE:
      Serial.print(rotary1.getId());
      Serial.println("d");
      break;
  }
}

void isr1sw() {
  static unsigned long lastPress = 0;
  if (digitalRead(SW1) && (millis() - lastPress) > 50) {
    Serial.print(rotary1.getId());
    Serial.println("s");
    lastPress = millis();
  }
}

void isr2() {
  Rotary::Direction direction = rotary2.processInputs();
  switch (direction) {
    case Rotary::Direction::CLOCKWISE:
      Serial.print(rotary2.getId());
      Serial.println("u");
      break;
    case Rotary::Direction::ANTI_CLOCKWISE:
      Serial.print(rotary2.getId());
      Serial.println("d");
      break;
  }
}

void isr2sw() {
  static unsigned long lastPress = 0;
  if (digitalRead(SW2) && (millis() - lastPress) > 50) {
    Serial.print(rotary2.getId());
    Serial.println("s");
    lastPress = millis();
  }
}
