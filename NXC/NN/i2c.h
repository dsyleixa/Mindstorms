// I2C.lib

#define _I2C_LIB_

//====================================


void I2CClearBus(byte port) {
  byte buf = 0; byte status;
  do {status=I2CRead(port, 1, buf);}
  while (status != NO_ERR);

}



void I2CWaitUntilReady(byte port) {
   byte bReady=0; byte status;

   while(true) {
     status = I2CStatus(port, bReady);
     switch (status)
     {
        case NO_ERR:             return;

        case STAT_COMM_PENDING:  break;

        case ERR_COMM_BUS_ERR:   PlayTone(27, 1);
                                 I2CClearBus(port);
                                 break;

        default:                 break;

     }
     Wait(1);
   }
}

//====================================
