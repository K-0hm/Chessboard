/*******************************************************************************

  Bare Conductive MPR121 library
  ------------------------------

  SimpleTouch.ino - simple MPR121 touch detection demo with serial output

  Based on code by Jim Lindblom and plenty of inspiration from the Freescale
  Semiconductor datasheets and application notes.

  Bare Conductive code written by Stefan Dzisiewski-Smith and Peter Krige.

  This work is licensed under a MIT license https://opensource.org/licenses/MIT

  Copyright (c) 2016, Bare Conductive

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.

*******************************************************************************/

// touch includes
#include <Wire.h>
#include "MPR121.h"
#include "MPR121_Datastream.h"

// touch constants
const uint32_t BAUD_RATE = 115200;
const uint8_t MPR121_ADDR = 0x5A;  // 0x5C is the MPR121 I2C address on the Bare Touch Board
const uint8_t MPR121_INT = 4;  // pin 4 is the MPR121 interrupt on the Bare Touch Board

// MPR121 datastream behaviour constants
const bool MPR121_DATASTREAM_ENABLE = false;



//OUR declarations

enum {
  freed,
  occupied,
  held,
};

int CaseState = freed;
int LastState[12] = {0}; //store last case State for every pin,initialized free
int CurrentState[12] = {0};
uint16_t HumanTreshold = 300;



//---- OUR function declaration -----

void UpdateAllCase() {

  //base library update
  MPR121.updateAll();

  //additional from Côme
  for (int i = 0; i < 12; i++) { //updates
    CurrentState[i] = CaseUpdate(i);
  }
}

int CaseUpdate(int Case) {
  if (MPR121.getTouchData(Case)) {
    if (MPR121.getFilteredData(Case) < (MPR121.getBaselineData(Case) - 100)) {
      return held;
    }
    else {
      return occupied;
    }
  }
  if (!MPR121.getTouchData(Case)) {
    return freed;
  }
}

//Data for next loop
void RememberStates() {
  for (int i = 0; i < 12; i++) {
    LastState[i] = CurrentState[i];
  }
}

//Print
void PrintValues(bool filtered, bool base) {
  for (int i = 0; i < 12; i++) {
    Serial.print( "electrode ");
    Serial.print(i, DEC);
    Serial.print(" :");
    if (filtered) {
      Serial.print( "   Filtered ");
      Serial.print(MPR121.getFilteredData(i), DEC);
    }
    if (base) {
      Serial.print( "   Baseline ");
      Serial.println(MPR121.getBaselineData(i), DEC);
    }
  }
}

void PrintNewTouch() {
  for (int i = 0; i < 12; i++) {
    if (MPR121.isNewTouch(i)) {
      Serial.print("electrode ");
      Serial.print(i, DEC);
      Serial.println(" was just touched");
    } else if (MPR121.isNewRelease(i)) {
      Serial.print("electrode ");
      Serial.print(i, DEC);
      Serial.println("  was just released");
    }
  }
}

void PrintNewState() {
  for (int i = 0; i < 12; i++) {
    if (CurrentState[i] != LastState[i]) {
      Serial.print( "electrode ");
      Serial.print(i, DEC);
      Serial.print(" : ");
      switch (CurrentState[i]) {
        case (freed):
          Serial.println("freed");
          break;

        case (occupied):
          Serial.println("occupied");
          break;

        case (held):
          Serial.println("held");
          break;

        default:
            Serial.print(CurrentState[i], DEC);
          Serial.println(" Illegal  CaseState Value");//ultimately useless

          break;
      }
    }
  }
}

void setup() {
  Serial.begin(BAUD_RATE);

  if (!MPR121.begin(MPR121_ADDR)) {
    Serial.println("error setting up MPR121");
    switch (MPR121.getError()) {
      case NO_ERROR:
        Serial.println("no error");
        break;
      case ADDRESS_UNKNOWN:
        Serial.println("incorrect address");
        break;
      case READBACK_FAIL:
        Serial.println("readback failure");
        break;
      case OVERCURRENT_FLAG:
        Serial.println("overcurrent on REXT pin");
        break;
      case OUT_OF_RANGE:
        Serial.println("electrode out of range");
        break;
      case NOT_INITED:
        Serial.println("not initialised");
        break;
      default:
        Serial.println("unknown error");
        break;
    }
    while (1);
  }

  MPR121.setInterruptPin(MPR121_INT);

  if (MPR121_DATASTREAM_ENABLE) {
    MPR121.restoreSavedThresholds();
    MPR121_Datastream.begin(&Serial);
  } else {
    MPR121.setTouchThreshold(40);  // this is the touch threshold - setting it low makes it more like a proximity trigger, default value is 40 for touch
    MPR121.setReleaseThreshold(20);  // this is the release threshold - must ALWAYS be smaller than the touch threshold, default value is 20 for touch
  }

  MPR121.setFFI(FFI_10);
  MPR121.setSFI(SFI_10);
  MPR121.setGlobalCDT(CDT_4US);  // reasonable for larger capacitances

  digitalWrite(LED_BUILTIN, HIGH);  // switch on user LED while auto calibrating electrodes
  delay(1000);
  MPR121.autoSetElectrodes();  // autoset all electrode settings
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  UpdateAllCase();
  //PrintValues(true,true)();//do not run much Without delay() or Time Control.
  PrintNewTouch();
  PrintNewState();
  RememberStates();



  if (MPR121_DATASTREAM_ENABLE) {
    MPR121_Datastream.update();
  }
  delay(500);//for debug
}
