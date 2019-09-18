// stdio.h

#ifndef _STDIO_H_
   #define _STDIO_H_
  


/*****************************************************************************/
// Display output

char LCDline[]={56,48,40,32,24,16,8,0};

int _cur_x_=0, _cur_y_=56;      // cursor home for NXC  = upper left = (0,56)
int _tab_width_=24;             // tab width by default = 24 = 4* 6-point letter
int _page_=1;                   // current screen page


unsigned long _TEXTSTYLE_ = DRAW_OPT_NORMAL;   // text style by default

#define cls()             { ClearScreen();  _cur_x_=0; _cur_y_=56; }

#define gotoxy( x, y )    { _cur_x_=x; _cur_y_=y; }   // move cursor to position
#define curxy( x, y )     { _cur_x_=x; _cur_y_=y; }   // move cursor to (alias)
#define curx( x )         { _cur_x_=x; }              // move cursor to x-pos
#define cury( y )         { _cur_y_=y; }              // move cursor to y-pos
#define curhome()         { _cur_x_=0; _cur_y_=56; }  // move cursor home
#define settabwidth( t )  { _tab_width_ = t; }        // redefine tab width
#define settextstyle( t ) { _TEXTSTYLE_ = t; }        // redefine text style

#define endoflastline     ((_cur_x_ >=95)&&(_cur_y_ <=16))


inline string strsplit(string &src, string mark) {
  string s="";
  int p=-1, l;
  p=Pos(mark, src);
  if (p>=0) {
    l=strlen(mark);
    s= SubStr(src, 0, p);
    src=SubStr(src, p+l, strlen(src)-p);
  }
  return s;
}


string strexch(string src, string ex, string ch) {
  string s;
  s=strsplit(src,ex);
  return (StrCat(s,ch,src));
}


void backspace() {
  TextOut(_cur_x_, _cur_y_, " ");
  if (_cur_x_ >=6) _cur_x_-=6;
  else if ((_cur_x_ ==0)&&(_cur_y_ <56)) {_cur_x_=90; _cur_y_ +=8; }
  TextOut(_cur_x_, _cur_y_, " ");
}

// printfxy()
// featuring "\i" for writing inverted
//******************************************************************************
#define printfxy( x_, y_, f_, v_) {       \
  _cur_y_=y_;   string s1_, s2_, sv_;     \
  s2_=f_;                                 \
  if (Pos("\i",s2_)>=0) {                 \
    _TEXTSTYLE_= DRAW_OPT_INVERT;         \
    s2_=strexch(s2_,"\i","");             \
  }                                       \
  int len=0;                              \
  if (Pos("%",s2_)==-1)  { sv_=s2_; }     \
  else { sv_ = FormatVal(s2_, v_);  }     \
  TextOut(x_, y_, sv_, _TEXTSTYLE_);      \
  len=strlen(sv_);                        \
  _cur_x_=x_+6*(len);                     \
  _TEXTSTYLE_= DRAW_OPT_NORMAL;           \
}


// printfEx redefined as printf()
// featuring \n, \t, and "\i" for writing inverted
//******************************************************************************
#define printfEx(_fmt, _val) {            \
  int _x=_cur_x_; int _y=_cur_y_;         \
  string _sf, _s;                         \
  _sf=_fmt;                               \
  while (Pos("\n",_sf)>=0) {              \
    _s=strsplit(_sf,"\n");                \
    while (Pos("\t",_s)>=0) {             \
      _x=(1+_x/_tab_width_)*_tab_width_;  \
      _s=strexch(_s, "\t", ""); }         \
    printfxy( _x, _y, _s, _val);          \
    _x=0;  _y-=8;                         \
  }                                       \
  while (Pos("\t",_sf)>=0) {              \
      _x=(1+_x/_tab_width_)*_tab_width_;  \
      _sf=strexch(_sf, "\t", ""); }       \
  if(_x>=94){_x=0;_y-=8;}                 \
  printfxy( _x, _y, _sf, _val);           \
}


//*********************************************

#define printf1( _x, _y, _format1, _value1) {  \
  string sval1 = FormatVal(_format1, _value1); \
  TextOut(_x, _y, sval1); \
}

#define printf2( _x, _y, _format1, _format2, _value1, _value2) { \
  string sval1 = FormatVal(_format1, _value1); \
  string sval2 = FormatVal(_format2, _value2); \
  string s =sval1+sval2; \
  TextOut(_x, _y, s); \
}

#define printf3( _x, _y, _format1, _format2, _format3, _value1, _value2, _value3) { \
  string sval1 = FormatVal(_format1, _value1); \
  string sval2 = FormatVal(_format2, _value2); \
  string sval3 = FormatVal(_format3, _value3); \
  string s =sval1+sval2+sval3; \
  TextOut(_x, _y, s); \
}

#define printf4( _x, _y, _format1,_format2,_format3,_format4,_value1,_value2,_value3,_value4) { \
  string sval1 = FormatVal(_format1, _value1); \
  string sval2 = FormatVal(_format2, _value2); \
  string sval3 = FormatVal(_format3, _value3); \
  string sval4 = FormatVal(_format4, _value4); \
  string s =sval1+sval2+sval3+sval4; \
  TextOut(_x, _y, s); \
}

#define printf5( _x, _y, _format1,_format2,_format3,_format4,_format5,_value1,_value2,_value3,_value4,_value5) { \
  string sval1 = FormatVal(_format1, _value1); \
  string sval2 = FormatVal(_format2, _value2); \
  string sval3 = FormatVal(_format3, _value3); \
  string sval4 = FormatVal(_format4, _value4); \
  string sval5 = FormatVal(_format5, _value5); \
  string s =sval1+sval2+sval3+sval4+sval5; \
  TextOut(_x, _y, s); \
}

/*****************************************************************************/
// printf_g graphic FontTextOut at ANY (x,y)
// 1 variable:  printf1g
// 2 variables: printf2g
// 3 variables: printf3g


string __gFTFontName;
inline void SetFont(string name) { __gFTFontName = name; }

#define printfsg( _x, _y, _s) { FontTextOut(_x, _y, __gFTFontName, _s);}

#define printfg( _x, _y, _format1, _value1) { \
  string sval1 = FormatVal(_format1, _value1); \
  FontTextOut(_x, _y, __gFTFontName, sval1); \
}


#define printf2g( _x, _y, _format1, _format2, _value1, _value2) { \
  string sval1 = FormatVal(_format1, _value1); \
  string sval2 = FormatVal(_format2, _value2); \
  string s =sval1+sval2; \
  FontTextOut(_x, _y, __gFTFontName, s); \
}


#define printf3g( _x, _y, _format1, _format2, _format3, _value1, _value2, _value3) { \
  string sval1 = FormatVal(_format1, _value1); \
  string sval2 = FormatVal(_format2, _value2); \
  string sval3 = FormatVal(_format3, _value3); \
  string s =sval1+sval2+sval3; \
  FontTextOut(_x, _y, __gFTFontName, s); \
}



/******************************************************************************/
#define btn(b) ButtonPressed(b, false)
/******************************************************************************/

/******************************************************************************/
inline int btnin() { // don't wait until BtnUp - if to wait: use getchar() !
/******************************************************************************/
    int result = -1;
    if (ButtonPressed(BTN1, false))
      result = BTN1;
    else if (ButtonPressed(BTN2, false))
      result = BTN2;
    else if (ButtonPressed(BTN3, false))
      result = BTN3;
    else if (ButtonPressed(BTN4, false))
      result = BTN4;
    return result;
}
/******************************************************************************/
#define keypressed btnhit    // for legacy compatibility
/******************************************************************************/
inline bool btnhit(){
/******************************************************************************/
   return ( ButtonPressed(BTN1, false) || ButtonPressed(BTN2, false)
         || ButtonPressed(BTN3, false) || ButtonPressed(BTN4, false));
}

/*****************************************************************************/
// array viewer
/******************************************************************************/
void PrintArray2(float &x[], float &y[], string caption, int ArrLen) {
/******************************************************************************/

   int i, j, k;
   char ExitBtnSwitchMode=1;    // toggle between ^home and ^end

   float  scale  = 1;
   string FmtStr = "%6.3f ";

   ClearScreen();
   printf1(0,LCDline[0], "%s", caption);
   printf1(0,LCDline[1], "%s", "idx  [x]    [y]");

   i=j=k=0;
   while(j<ArrLen) {
     k=0;
     for (i=0; i<6; ++i) {
       if (j<ArrLen) {
          printf1( 0, LCDline[i+2], "%d   ", j);
          printf1(18, LCDline[i+2], FmtStr, x[j]*scale);
          printf1(60, LCDline[i+2], FmtStr, y[j]*scale);
          ++k;
       }
       ++j;
     }
     if (k<5) {
       while (k<6) {
         printf1(0,LCDline[k+2], "%s", "                    ");
         ++k;
       }
     }

     while  (!btnhit());
     if (ButtonPressed(BTNLEFT, false)) {      // scroll back
       j=j-12;
       if (j<0) j=0;
       if (btnhit()) Wait(300);
     }
     else
     if (ButtonPressed(BTNRIGHT, false)) {     // scroll fwd
       if (j>ArrLen-5) j=ArrLen-5;
       if (j<0) j=0;
       if (j>=ArrLen-5) printf1(0,LCDline[7], "%s", "quit: BtnCenter ");
       if (btnhit()) Wait(300);
     }
     else
     if (ButtonPressed(BTNCENTER, false)) {    // quit
       while  (btnhit());
       if (j<ArrLen-1) j=j-6;
       if (j<0) j=0;
     }
     else
     if (ButtonPressed(BTNEXIT, false)) {      // last <-> first
       if (ExitBtnSwitchMode) {
         j=ArrLen-5;
         if (j<0) j=0;
         printf1(0,LCDline[7], "%s", "quit: BtnCenter ");
       }
       else {
         j=0;
        }
       ExitBtnSwitchMode^=1;
       while (btnhit());
     }

   }
   ClearScreen();
}



/*****************************************************************************/
// Sensors


//*****************************************
//Set a sensor counter value
/*
* Clear a sensor value.
* Clear the value of a sensor - only affects sensors that are configured
* to measure a cumulative quantity such as rotation or a pulse count.
* \param port The port to clear. Use a constant (e.g., S1, S2, S3, or S4) or a variable.
* To set it to a value other than zero you could write a function like this one
* that takes an additional value parameter and pass it into the asm block instead of the 0.
*/

inline void SetSensorValue(byte port) { asm { setin 0, port, ScaledValue } }


//*****************************************
// HT Touch Sensor Multiplexer
// use:
// int  tmux[4];
// SetSensor(S2, SENSOR_TOUCH);
// HTTouchMuxValue(S2, tmux);

inline void HTTouchMuxValue(byte port, int & tArray[]) {
    long switches, value;

    value=1023-SensorRaw(S2);
    switches=339*value;
    switches/=1023-value;
    switches+=5;
    switches/=10;

    tArray[3]=((switches&8)==8);
    tArray[2]=((switches&4)==4);
    tArray[1]=((switches&2)==2);
    tArray[0]=((switches&1)==1);
}


/*****************************************************************************/
// File I/O

void ReadFile() {
  string sFileName="myfile.dat";
  unsigned int nFileSize;
  byte fHandle;
  int IOresult;
  int mydata1, mydata2, mydata3, mydata4; // dummies


  IOresult = OpenFileRead(sFileName, nFileSize, fHandle);
  if (IOresult == LDR_SUCCESS) {

    ReadLn (fHandle, mydata1);
    ReadLn (fHandle, mydata2);
    ReadLn (fHandle, mydata3);
    ReadLn (fHandle, mydata4);  // ...oder Müller ...

    CloseFile(fHandle);
    TextOut(0,0,"FILE READ OK!");

  }
  else   {
    TextOut(0,0,"FILE ERROR ! ");
  }

}


void WriteFile() {
  string sFileName = "myfile.dat";
  unsigned int nFileSize = 256; // estimated file zize
  byte fHandle;
  int IOresult;
  int mydata1, mydata2, mydata3, mydata4;  // dummies

  DeleteFile(sFileName);
  IOresult = CreateFile(sFileName, nFileSize, fHandle);
  if (IOresult == LDR_SUCCESS) {

    WriteLn (fHandle, mydata1);
    WriteLn (fHandle, mydata2);
    WriteLn (fHandle, mydata3);
    WriteLn (fHandle, mydata4);   // ...oder was...

    CloseFile(fHandle);
    TextOut(0,0, "FILE WRITE OK!  ");
  }
  else  {
    TextOut(0,0, "FILE WRITE ERROR!");
  }
}

/*****************************************************************************/


#endif  /* __STDIO_H_ */



