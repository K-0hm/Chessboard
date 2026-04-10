#include "Base_Config.h"

//autres prog:

int TTT_Array[TAILLE][TAILLE]={0};
int TTTturn=red;
int TTTwon=blank;
int TTTwriten=0;
int TTTWinArray[TAILLE][2]={0};//store winning line

////////// TTT functions//////////

/*//??
int* ProtoGetMPRpos(int index){//de l'index des donne la position echiquier //wiing disposition in columns:
  int pointy[2];
  pointy[0]=(index/TAILLE);//x?// wired from  0 to 9
  pointy[1]=(index%TAILLE);//y?
  return (pointy);
}*/

void TTT_Check_Win(int x,int y){
  if (TTTwriten>=5){
    for(int dirx=-1; dirx<=1; dirx++){
      for(int diry=-1; diry<=1; diry++){
        if(!(dirx==0 and diry==0)){//not 0
          int dirx2=dirx+dirx; int diry2=diry+diry;
          if ((dirx2+x>=0 &&dirx2+x<TAILLE)&&(diry2+y>=0 &&diry2+y<TAILLE)){//border effects
            if ((TTT_Array[x+dirx][y+diry]==TTTturn)&&(TTT_Array[x+dirx2][y+diry2]==TTTturn)) {//WinCon
              TTTwon=TTTturn;//switch loop//remember winner
          
              for (int i=0; i<TAILLE;i++){//prepare for display
                  for(int j=0; j<TAILLE;j++){
                   TTT_Array[i][j]=blank;
                 }
              }
              TTT_Array[x][y]=TTTwon;
              TTT_Array[x+dirx][y+diry]=TTTwon;
              TTT_Array[x+dirx2][y+diry2]=TTTwon;
            }
          }
        }
      }
    }
    if((TTTwon==blank)&&TTTwriten>=9) TTTwon=equal;
  }
}


void TTTReset(){
  TTTturn=red;
  TTTwriten=0;
  TTTwon=blank;
  for (int i=0; i<TAILLE;i++){
    for(int j=0; j<TAILLE;j++){
      TTT_Array[i][j]=blank;
    }
  }
}

void TTTSwitchColor(){
  for (int x=0; x<TAILLE; x++){
    for (int y=0; y<TAILLE; y++){
    if(TTT_Array[x][y]==blue) (TTT_Array[x][y]=red);
    else if(TTT_Array[x][y]==red) TTT_Array[x][y]=blue;
   }
  }
}
