/*
  coal-burner.h - Control panel library for Energy Distribution - Gateway to Science
  Joe Meyer created 9/16/2020 at the science museum of mn
*/
#include "Arduino.h"
#include "Adafruit_NeoPixel.h"

// ensure this library description is only included once
#ifndef Coal_Burner_h
#define Coal_Burner_h

// library interface description
class Coal_Burner
{
  // user-accessible "public" interface
public:
  Coal_Burner(Adafruit_NeoPixel *, int, long, long); //neopixel, first pixel, cable bit mask, switch bit mask
  int getPowerProduced();
  void updateInputs(long);
  void update(void);

  // library-accessible "private" interface
private:
  void updatePixels();
  Adafruit_NeoPixel *led_strip;
  long switch_bit_mask;
  long cable_bit_mask;
  int first_pixel;
  bool switch_state;
  int outputPercent;
  bool is_connected;
  uint32_t warming_color = 0x150300; //RRGGBB
  uint32_t production_color = 0x002000;
  uint32_t light_color;
};

#endif