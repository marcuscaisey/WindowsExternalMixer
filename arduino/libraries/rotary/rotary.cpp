// This is based on the code that accompanied Ben Buxton's blog post on handling
//  rotary encoders. Blog post:
//  http://www.buxtronix.net/2011/10/rotary-encoders-done-properly.html.
//  Code: https://github.com/buxtronix/arduino/tree/master/libraries/Rotary.
//
//  I've rewritten the Rotary class in a more OOP style and "fixed" the state
//  table as it registered turns of the encoder incorrectly (see
//  https://github.com/buxtronix/arduino/issues/3).

#include "rotary.h"

#include "Arduino.h"

namespace rotary {

// A clockwise turn of the encoder should be registered when the state of its
// pins go through the following* binary sequence: 00 -> 10 -> 11 -> 01 -> 00,
// where the value of the 2 bit is the signal from CLK output and the value of
// the 1 bit is the signal from the DT output. The only exception is that the
// state may bounce between two consecutive elements of the sequence, which
// accounts for signal bounce. For example, if the state is currently 10 and the
// signal from the DT output changes to 1, it may bounce between 1 and 0 before
// stabilising at 1, making the state bounce between 10 and 11.
//
// This behaviour is described by the finite state machine in statemachine.png,
// where the arrival at either of the states CW_FINISH or ACW_FINISH indicates
// that the encoder has been turned clockwise and anticlockwise respectively.
//
// *An anti-clockwise turn follows the sequence in reverse.
enum Rotary::State : unsigned char {
  START = 0b0,
  CW1 = 0b1,
  CW2 = 0b10,
  CW3 = 0b11,
  ACW1 = 0b100,
  ACW2 = 0b101,
  ACW3 = 0b110,
  FAULT = 0b111,
  CW_FINISH = 0b1000,
  ACW_FINISH = 0b10000,
};

// This table encodes the finite state machine representing the encoders
// behaviour. Each row corresponds to a state and the nth element of each row
// is the next state of the encoder where the binary representation of n is the
// current pin state. I.e. if the current state is CW1 and the current pin state
// is 11, then the next state is given by NEXT_STATE[1][3] = CW1.
const Rotary::State Rotary::NEXT_STATE[8][4] = {
    // START
    {START, ACW1, CW1, START},
    // CW1
    {START, FAULT, CW1, CW2},
    // CW2
    {START, CW3, CW1, CW2},
    // CW3
    {CW_FINISH, CW3, FAULT, CW2},
    // ACW1
    {START, ACW1, FAULT, ACW2},
    // ACW2
    {START, ACW1, ACW3, ACW2},
    // ACW3
    {ACW_FINISH, FAULT, ACW3, ACW2},
    // FAULT
    {START, FAULT, FAULT, FAULT},
};

Rotary::Rotary(unsigned char clk, unsigned char dt)
    : clk(clk), dt(dt), state(State::START) {
  pinMode(clk, INPUT);
  pinMode(dt, INPUT);
}

Rotary::Direction Rotary::processInputs() {
  unsigned char pinState = (digitalRead(clk) << 1) | digitalRead(dt);

  // States are unchanged when ANDed with 0b111 apart from CW_FINISH and
  // ACW_FINISH which go to 0.
  state = NEXT_STATE[state & 0b111][pinState];

  // Returns CW_FINISH and ACW_FINISH for each respectively, and 0 for all other
  // states.
  return (Direction)(state & 0b11000);
}
}  // namespace rotary