// stdlib.h

#ifndef _STDLIB_H_
  #define _STDLIB_H_
  
//******************************************************************************
// global constants
//******************************************************************************


//******************************************************************************
// min, max, round, range, type_cast
//******************************************************************************


#define min(a,b) {a<b?a:b;}

//--------------------------------------------

#define max(a,b) {a>b?a:b;}



//--------------------------------------------

inline float FLOAT(long x){return(x);}

//--------------------------------------------

#define INT  LONG

inline long  LONG(float x){return(x);}


//--------------------------------------------


inline long round(float f)
{
  if (f>=0) return (f + 0.5);
  else  return (f - 0.5);
}


//--------------------------------------------


inline char about(float x, float target, float thr) {
  return ( (x >= target-thr) && (x < target+thr) );
}

  

//******************************************************************************
// time
//******************************************************************************
// C-like time functions

#define clock() (CurrentTick()-FirstTick())  // time since program start

#define time(t) (CurrentTick() - t)          // time since timer set to t

inline void starttimer(long &t)   {t=CurrentTick()}    // start timer t

inline void restarttimer(long &t) {t=CurrentTick()-t}  // restart timer t


//--------------------------------------------


string gettimeofday(unsigned long lTime) {
  int s1000, sec, mint, hour, day;
  string sTime;

  s1000=lTime%1000;
  lTime/=1000;
  sec=lTime%60;
  lTime/=60;
  mint=lTime%60;
  lTime/=60;
  hour=lTime%24;
  lTime/=24;

  sTime=StrCat(FormatNum("%d",lTime),"d ",FormatNum("%02d",hour),":",FormatNum("%02d",mint),":",FormatNum("%02d",sec),"\"", FormatNum("%03d",s1000));
  return(sTime);
}


//******************************************************************************
// randomize functions
//******************************************************************************

//-------------------------------------------------------
// Kernighan & Ritchie LCG (linear congruence generator)
//-------------------------------------------------------

// global variables and functions to plant a random seed and to randomize
// how to use:
// sKrand(x)                         // x = val. for repet. sequences
// myVar = Krand() % 100;           // assignes a random number


unsigned long _RAND_SEED_ = 1;      // 1= default value (program start)
unsigned long _OLD_SEED_  = 1;


//--------------------------------------------

unsigned long randK()               // custom random function
{
  _RAND_SEED_ = _RAND_SEED_ * 1103515245 + 12345;
  return (_RAND_SEED_ % (RAND_MAX + 1));
}

//--------------------------------------------


void srandK(unsigned long seed)      // seeds for a new random series
{

  if (seed==0)                      // 0: a "real" randomized random seed
    {seed = abs(CurrentTick()*BatteryLevel());}  // substitute to time(0)

  else
  if (seed==-1) {                   // -1: restore last random series
    seed = _OLD_SEED_;
  }

  _OLD_SEED_  = seed;
  _RAND_SEED_ = seed;               // patch for rand_ function
}


//---------------------------------------------------------
// Mersenne Twister by Makoto Matsumoto & Takuji Nishimura
//---------------------------------------------------------


int N_MTW;
int M_MTW;
unsigned long A_MTW[2];
unsigned long y_MTW[25];
int i_MTW;
char virgin_MTW=true, new_MTW=false;

void InitMTW() {
  N_MTW = 25;
  M_MTW = 7;
  A_MTW[0] = 0;
  A_MTW[1] = 0x8ebfd028;
  i_MTW = N_MTW+1;
  ArrayInit(y_MTW,0,25);
}

//------------------------------------------

unsigned long srandM(unsigned long x) {
  unsigned long r, s;
  int k;
  if (x==1) InitMTW();
  else
  if (x==0) {
    for (k=0; k<25; ++k) y_MTW[k]=Random(CurrentTick()*BatteryLevel());
    new_MTW=true;
  }

  else {
    InitMTW();
    for (k=0 ; k<25 ; ++k) {
      InitMTW();
      r = 9*x;
      s = 3402*x;
      for (k=0 ; k<25 ; ++k) {
           r = 509845221 * r + 3;
           s *= s + 1;
           y_MTW[k] = s + (r >> 10);
      }
      new_MTW=true;
    }
  }
}

//------------------------------------------

unsigned long randM(void) {
  unsigned long r, s, e;
  int k;

  if (virgin_MTW) {virgin_MTW=FALSE; InitMTW(); }

  if (i_MTW >= N_MTW) {
      if ((i_MTW > N_MTW) && !new_MTW) {
         r = 9;
         s = 3402;
         for (k=0 ; k<N_MTW ; ++k) {
           r = 509845221 * r + 3;
           s *= s + 1;
           y_MTW[k] = s + (r >> 10);
         }
      }
      for (k=0 ; k<N_MTW-M_MTW ; ++k)
         y_MTW[k] = y_MTW[k+M_MTW] ^ (y_MTW[k] >> 1) ^ A_MTW[y_MTW[k] & 1];
      for (; k<N_MTW ; ++k)
         y_MTW[k] = y_MTW[k+(M_MTW-N_MTW)] ^ (y_MTW[k] >> 1) ^ A_MTW[y_MTW[k] & 1];
      i_MTW = 0;
  }

  new_MTW=false;
  e = y_MTW[i_MTW++];
  e ^= (e << 7)  & 0x2b5b2500;
  e ^= (e << 15) & 0xdb8b0000;
  e ^= (e >> 16);
  return e;
}



//******************************************************************************
// string + array functions
//******************************************************************************


#define pos Pos

//--------------------------------------------
// Array Resize

#define ArrRze(&_array_[], _temp_[], _newsize_)  {  \
   int _oldlen_ = ArrayLen(_array_);                   \
   if(_newsize_ > _oldlen_) {                          \
     int _diffsize_ = _newsize_ - _oldlen_;            \
     ArrayInit (_temp_, 0, _diffsize_ );               \
     ArrayBuild(_array_, _array_, _temp_);             \
   }                                                   \
   else                                                \
   if(_newsize_ < _oldlen_) {                          \
     ArraySubset(_array_, _array_, 0, _newsize_);      \
   }                                                   \
}

#define ArrayResize(a,b)  ArrRze(a,a,b)

//--------------------------------------------

#define ArrayInit2D(array, tmp, init_val, dimx, dimy) { \
  ArrayInit(tmp, init_val, dimy);  \
  ArrayInit(array, tmp, dimx);  \
}

//--------------------------------------------

inline long ArrayMedianI(int src[], int len)
{
  int temp[];

  ArraySort(temp, src, NA, NA)
  return temp[(len-1)/2];
}

//--------------------------------------------


inline long  MedianI3(int src[])
{
  int temp[];

  ArraySort(temp, src, 0, 3)
  return temp[1];
}

//--------------------------------------------

inline void ArrayPushI(long &src[], long _new, int len)
{
  long tmp[];
  ArraySubset(tmp, src, 1, NA);
  ArrayBuild(src, _new, tmp);
  ArrayInit(tmp, 0, 0);
}


//--------------------------------------------

inline int ArrayPopI(long &src[], int len)
{
  long _zero=src[0];
  for (int i=0; i<len; ++i) {src[i]=src[i+1];} // shift down
  return _zero;
}

//--------------------------------------------

inline void ArrayPushF(float &src[], float _new, int len)
{
  float tmp[];
  ArraySubset(tmp, src, 1, NA);
  ArrayBuild(src, _new, tmp);
  ArrayInit(tmp, 0, 0);
}

//--------------------------------------------

inline float ArrayPopF(float &src[], int len)
{
  float _zero=src[0];
  for (int i=0; i<len; ++i) {src[i]=src[i+1];} // shift down
  return _zero;
}

//--------------------------------------------

int IdxOfArrayMax(float array[]) {
  int maxidx=0, i, len;
  len=ArrayLen(array);
  float fmax;
  fmax=array[0];
  for(i=0; i<len; ++i) {
    if (array[i]>fmax) {fmax=array[i]; maxidx=i;}
  }
  return(maxidx);
}

//--------------------------------------------


int IdxOfArrayMin(float array[]) {
  int minidx=0, i, len;
  len=ArrayLen(array);
  float fmin;
  fmin=array[0];
  for(i=0; i<len; ++i) {
    if (array[i]<fmin) {fmin=array[i]; minidx=i;}
  }
  return(minidx);
}


//--------------------------------------------
// Barcode Code 39

unsigned char c39_bp[44][10] =       // 0=narrow, 1=wide; 3 wide of 9
{
  {'0', 0, 0, 0, 1, 1, 0, 1, 0, 0},
  {'1', 1, 0, 0, 1, 0, 0, 0, 0, 1},
  {'2', 0, 0, 1, 1, 0, 0, 0, 0, 1},
  {'3', 1, 0, 1, 1, 0, 0, 0, 0, 0},
  {'4', 0, 0, 0, 1, 1, 0, 0, 0, 1},
  {'5', 1, 0, 0, 1, 1, 0, 0, 0, 0},
  {'6', 0, 0, 1, 1, 1, 0, 0, 0, 0},
  {'7', 0, 0, 0, 1, 0, 0, 1, 0, 1},
  {'8', 1, 0, 0, 1, 0, 0, 1, 0, 0},
  {'9', 0, 0, 1, 1, 0, 0, 1, 0, 0},
  {'A', 1, 0, 0, 0, 0, 1, 0, 0, 1},
  {'B', 0, 0, 1, 0, 0, 1, 0, 0, 1},
  {'C', 1, 0, 1, 0, 0, 1, 0, 0, 0},
  {'D', 0, 0, 0, 0, 1, 1, 0, 0, 1},
  {'E', 1, 0, 0, 0, 1, 1, 0, 0, 0},
  {'F', 0, 0, 1, 0, 1, 1, 0, 0, 0},
  {'G', 0, 0, 0, 0, 0, 1, 1, 0, 1},
  {'H', 1, 0, 0, 0, 0, 1, 1, 0, 0},
  {'I', 0, 0, 1, 0, 0, 1, 1, 0, 0},
  {'J', 0, 0, 0, 0, 1, 1, 1, 0, 0},
  {'K', 1, 0, 0, 0, 0, 0, 0, 1, 1},
  {'L', 0, 0, 1, 0, 0, 0, 0, 1, 1},
  {'M', 1, 0, 1, 0, 0, 0, 0, 1, 0},
  {'N', 0, 0, 0, 0, 1, 0, 0, 1, 1},
  {'O', 1, 0, 0, 0, 1, 0, 0, 1, 0},
  {'P', 0, 0, 1, 0, 1, 0, 0, 1, 0},
  {'Q', 0, 0, 0, 0, 0, 0, 1, 1, 1},
  {'R', 1, 0, 0, 0, 0, 0, 1, 1, 0},
  {'S', 0, 0, 1, 0, 0, 0, 1, 1, 0},
  {'T', 0, 0, 0, 0, 1, 0, 1, 1, 0},
  {'U', 1, 1, 0, 0, 0, 0, 0, 0, 1},
  {'V', 0, 1, 1, 0, 0, 0, 0, 0, 1},
  {'W', 1, 1, 1, 0, 0, 0, 0, 0, 0},
  {'X', 0, 1, 0, 0, 1, 0, 0, 0, 1},
  {'Y', 1, 1, 0, 0, 1, 0, 0, 0, 0},
  {'Z', 0, 1, 1, 0, 1, 0, 0, 0, 0},
  {'-', 0, 1, 0, 0, 0, 0, 1, 0, 1},
  {'.', 1, 1, 0, 0, 0, 0, 1, 0, 0},
  {' ', 0, 1, 1, 0, 0, 0, 1, 0, 0},
  {'*', 0, 1, 0, 0, 1, 0, 1, 0, 0},
  {'$', 0, 1, 0, 1, 0, 1, 0, 0, 0},
  {'/', 0, 1, 0, 1, 0, 0, 0, 1, 0},
  {'+', 0, 1, 0, 0, 0, 1, 0, 1, 0},
  {'%', 0, 0, 0, 1, 0, 1, 0, 1, 0}
};


//******************************************************************************





///Array indices for hsl array (Hue). \see rgb2hsl()
#define INPUT_HUE    0
///Array indices for hsl array (Saturation). \see rgb2hsl()
#define INPUT_SAT    1
///Array indices for hsl array (Lightness). \see rgb2hsl()
#define INPUT_LIGHT  2

/**
 * \brief Convert RGB to HSL colour space
 *
 * Convert red, green, blue raw values to
 * hue, saturation, lightness values to draw easier distinctions
 * between different colors.<p>
 * hue: 0 - 360             <br>
 *    0   -> red            <br>
 *    60  -> yellow  (for nxt 10-15, problems with orange, yellow) <br>
 *    120 -> green   <br>
 *    180 -> light blue <br>
 *    240 -> blue        <br>
 *    300 -> pink?             <p>
 * saturation: 0% - 100%   (nxt color sensor sat values are usually low)  <br>
 *    0%   -> chromatic (grey) <p>
 * lightness: 0% - 100%        <br>
 *   0%   -> dark   (black)    <br>
 *   100% -> bright (white)  (throw away all measurements that have light values over 75%)
 * <p>
 * More information:
 * http://en.wikipedia.org/wiki/HSL_and_HSV
 * <p>
 * Array indices for hsl:      <br>
 *   #define INPUT_HUE    0    <br>
 *   #define INPUT_SAT    1    <br>
 *   #define INPUT_LIGHT  2    <br>
 *
 * \param rgb[]    An array containing at least three raw color values.
                   See Color sensor array indices (nxc api).
 * \param hsl[]    An initialized array containing at least three elements.
 * \param admax    The maximum of rgb values (255 or 1024)
*/

// (c) Martin0x5E


void rgb2hsl(int &rgb[], int &hsl[], int admax = 1024)
{
   float r = rgb[0] / admax;
   float g = rgb[1] / admax;
   float b = rgb[2] / admax;
   float h = 0;
   float s = 0;
   float l = 0;
   float min_ = ArrayMin(rgb,NA,NA) / admax;
   float max_ = ArrayMax(rgb,NA,NA) / admax;

   l = (max_ + min_) / 2;
   if(l > 0) {
      float del = max_ - min_;
      s = del;
      if(s > 0) {
         if(l < 0.5)
            s = del / (max_ + min_);
         else
            s = del / (2 - del);

         if(r == max_)
            h = ( (g - b) / del ) % 6;
         else if (g == max_)
            h = ( (b - r) / del ) + 2;
         else if (b == max_ )
            h = ( (r - g) / del ) + 4;
      }
   }
   hsl[0] = (h < 0) ?  (h * 60) + 360 : (h * 60);
   hsl[1] = s * 100;
   hsl[2] = l * 100;
}


/*****************************************************************************/
// Sounds

/*
  c_1   131    c     262     c1   523    c2   1047    c3   2093
  cis_1 139    cis   277     cis1 554    cis2 1109    cis3 2217
  d_1   147    d     294     d1   587    d2   1175    d3   2349
  dis_1 155    dis   311     dis1 622    dis2 1245    dis3 2489
  e_1   115    e     330     e1   659    e2   1319    e3   2637
  f_1   125    f     349     f1   698    f2   1397    f3   2794
  fis_1 135    fis   370     fis1 740    fis2 1480    fis3 2960
  g_1   146    g     392     g1   784    g2   1568    g3   3136
  gis_1 208    gis   415     gis1 831    gis2 1661    gis3 3322
  a_1   220    a     440     a1   880    a2   1760    a3   3520
  ais_1 233    ais   466     ais1 932    ais2 1865    ais3 3729
  h_1   247    h     494     h1   988    h2   1976    h3   3951
*/



// use with function void PlayMelody(int melody[] :

int ChordUp[] = {TONE_C4, 50, TONE_E4, 50, TONE_G4, 50,
  TONE_C5, 50, TONE_E5, 50, TONE_G5, 50, TONE_C6, 200};
int ChordDn[] = {TONE_C6, 50, TONE_G5, 50, TONE_E5, 50,
  TONE_C5, 50, TONE_G4, 50, TONE_E4, 50,  TONE_C4, 200};
int Chord[] = {TONE_C4, 50, TONE_E4, 50, TONE_G4, 50, TONE_C5, 50};
int Beep[] = {TONE_C5, 200};
int BeepBeep[] = {TONE_C5, 200 , 0, 100, TONE_C5, 200};
int Blip[] = {TONE_C7, 10 };
int BlipBlip[] = {TONE_C7, 10, 0, 20, TONE_C7, 10 };
int Buzz[] = {220, 200 };
int sdError[] = {TONE_C4, 50, 0, 50, TONE_C4, 50, 0, 50, TONE_C4, 50};



safecall void PlayMelody( int melody[] ) {
  int l=SizeOf(melody)/2;
  for (int i=0; i<l-1; i+=2) {
     PlayTone(melody[i], melody[i+1]);
     Wait(melody[i+1]);
  }
}



#endif // _STDLIB_H_
