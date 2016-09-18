#ifndef _DECODE_H_
#define _DECODE_H_

#define FFT_LENGTH      256     /* in samples */

/* Declares global variables */
extern int speed;
extern int debug;
extern int format;
extern int stereo;

struct pair
{
	double first;
	double second;
};

void init_decode();
unsigned char tuner_get_char();
void close_decode();

#endif /*_DECODE_H_*/
