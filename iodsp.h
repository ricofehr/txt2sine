#ifndef _IODSP_H_
#define _IODSP_H_

void open_dsp(int nbChannels, int speed, int format, int wr, int fragments);
int read_dsp(signed char *buffer, int sz);
int write_dsp(signed char *buffer, int sz);
void close_dsp();

#endif /*_IODSP_H_*/
