/*
----- To-Do-Liste: ------

  ------ Fait: -------
  
  -Représentation échiquier
  -Adressage des leds
  -Affichage Coups et captures  Normales avec Collision  (dont saut du pion)
  -Déplacements du roi  (dont roques et guarde fou des echecs)//a Tester 8*8
  -EnPassant//a tester 8*8
  
  ------ A faire: ------
  -Intégrer Kingcheck() dans KingMove()
  -Promotion
  -échecs
  -clouage
  -(Dans la Loop)  structure logique qui s'active sur 'déclenchement' d'un capteur
  -Détection Mat/Pat
  -Déplacement effectif des pièce sur l'échiquier(dans la mémoire de l'arduino) en utilisant ShowCaseMoves()
  -Effets Visuels controlés dans le temps (à l'aide de millis())
  
  -Récupération et traitement des données des capteurs
      -Ajustement pour le toucher(-->requiert echiquier matériel)
      -Implémentation dans la boucle
  -Tester sur (8x8)
  
*/
#include "Base_Config.h"  //toutes declaration indépendantes du hardware

int coup_possibles[TAILLE][TAILLE]={0};
bool WhiteControl[TAILLE][TAILLE]={0};
bool BlackControl[TAILLE][TAILLE]={0};

const int lpB = 1; const int lpN = TAILLE-2; //lignes initales des pions

bool Turn;

//bool roque
bool WhiteKingMoved;
bool WhiteATowerMoved;
bool WhiteHTowerMoved;
bool BlackKingMoved;
bool BlackATowerMoved;
bool BlackHTowerMoved;

bool WhiteChecked;
bool BlackChecked;

bool BoolPawnJump;//==0 si le dernier coup n'est pas un saut de pion 
int ColonnePawnJump; // Colonne du dernier saut de pion (couleur autre que le pion regardé) 

//--------enums et structs

//initialisation pièces blanches et noires
TYPEPIECE Empty = {empty,Vierge};
TYPEPIECE rookN = {rook,Noir};
TYPEPIECE rookB = {rook,Blanc};
TYPEPIECE knightN = {knight,Noir};
TYPEPIECE knightB = {knight,Blanc};
TYPEPIECE bishopN = {bishop,Noir};
TYPEPIECE bishopB = {bishop,Blanc};
TYPEPIECE queenN = {queen,Noir};
TYPEPIECE queenB = {queen,Blanc};
TYPEPIECE kingN = {king,Noir};
TYPEPIECE kingB = {king,Blanc};
TYPEPIECE pawnN = {pawn,Noir};
TYPEPIECE pawnB = {pawn,Blanc};

//abréviations
TYPEPIECE Emp=Empty;
TYPEPIECE roN = rookN;    TYPEPIECE roB = rookB;
TYPEPIECE knB = knightB;  TYPEPIECE knN = knightN;
TYPEPIECE biB = bishopB;  TYPEPIECE biN = bishopN;
TYPEPIECE quB = queenB;   TYPEPIECE quN = queenN;
TYPEPIECE kiB = kingB;    TYPEPIECE kiN = kingN;
TYPEPIECE paB = pawnB;    TYPEPIECE paN = pawnN;



//----declaration et initialisation de l'échiquier
/*
TYPEPIECE StartEchiquier[8][8] =
{
  //{roN, knN, biN, quN, kiN, biN, knN, roN},
  //{paN, paN, paN, paN, paN, paN, paN, paN},
  {Emp, Emp, Emp, Emp, Emp, Emp, Emp, Emp},
  {Emp, Emp, Emp, Emp, Emp, Emp, Emp, Emp},
  {Emp, Emp, Emp, Emp, Emp, Emp, Emp, Emp},
  {Emp, Emp, Emp, Emp, Emp, Emp, Emp, Emp},
  {Emp, Emp, Emp, Emp, Emp, Emp, Emp, Emp},
  {Emp, Emp, Emp, Emp, Emp, Emp, Emp, Emp},
  //{paB, paB, paB, paB, paB, paB, paB, paB},
  //{roB, knB, biB, quB, kiB, biB, knB, roB},
  {Emp, Emp, Emp, Emp, Emp, Emp, Emp, Emp},
  {roB, Emp, Emp, Emp, kiB, Emp, Emp, roB},
};
*/


TYPEPIECE StartEchiquier[3][3]={ // vue naturelle de l'échiquier
  {Emp,roN,biN},
  {knN,paB,paN},
  {Emp,quB,biN}
};

TYPEPIECE echiquier[TAILLE][TAILLE]={0};




//protos
void Refresh(bool couleur);
void KingRoque(bool couleur);
void KingCheck(int x, int y, bool couleur);

void NaturalToEchiquier(){
  for (int i=0;i<TAILLE;i++){
    for(int j=0;j<TAILLE;j++){
      echiquier[i][j] = StartEchiquier[TAILLE-1-i][j];
    }
  }
}

void ResetGame(){
  //echiquier
  NaturalToEchiquier();

  Turn=Blanc;
  
  //roque
  WhiteKingMoved = false;
  WhiteATowerMoved = false;
  WhiteHTowerMoved = false;
  BlackKingMoved = false;
  BlackATowerMoved = false;
  BlackHTowerMoved = false;
  
  WhiteChecked = false;
  BlackChecked = false;

  
  BoolPawnJump = false;
  
}


TYPEPIECE GetPiece(int x,int y){
  return echiquier[x][y];
}


void pos_piece(int x, int y,bool couleur){//allume la pèce à sa position
  if (couleur==Noir) coup_possibles[x][y] = Black;
  if (couleur==Blanc) coup_possibles[x][y] = White;
}



bool AssignMove(int x,int y, bool couleur){ //! Modifie le tableau coups_possible et renvoie un booleen d'arret si piece bloquée
  int cible=echiquier[x][y].couleur;
        if (cible == Vierge) {       //case vide
          coup_possibles[x][y] = Move;//déplacement libre
          return true;                //continue le movement
          } 
        else if (cible != couleur) {  //couleur opposée
          coup_possibles[x][y] = Take; //capture possible
          return false;                //fin du déplacement
        }
        else return false;  // couleur alliée, pas de deplacement
}

//Knight
int KnightMove[8][2] = {
  {2, 1}, {1, 2}, {-1, 2}, {-2, 1},
  {-2, -1}, {-1, -2}, {1, -2}, {2, -1}
};

void KnightMoves(int x, int y,bool couleur) {

  for (int i = 0; i < 8; i++) {
    int nx = x + KnightMove[i][0];
    int ny = y + KnightMove[i][1];
    if (nx >= 0 && nx < TAILLE && ny >= 0 && ny < TAILLE) {
      
      int cible=echiquier[nx][ny].couleur;
        if (cible == Vierge) coup_possibles[nx][ny] = Move;
        else if (cible != couleur) coup_possibles[nx][ny] = Take;//couleur opposée, capture possible
        // couleur alliée, pas de deplacement   
    }
  }
}

//King
int KingMove[8][2] = {
  {-1, 1}, {0,1}, {1, 1},
  {-1, 0},         {1, 0},           
  {-1, -1},{0,-1}, {1, -1}
  };

void KingMoves(int x, int y, bool couleur){
  
  for (int i = 0; i < 8; i++) {
    int incrxp = x + KingMove[i][0];
    int incryp = y + KingMove[i][1];
    if (incrxp >= 0 && incrxp < TAILLE && incryp >= 0 && incryp < TAILLE) {
      
     int cible=echiquier[incrxp][incryp].couleur;
        if (cible == Vierge) coup_possibles[incrxp][incryp] = Move;
        else if (cible != couleur) coup_possibles[incrxp][incryp] = Take;//couleur opposée, capture possible
        // couleur alliée, pas de deplacement   
    }
  }
}


//bishop
void BishopMoves(int x, int y,bool couleur){
  
  bool continu=true;
  for (int incr=1;((incr+x<TAILLE)&&(incr+y<TAILLE))&&(continu==true); incr++){    //diagonale xy +
      continu=AssignMove(incr+x,incr+y,couleur);     //s'occupe de l'assignation tableau et des collisions
  }
  continu=true;
  for (int incr=1;((incr+x<TAILLE)&&(y-incr>=0))&&(continu==true); incr++){    ////diagonale x +, y -
      continu=AssignMove(incr+x,y-incr,couleur);
  }

  continu=true;
  for (int incr=1;((x-incr>=0)&&(y-incr>=0))&&(continu==true); incr++){    //diagonale xy -
      continu=AssignMove(x-incr,y-incr,couleur);  
  }
  continu=true;
  for (int incr=1;((x-incr>=0)&&(incr+y<TAILLE))&&(continu==true); incr++){    //diagonale x -, y +
      continu=AssignMove(x-incr,incr+y,couleur);
  }
}


//rook
void RookMoves(int x, int y,bool couleur){//rajouter else coup_possibles[][]= false;?
  bool continu=true;
  for (int incrxp=x+1;(incrxp<TAILLE)&&(continu==true); incrxp++){    // colonne -> y = // colonne +
      continu=AssignMove(incrxp,y,couleur);     //s'occupe de l'assignation tableau et des collisions
  }
  continu=true;
  for (int incrxn=x-1;(incrxn>=0)&&(continu==true); incrxn--){      // colonne -> y = cst  //colonne -
      continu=AssignMove(incrxn,y,couleur);
  }
  continu=true;
  for (int incryp=y+1;(incryp<TAILLE)&&(continu==true); incryp++){    // ligne -> x = cst  //ligne +
      continu=AssignMove(x,incryp,couleur);     //s'occupe de l'assignation tableau et des collisions
  }
  continu=true;
  for (int incryn=y-1;(incryn>=0)&&(continu==true); incryn--){     // ligne -> x = cst  //ligne -
      continu=AssignMove(x,incryn,couleur);
  }

}

//queen
void QueenMoves(int x, int y,bool couleur){
  RookMoves(x,y,couleur);
  BishopMoves(x,y,couleur);
}

void EnPassant(int ligne,int colonne,bool couleur){
  if (BoolPawnJump){
    if(couleur){
      if(ligne==lpN-1){
        if (colonne+1==ColonnePawnJump){  //comprend les effets de bords
          coup_possibles[ligne][colonne+1] = TakeEnPassant;
          coup_possibles[ligne+1][colonne+1] = TakeEnPassant;
        }
        if (colonne-1==ColonnePawnJump-1){
          coup_possibles[ligne][colonne-1] = TakeEnPassant;
          coup_possibles[ligne+1][colonne-1] = TakeEnPassant;
        }
      }
    }
    else{
      if(ligne==lpB+1){
        if (colonne+1==ColonnePawnJump){
          coup_possibles[ligne][colonne+1] = TakeEnPassant;
          coup_possibles[ligne-1][colonne+1] = TakeEnPassant;
        }
        if (colonne-1==ColonnePawnJump-1){
          coup_possibles[ligne][colonne-1] = TakeEnPassant;
          coup_possibles[ligne-1][colonne-1] = TakeEnPassant;
        }
      }
    }
  }
 }

//pawn capture
void PawnCapture(int ligne,int colonne,bool couleur,bool MoveThatPiece){      
 
  if (couleur == Blanc && ligne+1<TAILLE){
    if (colonne+1<TAILLE && (echiquier[ligne+1][colonne+1].couleur == Noir||(MoveThatPiece==0)))coup_possibles[ligne+1][colonne+1] = 2;
    if (colonne-1>=0 && (echiquier[ligne+1][colonne-1].couleur == Noir||(MoveThatPiece==0)))coup_possibles[ligne+1][colonne-1] = 2;
    }
  
  if (couleur == Noir && ligne-1>=0){
    if (colonne+1<TAILLE && (echiquier[ligne-1][colonne+1].couleur == Blanc||(MoveThatPiece==0)))coup_possibles[ligne-1][colonne+1] = 2;
    if (colonne-1>=0 && (echiquier[ligne-1][colonne-1].couleur == Blanc||(MoveThatPiece==0)))coup_possibles[ligne-1][colonne-1] = 2;
  }
}

//pawn
void PawnMoves(int x, int y, bool couleur){
  
  if (couleur == Blanc && x+1<TAILLE && echiquier[x+1][y].type==empty){
    coup_possibles[x+1][y] = true;
    if (x==lpB && echiquier[x+2][y].type==empty) coup_possibles[x+2][y] = true; //si pion sur sa case de départ, alors peut avancer de 2
    } 
  
  if (couleur == Noir && x-1>=0 && echiquier[x-1][y].type==empty){
    coup_possibles[x-1][y] = true;
    if (x==lpN && echiquier[x-2][y].type==empty) coup_possibles[x-2][y] = true;  //si pion sur sa case de départ, alors peut avancer de 2
  }
}



void ShowPieceMoves(int x,int y,TYPEPIECE piece,bool MoveThatPiece){    //MoveThatPiece==true si mouvement normal, ==false pour la vision roi
  
  bool couleur=piece.couleur; 
  
  switch(piece.type){
    
    case rook: 
      RookMoves(x,y,couleur);
    break;
    
    case knight:
      KnightMoves(x,y,couleur);
    break;
    
    case bishop:
      BishopMoves(x,y,couleur);
    break;
    
    case pawn:
      PawnCapture(x, y,couleur,MoveThatPiece); //ne regarde pas encore si il doit verifier qu'une case se trouve sur son chemin//
      
      if (MoveThatPiece){   //when you want to move that piece
        PawnMoves(x,y,couleur);
        //PawnPassant(x,y,couleur);
        //PawnPromotion(x,y,couleur);
      }
    break;
    
    case queen:
      QueenMoves(x,y,couleur);
    break;
    
    case king:
    if (MoveThatPiece) {
      Refresh(couleur);
      KingRoque(couleur); // le roque est fait avant les mouvements du roi pas qu'il ne pense se menacé lui même
    KingMoves(x,y,couleur);
    if (MoveThatPiece) KingCheck(x,y,couleur);   //regarde si les mouvements du roi ne sont pas controlés.
    }
    break;
  }
  if (MoveThatPiece && (piece.type != empty)) {    //ne montre/cloue pas la piece pour (kingMoves)
    pos_piece(x,y,couleur);
    //pin_check(x,y,piece,couleur)
  }
  
}


void ShowCaseMoves(int x,int y){
  ShowPieceMoves(x,y,GetPiece(x,y),true);
}

void Resetcoup_possibles(){
  for(int i=0;i<TAILLE;i++){
    for(int j=0;j<TAILLE;j++){
      coup_possibles[i][j]=false;
    }
  }
}


//-------------Spécialité_Du_Roi------------- 

void ResetWhiteControl(){
  for(int i=0;i<TAILLE;i++){
    for(int j=0;j<TAILLE;j++){
      WhiteControl[i][j]=false;
    }
  }
}
void ResetBlackControl(){
  for(int i=0;i<TAILLE;i++){
    for(int j=0;j<TAILLE;j++){
      BlackControl[i][j]=false;
    }
  }
}

void RefreshWhiteControl(){//etablit les cases controlees par les blanc
  ResetWhiteControl();
    for(int i=0;i<TAILLE;i++){
      for(int j=0;j<TAILLE;j++){
        TYPEPIECE piece=GetPiece(i,j);
        if (piece.couleur==Blanc) {
          ShowPieceMoves(i,j,piece,false);
        }
      }
    }
  for(int i=0;i<TAILLE;i++){
      for(int j=0;j<TAILLE;j++){
        if (coup_possibles[i][j]!=0) {
          WhiteControl[i][j]=true;
        }
      }
    } 
}

void RefreshBlackControl(){//etablit les cases controlees par les noirs
    ResetBlackControl();
    
    for(int i=0;i<TAILLE;i++){
      for(int j=0;j<TAILLE;j++){
        TYPEPIECE piece=GetPiece(i,j);
        if (piece.couleur==Noir) {//if piece noire
          ShowPieceMoves(i,j,piece,false);
        }
      }
    }
    
    for(int i=0;i<TAILLE;i++){
      for(int j=0;j<TAILLE;j++){
        if (coup_possibles[i][j]!=0 ) {//alterrnative (ne prend pas une promotion/capture en compte)//"==2 || coup_possibles[i][j]==1"
          BlackControl[i][j]=true;
        }
      }
    } 
  }

void Refresh(bool couleur){
  if (couleur) RefreshBlackControl();
  else RefreshWhiteControl();
}
void KingCheck(int x, int y,bool couleur){//'garde fou':vérifie que les mouvements normaux du roi ne le mettent pas en échec  
  for (int i = 0; i < 8; i++) {
    int incrx = x + KingMove[i][0];
    int incry = y + KingMove[i][1];
    if (incrx >= 0 && incrx < TAILLE && incry >= 0 && incry < TAILLE) {
      if (couleur==Blanc && BlackControl[incrx][incry])
        coup_possibles[incrx][incry]==false;
      if (couleur==Noir && WhiteControl[incrx][incry])
        coup_possibles[incrx][incry]==false;
    }
  }
}

bool CaseLibreBlanc(int x,int y){// pour le roque des blancs
  bool boule = (BlackControl[x][y]==false && echiquier[x][y].type==empty);
  return boule;
}

void PetitRoqueBlanc(){
  if (WhiteHTowerMoved==false){
    if (CaseLibreBlanc(0,5) && CaseLibreBlanc(0,6)){  //F1,G1 Libres?
      coup_possibles[0][6]=Roque;
    }
  }
}
void GrandRoqueBlanc(){
  if (WhiteATowerMoved==false){
    if (CaseLibreBlanc(0,1) && CaseLibreBlanc(0,2) && CaseLibreBlanc(0,3)){  //B1,C1,D1 Libres?
      coup_possibles[0][1]=Roque;
    }
  }
}

bool CaseLibreNoir(int x,int y){// pour le roque
  return WhiteControl[x][y]==false && echiquier[x][y].type==empty;
}

void PetitRoqueNoir(){
  if (BlackHTowerMoved==false){
    if (CaseLibreNoir(7,5) && CaseLibreNoir(7,6)){  //F8,G8 Libres?
      coup_possibles[7][6]=Roque;
    }
  }
}
void GrandRoqueNoir(){
  if (BlackATowerMoved==false){
    if (CaseLibreNoir(7,1) && CaseLibreNoir(7,2) && CaseLibreNoir(7,3)){  //B8,C8,D8  Libres?
      coup_possibles[7][1]=Roque;
    }
  }
}

void KingRoque(bool couleur){
  switch (couleur){
    
    case(Blanc):
      if (WhiteKingMoved==false && WhiteChecked==false){
       PetitRoqueBlanc();
       //affiche_led;//debug
       GrandRoqueBlanc();
      }
    break;

    case(Noir):
      if (BlackKingMoved==false && WhiteChecked==false){
       PetitRoqueNoir();
       GrandRoqueNoir();
      }
    break;
  }
}


//-------fonctions affichages
/*
void test_piece(){      //for display,debug only
  for(int piece=bishop;piece<=bishop;piece++){  //enumère les pieces
    for (int i=0; i<TAILLE; i++){
      for (int j=0; j<TAILLE; j++){
       Resetcoup_possibles();
       ShowPieceMoves(i,j,{piece,Blanc},true);
       affiche_led();
       delay(1000);
      }
    }
    if (piece==pawn){    //affiche pion Noir
      for (int i=0; i<TAILLE; i++){
      for (int j=0; j<TAILLE; j++){
       Resetcoup_possibles();
       ShowPieceMoves(i,j,pawnN,true);
       affiche_led();
       delay(1000);
      }}
      }
  }
 }
 

void test_echiquier(){  //for debug only
  for (int i=0; i<TAILLE; i++){
      for (int j=0; j<TAILLE; j++){
        Resetcoup_possibles();
        //RefreshBlackControl();//debug only
        //afficheTableau(BlackControl);//debug only
        ShowCaseMoves(i,j);
        affiche_led();
        delay(1000);
      }
   }
}
*/
