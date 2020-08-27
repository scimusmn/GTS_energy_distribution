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
// #include "arduino-base/Libraries/SerialController.hpp"

#define bar_graphs_Pin 6
#define production_Pin 7
#define demand_Pin 8
#define cablesLatchPin 4
#define cablesDataPin 3
#define cablesClockPin 2
#define hydroAnalogPin A1

// SerialController serialController;
// long baudrate = 115200;

// Declare NeoPixel strip object for bar graphs:
Adafruit_NeoPixel barGraphs(95, bar_graphs_Pin, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel ProductionGraph(70, production_Pin, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel DemandGraph(70, demand_Pin, NEO_GRB + NEO_KHZ800);

Source Coal(&barGraphs, 0);
Source Gas(&barGraphs, 18);
Source Hydro(&barGraphs, 37);
Source Solar(&barGraphs, 56);
Source Wind(&barGraphs, 75);

long cableStates = 0; // GGGGCCCCHHSSSWWW   Gas Coal Hydro Solar Wind
long prevCableStates = 2;
int simulationMinutes = 0;
int demand_mw = 0;

unsigned long currentMillis, prevSendMillis = 0;

void setup()
{
  // Ensure Serial Port is open and ready to communicate
  // serialController.setup(baudrate, &onParse);

  Serial.begin(115200);

  //define pin modes
  pinMode(cablesLatchPin, OUTPUT);
  pinMode(cablesClockPin, OUTPUT);
  pinMode(cablesDataPin, INPUT);
  pinMode(hydroAnalogPin, INPUT);

  barGraphs.begin();
  DemandGraph.begin();
  ProductionGraph.begin();
  barGraphs.clear();
  DemandGraph.clear();
  ProductionGraph.clear();

  Serial.println("Minutes, Coal, Gas, Hydro, Solar, Wind, Demand");
}

void loop()
{
  currentMillis = millis();
  // serialController.update();

  updateCableStates();

  if ((currentMillis - prevSendMillis) > 250)
  {
    updateHydro(); // read controls and update

    // generates fake solar/wind data
    int cosValue = (35 - 65 * cos((simulationMinutes / 60) / 3.8));
    cosValue = constrain(cosValue, 0, 100);
    Wind.setPercentageActive(cosValue);
    Solar.setPercentageActive(cosValue);

    // demand is just a sine wave
    demand_mw = 800 - 300 * cos((simulationMinutes / 60) / 3.8);

    Coal.setPercentageActive(100); // todo read switches!
    Gas.setPercentageActive(100);  // todo read buttons

    updateProductionGraph();
    updateDemandGraph(demand_mw);

    Serial.print(simulationMinutes);
    Serial.print(", ");
    Serial.print(Coal.getPowerProduced());
    Serial.print(", ");
    Serial.print(Gas.getPowerProduced());
    Serial.print(", ");
    Serial.print(Hydro.getPowerProduced());
    Serial.print(", ");
    Serial.print(Solar.getPowerProduced());
    Serial.print(", ");
    Serial.print(Wind.getPowerProduced());
    Serial.print(", ");
    Serial.println(demand_mw);

    simulationMinutes = simulationMinutes + 15; // 15 minutes pass every 250 ms (1/4 second)
    prevSendMillis = currentMillis;
  }
}

void updateProductionGraph()
{
  int powerProduced = Coal.getPowerProduced() + Gas.getPowerProduced() + Hydro.getPowerProduced() + Solar.getPowerProduced() + Wind.getPowerProduced();
  powerProduced = map(powerProduced, 0, 1000, 0, 70);
  ProductionGraph.clear();
  for (int i = 0; i < powerProduced; i++)
  {
    ProductionGraph.setPixelColor(i, ProductionGraph.Color(0, 0, 20));
  }
  ProductionGraph.show();
}

void updateDemandGraph(int demand)
{
  int powerDemanded = map(demand, 0, 1000, 0, 70);
  DemandGraph.clear();
  for (int i = 0; i < powerDemanded; i++)
  {
    DemandGraph.setPixelColor(i, DemandGraph.Color(20, 0, 0));
  }
  DemandGraph.show();
}

void updateCableStates()
{
  //Pulse the latch pin:
  //set it to 1 to collect parallel data
  digitalWrite(cablesLatchPin, 1);
  //set it to 1 to collect parallel data, wait
  delayMicroseconds(20);
  //set it to 0 to transmit data serially
  digitalWrite(cablesLatchPin, 0);

  byte statesIn;
  //while the shift register is in serial mode
  //collect each shift register into a byte
  //the register attached to the chip comes in first
  statesIn = shiftIn(cablesDataPin, cablesClockPin);
  cableStates = statesIn;
  statesIn = shiftIn(cablesDataPin, cablesClockPin);
  cableStates = cableStates << 8;
  cableStates = cableStates | statesIn;

  statesIn = shiftIn(cablesDataPin, cablesClockPin);
  cableStates = cableStates << 8;
  cableStates = cableStates | statesIn;

  statesIn = shiftIn(cablesDataPin, cablesClockPin);
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
        else if (n < 24)
        {
          numCables[4]++;
        }
        else
        {
          Serial.println("cable error");
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
  hydroValue = analogRead(hydroAnalogPin);
  // hydroValue = random(1023); //todo Remove once connected to Pot
  hydroValue = map(hydroValue, 35, 380, 0, 100);
  hydroValue = constrain(hydroValue, 0, 100);
  Hydro.setPercentageActive(hydroValue);
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
