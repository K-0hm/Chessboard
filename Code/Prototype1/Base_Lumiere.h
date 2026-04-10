//Libraries
#include<Arduino.h>
#include "Base_Config.h"
#include <FastLED.h>

//Constants
#define NUM_STRIPS 1
//#define NUM_LEDS 9  //deported to config
#define BRIGHTNESS 5
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB//RGB
#define FASTLED_ALLOW_INTERRUPTS 0
#define FASTLED_INTERRUPT_RETRY_COUNT 2 // mettre 1 pour Arduino IDE
#define FRAMES_PER_SECOND 60
#define COOLING 55
#define SPARKING 120


//Port LED
//#define stripPin = 3 //deported to multibase

//Objects
CRGB leds[NUM_LEDS];


void LEDsetup() {
    //Init led strips
  FastLED.addLeds<LED_TYPE, stripPin, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(  BRIGHTNESS );
}



int GetLedIndex(int x, int y){ //de la position echiquier donne le numero LED correspondant
  if(x%2) return x*TAILLE-y+TAILLE-1;
  else return x*TAILLE+y;
}


void afficheTableau(bool T[TAILLE][TAILLE]){//affiche un tableau booleen en 
  for (int i=0; i<TAILLE; i++){
      for (int j=0; j<TAILLE; j++){
        if(T[0][j]){
        leds[GetLedIndex(i,j)].setRGB(255,0,255);
        }else leds[GetLedIndex(i,j)].setRGB(0,0,0);
      }
    }  
}

void NoLight(){
  for (int i=0; i<TAILLE*TAILLE; i++){
    leds[i].setRGB(0,0,0);
  }
}

void affiche_led(int T[TAILLE][TAILLE]){   //attribution des sorties
//  1 = green(coup légal),2 = red(capture possible), 3 = blue(pos_piece_noir),4 = blanc(pos_piece_noir),  0 = étein 
 for (int i=0; i<TAILLE; i++){
   for (int j=0; j<TAILLE; j++){
    int coord_led = GetLedIndex(i, j);
    switch(T[i][j])
    { 
     case Blank:
     leds[coord_led].setRGB(0,0,0);
      break;
     case Move:
      leds[coord_led].setRGB(0,255,0);
     break;
     case Take:
      leds[coord_led].setRGB(255,0,0);
      break;
     case Black:
      leds[coord_led].setRGB(0,0,255);
      break;
     case White:
      leds[coord_led].setRGB(255,255,255);
      break;
     case Roque:
     leds[coord_led].setRGB(255,255,0);
      break;
     case TakeEnPassant:
     leds[coord_led].setRGB(255,0,255);
      break;
     }
   }
 }
}
void affiche_value(){//brouillon, à paramètrer
  for (int Case=0; Case<10; Case++) {
    int strength=MPR121.getBaselineData(Case)-MPR121.getFilteredData(Case);
             if (strength>255) strength=255;
              if (strength>0) leds[Case].setRGB(strength,strength,strength);
             else leds[Case].setRGB(0,0,-strength);
  }
}

void affiche_led_lineArray(int T[TAILLE*TAILLE]){   //attribution des sorties
//  1 = green(coup légal),2 = red(capture possible), 3 = blue(pos_piece_noir),4 = blanc(pos_piece_noir),  0 = étein 
 for (int i=0; i<TAILLE; i++){
   for (int j=0; j<TAILLE; j++){
    int coord_led = GetLedIndex(i, j);
    switch(T[coord_led])
    { 
     case Blank:
     leds[coord_led].setRGB(0,0,0);
      break;
     case Move:
      leds[coord_led].setRGB(0,255,0);
     break;
     case Take:
      leds[coord_led].setRGB(255,0,0);
      break;
     case Black:
      leds[coord_led].setRGB(0,0,255);
      break;
     case White:
      leds[coord_led].setRGB(255,255,255);
      break;
     case Roque:
     leds[coord_led].setRGB(255,255,0);
      break;
     case TakeEnPassant:
     leds[coord_led].setRGB(255,0,255);
      break;
     }
   }
 }
}
