/*
  Control.h - Control library for power distribution Gateway to Science
  Joe Meyer created 10/17/2019 at the science museum of mn
*/

#include "Arduino.h"
#include "hydro-generator.h"
#include "Adafruit_NeoPixel.h"

Hydro_Generator::Hydro_Generator(Adafruit_NeoPixel *_led_strip, int _first_pixel, long _cable_bit_mask, int _analog_pin)
{
  first_pixel = _first_pixel;
  this->led_strip = _led_strip;
  analog_pin = _analog_pin;
  pinMode(analog_pin, INPUT);
  cable_bit_mask = _cable_bit_mask;
}

// Public Methods //////////////////////////////////////////////////////////////

void Hydro_Generator::update(long _inputStates)
{
  if (!(_inputStates & cable_bit_mask)) // check if cable is connected.
  {
    //cable is plugged in.
    is_connected = true;
  }
  else
  {
    //cable is unplugged.
    is_connected = false;
    outputPercent = 0;
    updatePixels();
  }

  if (is_connected)
  {
    int reading = analogRead(analog_pin);
    outputPercent = map(reading, 40, 380, 0, 100);
    outputPercent = constrain(outputPercent, 0, 100);
    updatePixels();
  }
}

int Hydro_Generator::getPowerProduced()
{
  return outputPercent;
}

// Private Methods //////////////////////////////////////////////////////////////
void Hydro_Generator::updatePixels()
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
