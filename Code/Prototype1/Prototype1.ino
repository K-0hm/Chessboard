
#include "Base_Config.h"  //toutes declaration indépendantes du hardware
#include "Base_Toucher.h"
#include "Base_Lumiere.h"

#include "Base_Echec.h"
#include "Base_TicTacToe.h" 
#define SWITCHPIN 11   //declare a pin to use to switch game

#define FRAMETIME 500


int game=None;
bool StartANew=true;

unsigned long TIMENEW=0;
unsigned long lastgamechange=0;
unsigned long LastTick=0; //communicate to serial ... order display... //memory of last timed display instance
unsigned long TIMERSTART=0;
int TimerLenght=1000; //!variable
void setup() {
  // put your setup code here, to run once:
  MPRSetup();
  LEDsetup();
  TIMENEW=millis();
  Serial.print("Satup");
}

void loop() {
  // put your main code here, to run repeatedly:
  TIMENEW=millis();
  UpdateAllCase();
  if (CurrentState[SWITCHPIN]==true and TIMENEW > lastgamechange + 1000){//Switching gamemodes
    lastgamechange=TIMENEW;    //the delay make it usable in the real world.
    StartANew=true;
    if (game==MPReverything) game=Chess;//switch
    else game+=1;
    }
  
  switch(game){
    
    case None://input detection
      if (initialised==false and TIMENEW>2000){ //better use setup delay?
        MPR121.updateBaselineData();//since taken off from all updates
       initialised=true; 
       //Serial.print("N");//debug only
       leds[0].setRGB(0,255,0);//signaling setUP is done.
      }
      break;
    
    case Chess:
        if(StartANew==true){
          NoLight();
          ResetGame();
          StartANew=false;
          //Serial.print("C");//debug only
          leds[0].setRGB(255,255,255);//signaling chess
        }
        ChessLoop();
        //affiche_led(coup_possibles);//ShowLights
        break;

    case TTT:
        if(StartANew==true){
          TTTReset();
          StartANew=false;
          //Serial.print("T");//debug only
        }
        TTTLoop();
        affiche_led(TTT_Array);//ShowLights
      break;
 
    case MPRtest:
        if(StartANew==true){
          StartANew=false;
          //Serial.print("M");//debug only
          NoLight();
        }
        //MPRLoop();//uncomment to send electrode data to serial monitor
        affiche_led_lineArray(CurrentState);
      break;
      
    case MPReverything:
        if(StartANew==true){
          StartANew=false;
          //Serial.print("E");//debug only
          NoLight();
        }
        affiche_value();
      break;

    default:
      Serial.print("How did we get here");
      game=None;
      NoLight();
      break;
  }
  Remember();
  FastLED.show();
  if (MPR121_DATASTREAM_ENABLE) {
    MPR121_Datastream.update();
  }
  //delay(10);//for debug only //better use TimeNew 
}






void MPRLoop() {
  if(CurrentState[8]!=LastState[8]){
    Serial.print(" State: ");
    Serial.println(CurrentState[8],DEC);
  }
  if (TIMENEW-LastTick>500){
    LastTick=TIMENEW;
    
    //PrintValues(true,true,true)();//do not run much Without delay() or Time Control.
    PrintValue(8,true,true,true);
    // PrintNewTouch()
    PrintNewState();
  }
  

  if (MPR121_DATASTREAM_ENABLE) {
    MPR121_Datastream.update();
  }
  //delay(10);//for debug
}


void TTTLoop(){
  switch(TTTwon){
    case red:
    case blue:
      if (TIMENEW > LastTick+FRAMETIME)
         //somehow 
        TTTSwitchColor();
    case equal:
      if (TIMENEW>TIMERSTART+TimerLenght){
        TTTReset();
      }
      Serial.print(TTTwon,DEC);//debug only
    break;
      
    case blank:
  for (int x=0; x<TAILLE; x++){
    for(int y=0; y<TAILLE; y++){
      if(CurrentState[TAILLE*x+y]>freed) {
        if(TTT_Array[x][y]==blank){//(TTT_Valid){
          TTTwriten++;
          Serial.print(TTTwriten,DEC);
          Serial.print("v");
          TTT_Array[x][y]=TTTturn;
          TTT_Check_Win(x,y);
          if (TTTwon!=blank){
            TIMERSTART=TIMENEW;
            TimerLenght=3000;
            }
          switch(TTTturn){
            case blue:
              TTTturn=red;
              break;
              
            case red:
              TTTturn=blue;
              break;
              
            default:
              Serial.print("who is playing?");
              Serial.print(TTTturn,DEC);
              break;
          }
        }
      }
    }
  }
  break;
  }
}

void ChessLoop(){
  //Serial.print("C");//debug only
  int x;
}
