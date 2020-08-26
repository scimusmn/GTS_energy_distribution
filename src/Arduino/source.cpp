/*
  Control.h - Control library for power distribution Gateway to Science
  Joe Meyer created 10/17/2019 at the science museum of mn
*/

#include "Arduino.h"
#include "source.h"
#include "Adafruit_NeoPixel.h"

Source::Source(Adafruit_NeoPixel* _led_strip, int _first_pixel)
{
  first_pixel = _first_pixel;
  this->led_strip = _led_strip;
  powerOutput = 0;
  maxCapacity = 0;
}

// Public Methods //////////////////////////////////////////////////////////////
void Source::setNumCables(int numCables){
    int P = map(numCables,0,4,0,18);
    led_strip->clear();
    for (int i=0; i<P; i++){
        led_strip->setPixelColor(first_pixel + i, 0, 40, 0);
    } 
    
    led_strip->show();
    maxCapacity = numCables * 100;  
}

void Source::setPercentageActive(int percent){
    powerOutput = (maxCapacity * percent)/100;

    int P = map(percent,0,100,0,18);
    led_strip->clear();
    for (int i=0; i<P; i++){
        led_strip->setPixelColor(first_pixel + i, 0, 0, 100);
    }    
    led_strip->show();

}


int Source::getPowerProduced(){
  return powerOutput;
}