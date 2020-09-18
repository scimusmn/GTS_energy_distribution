/*
  Control.h - Control library for power distribution Gateway to Science
  Joe Meyer created 10/17/2019 at the science museum of mn
*/

#include "Arduino.h"
#include "renewable.h"
#include "Adafruit_NeoPixel.h"

Renewable::Renewable(Adafruit_NeoPixel *_led_strip, int _first_pixel, int _cable_bit_num, int _analog_pin, uint32_t _color)
{
  first_pixel = _first_pixel;
  this->led_strip = _led_strip;
  analog_pin = _analog_pin;
  if (analog_pin > 0)
    pinMode(analog_pin, INPUT);
  cable_bit_mask = numToMask(_cable_bit_num);
  outputPercent = 0;
  light_color = _color;
}

// Public Methods //////////////////////////////////////////////////////////////

void Renewable::update(long _inputStates)
{
  if (!(_inputStates & cable_bit_mask)) // check if cable is connected.
  {
    //cable is plugged in.
    if (analog_pin > 0)
    {
      int reading = analogRead(analog_pin);
      outputPercent = map(reading, 40, 380, 0, 100);
    }
    else
    {
      outputPercent = percent_available;
    }

    outputPercent = constrain(outputPercent, 0, 100);
    updatePixels();
  }
  else
  {
    //cable is unplugged.
    outputPercent = 0;
    updatePixels();
  }
}

int Renewable::getPowerProduced()
{
  return outputPercent;
}

void Renewable::setPercentage(int percent)
{
  percent = constrain(percent, 0, 100);
  percent_available = percent;
}

// Private Methods //////////////////////////////////////////////////////////////
void Renewable::updatePixels()
{
  int level = map(outputPercent, 0, 100, 0, 8);
  for (int i = 0; i < 8; i++)
  {
    if (i < level)
    {
      led_strip->setPixelColor(first_pixel + i, light_color); // turn on pixel
    }
    else
      led_strip->setPixelColor(first_pixel + i, 0x0); // turn off pixel
  }
}

long Renewable::numToMask(int num)
{
  long mask = 1;
  for (int i = 0; i < num; i++)
  {
    mask = mask << 1;
  }
  return mask;
}
