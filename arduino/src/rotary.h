#ifndef ROTARY_H
#define ROTARY_H

#include "Arduino.h"

namespace rotary {
/*
 * Represents a digital encoder.
 */
class Rotary {
 public:
  /**
   * Instantiate the Rotary object.
   *
   * @param clk The pin that CLK output from the encoder is connected to.
   * @param dt The pin that DT output from the encoder is connected to.
   */
  Rotary(byte clk, byte dt);

  /**
   * Used to indicate that the encoder has been turned either clockwise,
   * anticlockwise, or in no direction.
   */
  enum Direction : byte {
    NONE = 0,
    CLOCKWISE = 0b1000,
    ANTI_CLOCKWISE = 0b10000,
  };

  /**
   * Read the input pins and return the direction that the encoder has been
   * turned.
   *
   * @returns Either CLOCKWISE/ANTI_CLOCKWISE if the encoder has been turned
   *          and NONE if not.
   */
  Direction processPinState();

  /**
   * Whether the encoder is connected to a pin.
   *
   * @param pin The pin.
   * @return true or false.
   */
  bool isConnectedTo(byte pin) const;

  byte getClk() const { return clk; }

  byte getDt() const { return dt; }

 private:
  enum State : byte;
  static const State NEXT_STATE[8][4];
  State state;
  byte clk;
  byte dt;
};
}  // namespace rotary

#endif