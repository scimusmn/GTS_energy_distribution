/*
  control.h - Control library for power distribution Gateway to Science
  Joe Meyer created 10/17/2019 at the science museum of mn
*/
#include "Arduino.h"
#include "Adafruit_NeoPixel.h"

// ensure this library description is only included once
#ifndef Source_h
#define Source_h

// library interface description
class Source
{
  // user-accessible "public" interface
public:
  Source(Adafruit_NeoPixel *, int); //neopixel, first pixel
  int getPowerProduced();
  void setPercentageActive(int);
  void setNumCables(int);

  // library-accessible "private" interface
private:
  void lightGraph();
  Adafruit_NeoPixel *led_strip;
  int first_pixel;
  int powerOutput;
  int maxCapacity;
};

#endif