/*
  Control.h - Control library for power distribution Gateway to Science
  Joe Meyer created 10/17/2019 at the science museum of mn
*/

#include "Arduino.h"
#include "source.h"
#include "Adafruit_NeoPixel.h"

Source::Source(Adafruit_NeoPixel *_led_strip, int _first_pixel)
{
  first_pixel = _first_pixel;
  this->led_strip = _led_strip;
  powerOutput = 0;
  maxCapacity = 0;
}

// Public Methods //////////////////////////////////////////////////////////////
void Source::setNumCables(int numCables)
{
  maxCapacity = numCables * 100;
  lightGraph();
}

void Source::setPercentageActive(int percent)
{
  powerOutput = (maxCapacity / 100) * percent;
  lightGraph();
}

void Source::lightGraph()
{
  int C = map(maxCapacity, 0, 400, 0, 18);
  int P = map(powerOutput, 0, 400, 0, 18);
  for (int i = 0; i < 18; i++)
  {
    if (i < P)
    {
      led_strip->setPixelColor(first_pixel + i, led_strip->Color(0, 0, 40));
    }
    else if (i < C)
    {
      led_strip->setPixelColor(first_pixel + i, led_strip->Color(0, 5, 0));
    }
    else
    {
      led_strip->setPixelColor(first_pixel + i, led_strip->Color(0, 0, 0));
    }
  }
  led_strip->show();
}

int Source::getPowerProduced()
{
  return powerOutput;
}