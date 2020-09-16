/*
  hydro-generator.h - Control panel library for Energy Distribution - Gateway to Science
  Joe Meyer created 9/16/2020 at the science museum of mn
*/
#include "Arduino.h"
#include "Adafruit_NeoPixel.h"

// ensure this library description is only included once
#ifndef Hydro_Generator_h
#define Hydro_Generator_h

// library interface description
class Hydro_Generator
{
  // user-accessible "public" interface
public:
  Hydro_Generator(Adafruit_NeoPixel *, int, long, int); //neopixel, first pixel, analog pin
  int getPowerProduced();
  void update(long);

  // library-accessible "private" interface
private:
  void updatePixels();
  Adafruit_NeoPixel *led_strip;
  int first_pixel;
  long cable_bit_mask;
  int analog_pin;
  int outputPercent;
  bool is_connected;
  uint32_t light_color = 0x002000;
};

#endif