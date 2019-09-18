// stdio.h

#ifndef __STDIO_H_
   #define __STDIO_H_



/*****************************************************************************/
// Display output

char LCDline[]={56,48,40,32,24,16,8,0};

int _cur_x_=0, _cur_y_=56;      // cursor home for NXC  = upper left = (0,56)
int _tab_width_=24;             // tab width by default = 24 = 4* 6-point letter

unsigned long _TEXTSTYLE_ = DRAW_OPT_NORMAL;   // text style by default


#define scrclr()          {  ClearScreen();  _cur_x_=0; _cur_y_=56; }
#define cls ()            {  ClearScreen();  _cur_x_=0; _cur_y_=56; }

#define curhome ()        { _cur_x_=0; _cur_y_=56; }  // move cursor home
#define settabwidth( t )  { _tab_width_ = t; }        // redefine tab width
#define settextstyle( t ) { _TEXTSTYLE_ = t; }        // redefine text style
//*********************************************

inline string strsplit(string &src, string mark) {
  string _sret="";
  int p=-1, l;
  p=Pos(mark, src);
  if (p>=0) {
    l=strlen(mark);
    _sret= SubStr(src, 0, p);
    src=SubStr(src, p+l, strlen(src)-p);
  }
  return _sret;
}

//*********************************************

string strexch(string src, string ex, string ch) {
  string _sst;
  _sst=strsplit(src,ex);
  if (_sst !="") return (StrCat(_sst,ch,src));
  else return("");
}


// printfxy()
// featuring "\i" for writing inverted
//******************************************************************************
#define printfxy( _x_, _y_, _f_, _v_) { \
  _cur_y_=_y_;   string  _s2, _sv;      \
  _s2=_f_;                              \
  if (Pos("\i",_s2)>=0) {               \
    _TEXTSTYLE_= DRAW_OPT_INVERT;       \
    _s2=strexch(_s2,"\i","");           \
  }                                     \
  int len=0;                            \
  if (Pos("%",_s2)==-1)  { _sv=_s2; }   \
  else { _sv = FormatVal(_s2, _v_);  }  \
  TextOut(_x_, _y_, _sv, _TEXTSTYLE_);  \
  len=strlen(_sv);                      \
  _cur_x_=_x_+6*(len);                  \
  _TEXTSTYLE_= DRAW_OPT_NORMAL;         \
}


// printfEx redefined as printf()
// featuring \n, \t, and "\i" for writing inverted
//******************************************************************************
#define printf(_fmt, _val) {             \
  int _x=_cur_x_; int _y=_cur_y_;        \
  string _sf, _s;                        \
  _sf=_fmt;                              \
  while (Pos("\n",_sf)>=0) {             \
    _s=strsplit(_sf,"\n");               \
    while (Pos("\t",_s)>=0) {            \
      _x=(1+_x/_tab_width_)*_tab_width_; \
      _s=strexch(_s, "\t", ""); }        \
    printfxy( _x, _y, _s, _val);         \
    _x=0;  _y-=8;                        \
  }                                      \
  while (Pos("\t",_sf)>=0) {             \
     _x=(1+_x/_tab_width_)*_tab_width_;  \
     _sf=strexch(_sf, "\t", ""); }       \
     if(_x>=96) {_x=0; _y-=8;}           \
     if(_y<0) {scrclr(); _y=56;}         \
  printfxy( _x, _y, _sf, _val);          \
}

//*********************************************


#endif


