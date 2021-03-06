//CHESS_HAL_MOVE_GENERATOR.h

#ifndef CHESS_AI_MOVE_GENERATOR_H

  #define CHESS_AI_MOVE_GENERATOR_H


//string chessversion ="4.24.RoBT.M";
                    // playing manually + automatically  possible
                    // featuring: E.P., Castling, Pawn Promotion
                    // validity test ok! (P+, P-, N, K, B, R, Q)
                    // recognizes: put king in CHECK! (man. + auto)
                    // recognizes: king still in CHECK! => King-Capture !
                    // recognizes: king wants to move to CHECK-threatened sqr
                    // recognizes: discovered check (own + opp. king)
                    // recognizes CHECKMATE
                    // able to move out of CHECK!
                    // auto opening: prefers K.Pawn or Q.Pawn  (d4/e4)

                    // M2 AI auto move generator: 3 complete ply layers

/******************************************************************************/
// (c) H. Wunder 2011
/******************************************************************************/

mutex   tmutex, mmutex, smutex;   //  ToneMutex, MotorMutex, SensorMutex

// (local) char board[129];       // board[sqr-number]= (piece value or (empty=0)


// valid square number: (!= square_number & 0x88), 0<=square_number<=119
// invalid off board i-p file: (square_number & 0x08)
// invalid off board <a/>h rank: (square_number & 0x80)

// white piece:    (board[n] &  8)
// black piece:    (board[n] & 16)
// virgin piece:   (board[n] & 32)
// coloured piece: (board[n] & 15)
// piece type:     (board[n] &  7)     (1,2,3,4,5,6,7)
// sliding piece:  (board[n] &  7) > 4 (5,6,7)
// crawling piece: (board[n] &  7) < 5 (1,2,3,4)
// pawns:          (board[n] &  7) < 3 (1,2)

char pvalue[]={0,2,2,6,-1,7,10,18};// piece value {empty, P+, P-,N, K, B, R, Q}
//piece number:0 1 2 3  4 5  6  7  // Figurenwert: leer,  B+  B- S  K  L  T  D



//string pieces=".?+nkbrq?*?NKBRQ";   // pieces for print; UpperCase=white
  string pieces="-?*skltd?*?SKLTD";   // Figuren-Symbole;  GROSS-Buchstaben=wei�

char dirlst[]={
    16, 15, 17, 0,                    // 0-3:   1=white pawn(up):16=move; 15/17=beat
   -16,-15,-17, 0,                    // 4-7:   2=black pawn(dn):-16;    -15/-17
   14,18,-14,-18,31,33,-31,-33, 0,    // 8-16:  3=knight=Springer
   1,16,15,17,-1,-16,-15,-17,3,-2,0,  // 17-27: 4=king=K�nig
   15,17,-15,-17, 0,                  // 28-32: 5=bishop=L�ufer
   1,16,-1,-16, 0,                    // 33-37: 6=rook=Turm
   1,16,15,17,-1,-16,-15,-17, 0};     // 38-46: 7=queen=Dame


char o[]= {0,  0, 4, 8, 17, 28, 33, 38 }; // o[Piece_type]= first dir.in dirlst[]
           //  1  2  3   4   5   6   7
           //  P+ P- N   K   B   R   Q


#define tWHITE (turn==8)
#define tBLACK (turn==16)

#define INFINITY 999

#define mOffb 0x88;                   // mask 136=0x88: board system
#define mRank 0x70;                   // mask 112=0x70: rank mask



#define scBlackP 104                  // score of piece values
#define scBlackS 105                  // score for strategy (positioning)
#define scBlackC 106                  // score for check

#define scWhiteP 108                  // score of piece values
#define scWhiteS 109                  // score for strategy (positioning)
#define scWhiteC 110                  // score for check

#define RookNew  124                  // Rook pos after Castling
#define RookEdge 125                  // Rook pos original before castling

#define EPturn   126                  // turn which makes E.P. move possible
#define EPpawn   127                  // pos of E.P. pawn to be beaten
#define EPdest   128                  // pos of beating pawn after E.P. beat


char AUTOMV_RDY=false;

char fontWi=7, fontHi=8,              // font sizes
     loc[]={0,8,16,24,32,40,48,56};   // lines (files) starting at bottom line
     

/******************************************************************************/
// Basics
/******************************************************************************/

char key;
char CursPos=120;
/******************************************************************************/
string __sval1__;
#define printf1( _x, _y, _format1, _value1) { \
  __sval1__ = FormatNum(_format1, _value1); \
  TextOut(_x, _y, __sval1__); \
}

/******************************************************************************/
#define min(a,b) (a<b?a:b)
/******************************************************************************/
#define max(a,b) (a>b?a:b)

/******************************************************************************/
inline long round(float f)
{ return (f<0?f-0.5:f+0.5); }

/******************************************************************************/
inline char inArea(long x, long v, long thr) {
  return ((x>=v-thr)&&(x<=v+thr));
}


/******************************************************************************/
#define btn(b) ButtonPressed(b, false)

/******************************************************************************/
inline bool btnhit(){
/******************************************************************************/
   char test;
   test=( ButtonPressed(BTN1, false) || ButtonPressed(BTN2, false)
       || ButtonPressed(BTN3, false) || ButtonPressed(BTN4, false));
   return test;
}


/******************************************************************************/
inline char btnin() {
/******************************************************************************/
  char result = -1;

    if (ButtonPressed(BTNCENTER, false))
      result = BTNCENTER;
    else if (ButtonPressed(BTNEXIT, false))
      result = BTNEXIT;
    else if (ButtonPressed(BTNLEFT, false))
      result = BTNLEFT;
    else if (ButtonPressed(BTNRIGHT, false))
      result = BTNRIGHT;

    return result;
}

/******************************************************************************/
string num2anot(int i) {   // field number-> algebr.notation of chess fields
/******************************************************************************/
  char file, rank;
  string sfile, srank;

   file=97+(i&15);            // generates chess square notation, e.g.
   rank=8-(i>>4);             // lower left (112) = a1, lower right (119) = h1
   sfile=" ";                 // upper left ( 0 ) = a8, upper right ( 7 ) = h8
   sfile[0]=file;
   srank=NumToStr(rank);
   return (sfile+srank);
}

/******************************************************************************/
// Sound: PlayTones
/******************************************************************************/

safecall void beep(int frq,int dur) {
   PlayTone(frq, dur); Wait(dur);
}


safecall void PlayTonesSCall(Tone snd[]) {
  Acquire(tmutex);
  PlayTones (snd);
  Release(tmutex);
}


Tone sndBeep[]      = {TONE_G5,200};
Tone sndBeepBeep[]  = {TONE_C5,200 , 0,100, TONE_C5,200};
Tone sndBoop[]      = {TONE_C4,200};
Tone sndChordUp[]   = {TONE_C4,50, TONE_E4,50, TONE_G4,50,
                        TONE_C5,50, TONE_E5,50, TONE_G5,50, TONE_C6,200};
Tone sndChordDn[]   = {TONE_C6,50, TONE_G5,50, TONE_E5,50,
                        TONE_C5,50, TONE_G4,50, TONE_E4,50,  TONE_C4,200};
Tone sndChord[]     = {TONE_C4,50, TONE_E4,50, TONE_G4,50, TONE_C5,50};
Tone sndBlip[]      = {TONE_C7,10 };
Tone sndBlipBlip[]  = {TONE_C7,10, 0,20, TONE_C7,10 };
Tone sndBuzz[]      = {TONE_C4/2, 200 };
Tone sndError[]     = {TONE_C4,50, 0,50, TONE_C4,50, 0,50, TONE_C4,50};
Tone sndTaDaa[]     = {TONE_G4,100, 0,100, TONE_C5,200};
Tone sndTaDoo[]     = {TONE_G4,100, 0,100, TONE_C4,200};
Tone sndFuneral[]   = {TONE_C4,450, 0,50, TONE_C4,450, 0,50, TONE_C4,450, 0,50,
                        TONE_CS4,400, TONE_C4,150, 0,50, TONE_C4,400, TONE_AS3,150,
                        0,50,TONE_AS3,400, TONE_A3,150, 0,50, TONE_AS3,400 };




/******************************************************************************/
void BoardSetup(char &board[]) {
/******************************************************************************/
  char i, j, t;
  string stat, virg, stat6, stat5, stat4, stat3, stat2, stat1, stat0, virg1, virg0;

  // start positionings for testing
  // verschiedene Start-Aufstellungen zum Testen

  stat = "tsldklst" +       // standard
         "bbbbbbbb" +
         "........" +
         "........" +
         "........" +
         "........" +
         "BBBBBBBB" +
         "TSLDKLST" ;

  stat1= "Ls..klst" +       // B x t => pawn->Queen
         ".l.b.b.b" +
         "....b.b." +
         ".b......" +
         "........" +
         "...DS..." +
         "B.BBBBBB" +
         "T...KLST" ;

  stat2= ".....lst" +       // L x s => CHECK!
         ".DL.kb.b" +
         "........" +
         "s...b..." +
         "......b." +
         "....S..." +
         "B.BBBBBB" +
         "T...KLST" ;

  stat3= "tsl.klst" +       // B x t => pawn->Queen
         ".B.b.b.b" +
         "....b.b." +
         ".b......" +
         "d...L..." +
         "...DS..." +
         "B.BBBBBB" +
         "T...KLST" ;

  stat4= "......s." +       // CHECK!
         "k....l.." +
         "........" +
         "........" +
         "......b." +
         "....S..." +
         ".......K" +
         "......S." ;

  stat5= "....l.s." +       // CHECKMATE!
         "k......." +
         "........" +
         "........" +
         ".D....b." +
         "..T.S..." +
         "........" +
         "....K..." ;

  stat6= "......k." +
         "..b..b.." +
         "...t..b." +
         "........" +
         "...t...." +
         "..D....B" +
         "......BK" +
         "........" ;

  // virgin flags : markieren der unber�hrten Figuren

  virg = "XXXXXXXX" +       // standard flags for standard start pos.
         "XXXXXXXX" +       // Standard-Markierungen f�r Standard-Startpos.
         "........" +
         "........" +
         "........" +
         "........" +
         "XXXXXXXX" +
         "XXXXXXXX" ;

  virg1= ".XXXXXXX" +       // test flags
         ".....XXX" +
         "........" +
         "........" +
         "........" +
         "........" +
         "XXXXXXXX" +
         "XXXXXXXX" ;

  virg0 = "........" +       // test flags
         ".XXX...X." +
         "........" +
         "........" +
         "........" +
         "........" +
         "......X." +
         "........" ;

  j=0;
  for (i=0; i<64; ++i) {
          if (stat[i]==  46) t= 0;      // .
    else  if (stat[i]== 116) t=6+16;    // t
    else  if (stat[i]==  84) t=6+ 8;    // T
    else  if (stat[i]== 115) t=3+16;    // s
    else  if (stat[i]==  83) t=3+ 8;    // S
    else  if (stat[i]== 108) t=5+16;    // l
    else  if (stat[i]==  76) t=5+ 8;    // L
    else  if (stat[i]== 100) t=7+16;    // d
    else  if (stat[i]==  68) t=7+ 8;    // D
    else  if (stat[i]== 107) t=4+16;    // k
    else  if (stat[i]==  75) t=4+ 8;    // K
    else  if (stat[i]==  98) t=2+16;    // b
    else  if (stat[i]==  66) t=1+ 8;    // B
    else t=0;

    if ((virg[i]>65)&&(t>0)) t+=32;    // 16=black, 8=white, 32=virgin bit
    j=(i%8)+16*(i/8);
    board[j++]=t;
  }
}



/******************************************************************************/
void Score (char CK, char RK, char &board[]) {
/******************************************************************************/
// scBlackP 104                 // score of piece values
// scBlackS 105                 // score for strategy (positioning)
// scBlackC 106                 // score for check
// scWhiteP 108
// scWhiteS 109
// scWhiteC 110


  float pwi,pbl,    // sum of piece values
        cwi,cbl,    // sum of check values
        swi,sbl;    // sum of strategy values
  int   n,          // current sqr number index
        b,          // square number
        pv,         // piece value
        piece;      // piece type
  char  WiK, BlK;   // flags for king found


  cwi=cbl=pwi=pbl=swi=sbl=0;
  WiK=BlK=false;

  for (n=0;n<120;++n) {
    b=board[n];                              // square number
    piece=b&7;                               // piece type

    if (!(n&0x88)) {                         // valid square
       pv=pvalue[b&piece];                   // piece value

       if (b&8)  {                           // white
         if (pv<0) WiK=true;                 // King found
             else pwi+=pv;                   // add piece value to score
         if ((b&32) && (piece==4)) swi+=1;   // bonus for virgin K
         if ((b&32) && (piece==6)) swi+=0.6; // bonus for virgin R
         if ((piece==1)&&(n<72))   swi+=0.5; // bonus for pawn ahead
         if ((piece==1)&&(n<23))   swi+=1;   // bonus for pawn at opp.side
         if ((piece==1)&&                    // opening: P in center pos
              ( ( (n==68)&& (!board[67]))||( (n==67)&&(!board[68]) ) )
              &&(board[97]&32)&&(board[98]&32)&&(board[101]&32)&&(board[102]&32))
             swi+=2;
         if ((piece==3)&&(n<112)&&(n>8)) ++swi;    // bonus for knight off 1||8
         if ((pv==1)&&(n<8)) swi+=16;   // pawn->Queen (+extra Queen piece value)
       }

       else
       if (b&16) {                           // black
         if (pv<0) BlK=true;                 // King found
             else pbl+=pv;                   // add piece value to score
         if ((b&32) && (piece==4)) sbl+=1;   // bonus for virgin K
         if ((b&32) && (piece==6)) sbl+=0.6; // bonus for virgin R
         if ((piece==2)&&(n>47))   sbl+=0.5; // bonus for pawn ahead
         if ((piece==2)&&(n>95))   sbl=+1;   // bonus for pawn at opp.side
         if ((piece==2)&&                    // opening: P in center pos
              (( (n==51)&&(!board[52]))||((n==52)&&(!board[51])))
              &&(board[17]&32)&&(board[18]&32)&&(board[21]&32)&&(board[22]&32))
             sbl+=2;
         if ((piece==3)&&(n<112)&&(n>8)) ++sbl;    // bonus for knight off 1||8
         if ((pv==1)&&(n>111)) sbl+=16; // pawn->Queen (+extra Queen piece value)
       }
    }
  }


  if((CK & 8)) {cwi=-1;  }
  if((CK &16)) {cbl=-1;  }

  if (!WiK) {pwi=cwi=swi=-120; }  // king capture marker
  if (!BlK) {pbl=cbl=sbl=-120; }

  board[104]=pbl; board[105]=sbl; board[106]=cbl;
  board[108]=pwi; board[109]=swi; board[110]=cwi;

}


/******************************************************************************/
// GUI tools: print board and squares on display
/******************************************************************************/

/******************************************************************************/
void MarkPos(int K, int i, char board[]){
/******************************************************************************/
  char x, y, rank, color=0, xp, yp, rankp, colorp=0;
  string cs, ps;

   rank=7-(i>>4);
   y=loc[rank];

   x=11+(i&15)*fontWi;
   cs=SubStr(pieces, board[i]&15,1);

   if (i&8) {cs=" auto"; Wait(50); }
   color=!(board[i]&16);                  // color=?   <<<<<<<<<<<<<<<<
   TextOut(x, y, cs, color?4:0);

   if ((K!=-1 )) {
     rankp=7-(K>>4);

     yp=loc[rankp];
     xp=11+(K&15)*fontWi;

     ps=SubStr(pieces, board[K]&15,1);
     colorp=!(board[i]&16);               // color=?   <<<<<<<<<<<<<<<<
     TextOut(xp, yp, ps, colorp?4:0);

   }

   Wait(150);
   TextOut(x, y, cs, color? 0:4);
   if ((K!=-1 )) TextOut(xp,yp,ps,colorp?0:4);
   Wait(100);

}


/******************************************************************************/
void PrintBoard(char K, char L, char turn, char CK, char CM, char RK, char board[]){
/******************************************************************************/
  int i;
  char x, y, rank, color=0;
  string sp;

  for (i=0;i<121;++i) { // a8=119: highest field number, 120=dummy for auto mode
     rank=7-(i>>4);
     y=loc[rank];

     if (i&8) {
       NumOut(0, y, 8-(i>>4)); // (i&8: new rank=> print rank number, skip the next 7 indices
       i+=7;
     }
     else {             // !(i&8): only the first 8 fields of each rank are valid
       x=11+(i&15)*fontWi;
       sp=SubStr(pieces, board[i]&15,1);  // board[i]&15 : piece on field
       color=!(board[i]&16);               // color=?   <<<<<<<<<<<<<<<<
       TextOut(x, y, sp, color?0:4 );
     }
  }

  Score (CK, RK, board);
  printf1(68,48,"B%4d", board[104]+board[105]+board[106]);      // black score
  printf1(68,40,"W%4d", board[108]+board[109]+board[110]);      // white score

  if ((K>=0)&&(L>=0)) {
    TextOut(68,32, SubStr(pieces, board[L]&15,1)+num2anot(K)+num2anot(L));
    printf1(68,16, "K %3d", K);
    printf1(68, 8, "L %3d", L);

  }
  if (CK&turn) TextOut(68,24, "+");
  if (CM&turn) TextOut(68,24, "++");

  if (tWHITE) { TextOut(68, 56, "white");  }
  else         { TextOut(68, 56, "black");  }
}

/******************************************************************************/
void MovePiece(int &K, int &L, char turn, char &EP, char &RK, char &board[]) {
/******************************************************************************/
  char PP=0;                                                // move from K to L

   if ((K<0)||(L<0)) {                  // debug
     PlayTones(sndError);
     TextOut(68,0, "mov-1");
     //getchar();
     return;
   }

   if (board[K]&32) board[K]-=32;       // remove virgin flag
   board[L]=board[K];                   // move K -> L
   board[K]=0;                          // start K=empty

   if ( ((board[L]&7)<3)&& ((L<8)||(L>111))) {
        board[L]=7+turn;                // pawn promotion -> Queen
        PP=true;
   }

   if (EP) {                            // E.P. move done
     board[board[127]]=0;               // delete opponent pawn at E.P. sqr
     EP=0;                              // delete E.P.
   }
   else
   if (board[126]!=turn) {
     board[126]=0;                      // delete markers for possible E.P.
     board[127]=0;
     board[128]=0;
   }

   if (RK) {
     board[board[124]]=board[board[125]]&15;
     board[board[125]]=0;
     board[125]=0;                      // delete markers for castling=Rochade
     board[124]=0;
     RK=0;
   }
}

/*
#define RookNew  124                  // Rook pos after Castling
#define RookEdge 125                  // Rook pos original before castling

#define EPturn   126                  // turn which makes E.P. move possible
#define EPpawn   127                  // pos of E.P. pawn to be beaten
#define EPdest   128                  // pos of beating pawn after E.P. beat
*/

/******************************************************************************/
// AI tools
/******************************************************************************/

/******************************************************************************/
char FindKing(char turn, char board[]) {
/******************************************************************************/
  char n, tPiece;

  tPiece=4; // 4=King
  for (n=0; n<120; ++n){
     if ( ((board[n]&7)==tPiece) && (board[n]&turn) && !(n&0x88)) return n;
  }
}



/******************************************************************************/
char ValidityTest(char K, char L, char turn, char &board[]) {
/******************************************************************************/
  char tPiece, Piece, j, d,
       delta, adelta,
       leap, aleap, fa, dest,
       curr,
       go;
  char SqrKingThrone, SqrRookEdge;                   // sqr for casteling



  if  (board[L]&turn)    return 0;                   // same color
  if  (K & 0x88)    return 0;                        // off-board
  if  (L & 0x88)    return 0;

  tPiece = board[K] & 7;
  Piece  = board[K] & 15;
  delta=K-L;
  d=o[tPiece];

//..............................................................................
  if ((tPiece==1) || (tPiece==2)) {                  // P+, P- / Bauer

     if ((!board[L])&&(delta==dirlst[d])) return 1;  // empty sq.
     else
     if ((board[K]&32) &&                            // if virgin Pawn
        (!board[L])  &&  (!board[K-dirlst[d]]) &&    // 2 empty sqrs,
        ( delta==2*dirlst[d]))  {
            board[128]=K-dirlst[d];                  // mark E.P. sqr.
            board[127]=K-2*dirlst[d];                // mark E.P. Pawn
            board[126]=turn;                         // mark turn color
                                        return 1;    // target ok
        }
     else
     if  (board[L]) {                                // opponent sq.
       if ( ((delta==dirlst[d+1])
         || ( delta==dirlst[d+2]))
         && ((board[K]&turn)!=(board[L]&turn)))   return 1;
     }
     else
     if  ((L==board[128])   ) {                      // E.P. opponent sq.
                                        return 2;
     }

     return 0;
  }


//..............................................................................
  else
  if (tPiece==3)  {                                         // Knight/Springer
     while(dirlst[d]) {
        if (delta==dirlst[d])           return 1;
        else { ++d;  }
     }
     return 0;
  }


//..............................................................................
  else
  if (tPiece==4)  {                                        // King

     if  (tWHITE)  SqrKingThrone=116;
     else  SqrKingThrone=4;

     if ((L==SqrKingThrone-3)&&(board[K]&32)) {
       SqrRookEdge=SqrKingThrone-4;

       if ((!board[K-1])&&(!board[K-2])&&(!board[K-3])     // empty squares
          &&(board[K]&32)&&(board[SqrRookEdge]&32))        // & virgin R+K
       {                                                   // for left castling
            board[125]=SqrRookEdge;                        // left Castlng rook
            board[124]=SqrRookEdge+2;                      // new rook sqr.
                                        return 3;
       }
     }
     else
     if ((L==SqrKingThrone+2)&&(board[K]&32)) {
       SqrRookEdge=SqrKingThrone+3;

       if ((!board[K+1])&&(!board[K+2])                    // empty squares
         &&(board[K]&32)&&(board[L+1]&32))                 // & virgin R+K
       {                                                   // for right castling
            board[125]=SqrRookEdge;                        // right Castlng rook
            board[124]=SqrRookEdge-2;                      // new rook sqr.
                                        return 3;
       }
     }
     else {
       while(dirlst[d]) {
         if (delta==dirlst[d]) {
           if (((dirlst[d]==3)||(dirlst[d]==-2))&& !(board[K]&32))  ++d;
           else                         return 1;
         }
         else { ++d;  }
       }
     }
  }

//..............................................................................
  else
  if (tPiece>=5)  {                  // Bishop, Rook, Queen (sliding pieces)

     d=o[tPiece];                    // array number for 1st direction for piece
     j=1;
     leap=dirlst[d];
     while(leap) {
       leap=dirlst[d];

       for (j=1; j<8; ++j) {                         // loop over sliding sqrs
         curr=K-j*leap;                              // current sqr number

         if  ((curr&0x88)||(curr<0)||(curr>119))
           { j=8; break; }                           // invalid sqr => break

         else
         if ( board[curr]&turn ) break;              // own piece/sqr => break

         else
         if ( (delta==j*leap) )            return 1; // L found=>ready
         if (board[curr]) { j=8; break; }            // opp.piece => stop inc

       } // for j

       ++d; // next direction in direction list

     } // while dirlst[d]

  }  // if tPiece>=5

}



/******************************************************************************/
// AI: HAL auto move unit
/******************************************************************************/

/******************************************************************************/
char FieldInCheck(char turn, char L, char board[]) {
/******************************************************************************/
  char n, tPiece, OtherTurn, test;

  OtherTurn=24-(turn);

  for(n=0; n<120; ++n) {
    if ((n&0x88)&&(n<104)) n+=8;               // skip off-board squares
    tPiece = board[n]&7;

    if ((board[n] & OtherTurn) && !(n&0x88) )  {
       test=ValidityTest(n, L, OtherTurn, board);
       if(test) return 1;
    }
  }
  return 0;
}

/******************************************************************************/
// deepest ply layer
/******************************************************************************/
/******************************************************************************/
int AutoMove9(int &K, int &L, char turn,     // from K to L, side on turn
              char &EP, char &RK,             // EnPassant, Castling,
              char &CK, char &CM,             // Check!
              char &board[]) {                // board[129], singleEvaluation
/******************************************************************************/
  char pc, pcmax,                             // piece counter
       n,                                     // from-square-number(loop counter)
       dest,                                  // dest-sqr-number n->dest
       tPiece,                                // piece type
       d,                                     // dirlst[d]=direction
       j,                                     // sliding move counter (max leaps)
       bestK, bestL,                          // best move bestL->bestL
       tK, tL,                                // test dummies for K, L
       uK, uL,                                // dummies for next ply uK->LK
       found, ok, go,                         // boolean flags
       tEP, tRK, tCK,                         // dummies for test EP, RK, CK
       uEP, uRK, uCK, uCM,                    // future values for EP, RK, CK, CM
       bEP, bRK, bCK,                         // best values for EP, RK, CK
       delta, leap,                           // delta=K-L, leap=dir in dirlist
       OtherTurn,                             // turn/OtherTurn = whi/blk=8/16
       tThrone, oThrone, buf;                 // king's pos., dummy

  int  tscore,  bscore, uscore,               // score=scoreWhite-scoreBlack
       tvalid, bvalid, uvalid,                // move validity  (test, best)
       scWi, scBl;                            // single WhiteScore, BlackScore

  char tboard[129], uboard[129];              // testboard, dummyboard


  OtherTurn=24-(turn);

  bCK=bestK=bestL=-1;
  dest=pc=found=0;
  Score(CK, RK, board);
  scBl=board[104]+board[105]+board[106];
  scWi=board[108]+board[109]+board[110];

  tscore=bscore=-INFINITY;
  bvalid=1;

//..............................................................................


  pcmax=16;

  for (n=0; ((n<120) && (pc<=pcmax)); ++n) {   // loop over square_number && (pc<=16)

    if ((n&0x88)&&(n<104)) n+=8;               // skip off-board squares

    tPiece = board[n] & 7;

    if ( (board[n]&turn))  {                   // own piece

       ++pc;
       d=o[tPiece];                            // rook=6, d=o[6]=8, dirlst[8]=1

       found=false;                            // no best move found yet

       while (dirlst[d]) {                     // loop over directions

         dest = n - dirlst[d];                           // L = K-leap

         if ((dest&0x88)||(n&0x88)||(n<0)||(n>119) ) {goto WHILE_INC_d9;}
                                                         // dest == off-board
         else {

//..............................................................................
           j=1;                                // search biggest leap sizes
//..............................................................................

           if (tPiece>=5) {   // Bishop,Rook,Queen (L�ufer, Turm, Dame)
              ok=true;                                   // sliding peaces
              go=true;
              j=0;
              while ((j<7)&& ok && go)                   // inc j as possible
              {
                 if (go) ++j;
                 else break;

                 ok=false; go=false;                     // init OK-flag
                 leap=j*dirlst[d];                       // new virt. step size
                 dest= n-leap;                           // new virtual dest

                 if ( (dest&0x88)||(dest<0)||(dest>119)||(board[dest]&turn) ){
                     ok=false; go=false;                 // max. dest invalid:
                     if (j>1) --j;
                 }
                 else {                                    // dest on-board
                   if (!(board[dest])) {ok=true; go=true;} // dest sqr empty
                   else
                   if (!(board[dest])&turn ) {ok=true; go=false;} // dest=opponent
                   else {ok=false; go=false; break;}
                 }
              }                                      // if empty j++,
                                                     // if opponent: break (finished)

              leap=j*dirlst[d];                      // new virt. step size
              dest= n-leap;                          // new virtual dest

           }
//..............................................................................

           else
           if (tPiece<3) {    // (1,2)= pawns=Bauern P+, P-
             if (!(board[n]&32)) j=1;                    // normally 1 small step
             else
             if ((board[n]&32)) {            // >>but if virgin pawn
                                                         //  in case:
                if (((board[n]& 8)&& !(board[n-16])&& !(board[n-32]) ) // white P
                ||  ((board[n]&16)&& !(board[n+16])&& !(board[n+32]))) // black P
                  j=2;                                   // 2x straight at start
             }
           }
//..............................................................................
           else  j=1;         // 3=knight=Springer, 4=king=K�nig
                                             // only 1 small step
//..............................................................................
//..............................................................................


//..............................................................................

           while (j>=1)  {                             // loop over sliding sqrs
                                                       // start with max j>=1
             oThrone=FindKing(OtherTurn, board);       // find opponent king

             ArrayBuild(tboard, board);                // copy  origin board
                                                       // for valid+score test
             leap=j*dirlst[d];                         // max possible leap
             dest= n-leap;                             // max distant sqr number

             tK=n;   tL=dest;                            // dummies for move K->L

//..............................................................................
//..............................................................................
             // Quiescence Search
             tvalid=true;
 /*
             if (((board[tK]&7)<5)&&(board[tK]!=3))     // Quick Validity Check
               tvalid=ValidityTest(tK, tL, turn, tboard); // test validity K->L
*/
//..............................................................................
//..............................................................................

             if (!tvalid) {       // test move not valid
               if (j==1)  goto WHILE_INC_d9;            // smallest step invalid
                                                       // => next dir in dirlist
               else goto WHILE_DEC_j9;                  // bigger step invalid
             }                                         // => decrease step size


//..............................................................................
             TextOut(68, 8, SubStr(pieces,board[tK]&15,1));    // piece symbol
             TextOut(74, 8, num2anot(tK)+num2anot(tL));

             if (dest==oThrone) {                      // dest=opponent King   // <<<<<<<<<<<<<<<<<<<<
                  CM=OtherTurn;
                  beep(TONE_C3,200);
                  bvalid=0;
                  goto CHECKMATE9;                        // KING CAPTURE!
             }

//..............................................................................
             if ((dest>=0)&&(dest<120) && (!(tboard[dest]&turn)) ) {
               tK=n;   tL=dest;

               MovePiece(tK, tL, turn, tEP, tRK, tboard);  // virtual test move
//..............................................................................
               // recognizes: forbidden discovered check of own king by own move

               tThrone=FindKing(turn, tboard);
               if (FieldInCheck(turn, tThrone, tboard)) { // forbidden move
                 beep(TONE_C6,10);
                 if (j==1)  goto WHILE_INC_d9;
                 else      goto WHILE_DEC_j9;
               }

//..............................................................................

               ArrayBuild(uboard, tboard);    // copy board for test search

//..............................................................................
            // recognizes: check/discovered check of opp. king by own piece
                                              // opponent king put in CHECK! ?
               buf=FieldInCheck(OtherTurn, oThrone, uboard);
               if (buf)  tCK=OtherTurn;
               else tCK=0;

//..............................................................................

               Score(tCK, tRK, tboard);                // of current ply
               scWi=tboard[108]+tboard[109]+tboard[110]; // white score
               scBl=tboard[104]+tboard[105]+tboard[106]; // black score

               if (tWHITE) tscore=scWi-scBl;              // board score
               else  tscore=scBl-scWi;

//..............................................................................
//..............................................................................
               // no further deepening //
//..............................................................................
//..............................................................................
               if (  (tscore>=bscore))    {

                 if (!(found) || (tscore>bscore)  )
                 {
                   bscore=tscore;                        // store best score
                   bestK=n;                              // store best K
                   bestL=dest;                           // store best L
                   bCK=tCK;                              // store CHECK flag
                 }                                       // 1 better move found
                 found=Random(2);                        // random move
               }

//..............................................................................
             } //
          WHILE_DEC_j9:
             if (j>0) --j;                               // decrease step size j

           }
        WHILE_INC_d9:
           if (dirlst[d]) ++d ;                          // next step direction

         }  // not off-board
       }  // while dirlst>0
     }  // own piece

   }  // for...loop over square_number

   if ((bestL<0)||(bCK)<0) {
     CM=OtherTurn;
     PlayTones(sndBeepBeep);
     bvalid=INFINITY;
     TextOut(68, 0, "CM  9");
     //getchar();
     goto CHECKMATE9;
   }

   K=bestK;  L=bestL;   CK=bCK;                            // reload best move

   if ((board[K]&7)<5) bvalid=ValidityTest(K,L,turn,board);// check for flags


CHECKMATE9:


   return bvalid;                                    // validity code
                                    // 0= invalid; 1=valid; 2=E.P.; 3= Castling
}

/******************************************************************************/
// 2nd ply layer
/******************************************************************************/
/******************************************************************************/
int AutoMove2(int &K, int &L, char turn,     // from K to L, side on turn
              char &EP, char &RK,             // EnPassant, Castling,
              char &CK, char &CM,             // Check!
              char &board[]) {                // board[129], singleEvaluation
/******************************************************************************/
  char pc, pcmax,                             // piece counter
       n,                                     // from-square-number(loop counter)
       dest,                                  // dest-sqr-number n->dest
       tPiece,                                // piece type
       d,                                     // dirlst[d]=direction
       j,                                     // sliding move counter (max leaps)
       bestK, bestL,                          // best move bestL->bestL
       tK, tL,                                // test dummies for K, L
       uK, uL,                                // dummies for next ply uK->LK
       found, ok, go,                         // boolean flags
       tEP, tRK, tCK,                         // dummies for test EP, RK, CK
       uEP, uRK, uCK, uCM,                    // future values for EP, RK, CK, CM
       bEP, bRK, bCK,                         // best values for EP, RK, CK
       delta, leap,                           // delta=K-L, leap=dir in dirlist
       OtherTurn,                             // turn/OtherTurn = whi/blk=8/16
       tThrone, oThrone, buf;                 // king's pos., dummy

  int  tscore,  bscore, uscore,               // score=scoreWhite-scoreBlack
       tvalid, bvalid, uvalid,                // move validity  (test, best)
       scWi, scBl;                            // single WhiteScore, BlackScore

  char tboard[129], uboard[129];              // testboard, dummyboard


  OtherTurn=24-(turn);

  bCK=bestK=bestL=-1;
  dest=pc=found=0;
  Score(CK, RK, board);
  scBl=board[104]+board[105]+board[106];
  scWi=board[108]+board[109]+board[110];

  tscore=bscore=-INFINITY;
  bvalid=1;

//..............................................................................


  pcmax=16;


  for (n=0; ((n<120) && (pc<=pcmax)); ++n) {   // loop over square_number && (pc<=16)

    if ((n&0x88)&&(n<104)) n+=8;               // skip off-board squares

    tPiece = board[n] & 7;

    if ( (board[n]&turn))  {                   // own piece

       ++pc;
       d=o[tPiece];                            // rook=6, d=o[6]=8, dirlst[8]=1

       found=false;                            // no best move found yet

       while (dirlst[d]) {                     // loop over directions

         dest = n - dirlst[d];                           // L = K-leap

         if ((dest&0x88)||(n&0x88)||(n<0)||(n>119) ) {goto WHILE_INC_d2;}
                                                         // dest == off-board
         else {

//..............................................................................
           j=1;                                // search biggest leap sizes
//..............................................................................

           if (tPiece>=5) {   // Bishop,Rook,Queen (L�ufer, Turm, Dame)
              ok=true;                                   // sliding peaces
              go=true;
              j=0;
              while ((j<7)&& ok && go)                   // inc j as possible
              {
                 if (go) ++j;
                 else break;

                 ok=false; go=false;                     // init OK-flag
                 leap=j*dirlst[d];                       // new virt. step size
                 dest= n-leap;                           // new virtual dest

                 if ( (dest&0x88)||(dest<0)||(dest>119)||(board[dest]&turn) ){
                     ok=false; go=false;                 // max. dest invalid:
                     if (j>1) --j;
                 }
                 else {                                    // dest on-board
                   if (!(board[dest])) {ok=true; go=true;} // dest sqr empty
                   else
                   if (!(board[dest])&turn ) {ok=true; go=false;} // dest=opponent
                   else {ok=false; go=false; break;}
                 }
              }                                      // if empty j++,
                                                     // if opponent: break (finished)

              leap=j*dirlst[d];                      // new virt. step size
              dest= n-leap;                          // new virtual dest

           }
//..............................................................................

           else
           if (tPiece<3) {    // (1,2)= pawns=Bauern P+, P-
             if (!(board[n]&32)) j=1;                    // normally 1 small step
             else
             if ((board[n]&32)) {            // >>but if virgin pawn
                                                         //  in case:
                if (((board[n]& 8)&& !(board[n-16])&& !(board[n-32]) ) // white P
                ||  ((board[n]&16)&& !(board[n+16])&& !(board[n+32]))) // black P
                  j=2;                                   // 2x straight at start
             }
           }
//..............................................................................
           else  j=1;         // 3=knight=Springer, 4=king=K�nig
                                             // only 1 small step
//..............................................................................
//..............................................................................

           while (j>=1)  {                             // loop over sliding sqrs
                                                       // start with max j>=1
             oThrone=FindKing(OtherTurn, board);       // find opponent king

             ArrayBuild(tboard, board);                // copy  origin board
                                                       // for valid+score test
             leap=j*dirlst[d];                         // max possible leap
             dest= n-leap;                             // max distant sqr number

             tK=n;   tL=dest;                            // dummies for move K->L

//..............................................................................
//..............................................................................
             tvalid=true;
             if (((board[tK]&7)<5)&&(board[tK]!=3))     // Quick Validity Check

               tvalid=ValidityTest(tK, tL, turn, tboard); // test validity K->L

//..............................................................................
//..............................................................................

             if (!tvalid) {       // test move not valid
               if (j==1)  goto WHILE_INC_d2;            // smallest step invalid
                                                       // => next dir in dirlist
               else goto WHILE_DEC_j2;                  // bigger step invalid
             }                                         // => decrease step size

//..............................................................................
             TextOut(68,16, SubStr(pieces,board[tK]&15,1));    // piece symbol
             TextOut(74,16, num2anot(tK)+num2anot(tL));

             if (dest==oThrone) {                      // dest=opponent King   // <<<<<<<<<<<<<<<<<<<<
                  CM=OtherTurn;
                  beep(TONE_C3,200);
                  bvalid=0;
                  goto CHECKMATE2;                        // KING CAPTURE!
             }

//..............................................................................
             if ((dest>=0)&&(dest<120) && (!(tboard[dest]&turn)) ) {
               tK=n;   tL=dest;

               MovePiece(tK, tL, turn, tEP, tRK, tboard);  // virtual test move
//..............................................................................
               // recognizes: discovered check of own king by own piece

               tThrone=FindKing(turn, tboard);
               if (FieldInCheck(turn, tThrone, tboard)) { // forbidden move
                 beep(TONE_C6,10);
                 if (j==1)  goto WHILE_INC_d2;
                 else      goto WHILE_DEC_j2;
               }

//..............................................................................

               ArrayBuild(uboard, tboard);    // copy board for test search

//..............................................................................
           // recognizes: check/discovered check of opp. king by own piece
                                              // opponent king put in CHECK! ??
               buf=FieldInCheck(OtherTurn, oThrone, uboard);
               if (buf)  tCK=OtherTurn;
               else tCK=0;

//..............................................................................
               Score(tCK, tRK, tboard);                // of current ply
               scWi=tboard[108]+tboard[109]+tboard[110]; // white score
               scBl=tboard[104]+tboard[105]+tboard[106]; // black score

               if (tWHITE) tscore=scWi-scBl;              // board score
               else  tscore=scBl-scWi;

//..............................................................................
//..............................................................................
                                                                // 3rd ply
               uK=uL=-1;  uEP=uRK=uCK=uCM=0;
               // ArrayBuild(uboard, tboard);
  /*             uvalid=AutoMove9(uK, uL, OtherTurn, uEP, uRK, uCK, uCM, uboard);

               if (uvalid) {
                 if (uvalid==INFINITY) {tscore=uvalid;}
                 else {
                   MovePiece(uK, uL, OtherTurn, uEP, uRK, uboard);   // calculate:
                   Score(uCK, uRK, uboard);              // score of future ply
                   scWi=uboard[108]+uboard[109]+uboard[110]; // white score
                   scBl=uboard[104]+uboard[105]+uboard[106]; // black score
                   if(tCK& 8) scWi-=3;                 // + current check! score
                   if(tCK&16) scBl-=3;                 // of current ply

                   if (turn==8) uscore=scWi-scBl;            // board score
                   else  uscore=scBl-scWi;
                   tscore=uscore;
                 }

               }
  */
//..............................................................................
//..............................................................................
               if (  (tscore>=bscore))    {

                 if (!(found) || (tscore>bscore)  )
                 {
                   bscore=tscore;                        // store best score
                   bestK=n;                              // store best K
                   bestL=dest;                           // store best L
                   bCK=tCK;                              // store CHECK flag
                 }                                       // 1 better move found
                 found=Random(2);                        // random move
               }

               printf1(68,40,">%4d", bscore);
               TextOut(68,08, " "+num2anot(bestK)+num2anot(bestL)); // best K, L
//..............................................................................
             } //
          WHILE_DEC_j2:
             if (j>0) --j;                               // decrease step size j

           }
        WHILE_INC_d2:
           if (dirlst[d]) ++d ;                          // next step direction

         }  // not off-board
       }  // while dirlst>0
     }  // own piece

   }  // for...loop over square_number

   if ((bestL<0)||(bCK)<0) {
     CM=OtherTurn;
     PlayTones(sndBeepBeep);
     bvalid=INFINITY;
     TextOut(68, 0, "CM 2 ");
     //getchar();
     goto CHECKMATE2;
   }

   K=bestK;  L=bestL;   CK=bCK;  bvalid=1;                 // reload best move

   if ((board[K]&7)<5) bvalid=ValidityTest(K,L,turn,board);// check for flags


CHECKMATE2:


   return bvalid;                                    // validity code
                                    // 0= invalid; 1=valid; 2=E.P.; 3= Castling
}


/******************************************************************************/
// ply layer 1
/******************************************************************************/
safecall int  AutoMove(int &K, int &L, char turn,  // from K to L, side on turn
              char &EP, char &RK,             // EnPassant, Castling,
              char &CK, char &CM,             // Check!
              char &board[], char pmod) {   // board[129], singleEvaluation
/******************************************************************************/
  char pc, pcmax,                             // piece counter
       n,                                     // from-square-number(loop counter)
       dest,                                  // dest-sqr-number n->dest
       tPiece,                                // piece type
       d,                                     // dirlst[d]=direction
       j,                                     // sliding move counter (max leaps)
       bestK, bestL,                          // best move bestL->bestL
       tK, tL,                                // test dummies for K, L
       uK, uL,                                // dummies for next ply uK->LK
       rK, rL,                                // remote move
       found, ok, go,                         // boolean flags
       tEP, tRK, tCK,                         // dummies for test EP, RK, CK
       uEP, uRK, uCK, uCM,                    // future values for EP, RK, CK, CM
       bEP, bRK, bCK,                         // best values for EP, RK, CK
       rEP, rRK, rCK,                         // remote values for EP, RK, CK
       delta, leap,                           // delta=K-L, leap=dir in dirlist
       OtherTurn,                             // turn/OtherTurn = whi/blk=8/16
       tThrone, oThrone, buf;                 // king's pos., dummy

  int  tscore,  bscore, uscore, rscore,       // score=scoreWhite-scoreBlack
       tvalid, bvalid, uvalid,                // move validity  (test, best)
       scWi, scBl;                            // single WhiteScore, BlackScore

  char tboard[129], uboard[129];              // testboard, dummyboard

  printf1(68,48, "%s", "$....");
  printf1(68,40, "%s", ">....");
  printf1(68,32, "%s", "     ");
  printf1(68,24, "%s", "     ");
  printf1(68,16, "%s", "     ");
  printf1(68, 8, "%s", "     ");
  printf1(68, 0, "%s", "     ");


  OtherTurn=24-(turn);

  bCK=bestK=bestL=-1;
  dest=pc=found=0;
  Score(CK, RK, board);
  scBl=board[104]+board[105]+board[106];
  scWi=board[108]+board[109]+board[110];

  tscore=bscore=-INFINITY;
  bvalid=1;

//..............................................................................


  pcmax=16;
  found=false;                            // no best move found yet
  AUTOMV_RDY=0;
  
  for (n=0; ((n<120) && (pc<=pcmax)); ++n) {   // loop over square_number && (pc<=16)
    if ((n&0x88)&&(n<104)) n+=8;               // skip off-board squares
    if(n%3!=pmod) continue;

    tPiece = board[n] & 7;


    if ( (board[n]&turn))  {                   // own piece

       ++pc;
       d=o[tPiece];                            // rook=6, d=o[6]=8, dirlst[8]=1

       while (dirlst[d]) {                     // loop over directions
         dest = n - dirlst[d];                           // L = K-leap

         if ((dest&0x88)||(n&0x88)||(n<0)||(n>119) )
             {goto WHILE_INC_d;}                         // dest == off-board

         else {

//..............................................................................
           j=1;                                // search biggest leap sizes
//..............................................................................

           if (tPiece>=5) {   // Bishop,Rook,Queen (L�ufer, Turm, Dame)
              ok=true;                                   // sliding peaces
              go=true;
              j=0;
              while ((j<7)&& ok && go)                   // inc j as possible
              {
                 if (go) ++j;
                 else break;

                 ok=false; go=false;                     // init OK-flag
                 leap=j*dirlst[d];                       // new virt. step size
                 dest= n-leap;                           // new virtual dest

                 if ( (dest&0x88)||(dest<0)||(dest>119)||(board[dest]&turn) ){
                     ok=false; go=false;                 // max. dest invalid:
                     if (j>1) --j;
                 }
                 else {                                    // dest on-board
                   if (!(board[dest])) {ok=true; go=true;} // dest sqr empty
                   else
                   if (!(board[dest])&turn ) {ok=true; go=false;} // dest=opponent
                   else {ok=false; go=false; break;}
                 }
              }                                      // if empty j++,
                                                     // if opponent: break (finished)

              leap=j*dirlst[d];                      // new virt. step size
              dest= n-leap;                          // new virtual dest

           }
//..............................................................................

           else
           if (tPiece<3) {    // (1,2)= pawns=Bauern P+, P-
             if (!(board[n]&32)) j=1;                    // normally 1 small step
             else
             if ((board[n]&32)) {            // >>but if virgin pawn
                                                         //  in case:
                if (((board[n]& 8)&& !(board[n-16])&& !(board[n-32]) ) // white P
                ||  ((board[n]&16)&& !(board[n+16])&& !(board[n+32]))) // black P
                  j=2;                                   // 2x straight at start
             }
           }
//..............................................................................
           else  j=1;         // 3=knight=Springer, 4=king=K�nig
                                             // only 1 small step
//..............................................................................
//..............................................................................

           while (j>=1)  {                             // loop over sliding sqrs
                                                       // start with max j>=1
             oThrone=FindKing(OtherTurn, board);       // find opponent king

             ArrayBuild(tboard, board);                // copy  origin board
                                                       // for valid+score test
             leap=j*dirlst[d];                         // max possible leap
             dest= n-leap;                             // max distant sqr number

             tK=n;   tL=dest;                          // dummies for move K->L

//..............................................................................
//..............................................................................
             tvalid=true;
             if (((board[tK]&7)<5)&&(board[tK]!=3))     // Quick Validity Check
                tvalid=ValidityTest(tK, tL, turn, tboard); // test validity K->L

             TextOut(68,32, SubStr(pieces,board[tK]&15,1));    // piece symbol
             TextOut(74,32, num2anot(tK)+num2anot(tL));

//..............................................................................
//..............................................................................

             if (!tvalid) {       // test move not valid
               if (j==1)  goto WHILE_INC_d;            // smallest step invalid
                                                       // => next dir in dirlist
               else goto WHILE_DEC_j;                  // bigger step invalid
             }                                         // => decrease step size

//..............................................................................

             if (dest==oThrone)                {       // dest=opponent King
                  CM=OtherTurn;
                  beep(TONE_A4,200);
                  bvalid=0;
                  goto CHECKMATE;                        // KING CAPTURE!
             }

//..............................................................................
             if ((dest>=0)&&(dest<120) && (!(tboard[dest]&turn)) ) {
               tK=n;   tL=dest;

               MovePiece(tK, tL, turn, tEP, tRK, tboard);  // virtual test move
//..............................................................................
            // recognizes: discovered check of own king by own piece

               tThrone=FindKing(turn, tboard);
               if (FieldInCheck(turn, tThrone, tboard)) { // forbidden move
                 beep(TONE_C6,10);
                 if (j==1)  goto WHILE_INC_d;
                 else      goto WHILE_DEC_j;
               }

//..............................................................................

               ArrayBuild(uboard, tboard);          // copy board for deepening
//..............................................................................
            // recognizes: check/discovered check of opp. king by own piece
                                                // opponent king put in CHECK! ?
               buf=FieldInCheck(OtherTurn, oThrone, uboard);
               if (buf)  tCK=OtherTurn;
               else tCK=0;

//..............................................................................
               Score(tCK, tRK, tboard);                // of current ply
               scWi=tboard[108]+tboard[109]+tboard[110]; // white score
               scBl=tboard[104]+tboard[105]+tboard[106]; // black score

               if (tWHITE) tscore=scWi-scBl;              // board score
               else  tscore=scBl-scWi;
//..............................................................................
//..............................................................................
                                                      // 2nd ply
               uK=uL=-1;  uEP=uRK=uCK=uCM=0;
               uvalid=AutoMove2(uK, uL, OtherTurn, uEP, uRK, uCK, uCM, uboard);

               if (uvalid) {
                   if (uvalid==INFINITY) {tscore=uvalid;}
                   else {
                     MovePiece(uK, uL, OtherTurn, uEP, uRK, uboard);   // calculate:
                     Score(uCK, uRK, uboard);              // score of future ply

                     scWi=uboard[108]+uboard[109]+uboard[110]; // white score
                     scBl=uboard[104]+uboard[105]+uboard[106]; // black score
                     if(tCK& 8) scWi-=1;                 // + current check! score
                     if(tCK&16) scBl-=1;                 // of current ply

                     if (tWHITE) uscore=scWi-scBl;            // board score
                     else  uscore=scBl-scWi;                    // Beta Search
                     tscore=uscore;
                   }
               }

//..............................................................................
//..............................................................................
                                                     // search for best move

               if (  (tscore>=bscore))    {             // eq/better best score
                 if ( !found || (tscore>bscore) )      // prefer if better move
                 {
                   bscore=tscore;                       // store best score
                   bestK=n;                             // store best K
                   bestL=dest;                          // store best L
                   bCK=tCK;                             // store CHECK flag
                 }
                 found=Random(2);                       // 1 better move found
               }

             printf1(68,48,"$%4d", bscore);
             TextOut(68,24, " "+num2anot(bestK)+num2anot(bestL)); // best K, L
             //PlayTones(sndBlip);
             printf1(68, 0,"?%4d", tscore);
             //getchar();

             if (bscore==INFINITY) {CM=OtherTurn; goto FINISHED;}
//..............................................................................
             } //
          WHILE_DEC_j:
             if (j>0) --j;                               // decrease step size j

           }
        WHILE_INC_d:
           if (dirlst[d]) ++d ;                          // next step direction

         }  // not off-board
       }  // while dirlst>0
     }  // own piece

   }  // for...loop over square_number

FINISHED:
   if ( (bestL<0) || (bCK<0) ) {
     CM=OtherTurn;
     PlayTones(sndTaDaa);
     TextOut(68,0, "SM ++");
     goto CHECKMATE;
   }

   K=bestK;  L=bestL;   CK=bCK;                            // reload best move

   if ((board[K]&7)<5) bvalid=ValidityTest(K,L,turn,board);// check for flags


CHECKMATE:
   TextOut(68,32, "     ");  //
   TextOut(68,24, "     ");  //
   TextOut(68,16, "     ");  //
   TextOut(68, 8, "     ");  //

   if ((K>=0)&&(L>=0)) {
     TextOut(68,40, SubStr(pieces,board[K]&15,1));  // piece symbol
     TextOut(74,40, num2anot(K)+num2anot(L));       //  best move
   }
   if (CK&OtherTurn) TextOut(68,24, "+");
   if (CM&OtherTurn) {
     TextOut(68,24, "++");
     TextOut(68,16, "CHECK");
     TextOut(68,08, " MATE ");
     PlayTones(sndChord);
   }
   if(board[L]) TextOut(80,24, "X");
   TextOut(68, 0, "press");
   beep(TONE_G5,200);
   //getchar();
   beep(TONE_C6,10);

   AUTOMV_RDY=1;
   return bscore;                                 //

}


/******************************************************************************/
// HID unit:    choose - |auto mode|  - or -  |take piece manually|
/******************************************************************************/

/******************************************************************************/
char GetHIDinput(int &K, int &L, char turn,
                 char &EP, char &RK, char &CK, char &CM, char &board[]){
/******************************************************************************/
    char valid, buf, chosen,
         uK, LK, OtherTurn, tboard[129], uboard[129];;

//..............................................................................
    key=-1;
    chosen=false;
    OtherTurn=24-(turn);

    if  (CursPos==120)  {SetAbortFlag(BTNSTATE_LONG_PRESSED_EV); }
    else                {SetAbortFlag(BTNSTATE_NONE); }

    MarkPos(K, CursPos, board);

    if (btnhit()) {
      beep(TONE_C6,10);
      key=btnin();
    }
//..............................................................................
//..............................................................................

    if (((key==BTNLEFT) || (key==BTNRIGHT))&&(L==-1)) {
      if (key==BTNLEFT) {
        if (CursPos==0)  CursPos=120;
        else
        if (CursPos>0) {
          CursPos--;
          if (CursPos&8)  CursPos-=8;
        }
      }
      else
      if (key==BTNRIGHT){
        if (CursPos>=120) CursPos=0;
        else
        if (CursPos==119) CursPos=120;  // border field for choice: auto move!
        else {
          CursPos++;
          if ((CursPos)&8) CursPos+=8;
        }
      }
      if (CursPos==120) {
        TextOut(68,32,"     ");
        TextOut(68,24,"     ");
        TextOut(68,16,"     ");
        TextOut(68, 8,"     ");
      }
      else if ((K==-1)&&(CursPos!=120)) {
        TextOut(68,32, SubStr(pieces, board[CursPos]&15,1)+num2anot(CursPos)+"  ");
        printf1(68,16, "K %3d", CursPos);
        printf1(68, 0, "p %3d",board[CursPos]);
      }
      else if (K>=0) {
        TextOut(68,32, SubStr(pieces, board[K]&15,1)+num2anot(K)+num2anot(CursPos));
        printf1(68,16, "K %3d", K);
        printf1(68, 8, "L %3d", CursPos);
        printf1(68, 0, "p %3d", board[CursPos]);
      }
    }

//..............................................................................
//..............................................................................

    else
    if (key==BTNCENTER) {

      if ((K==-1 ) && (L==-1 )) {                      // nothing chosen yet
                                                           // choice = start!
        if (CursPos==120) {                                // auto play
           beep(TONE_A3,200);
           chosen=true;
        }
        else                                               // try to take piece
        {
           if (!board[CursPos]) beep(TONE_G4,200);        // invalid- empty field
           else
           if (!(board[CursPos]&turn)) beep(TONE_G4,200); // invalid- not his turn

          else {                                          // K valid chosen
             K=CursPos;
             buf=board[K];
             beep(TONE_G6,10);
             TextOut(68,32, SubStr(pieces, board[K]&15,1)+num2anot(K));
             printf1(68,16, "K %3d", CursPos);
             printf1(68, 0, "p %3d", board[CursPos]);
           }
        }
      }

      else

      if ((K!=-1 ) && (L==-1 )) {                 // start ok, no dest yet

        if (K==CursPos)  beep(TONE_G4,200);     // choice: invalid (start=dest)


//..............................................................................
        else {                                    // choice: destination
          valid=true;
          ArrayBuild(tboard, board);

          MovePiece(K, CursPos, turn, EP, RK, tboard); // virtual test move
          LK=FindKing(turn,tboard);
//..............................................................................
          // recognizes: discovered check of own king by own piece
          if (FieldInCheck(turn, LK, tboard)) valid=false; // forbidden move

          if (valid) valid=ValidityTest(K,CursPos,turn,board);

          if (valid) {
            beep(TONE_A4,200);

//..............................................................................
            if(valid) L=CursPos;
            if(valid==2) {   EP=1;  }                     // flag: En Passant
            if(valid==3) {   RK=1;  }                     // flag: Castling

            printf1(68, 8, "%4d", L);

            if (((board[L]&7)==4)&& !(board[L]&turn)) {   // dest=opponent King
                    CM=24-(turn);
            }

//..............................................................................
            // recognizes:   KING CAPTURE / CHECK MATE!
            LK=FindKing(OtherTurn,board); // find opponent king at sqr(LK)

            if(L==LK) {                   // opp. king already put in CHECK! ?
              CM=OtherTurn;
              return chosen;              // KING CAPTURE! CHECK MATE!
            }
//..............................................................................
            // recognizes: discovered check of opp. king by own piece
             ArrayBuild(uboard, board);    // copy board for test search
             MovePiece(K, L, turn, buf, buf, uboard);
                                           // opponent king put in CHECK! ??
             buf=FieldInCheck(OtherTurn, LK, uboard);   // LK = opponent king
             if (buf) {
               CK=OtherTurn;
             }
             else CK=0;

          }

//..............................................................................
          else PlayTones(sndError);       // invalid validity test
        }
      }
//..............................................................................
      else
      if ((K!=-1)&&(L!=-1)&&(K!=L))  {    // choice: valid, ready to move
        beep(TONE_A3,600);
        chosen=true;
      }
    }

//..............................................................................
//..............................................................................

    else
    if (key==BTNEXIT) {

      if ((K==-1 ) && (L==-1 ) && (CursPos!=120)) {  // Cursor scroll up
        if (!((CursPos-16)&0x88)) CursPos-=16;
        else
        CursPos=CursPos+112;
        TextOut(68,32, SubStr(pieces, board[CursPos]&15,1)+" "+num2anot(CursPos));
        printf1(68,16, "K %3d", CursPos);
        Wait(100);
      }

      if ((K!=-1 ) && (L!=-1 )) {          // choice: undo destination
        beep(TONE_C3,100);
        L=-1;
        TextOut(68, 16, "      ");
      }
      else
      if ((K!=-1 ) && (L==-1 )) {          // choice: undo choose piece
        beep(TONE_C3,100);
        K=-1;
        TextOut(68, 24, "      ");
        TextOut(68,  8, "      ");
        board[K]=buf;                      // in case: reset virgin bit
      }
    }

//..............................................................................

    if(CursPos==120)
      TextOut(68, 0," auto");                     // AI auto mode
    else
      printf1(68, 0, "p %3d", board[CursPos]);    // sqr value = piece code
//..............................................................................


    return chosen;
}



// task main()  {}


#endif
