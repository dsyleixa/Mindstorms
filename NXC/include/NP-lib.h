/************************************************************************/
/*                                                                      */
/* Program Name: NP-lib.h */
/* =============================                                        */

// modified by HaWe
/*                                                                      */
/* Copyright (c) 2010 by mindsensors.com                                */
/* Email: info (<at>) mindsensors (<dot>) com                           */
/*                                                                      */
/* This program is free software. You can redistribute it and/or modify */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation; version 3 of the License.              */
/* Read the license at: http://www.gnu.org/licenses/gpl.txt             */
/*                                                                      */

/************************************************************************/
/*
 * When        Who                      Comments
 * 10/10/10    Deepak Patil             Initial authoring.
 * 11/02/10    Matthew Richardson       adding functions: NP_DecodeKeys and
                                        NP_GetKeysPressed and cleaning up.
* 2012-11-18   HaWe                     changing  #defines, wait constants

*/

#ifndef _NP-lib_H_
   #define _NP-lib_H_
   
#define KEY_STATUS_REG 0x00
#define NPdevAddr 0xB4

char keyMap[] = { '4', '1', '7', '*', '5', '2', '8', '0', '3', '6', '9', '#' };

// Do not change the contents of these Group variables or the order.
byte Group1[] = { 0x2B, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0xFF, 0x02};
byte Group2[] = { 0x41, 0x0F, 0x0A, 0x0F, 0x0A, 0x0F, 0x0A, 0x0F, 0x0A, 0x0F};
byte Group3[] = { 0x4A, 0x0A, 0x0F, 0x0A, 0x0F, 0x0A, 0x0F, 0x0A, 0x0F};
byte Group4[] = { 0x52, 0x0A, 0x0F, 0x0A, 0x0F, 0x0A, 0x0F, 0x0A, 0x0F};
byte Group5[] = { 0x5C, 0x0b, 0x20, 0x0C};
byte Group6[] = { 0x7D, 0x9C, 0x65, 0x8C};

void NP_i2cwrite(byte port, byte addr, byte reg, byte data)
{
  byte result = -1;             // default: error value
  byte cmdbuf[];               // register number holder
  int loop, n, nByteReady;

  ArrayBuild(cmdbuf, addr, reg, data);
   loop = STAT_COMM_PENDING;
  while ( loop == STAT_COMM_PENDING ) {
    loop = I2CStatus(port, nByteReady);
  }

  // when the I2C bus is ready, send the message you built
  n = I2CWrite(port, 0, cmdbuf);
  while (I2CStatus(port, nByteReady) ==  STAT_COMM_PENDING);
}



// read data from sensor register(s)
int NP_i2cread(byte port, byte addr, byte reg, byte cnt)
{
  int result = -1;             // default: error value
  byte outbuf[];               // here we will get data
  byte cmdbuf[];               // register number holder
  int             loop;
  byte            nByteReady = 0;

  ArrayBuild(cmdbuf, addr, reg);
  loop = STAT_COMM_PENDING;
  while (loop == STAT_COMM_PENDING) {
      loop = I2CStatus(port, nByteReady);
  }
  if(I2CBytes(port, cmdbuf, cnt, outbuf))
  {
    result = outbuf[0];       // read value
    if(cnt==2)
      result = result + outbuf[1]*256;
// if 2 registers (16 bit), then add the MSB part
  }

  return result;              // returns -1 if I2CBytes failed
}
  
 void NP_writeBytes(byte port, byte addr, const byte & Group[])
 {
  int n, nbytes;
  // Wait for the end of previously sent data
  while(I2CStatus(port, nbytes)==STAT_COMM_PENDING);
  byte WriteBuf[];
  ArrayBuild(WriteBuf, addr, Group);
  I2CWrite(port, 0, WriteBuf);
}

void NP_InitializeKeypad(byte port, byte addr) {
// the following sequence of bytes initialize the sensor's
// configuration for performance.

// This function must be called at the beginning of every power cycle
// (call it at the beginning of your program, after you initialize the port).

// Do not change the values below 
// Or the order in which these values are written.
  NP_writeBytes(port, addr, Group2);
  NP_writeBytes(port, addr, Group3);
  NP_writeBytes(port, addr, Group4);
  NP_writeBytes(port, addr, Group5);
  NP_writeBytes(port, addr, Group1);
  NP_i2cwrite(port, addr, 0x7B, 0x0B);
  NP_writeBytes(port, addr, Group6);
}

bool NP_GetKeyPress(byte port, byte addr, int waitPeriod /* ms */, byte &keyHolder)
{
  int Touch;
  int Previous_Touch;
  int bit_test;
  int i, j;
  string a, msg, s;
  int w, cw;
  w = waitPeriod;  // <========================= ms!

  cw = 0;  // cumulative wait
  while(true)
  {

   Touch = NP_i2cread(port, addr, KEY_STATUS_REG, 2);
   Touch = Touch & 0x0FFF;     //Last 4 bits of byte 2 are not relevant

   int b;
   b = 0x01 << 11;

    if(Previous_Touch != Touch)
    {
      Previous_Touch = Touch;

        for ( j=0, i=0; j < 12; j++)
        {
          if ( Touch & b ) {
            keyHolder = keyMap[j]; 
            return true;
        }
        b = b >> 1;
      }
    }
    Wait(50);
    cw += 20;    // <================= 50
    if ( w != 0 && cw > w ) {
      return false;
    }
  }
  return false;
}

int NP_GetKeysPressed (byte port, byte addr)
{   //Returns a 12bit number containing the status of all 12 keys.
  int result;
  result=NP_i2cread(port, addr, 0x00, 2);
  result=result & 0x0FFF;
  return result;
}

int NP_DecodeKeys (int KeyBits, byte & Keys[])
{  //Bytes 0-11 of Array Keys contain the specific keys pressed.
	 //Byte 12 contains the number of buttons pressed (returned by the function).
  Keys[12]=0;
  if (KeyBits>=2048){KeyBits-=2048;Keys[12]++;Keys[4]=1;} else{Keys[4]=0;}
  if (KeyBits>=1024){KeyBits-=1024;Keys[12]++;Keys[1]=1;} else{Keys[1]=0;}
  if (KeyBits>=512) {KeyBits-=512; Keys[12]++;Keys[7]=1;} else{Keys[7]=0;}
  if (KeyBits>=256) {KeyBits-=256; Keys[12]++;Keys[10]=1;}else{Keys[10]=0;}
  if (KeyBits>=128) {KeyBits-=128; Keys[12]++;Keys[5]=1;} else{Keys[5]=0;}
  if (KeyBits>=64)  {KeyBits-=64;  Keys[12]++;Keys[2]=1;} else{Keys[2]=0;}
  if (KeyBits>=32)  {KeyBits-=32;  Keys[12]++;Keys[8]=1;} else{Keys[8]=0;}
  if (KeyBits>=16)  {KeyBits-=16;  Keys[12]++;Keys[0]=1;} else{Keys[0]=0;}
  if (KeyBits>=8)   {KeyBits-=8;   Keys[12]++;Keys[3]=1;} else{Keys[3]=0;}
  if (KeyBits>=4)   {KeyBits-=4;   Keys[12]++;Keys[6]=1;} else{Keys[6]=0;}
  if (KeyBits>=2)   {KeyBits-=2;   Keys[12]++;Keys[9]=1;} else{Keys[9]=0;}
  if (KeyBits>=1)   {KeyBits-=1;   Keys[12]++;Keys[11]=1;}else{Keys[11]=0;}
	return Keys[12];
}

#endif
