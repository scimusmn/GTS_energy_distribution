//**************************************************************//
//  Component: Energy Distribution                              //
//  Project: Gateway to Science                                 //
//  Author  : Joe Meyer                                         //
//  Date    : 8/21/2020                                         //
//  Version : 1.0                                               //
//  Description : Control Panel v2                              //
//****************************************************************
#include <Adafruit_NeoPixel.h>
#include "coal-burner.h"
#include "gas-burner.h"
#include "hydro-generator.h"
// #include "arduino-base/Libraries/SerialController.hpp"

#define bar_graphs_Pin 6
#define inputsLatchPin 4
#define inputsDataPin 3
#define inputsClockPin 2

// SerialController serialController;
// long baudrate = 115200;

// Declare NeoPixel strip object for bar graphs:
Adafruit_NeoPixel barGraphs(95, bar_graphs_Pin, NEO_GRB + NEO_KHZ800);

Coal_Burner coalBurner1(&barGraphs, 0, 0x10, 0x100000); //neopixel, first pixel, cable bit mask, switch bit mask
Coal_Burner coalBurner2(&barGraphs, 18, 0x20, 0x200000);
Coal_Burner coalBurner3(&barGraphs, 37, 0x40, 0x400000);
Coal_Burner coalBurner4(&barGraphs, 56, 0x80, 0x800000);
Gas_Burner gasBurner1(&barGraphs, 75, 0x80, 0x400000, 0x800000);
Hydro_Generator hydro1(&barGraphs, 84, 0x80, A1);

long inputStates = 0; // GGGGCCCCHHSSSWWW   Gas Coal Hydro Solar Wind
long prevInputStates = 2;
int simulationMinutes = 0;
int millisPer15Minutes = 250;
int gasBtnDebounce = 100; // debounce interval for gas burner buttons.

unsigned long currentMillis, sim15PrevMillis = 0, gas_btn_last_check;

void setup()
{
  // Ensure Serial Port is open and ready to communicate
  // serialController.setup(baudrate, &onParse);

  Serial.begin(115200);

  //define pin modes
  pinMode(inputsLatchPin, OUTPUT);
  pinMode(inputsClockPin, OUTPUT);
  pinMode(inputsDataPin, INPUT);

  barGraphs.begin();
  barGraphs.clear();
}

void loop()
{
  currentMillis = millis();
  // serialController.update();

  updateInputStates();

  if ((currentMillis - gas_btn_last_check) > gasBtnDebounce) // code runs every gasBtnDebounce millis.
  {
    gasBurner1.update(inputStates);
    hydro1.update(inputStates);
    // TODO add more gasburners

    gas_btn_last_check = currentMillis;
  }

  if ((currentMillis - sim15PrevMillis) > millisPer15Minutes) // code runs every "15 minutes" simulation time.
  {

    coalBurner1.update(inputStates); // coal take 6 hours to warm up.
    coalBurner2.update(inputStates);
    coalBurner3.update(inputStates);
    coalBurner4.update(inputStates);

    simulationMinutes = simulationMinutes + 15; // 15 minutes pass every 250 ms (1/4 second)
    sim15PrevMillis = currentMillis;
    barGraphs.show();
  }
}

void updateInputStates()
{
  //Pulse the latch pin:
  //set it to 1 to collect parallel data
  digitalWrite(inputsLatchPin, 1);
  //set it to 1 to collect parallel data, wait
  delayMicroseconds(20);
  //set it to 0 to transmit data serially
  digitalWrite(inputsLatchPin, 0);

  byte statesIn;
  //while the shift register is in serial mode
  //collect each shift register into a byte
  //the register attached to the chip comes in first
  statesIn = shiftIn(inputsDataPin, inputsClockPin);
  inputStates = statesIn;
  statesIn = shiftIn(inputsDataPin, inputsClockPin);
  inputStates = inputStates << 8;
  inputStates = inputStates | statesIn;

  statesIn = shiftIn(inputsDataPin, inputsClockPin);
  inputStates = inputStates << 8;
  inputStates = inputStates | statesIn;

  statesIn = shiftIn(inputsDataPin, inputsClockPin);
  inputStates = inputStates << 8;
  inputStates = inputStates | statesIn;

  if (prevInputStates != inputStates)
  {
    prevInputStates = inputStates;
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

// this function will run when serialController reads new data  TODO reconfig for Stele serial
// void onParse(char* message, char* value) {
//   if (strcmp(message, "led") == 0) {
//     // Turn-on led
//   }
//   else if (strcmp(message, "wake-arduino") == 0 && strcmp(value, "1") == 0) {
//     // you must respond to this message, or else
//     // stele will believe it has lost connection to the arduino
//     serialController.sendMessage("arduino-ready", "1");
//   }
//   else {
//     // helpfully alert us if we've sent something wrong :)
//     serialController.sendMessage("unknown-command", "1");
//   }
// }
