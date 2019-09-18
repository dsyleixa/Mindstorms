//math.h

#ifndef _MATH_H_
#define _MATH_H_

//********************************************

float min(float a, float b) {
  return (a<b? a: b);
}

float max(float a, float b) {
  return (a>b? a: b);
}

//********************************************

inline int round(float f)
{
  if (f>=0) {return (f + 0.5);}
  else
  {return (f - 0.5);}
}
  
  
//********************************************


int Median(int val[]) {
   int temp, Median;

   if (val[0] > val[1]) {temp=val[0]; val[0]=val[1]; val[1]=temp;}
   if (val[1] > val[2]) {temp=val[1]; val[1]=val[2]; val[2]=temp;}
   if (val[0] > val[1]) {temp=val[0]; val[0]=val[1]; val[1]=temp;}

   return val[1];

}


//********************************************

#endif //_MATH_H_ 