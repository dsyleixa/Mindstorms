//  RcxTMux
#ifndef _RCXTMUX_
#define _RCXTMUX_


inline bool MuxRange(int Wert, int Basis, int Toleranz)
{
	return((Wert>=Basis-Toleranz) && (Wert<=Basis+Toleranz));
}


inline bool RcxTMux1pressed(char Port, char MuxPort)
{
   bool pressed=0;
   
   if MuxPort==1
   {
      pressed=(MuxRange(Sensor(Port), 16, 3) || (MuxRange(Sensor(Port), 93, 5))
   }
   
   else if MuxPort==2
   {
      pressed=(MuxRange(Sensor(Port), 48, 3) || (MuxRange(Sensor(Port), 93, 5))
   }
   return pressed;
}



inline bool RcxTMux2pressed(char Port, char MuxPort)
{
   bool pressed=0;

   if MuxPort==1
   {
      pressed=(MuxRange(Sensor(Port),187, 5) || (MuxRange(Sensor(Port),565, 5))
   }

   else if MuxPort==2
   {
      pressed=(MuxRange(Sensor(Port),415, 5) || (MuxRange(Sensor(Port),565, 5))
   }
   return pressed;
}
