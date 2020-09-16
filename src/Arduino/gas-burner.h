/*
  gas-burner.h - Control panel library for Energy Distribution - Gateway to Science
  Joe Meyer created 9/16/2020 at the science museum of mn
*/
#include "Arduino.h"
#include "Adafruit_NeoPixel.h"

// ensure this library description is only included once
#ifndef Gas_Burner_h
#define Gas_Burner_h

// library interface description
class Gas_Burner
{
  // user-accessible "public" interface
public:
  Gas_Burner(Adafruit_NeoPixel *, int, long, long, long); //neopixel, first pixel, cable bit mask, up button bit mask, down button bit mask
  int getPowerProduced();
  void update(long);

  // library-accessible "private" interface
private:
  void updatePixels();
  Adafruit_NeoPixel *led_strip;
  long up_btn_bit_mask;
  long down_btn_bit_mask;
  long cable_bit_mask;
  int first_pixel;
  int outputPercent;
  bool is_connected;
  uint32_t light_color = 0x002000;
};

#endif