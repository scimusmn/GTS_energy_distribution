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
void Coal_Burner::updateInputs(long _inputStates)
{
  if (!(_inputStates & cable_bit_mask)) // check if cable is connected.
  {
    //cable is plugged in.
    is_connected = true;
    Serial.println("cable plugged");
  }
  else
  {
    //cable is unplugged.
    is_connected = false;
    outputPercent = 0;
    updatePixels();
    Serial.println("cable UNplugged");
  }

  if (is_connected)
  {
    if (_inputStates & switch_bit_mask) // if it's connected, check the switch.
    {
      //switch is off
      switch_state = 0;
      outputPercent = 0;
      updatePixels();

      Serial.println("switch OFF");
    }
    else //switch is on
    {
      switch_state = 1;
      Serial.println("switch on");
    }
  }
}

void Coal_Burner::update()
{
  if (is_connected)
  {
    if (switch_state && (outputPercent != 100))
    {
      Serial.print("+");
      outputPercent = outputPercent + 4;
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
