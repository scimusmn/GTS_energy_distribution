/*
  Renewable.h - Control panel library for Energy Distribution - Gateway to Science
  Joe Meyer created 9/16/2020 at the science museum of mn
*/
#include "Arduino.h"
#include "Adafruit_NeoPixel.h"

// ensure this library description is only included once
#ifndef Renewable_h
#define Renewable_h

// library interface description
class Renewable
{
  // user-accessible "public" interface
public:
  Renewable(Adafruit_NeoPixel *, int, long, int); //neopixel, first pixel, analog pin
  void setPercentage(int);
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