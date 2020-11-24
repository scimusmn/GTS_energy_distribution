//**************************************************************//
//  Component: Energy Distribution                              //
//  Project: Gateway to Science                                 //
//  Author  : Joe Meyer                                         //
//  Date    : 8/21/2020                                         //
//  Version : 1.0                                               //
//  Description : Control Panel v2                              //
//****************************************************************
#include <Adafruit_NeoPixel.h>
#include "source.h"
#include "arduino-base/Libraries/SerialController.hpp"

const int neopixel_pin = 6;
const int shift_in_latch_pin = 4;
const int shift_in_data_pin = 3;
const int shift_in_clock_pin = 2;
const int hydro_1_input_pin = A1;

SerialController serialController;
long baudrate = 115200;

// Declare NeoPixel strip object for bar graphs:
Adafruit_NeoPixel pixels(95, neopixel_pin, NEO_GRB + NEO_KHZ800);

Source Coal(&pixels, 0);
Source Gas(&pixels, 18);
Source Hydro(&pixels, 37);
Source Solar(&pixels, 56);
Source Wind(&pixels, 75);

long cableStates = 0; // GGGGCCCCHHSSSWWW   Gas Coal Hydro Solar Wind
long prevCableStates = 2;
int simulationMinutes = 0;
int toggleStates[4] = {0, 0, 0, 0};
bool ledState[4];

unsigned long currentMillis, prevSendMillis = 0;

void setup()
{
  // Ensure Serial Port is open and ready to communicate
  serialController.setup(baudrate, &onParse);

  //define pin modes
  pinMode(shift_in_latch_pin, OUTPUT);
  pinMode(shift_in_clock_pin, OUTPUT);
  pinMode(shift_in_data_pin, INPUT);
  pinMode(hydro_1_input_pin, INPUT);

  pixels.begin();
  pixels.clear();
}

void loop()
{
  currentMillis = millis();
  // serialController.update();

  updateCableStates();

  if ((currentMillis - prevSendMillis) > 250)
  {
    updateHydro(); // read controls and update
  }

  Coal.setPercentageActive(25); // todo read switches!
  Gas.setPercentageActive(100); // todo read buttons

  prevSendMillis = currentMillis;
}

void updateCableStates()
{
  //Pulse the latch pin:
  //set it to 1 to collect parallel data
  digitalWrite(shift_in_latch_pin, 1);
  //set it to 1 to collect parallel data, wait
  delayMicroseconds(20);
  //set it to 0 to transmit data serially
  digitalWrite(shift_in_latch_pin, 0);

  byte statesIn;
  //while the shift register is in serial mode
  //collect each shift register into a byte
  //the register attached to the chip comes in first
  statesIn = shiftIn(shift_in_data_pin, shift_in_clock_pin);
  cableStates = statesIn;
  statesIn = shiftIn(shift_in_data_pin, shift_in_clock_pin);
  cableStates = cableStates << 8;
  cableStates = cableStates | statesIn;

  statesIn = shiftIn(shift_in_data_pin, shift_in_clock_pin);
  cableStates = cableStates << 8;
  cableStates = cableStates | statesIn;

  statesIn = shiftIn(shift_in_data_pin, shift_in_clock_pin);
  cableStates = cableStates << 8;
  cableStates = cableStates | statesIn;

  if (prevCableStates != cableStates)
  {
    int numCables[] = {0, 0, 0, 0, 0}; // Coal, Gas, Hydro, Solar, Wind
    long mask = 1;
    for (int n = 0; n < 24; n++)
    {
      //iterate through the bits in cableStates
      //for those that return true (ie that pin) add to the
      //numCables array.
      if (!(cableStates & mask))
      {

        if (n < 4)
        {
          numCables[1]++;
        }
        else if (n < 8)
        {
          numCables[0]++;
        }
        else if (n < 12)
        {
          numCables[3]++;
        }
        else if (n < 16)
        {
          numCables[2]++;
        }
        else if (n < 20)
        {
          numCables[4]++;
        }
      }
      if ((n > 19) && (n < 24))
      {
        if (!(cableStates & mask))
        {
          if (toggleStates[n - 20] < 1)
          {
            toggleStates[n - 20] = 1;
            ledState[n - 20] = 1;
            // digitalWrite(ledPins[n - 20], HIGH);
          }
        }
        else
        {
          // toggleStates[n - 20] = 0;
          // ledState[n - 20] = 0;
          // digitalWrite(ledPins[n - 20], LOW);
        }
      }
      mask = mask << 1;
    }

    Coal.setNumCables(numCables[0]);
    Gas.setNumCables(numCables[1]);
    Hydro.setNumCables(numCables[2]);
    Solar.setNumCables(numCables[3]);
    Wind.setNumCables(numCables[4]);

    prevCableStates = cableStates;
  }
}

byte shiftIn(int myDataPin, int myClockPin)
{
  int i;
  int temp = 0;
  int pinState;
  byte myDataIn = 0;

  pinMode(myClockPin, OUTPUT);
  pinMode(myDataPin, INPUT);

  for (i = 7; i >= 0; i--)
  {
    digitalWrite(myClockPin, 0);
    delayMicroseconds(0.2);
    temp = digitalRead(myDataPin);
    if (temp)
    {
      pinState = 1;
      //set the bit to 0 no matter what
      myDataIn = myDataIn | (1 << i);
    }
    else
    {
      pinState = 0;
    }
    digitalWrite(myClockPin, 1);
  }
  return myDataIn;
}

void updateHydro()
{
  int hydroValue;
  hydroValue = analogRead(hydro_1_input_pin);
  // hydroValue = random(1023); //todo Remove once connected to Pot
  hydroValue = map(hydroValue, 35, 380, 0, 100);
  hydroValue = constrain(hydroValue, 0, 100);
  Hydro.setPercentageActive(hydroValue);
}

// this function will run when serialController reads new data  TODO reconfig for Stele serial
void onParse(char *message, char *value)
{
  if (strcmp(message, "led") == 0)
  {
    // Turn-on led
  }
  else if (strcmp(message, "wake-arduino") == 0 && strcmp(value, "1") == 0)
  {
    // you must respond to this message, or else
    // stele will believe it has lost connection to the arduino
    serialController.sendMessage("arduino-ready", "1");
  }
  else
  {
    // helpfully alert us if we've sent something wrong :)
    serialController.sendMessage("unknown-command", "1");
  }
}
