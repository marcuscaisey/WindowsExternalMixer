#ifndef ROTARY_H
#define ROTARY_H

namespace rotary {
class Rotary {
 public:
  /**
   * Instantiate the Rotary object and set up the input pins.
   *
   * @param clk The pin that CLK output from the encoder is plugged into.
   * @param dt The pin that DT output from the encoder is plugged into.
   */
  Rotary(unsigned char clk, unsigned char dt);

  /**
   * Used to indicate that the encoder has been turned either clockwise,
   * anticlockwise, or in no direction.
   */
  enum Direction : unsigned char {
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
  Direction processInputs();

 private:
  enum State : unsigned char;
  static const State NEXT_STATE[8][4];
  State state;
  unsigned char clk;
  unsigned char dt;
};
}

#endif