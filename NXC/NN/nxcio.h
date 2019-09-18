// nxcIO.h

#ifndef _NXCIO_H_
#define _NXCIO_H_

inline byte pos(string sPart, string sLong) {
  byte i; byte position=0;
  byte p=strlen(sPart);  byte l=strlen(sLong);
  for(i=0;i<l+1-p;i++) {
    if (SubStr(sLong,i,p)==sPart) {position=i; break;}
  }
  return position;
}

/*****************************************************************************/
// printf at (x,y)
// 1 variable:  printf1
// 2 variables: printf2
// 3 variables: printf3

#define printf1( _x, _y, _format1, _value1) { \
  string sval1 = FormatNum(_format1, _value1); \
  TextOut(_x, _y, sval1); \
}


#define printf2( _x, _y, _format1, _format2, _value1, _value2) { \
  string sval1 = FormatNum(_format1, _value1); \
  string sval2 = FormatNum(_format2, _value2); \
  string s =sval1+sval2; \
  TextOut(_x, _y, s); \
}


#define printf3( _x, _y, _format1, _format2, _format3, _value1, _value2, _value3) { \
  string sval1 = FormatNum(_format1, _value1); \
  string sval2 = FormatNum(_format2, _value2); \
  string sval3 = FormatNum(_format3, _value3); \
  string s =sval1+sval2+sval3; \
  TextOut(_x, _y, s); \
}

/*****************************************************************************/
// printf_g graphic FontTextOut at ANY (x,y)
// string:      printfsg
// 1 variable:  printf1g
// 2 variables: printf2g
// 3 variables: printf3g


string __gFTFontName;

inline void SetFont(string name) { __gFTFontName = name; }

#define printfsg( _x, _y, _s) { FontTextOut(_x, _y, __gFTFontName, _s);}

#define printf1g( _x, _y, _format1, _value1) { \
  string sval1 = FormatNum(_format1, _value1); \
  FontTextOut(_x, _y, __gFTFontName, sval1); \
}


#define printf2g( _x, _y, _format1, _format2, _value1, _value2) { \
  string sval1 = FormatNum(_format1, _value1); \
  string sval2 = FormatNum(_format2, _value2); \
  string s =sval1+sval2; \
  FontTextOut(_x, _y, __gFTFontName, s); \
}


#define printf3g( _x, _y, _format1, _format2, _format3, _value1, _value2, _value3) { \
  string sval1 = FormatNum(_format1, _value1); \
  string sval2 = FormatNum(_format2, _value2); \
  string sval3 = FormatNum(_format3, _value3); \
  string s =sval1+sval2+sval3; \
  FontTextOut(_x, _y, __gFTFontName, s); \
}

/*****************************************************************************/
// Buttons Read

inline bool keypressed(){
   char test;
   test=( ButtonPressed(BTN1, false) || ButtonPressed(BTN2, false)
       || ButtonPressed(BTN3, false) || ButtonPressed(BTN4, false));
   return test;
}


inline int readkey() {
  int result = -1;

    if (ButtonPressed(BTN1, false))
      result = BTN1;
    else if (ButtonPressed(BTN2, false))
      result = BTN2;
    else if (ButtonPressed(BTN3, false))
      result = BTN3;
    else if (ButtonPressed(BTN4, false))
      result = BTN4;

    if (result <> N-1)  while(ButtonPressed(result, false));

    return result;
}


inline int _getchar() {
  int result = -1;
  while (true) {
    if (ButtonPressed(BTN1, false))
      result = BTN1;
    else if (ButtonPressed(BTN2, false))
      result = BTN2;
    else if (ButtonPressed(BTN3, false))
      result = BTN3;
    else if (ButtonPressed(BTN4, false))
      result = BTN4;
    if (result <> -1)
      break;
    else
      Yield();
  }
  while(ButtonPressed(result, false));
  return result;
}

/*****************************************************************************/
// Sound: PlayNotes


struct Note
{
  unsigned int Frequency;
  unsigned int Duration;
};

Note ChordUp[] = {TONE_C4, 50, TONE_E4, 50, TONE_G4, 50,
  TONE_C5, 50, TONE_E5, 50, TONE_G5, 50, TONE_C6, 200};

Note ChordDn[] = {TONE_C6, 50, TONE_G5, 50, TONE_E5, 50,
  TONE_C5, 50, TONE_G4, 50, TONE_E4, 50,  TONE_C4, 200};

Note BeepBeep[] = {TONE_C5, 200 , 0, 100, TONE_C5, 200};

Note Blip[] = {TONE_C7, 50 };

Note BlipBlip[] = {TONE_C7, 50, 0, 10, TONE_C7, 50 };


void PlayTones(Note data[])
{
  for (int i = 0; i < ArrayLen(data); i++) {
    Note tmp = data[i];
    PlayTone(tmp.Frequency, tmp.Duration);
    Wait(tmp.Duration);
  }
}



/*****************************************************************************/


#endif  /* _NXCIO_H_ */


