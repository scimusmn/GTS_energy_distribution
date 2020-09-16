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
#include "renewable.h"
#include "arduino-base/Libraries/SerialController.hpp"

#define bar_graphs_Pin 6
#define inputsLatchPin 4
#define inputsDataPin 3
#define inputsClockPin 2

SerialController serialController;
long baudrate = 115200;

// Declare NeoPixel strip object for bar graphs:
Adafruit_NeoPixel barGraphs(95, bar_graphs_Pin, NEO_GRB + NEO_KHZ800);

Coal_Burner coalBurner1(&barGraphs, 0, 0x10, 0x100000); //neopixel, first pixel, cable bit mask, switch bit mask
Coal_Burner coalBurner2(&barGraphs, 18, 0x20, 0x200000);
Coal_Burner coalBurner3(&barGraphs, 37, 0x40, 0x400000);
Coal_Burner coalBurner4(&barGraphs, 56, 0x80, 0x800000);
Gas_Burner gasBurner1(&barGraphs, 27, 0x8, 0x400000, 0x800000);
Renewable hydro1(&barGraphs, 46, 0x1000, A1, 0x000020); // provide an analog pin for hydro.
Renewable solar1(&barGraphs, 65, 0x80, 0, 0x101000);    // 0 for the analog pin indicates it's wind or solar.
Renewable wind1(&barGraphs, 84, 0x80, 0, 0x070707);     // 0 for the analog pin indicates it's wind or solar.

long inputStates = 0; // GGGGCCCCHHSSSWWW   Gas Coal Hydro Solar Wind
long prevInputStates = 2;
int simulationMinutes = 0;
int millisPer15Minutes = 250;
int gasBtnDebounce = 100; // debounce interval for gas burner buttons.

unsigned long currentMillis, sim15PrevMillis = 0, gas_btn_last_check;

void setup()
{
  // Ensure Serial Port is open and ready to communicate
  serialController.setup(baudrate, &onParse);

  // Serial.begin(115200);

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
  serialController.update();

  if ((currentMillis - gas_btn_last_check) > gasBtnDebounce) // code runs every gasBtnDebounce millis.
  {
    updateInputStates();
    gasBurner1.update(inputStates);
    hydro1.update(inputStates);
    solar1.update(inputStates);
    wind1.update(inputStates);
    gas_btn_last_check = currentMillis;
    barGraphs.show();
  }

  if ((currentMillis - sim15PrevMillis) > millisPer15Minutes) // code runs every "15 minutes" simulation time.
  {
    coalBurner1.update(inputStates); // coal take 6 hours to warm up.
    coalBurner2.update(inputStates);
    coalBurner3.update(inputStates);
    coalBurner4.update(inputStates);

    simulationMinutes = simulationMinutes + 15; // 15 minutes pass every millisPer15Minutes (1/4 second)
    sim15PrevMillis = currentMillis;
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
void onParse(char *message, char *value)
{
  if (strcmp(message, "wind") == 0)
  {
    wind1.setPercentage(atoi(value));
  }
  else if (strcmp(message, "solar") == 0)
  {
    solar1.setPercentage(atoi(value));
  }
  else if (strcmp(message, "power") == 0)
  {
    serialController.sendMessage("################", "0");
    int coal_power = coalBurner1.getPowerProduced() + coalBurner2.getPowerProduced() + coalBurner3.getPowerProduced() + coalBurner4.getPowerProduced();
    serialController.sendMessage("coal", coal_power);
    int gas_power = gasBurner1.getPowerProduced();
    serialController.sendMessage("gas", gas_power);
    int hydro_power = hydro1.getPowerProduced();
    serialController.sendMessage("hydro", hydro_power);
    int solar_power = solar1.getPowerProduced();
    serialController.sendMessage("solar", solar_power);
    int wind_power = wind1.getPowerProduced();
    serialController.sendMessage("wind", wind_power);
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
