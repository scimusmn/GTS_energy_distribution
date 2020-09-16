/*
  Control.h - Control library for power distribution Gateway to Science
  Joe Meyer created 10/17/2019 at the science museum of mn
*/

#include "Arduino.h"
#include "gas-burner.h"
#include "Adafruit_NeoPixel.h"

Gas_Burner::Gas_Burner(Adafruit_NeoPixel *_led_strip, int _first_pixel, long _cable_bit_mask, long _up_btn_bit_mask, long _down_btn_bit_mask)
{
  first_pixel = _first_pixel;
  this->led_strip = _led_strip;
  up_btn_bit_mask = _up_btn_bit_mask;
  down_btn_bit_mask = _down_btn_bit_mask;
  cable_bit_mask = _cable_bit_mask;
}

// Public Methods //////////////////////////////////////////////////////////////

void Gas_Burner::update(long _inputStates)
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
    if (_inputStates & up_btn_bit_mask) // if it's connected, check the btn.
    {
      //switch is off
      Serial.println("up btn OFF");
    }
    else //switch is on
    {
      Serial.println("up btn on");
      outputPercent = outputPercent + 10;
      outputPercent = constrain(outputPercent, 0, 100);
    }

    if (_inputStates & down_btn_bit_mask) // if it's connected, check the btn.
    {
      //switch is off
      Serial.println("down btn OFF");
    }
    else //switch is on
    {
      Serial.println("down btn on");
      outputPercent = outputPercent - 10;
      outputPercent = constrain(outputPercent, 0, 100);
    }

    updatePixels();
  }
}

int Gas_Burner::getPowerProduced()
{
  return outputPercent;
}

// Private Methods //////////////////////////////////////////////////////////////
void Gas_Burner::updatePixels()
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
