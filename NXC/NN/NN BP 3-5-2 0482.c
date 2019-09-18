// Lernfaehiges 2-schichtiges Neuronales Netz
// Backpropagation Netz mit 3 Sensor-Eingaengen (Touch an S1, S2, S3)
// an 5 verdeckten Neurons
// dann 2 Ausgabe-Neurons mit 2 Outputs (Anzeige auf dem Display)
// (c) H. W. 2008
// neu: automatisches Training nach Daten
   string Version="0.482";


#define printXY nxtDisplayStringAt
#define println nxtDisplayTextLine


//**********************************************************************
// Basisdeklarationen fuer Neuronale Netze
//**********************************************************************


const int L0 = 5; // max. Neurons in Schicht (Layer) 0 (Schicht fuer Inputs)
const int L1 = 2; // max. Neurons in Schicht (Layer) 1 (Ausgabe Schicht bei 2-schichtigen)
const int L2 = 1; // max. Neurons in Schicht (Layer) 2
const int L3 = 1; // max. Neurons in Schicht (Layer) 3

const int ni = 5; // max. Dendriten-Eingaenge (Zaehler ab 0)
const int nS = 3; // Sensoren-Zahl (Zaehler ab 0)



//**********************************************************************
// Neurons & Netzwerk
//**********************************************************************

float sollOut;

typedef struct{
    float in[ni]; // Einzel-Inputs (Dendriten)
    float w[ni]; // Einzel-Wichtungen (jedes Dendriten)
    float net; // totaler Input
    float th; // Schwellenwert (threshold)
    float d; // delta=Fehlersignal
    float out; // Output (Axon): z.B. 0 oder 1
} tNeuron;

//**********************************************************************

tNeuron Neuron0[L0]; // Neurons-Schicht 0 (Schicht fuer Inputs)
tNeuron Neuron1[L1]; // Neurons-Schicht 1 (Ausgabe Schicht bei 2-schichtigen)
tNeuron Neuron2[L2]; // Neurons-Schicht 2
tNeuron Neuron3[L3]; // Neurons-Schicht 3


//**********************************************************************
// Neuron internals
//**********************************************************************

float lf = 0.7; // Lern-Faktor
int CalcTime = (L0+L1+L2+L3+1)*10; // Network CalcTime




//**********************************************************************
// mathematische Hilfsfunktionen
//**********************************************************************


float tanh(float x) // Tangens hyperbolicus
{
    float e2x;
    e2x=exp(2*x);
    return((e2x-1)/(e2x+1));
}

//**********************************************************************
// Ein-/ Ausgabefunktionen (Tatstatur, Display)
//**********************************************************************

int key; // gedrueckte NXT-Taste fuer Menues (-> Display)
int choice; // gedrueckte NXT-Taste (keine Anzeige)


int buttonPressed(){

  TButtons nBtn;
  nNxtExitClicks=100; // gegen versehentliches Druecken

  nBtn = nNxtButtonPressed; // check for button press
    switch (nBtn) {
      case kLeftButton: {
           return 1; break; }

          case kEnterButton: {
               return 2; break; }

          case kRightButton: {
               return 3; break; }

          case kExitButton: {
               return 4; break; }

          default: {
               return 0; break; }
    }
    return 0;
}

//*****************************************

int getkey() {
    int k, buf;

    k=buttonPressed();
    buf=buttonPressed();
  while (buf!=0)
  { buf=buttonPressed();
    wait1Msec(20);}
  return k;
}

void pause() {

    int k=0;

    do {k=getkey();}
    while (k==0);

}



//**********************************************************************
string MenuText=""; // Menue-Steuerung

task DisplayValues(){


    while(true) {

    printXY(00, 63, "%2.0f", Neuron0[0].in[0]); // Sensoren (3)
    printXY(16, 63, "%2.0f", Neuron0[0].in[1]);
    printXY(32, 63, "%2.0f", Neuron0[0].in[2]);


    printXY(00, 55, "%2.1f", Neuron0[0].out); // Neurons L0
    printXY(20, 55, "%2.1f", Neuron0[1].out); // Eingabe-Schicht
    printXY(40, 55, "%2.1f", Neuron0[2].out);
    printXY(60, 55, "%2.1f", Neuron0[3].out);
    printXY(80, 55, "%2.1f", Neuron0[4].out);


    printXY(00, 23, "%3.1f", Neuron1[0].out); // outputs (2)
    printXY(25, 23, "%3.1f", Neuron1[1].out);


    println(7, "%s", MenuText); // Menue-Zeile fuer Tastatur-Steuerung

  }
  return;
}

//**********************************************************************



//**********************************************************************
// File I/O
//**********************************************************************
const string sFileName = "Memory.dat";

TFileIOResult nIoResult;
TFileHandle fHandle;

int nFileSize = (L0 + L1 + L2 + L3 +1)*100;


void SaveMemory()
{
    int i, j;

    CloseAllHandles(nIoResult);
    println(6,"%s","Save Memory...");
    wait1Msec(500);
    PlaySound(soundBeepBeep);
    wait1Msec(11);

    Delete(sFileName, nIoResult);

  OpenWrite(fHandle, nIoResult, sFileName, nFileSize);
  if (nIoResult==0) {
    eraseDisplay();

    for (j=0;j<L0;j++) {
      for (i=0; i<ni;i++)
      { WriteFloat (fHandle, nIoResult, Neuron0[j].w[i]); }
        WriteFloat (fHandle, nIoResult, Neuron0[j].th); }

    for (j=0;j<L1;j++) {
      for (i=0; i<ni;i++)
      { WriteFloat (fHandle, nIoResult, Neuron1[j].w[i]); }
        WriteFloat (fHandle, nIoResult, Neuron1[j].th); }


    Close(fHandle, nIoResult);
    if (nIoResult==0) {
        PlaySound(soundUpwardTones);
        println(6,"%s","Save Memory: OK"); }
    else {
        PlaySound(soundException);
        println(6,"%s","Save Memory: ERROR"); }
  }
  else PlaySound(soundDownwardTones);

}

//*****************************************

void RecallMemory()
{
  int i, j;
    println(6,"%s","Recall Memory");
  CloseAllHandles(nIoResult);
    wait1Msec(500);
    PlaySound(soundBeepBeep);
    wait1Msec(11);

    OpenRead(fHandle, nIoResult, sFileName, nFileSize);
  if (nIoResult==0) {

  for (j=0;j<L0;j++) {
     for (i=0; i<ni;i++)
     { ReadFloat (fHandle, nIoResult, Neuron0[j].w[i]); }
       ReadFloat (fHandle, nIoResult, Neuron0[j].th); }

  for (j=0;j<L1;j++) {
     for (i=0; i<ni;i++)
     { ReadFloat (fHandle, nIoResult, Neuron1[j].w[i]); }
       ReadFloat (fHandle, nIoResult, Neuron1[j].th); }


    Close(fHandle, nIoResult);
    if (nIoResult==0) PlaySound(soundUpwardTones);
    else {
        PlaySound(soundException);
        println(6,"%s","Recall: ERROR"); }
  }
  else PlaySound(soundDownwardTones);
  eraseDisplay();

}


//**********************************************************************
// Funktionen des neuronalen Netzes
//**********************************************************************

//**********************************************************************
// Inputs
//**********************************************************************

task RefreshInputLayer(){ // Inputs aus Sensorwerten
int i, j;
  while(true){
  for (j=0; j<L0; j++) { // an alle Eingangs-Neuronen
    for (i=0; i<nS; i++) { // alle Inputs als Bitmuster
      Neuron0[j].in[i]=(float)SensorValue(i);
      }
    }
  }
  return;
}

//*****************************************



void SetInputPattern(int i) // Inputs virtuell generiert
{
    int j, n;

    printXY(80, 63, "%d", i);
    for (j=0; j<L0;j++) // an alle Eingangs-Neuronen
  {
      for (n = 0; n<nS; n++) // alle Inputs als Bitmuster
    {
      Neuron0[j].in[n]= i & 1;
      i >>= 1;
    }
  }
}





//**********************************************************************
// Propagierungsfunktionen: Eingaenge gewichtet aufsummieren (in -> net)
//**********************************************************************

void netPropag(tNeuron &neur, int max){ // Propagierungsfunktion 1
  int i=0; // kalkuliert den Gesamt-Input (net)
  float s=0;

  for(i=0;i<max;i++){
      s+= (neur.in[i]*neur.w[i]); // gewichtete Summe
  }
  neur.net=s;
}

void netPropagThr(tNeuron &neur, int max){ // Propagierungsfunktion 2
  int i=0; // kalkuliert den Gesamt-Input (net)
  float s=0; // von den ersten (max) Eingaengen
                                            // samt Schwellwert
  for(i=0;i<max;i++){
      s+= (neur.in[i]*neur.w[i]); // gewichtete Summe
  }
  neur.net=s-neur.th; // abzueglich Schwellwert
}

//**********************************************************************
// Aktivierungsfunktionen inkl. Ausgabe (net -> act -> out)
//**********************************************************************


void act_01(tNeuron &neur){ // Aktivierungsfunktion 1 T1: x -> [0; +1]
   if (neur.net>=0) // 0-1-Schwellwertfunktion
      {neur.out=1;} // Fkt.-Wert: 0 oder 1
   else {neur.out=0;}
}

void actIdent(tNeuron &neur){ // Aktivierungsfunktion 2 T2: x -> x
   neur.out=neur.net; // Identitaets-Funktion
} // Fkt-Wert: Identitaet


void actFermi(tNeuron &neur){ // Aktivierungsfunktion 3 T3: x -> [0; +1]
  float val; // Fermi-Fkt. (Logistisch, differenzierbar)
  float c=5.0; // c= Steilheit, bei c=1: flach,
                                    // c=10: Sprung zwischen x E [-0.1; +0.1]

  val= (1/(1+(exp(-c*neur.net))));
  neur.out=val;
}

void actTanH(tNeuron &neur){ // Aktivierungsfunktion 4 T4: x -> [-1; +1]
   float val; // Tangens Hyperbolicus, differenzierbar
   float c=2.0; // c= Steilheit, bei c=1: flach
  val= tanh(c*neur.net); // c=3: Sprung zwischen x E [-0.1; +0.1]
  neur.out=val;
}



//**********************************************************************
// Reset / Init
//**********************************************************************

void ResetNeuron(tNeuron &neur, int rnd){ // alles auf Null bzw. randomisiert
    int i;



    for (i=0; i<ni; i++) {
        neur.in[i]=0; // Einzel-Input (Dendrit)
      if (rnd==0) {neur.w[i]=0;} // Einzel-Wichtung (Dendrit)=0
        else
        neur.w[i]=-2.0+random(10)*0.4; // Einzel-Wichtung (Dendrit) randomomisiert

    }
    neur.net=0; // totaler Input
    if (rnd==0) {neur.th=0;} // Schwellenwert (threshold)=0
    else
    neur.th=-0.2 + random(4)*0.1; // Schwellenwert (threshold) randomisiert
    neur.out=0; // errechneter Aktivierungswert=Output
    }

//*****************************************

void InitAllNeurons(){ // alle Netz-Neurons resetten
    int j; // (0 oder randomisiert)


    for (j=0; j<L0; j++) { // Neuron-Schicht 0
         ResetNeuron(Neuron0[j],1);}

  for (j=0; j<L1; j++) { // Neuron-Schicht 1
         ResetNeuron(Neuron1[j],1);}

  for (j=0; j<L2; j++) { // Neuron-Schicht 2
         ResetNeuron(Neuron2[j],0);}

  for (j=0; j<L3; j++) { // Neuron-Schicht 3
         ResetNeuron(Neuron3[j],0);}
}

//*****************************************

void PrepThisNeuralNet() // for testing
{
    ; // defaults
}


//**********************************************************************
// einzelne Neurons schichtenweise durchrechnen
//**********************************************************************

task RefreshLayers(){
  int j, k;

  while(true){


      for (j=0;j<L0;j++) { // Layer 0 (alle Eingabe-Neurons)
        netPropagThr(Neuron0[j], nS); // net-Input bis zur Anzahl Sensor-Inputs (nS) berechnen
      actFermi(Neuron0[j]); // dann Aktivierung T: Fermi-Funktion -> out

      for (k=0;k<L1;k++) { // Synapse j.Neuron0 ->
        Neuron1[k].in[j] = Neuron0[j].out; } // -> j.Eingang von allen k.Neuron1
    }

    for (j=0;j<L1;j++) { // Layer 1 (alle Ausgabe-Neurons)
      netPropagThr(Neuron1[j],L0); // net-Input bis zur Anzahl gesamt-Inputs (ni=L0) berechnen
      actFermi(Neuron1[j]); // dann Aktivierung T: Fermi-Funktion -> out
    }

  }
  return;
}

//**********************************************************************
// Lernverfahren
//**********************************************************************



//**********************************************************************
void LearnPerceptronRule() { // Perceptron-Lern-Modus
  int ErrorCount;
  int in; // Sensor-Kombinationen
  int i; // Anzahl Inputs
  int j; // Anzahl Ausgabe-Neurons

 do
 {

  alive();
  ErrorCount=0;
  PlaySound(soundBeepBeep);
  MenuText="-- << ok >> ++";


    SetInputPattern(in); // virtuelles Muster praesentieren
    wait1Msec(CalcTime);

    for (j=0;j<2;j++) // 1 bis Anzahl Ausgabe-Neuron
    {

        sollOut=0;
        MenuText="-- << ok >> ++";
        printXY(0,31, "soll:");
        printXY(0+(j*25),15,"%2.0f", sollOut);
      do // erzeugten Output berichtigen
      {
          key=getkey();

          if (key==1) { if (sollOut>0) sollOut-=1; }
          else
          if (key==3) { if (sollOut< 1) sollOut+=1; }
        printXY(0,31, "soll:");
          printXY(0+(j*25),15,"%2.0f", sollOut);
        wait1Msec(100);
      } while ((key!=2)&&(key!=4));

      println(5, " ");

      //...................................................
      if (key==4) { // Lern-Modus ENDE
          PlaySound(soundException);
          key=0;
          return;
      }
      //....................................................

                                        // Lern-Modus START
      //....................................................
      if (sollOut==Neuron0[j].out ) // teachOut korrekt
         {
              PlaySound(soundBlip);
         PlaySound(soundBlip);
         wait1Msec(100);
      }
         //....................................................
      if (sollOut!=Neuron0[j].out) // teachOut falsch
         {
             PlaySound(soundException);
             wait1Msec(100);
        ErrorCount+=1;
             //...................................................
                                        // LERNEN

        for (i=0; i<=L0; i++) // fuer alle i (Inputs)
             { // Wichtungen anpassen (Delta-Regel)
                 Neuron0[j].w[i] = Neuron0[j].w[i]+ (lf *Neuron0[j].in[i]*(sollOut-Neuron0[j].out));
             }

             if (sollOut!=Neuron0[j].out) // Schwelle anpassen (Delta-Regel, erweitert)
             {
                 Neuron0[j].th = Neuron0[j].th + (lf *(sollOut-Neuron0[j].out));
             }
        //...................................................
      } // if (sollOut!=Neuron0[j].out)

   } // for j


 } while (ErrorCount>0);

PlaySound(soundUpwardTones);
PlaySound(soundUpwardTones);
}

//**********************************************************************
//**********************************************************************

int IOpattern[(1<<ni)][L1]; // fix 471-002 [2<<(ni-1)] -> [(1<<ni)]
//**********************************************************************


void LearnBackpropagation() { // Backpropagation-Lern-Modus
                                 // 1 verdeckte/Eingabe-Schicht(L0) + 1 Ausgabe-Schicht(L1)
  int idummy;

  int count;
  int maxCount=999;

  int in; // angelegtes Sensor/Input-Muster(Pattern)
  int i; // Zaehler Inputs
  int j, k; // Index fuer Ausgabe-Neurons L1
  int m; // Index fuer Eingabe-Neurons L0


  float f; // Fehler (sollout-out )
  float f_sig1; // Fehler-Signal Schicht 1 zum Lernen v. Wichtung und Schwelle
  float f_sig0; // Fehler-Signal Schicht 0 zum Lernen v. Wichtung und Schwelle

  float f_sum=0; // Fehler verdeckte Schicht (Summe (Wichtung*Fehlersignal))
  float out; // Neuron-out, Dummy;
  float fehler=0; // Summe der Fehlersignale
  float epsilon; // max. zulssiger Netz-Fehler

  float delta_w0, delta_w1; // Aenderungswert der Wichtung
  float delta_th0, delta_th1; // Aenderungswert des Schwellwerts

  bool LearnModeAuto=false; // automat. Lernen oder manuell per Taste


  count=maxCount;
  epsilon=(float)(ni*L1)*0.1; // max. 10% Fehler

  do {
   alive();

   fehler=0;
   count-=1;

   if (!LearnModeAuto) PlaySound(soundBeepBeep);
     else PlaySound(soundBlip);

     for (in=0; in < (1<<nS); in++) // in = 1. bis letzter Sensor
   {
      SetInputPattern(in); // Eingabe-Muster anlegen

      wait1Msec(CalcTime); // durch alle Schichten durchrechnen

      for (j=0;j<L1;j++) // j = 1. bis letztes Ausgabe-Neuron
      {
//=====================================================================================

          if (!LearnModeAuto) // einlesen per Tastatur
       {

         sollOut=0;
         MenuText="-- << ok >> ++";
         printXY(0,31, "soll:");
         printXY(0+(j*25),15,"%2.0f", sollOut);
         do
         {
           key=getkey();
           if (key==4) { // Lern-Schritt ueberspringen
             IOpattern[in][j]=-99;
             key=0;
             printXY(0+(j*25),15," ");
             goto _NEXT;
           } // if key

           if (key==1) { if (sollOut==1) sollOut=0; }
           else
           if (key==3) { if (sollOut==0) sollOut=1; }

           IOpattern[in][j]=sollOut; // IO-Muster in IO-array schreiben

           printXY(0,31, "soll:");
           printXY(0+(j*25),15,"%2.0f", sollOut);
           wait1Msec(100);
        } while ((key!=2));
      }
//=====================================================================================
      else // autom. einlesen per IO-array
      {
           PlaySound(soundBlip);
         if (IOpattern[in][j]!=-99)
         { sollOut=IOpattern[in][j];}
         else
         break;

         printXY(0+(j*25),15,"%2.0f", sollOut);
      }
//=====================================================================================

      PlaySound(soundBlip);
      wait1Msec(CalcTime);
      println(6, " ");

    // 1. Schritt: Fehlersignal Ausgabeschicht bestimmen
    // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

      out=Neuron1[j].out; // eigenes Out
      f= (sollOut-out);
      if ((abs(f))>=0.99999)
        { out*=0.9; sollOut*=0.9;}

      f_sig1=out*(1-out)* f; // Fehler-Signal (j) fuer Ausgabeschicht

      Neuron1[j].d=f_sig1; // im Neuron1[j] speichern

      fehler=fehler + abs(sollOut-out); // Gesamtfehler aller Ausgabeneuronen

   _NEXT:

    } // for j= 1. bis letztes Ausgabe-Neuron



     // 2. Schritt: Fehlersignale verdeckte/Eingabe-Schicht bestimmen
     // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

   for (k=0;k<L1;k++) // k = 1. bis letztes Neuron in der Eingabe-Schicht

   {
          if (IOpattern[in][k]==-99) continue;

          f_sig1=Neuron1[k].d; // Fehlersignal des Nachfolger-(Ausgabe)-Neurons (k)

          f_sum=0;
          for (m=0; m<L0; m++)
        { f_sum=f_sum + (Neuron1[k].w[m] * f_sig1); } // Summe ueber alle (Wichtungen(L1)*FSignale(L1)


        for (m=0; m<L0; m++)
        {
            out=Neuron0[m].out; // eigenes Out
          f_sig0 = out * (1-out) * f_sum; // Fehlersignal Eingabe/verdeckte Schicht L0
            Neuron0[m].d = f_sig0; // Fehlersignal im Neuron0 speichern
        }

     // 3. Schritt: neue Wichtungen und Schwellenwerte fuer Ausgabeschicht L1 berechnen
     // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

          for (m=0; m<L0; m++)
        {

          out=Neuron0[m].out; // Vorgaenger-out
          if (abs(f_sig1)<0.0001) {delta_w1=0;} // fix 0481.001
          else
          delta_w1 = lf * out * f_sig1; // Aenderungswert fuer Wichtungen Ausgabe-Schicht

          Neuron1[k].w[m] = Neuron1[k].w[m] + delta_w1; // neue Wichtungen fuer Ausgabe-Schicht L1
        }

        delta_th1 = lf * f_sig1; // Aenderungswert delta_th

        Neuron1[k].th = Neuron1[k].th - delta_th1; // neue Schwellenw. fuer Ausgabe-Schicht L1


     // 4. Schritt: neue Wichtungen und Schwellenwerte fuer Eingabeschicht L0 berechnen
     // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^


        for (m=0; m<L0; m++)
        {
            f_sig0 = Neuron0[m].d; // eigenes Fehlersignal
            f_sig1 = Neuron1[k].d; // Nachfolger-Fehlersignal

          for (i=0; i<nS; i++)
          {
              out=Neuron0[m].in[i]; // Vorgaenger-out = Sensor-out = eigener Input
              delta_w0 = lf * out * f_sig1; // Aenderungswert fuer Wichtungen Eingabe-Schicht
              Neuron0[m].w[i] = Neuron0[m].w[i] + delta_w0; // neue Wichtungen fuer Eingabe-Schicht L0
          }

          delta_th0 = lf * f_sig0; // Aenderungswert fuer Schellenw. Eingabe-Schicht
          Neuron0[m].th = Neuron0[m].th - delta_th0; // neue Schwellenw. fuer Eingabe-Schicht L0
        }

    } // for k = 1. bis letztes Neuron in der Ausgabe-Schicht

    //...................................................

    //...................................................
    if (LearnModeAuto) // Abbruch bei automatischer Teach Eingabe
    {
        alive();
        PlaySound(soundBlip);
        key=getkey();
        if (key!=0)
        {
           goto _ENDE; // Abbruch durch beliebige Taste
        }
    }
    //...................................................

  } // for in=1... (Eingabe-Muster)

  if (!LearnModeAuto) // Menue bei manueller Teach Eingabe
    {
        MenuText="Menu manual. auto";
      PlaySound(soundLowBuzzShort);
      do {
        key=getkey();
        if (key==1) { return; }
        if (key==2) { LearnModeAuto=false; }
        if (key==3) { LearnModeAuto=true; }
        if (key==4) { return; }
      }
      while (key==0);
      key=0;
    }

  if (fehler>4) lf=0.8; // Lernfaktor anpassen
  else
  if (fehler<1.5) lf=0.4;
  else
  lf=fehler/5;

  //...................................................

  eraseDisplay();
  idummy=(int)(lf*10);
  MenuText=(string)count; //+" lf=."+(string)idummy;
  MenuText=MenuText+" f="+(string)fehler;

  //...................................................
  if ((count==(maxCount/3))&&(fehler>=(4*epsilon) ))
  { InitAllNeurons(); PlaySound(soundDownwardTones); count=maxCount;}
  //...................................................

 } while ((fehler>epsilon)&&(count>=0));

//...................................................
_ENDE:
if (fehler>epsilon) PlaySound(soundDownwardTones);
else
{ PlaySound(soundUpwardTones); PlaySound(soundUpwardTones);}
println(6, "Weiter: <Taste>");
pause();
println(6, "");
}
//**********************************************************************
//**********************************************************************




//**********************************************************************
// Programmablauf-Steuerung, Menues
//**********************************************************************

int Menu_Recall() {
  eraseDisplay();
  MenuText="<RCALL ESC CLEAR>";
  println(7, "%s", MenuText);
  println(0, "%s", " Hal "+ Version);
  println(1, "%s", "----------------");
  println(2, "%s", "Reload my brain -");
  println(4, "%s", " Total Recall ?");
  do
  {
      key=getkey();
      if (key==1) { return 1; }
      if (key==2) { PlaySound(soundException); }
      if (key==3) { return 3; }
      if (key==4) { break; PlaySound(soundException); }

      wait1Msec(100);
  }
  while ((key==0)||(key==2)||(key==4));
  return 0;
}

//------------------------------------------------------

int Menu_Quit() {
  eraseDisplay();
  MenuText="<Quit Sav Resume>";
  println(7, "%s", MenuText);

  do
  {
      key=getkey();
      if (key==1) { return 1; }
      if (key==2) { SaveMemory(); }
      if (key==3) { return 3; }
      if (key==4) { PlaySound(soundException); }

      wait1Msec(100);
  }
  while ((key==0)||(key==2)||(key==4));
}

//------------------------------------------------------

int Menu_LearnSaveRun() {
  eraseDisplay();
  MenuText="<Learn Sav Run>";
  do
  {
      key=getkey();
      if (key==1) { return 1; }
      if (key==2) { SaveMemory(); }
      if (key==3) { return 3; }
      if (key==4) { PlaySound(soundException);
                      return 4;}

      wait1Msec(100);
  }
  while ((key==0)||(key==2));
}

//**********************************************************************
// Hauptprogramm
//**********************************************************************


task main(){
  SensorType(S1)=sensorTouch;
  SensorType(S2)=sensorTouch;
  SensorType(S3)=sensorTouch;

  nVolume=2;

  _START_NEW_:
  choice=Menu_Recall();
  if (choice==1) { RecallMemory(); } // altes Gedaechtnis laden
  else
  if (choice==3) { InitAllNeurons();
                    PrepThisNeuralNet();} // neu initialisieren


  StartTask (DisplayValues);
  StartTask (RefreshLayers);

//============================================================================================

  while(true)
  {
      choice=Menu_LearnSaveRun(); // Haupt-Menue
//============================================================================================
    if (choice==1)
    {
        StopTask(RefreshInputLayer);
        LearnBackpropagation(); // Lern-Modus Backpropagation
    }

//============================================================================================
    if (choice==4) // ENDE ?
    {
        StopTask(DisplayValues);
        eraseDisplay();

        MenuText="<Quit Resume>";
      println(7, "%s", MenuText);

      choice=0;
      choice=Menu_Quit();

        if (choice==1) { println(4, " E N D");
                          wait1Msec(500);
                          StopAllTasks();
                          goto _END_; }
      if (choice==2) { StartTask(DisplayValues);
                          break;}

        if (choice==3) { StartTask(DisplayValues);
                          goto _START_NEW_;

    }
    while ((choice==0));
   }
//============================================================================================

    MenuText="Menue: [ESC]"; // Run-Modus
    PlaySound(soundFastUpwardTones);
    StartTask (RefreshInputLayer);
    do
    {
        key=getkey();
      wait1Msec(100);
    } while (key!=4);
  }
//============================================================================================

  _END_:

}
