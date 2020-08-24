//**************************************************************//
//  Component: Energy Distribution                              //
//  Project: Gateway to Science                                 //
//  Author  : Joe Meyer                                         //
//  Date    : 8/21/2020                                         //
//  Version : 1.0                                               //
//  Description : Control Panel v2                              //
//****************************************************************
#include <Adafruit_NeoPixel.h>

#define bar_graphs_Pin 6
#define cablesLatchPin 4
#define cablesDataPin 3
#define cablesClockPin 2



// Declare our NeoPixel strip object:
Adafruit_NeoPixel barGraphs(95, bar_graphs_Pin, NEO_GRB + NEO_KHZ800);

long cableStates = 0;  // GGGGCCCCHHSSSWWW   Gas Coal Hydro Solar Wind
long prevCableStates = 0;
int numCables[] = {0,0,0,0,0}; // Coal, Gas, Hydro, Solar, Wind
byte statesIn = 72;

unsigned long currentMillis;


void setup() {
  //start serial
  Serial.begin(9600);

  //define pin modes
  pinMode(cablesLatchPin, OUTPUT);
  pinMode(cablesClockPin, OUTPUT);
  pinMode(cablesDataPin, INPUT);

  barGraphs.begin();
  barGraphs.show(); // Initialize all pixels to 'off'
}

void loop() {
  currentMillis = millis();

  //Pulse the latch pin:
  //set it to 1 to collect parallel data
  digitalWrite(cablesLatchPin,1);
  //set it to 1 to collect parallel data, wait
  delayMicroseconds(20);
  //set it to 0 to transmit data serially  
  digitalWrite(cablesLatchPin,0);

  //while the shift register is in serial mode
  //collect each shift register into a byte
  //the register attached to the chip comes in first
  statesIn = shiftIn(cablesDataPin, cablesClockPin);
//  statesIn = B00011000; // todo REMOVE AFTER TESTING
  cableStates = statesIn;
//  statesIn = B10000001; // todo REMOVE AFTER TESTING
  statesIn = shiftIn(cablesDataPin, cablesClockPin);
  cableStates = cableStates << 8;
  cableStates = cableStates | statesIn;
  
  statesIn = shiftIn(cablesDataPin, cablesClockPin);
  cableStates = cableStates << 8;
  cableStates = cableStates | statesIn;
  
  statesIn = shiftIn(cablesDataPin, cablesClockPin);
  cableStates = cableStates << 8;
  cableStates = cableStates | statesIn;
   
  if (prevCableStates != cableStates){    
    Serial.println(cableStates, BIN);
    numCables[0] = numCables[1] = numCables[2] = numCables[3] = numCables[4] = 0;

    for (int n=0; n<=15; n++)
    {
      //iterate through the bits in cableStates
      //for those that return true (ie that pin) add to the 
      //numCables array.
      if (!(cableStates & (1 << n))) { 
        if (n < 4) {
          numCables[0]++;
        }else if ((n > 3) && (n<8)) {
          numCables[1]++;
        }else if ((n>7) && (n<10)){
          numCables[2]++;
        }else if ((n>9) && (n<13)){
          numCables[3]++;
        } else {
          numCables[4]++;
        }
      }
    }

    barGraphs.clear();
    //white space
    Serial.println("-------------------");
    Serial.print("Coal: ");       
    Serial.println(numCables[0]);
    int n = map(numCables[0],0,4,0,18);
    for (int i=0; i<n; i++){
      barGraphs.setPixelColor(i, 0, 0, 40);
    } 
    Serial.print("Gas: ");
    Serial.println(numCables[1]);
    n = map(numCables[1],0,4,0,18);
    for (int i=0; i<n; i++){
      barGraphs.setPixelColor(i+18, 0, 0, 40);
    } 
    Serial.print("Hydro: ");
    Serial.println(numCables[2]);
    n = map(numCables[2],0,4,0,18);
    for (int i=0; i<n; i++){
      barGraphs.setPixelColor(i+37, 0, 0, 40);
    } 
    Serial.print("Solar: ");
    Serial.println(numCables[3]);
    n = map(numCables[3],0,4,0,18);
    for (int i=0; i<n; i++){
      barGraphs.setPixelColor(i+56, 0, 0, 40);
    } 
    Serial.print("Wind: ");
    Serial.println(numCables[4]);

    n = map(numCables[4],0,4,0,18);
    for (int i=0; i<n; i++){
      barGraphs.setPixelColor(i+75, 0, 0, 40);
    } 
    barGraphs.show();
    
    prevCableStates = cableStates;
  }
 
  

//delay so all these print satements can keep up.
delay(100);

}

//------------------------------------------------end main loop

////// ----------------------------------------shiftIn function
///// just needs the location of the data pin and the clock pin
///// it returns a byte with each bit in the byte corresponding
///// to a pin on the shift register. leftBit 7 = Pin 7 / Bit 0= Pin 0

byte shiftIn(int myDataPin, int myClockPin) {
  int i;
  int temp = 0;
  int pinState;
  byte myDataIn = 0;

  pinMode(myClockPin, OUTPUT);
  pinMode(myDataPin, INPUT);
//we will be holding the clock pin high 8 times (0,..,7) at the
//end of each time through the for loop

//at the begining of each loop when we set the clock low, it will
//be doing the necessary low to high drop to cause the shift
//register's DataPin to change state based on the value
//of the next bit in its serial information flow.
//The register transmits the information about the pins from pin 7 to pin 0
//so that is why our function counts down
  for (i=7; i>=0; i--)
  {
    digitalWrite(myClockPin, 0);
    delayMicroseconds(0.2);
    temp = digitalRead(myDataPin);
    if (temp) {
      pinState = 1;
      //set the bit to 0 no matter what
      myDataIn = myDataIn | (1 << i);
    }
    else {
      //turn it off -- only necessary for debuging
     //print statement since myDataIn starts as 0
      pinState = 0;
    }

    //Debuging print statements
    //Serial.print(pinState);
    //Serial.print("     ");
    //Serial.println (dataIn, BIN);

    digitalWrite(myClockPin, 1);

  }
  //debuging print statements whitespace
  //Serial.println();
  //Serial.println(myDataIn, BIN);
  return myDataIn;
}
