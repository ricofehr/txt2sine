#ifndef _DECODE_H_
#define _DECODE_H_

#define FFT_LENGTH      256     /* in samples */

/* Declares global variables */
extern const int speed;
extern const int debug;
extern const int format;
extern const int stereo;

struct pair
{
	double first;
	double second;
};

void init_decode();
unsigned char tuner_get_char();
void close_decode();

#endif /*_DECODE_H_*/
