/*
  Control.h - Control library for power distribution Gateway to Science
  Joe Meyer created 10/17/2019 at the science museum of mn
*/

#include "Arduino.h"
#include "coal-burner.h"
#include "Adafruit_NeoPixel.h"

Coal_Burner::Coal_Burner(Adafruit_NeoPixel *_led_strip, int _first_pixel, long _cable_bit_mask, long _switch_bit_mask)
{
  first_pixel = _first_pixel;
  this->led_strip = _led_strip;
  switch_bit_mask = _switch_bit_mask;
  cable_bit_mask = _cable_bit_mask;
}

// Public Methods //////////////////////////////////////////////////////////////

void Coal_Burner::update(long _inputStates)
{

  if ((!(_inputStates & cable_bit_mask)) && !(_inputStates & switch_bit_mask)) // check if cable is connected and switch is on.
  {
    if (is_on == 0)
    {
      outputPercent = 28;
      is_on = 1;
    }
  }
  else
  {
    outputPercent = 0;
    is_on = 0;
    updatePixels();
  }

  if (is_on && (outputPercent != 100))
  {
    outputPercent = outputPercent + 3; // each update gets +4%, meaning it'll take twenty five intervals (6 hours simulation time) to get to 100%.
    outputPercent = constrain(outputPercent, 0, 100);
    if (outputPercent == 100)
    {
      light_color = production_color;
    }
    else
    {
      light_color = warming_color;
    }
    updatePixels();
  }
}

int Coal_Burner::getPowerProduced()
{
  if (outputPercent == 100)
    return 100;
  else
    return 0;
}

// Private Methods //////////////////////////////////////////////////////////////
void Coal_Burner::updatePixels()
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
