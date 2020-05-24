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
   * @param clk The pin that the CLK output from the encoder is connected to.
   * @param dt The pin that the DT output from the encoder is connected to.
   * @param sw The pin that the SW output from the encoder is connected to.
   */
  Rotary(byte clk, byte dt, byte sw);

  /**
   * Setup the Arduino pins which are connected to the encoder.
   */
  void setupInputs();

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
   * Read the CLK and DT outputs and return the direction that the encoder has
   * been turned.
   *
   * @returns Either CLOCKWISE/ANTI_CLOCKWISE if the encoder has been turned
   *          and NONE if not.
   */
  Direction processRotation();

  /**
   * Read the SW output and return whether the encoder has been clicked.
   *
   * @return true or false.
   */
  bool processClick();

  /**
   * Whether the encoder is connected to a pin.
   *
   * @param pin The pin.
   * @return true or false.
   */
  bool isConnectedTo(byte pin) const;

  byte getClk() const { return clk; }
  byte getDt() const { return dt; }
  byte getSw() const { return sw; }

 private:
  enum State : byte;
  static const State NEXT_STATE[8][4];
  State state;
  unsigned long lastClick;
  static const byte bounceDelay{100};
  byte clk;
  byte dt;
  byte sw;
};
}  // namespace rotary

#endif