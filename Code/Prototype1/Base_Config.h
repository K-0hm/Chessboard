#ifndef BASE_CONFIG_H
#define BASE_CONFIG_H


//Hardware
#define TAILLE 3
#define NUM_LEDS 9 
#define NbCell 12 //nb of cell on the MPR121 sensor
#define stripPin 3


#define FilteredMemorySize 16//ours,choose nb of measures per sensor (less variance)

//enums
enum game{
  None,
  Chess,
  TTT,
  MPRtest,
  MPReverything
};

enum casestate{
  freed,
  occupied,
  held,
};

enum TTT{
  blank=0,
  red=2,
  blue=3,
  equal=4,
};

//--------enums et structs d'echecs
enum coups {
  Blank,
  Move=1,
  Take=2,//red
  Black=3,//print in blue
  White=4,
  Roque=5,
  //Promotion=6,
  TakeEnPassant=7,
  //Forced,
  //Anim,
  
} ;

enum Type_pieces {
  empty=0,
  rook=1,
  knight=2,
  bishop=3,
  pawn=4,
  queen=5,
  king=6,
} ;


enum couleurs {
  Noir=false,
  Blanc=true,
  Vierge=-1,
};

typedef struct {
  int type;
  int couleur;
}TYPEPIECE;

//protos

#endif
