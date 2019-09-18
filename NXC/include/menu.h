// menu.h

int menu() {
  int mmax = 8, i;

  string str[];
  static char ch=3;
  char   btn, lch;

  memset(str, 0, mmax);

  str[0]="0 few   ";
  str[1]="1       ";
  str[2]="2       ";
  str[3]="3 medium";
  str[4]="4       ";
  str[5]="5       ";
  str[6]="6 many  ";
  str[7]="7 exit  ";

  ClearScreen();

  for (i=0;i<mmax;++i) TextOut(0,LCDline[i], str[i]);

  TextOut(0,LCDline[ch], str[ch], DRAW_OPT_INVERT);

  do {
    lch=ch;
    NumOut(90,0, ch);
    btn=getchar();
    if (btn==BTNRIGHT) if(ch<mmax-1) ch++; else ch=0;
    else
    if (btn==BTNLEFT ) if(ch > 0) ch--; else ch=mmax-1;

    TextOut(0,LCDline[lch], str[lch], DRAW_OPT_NORMAL);
    TextOut(0,LCDline[ch], str[ch],   DRAW_OPT_INVERT);

  } while (btn!= BTNCENTER);

  return (ch);
}

