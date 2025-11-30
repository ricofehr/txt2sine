#ifndef IODSP_H_
#define IODSP_H_

extern void openDspWR() ;
extern void openDspRO() ;
extern void openDspROWR() ;
extern void openDSP(int nbChannels, int speed, int format, int wr, int fragments) ;
extern int readDSP(signed char *buffer, int sz) ;
extern int writeDSP(signed char *buffer, int sz) ;
extern void closeDSP() ;

#endif /*IODSP_H_*/
