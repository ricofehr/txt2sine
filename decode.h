#ifndef _DECODE_H_
#define _DECODE_H_

#define NSAMPLES (22050)
#define FFT_LENGTH      4096     /* in samples */
#define PAD_FACTOR      2        /* 1/pf of the FFT samples are real samples, the rest are padding */
#define OVERLAP         4        /* 1/ol samples will be replaced in the buffer every frame. Should be
				  * a multiple of 2 for the Hamming window (see
				  * http://www-ccrma.stanford.edu/~jos/parshl/Choice_Hop_Size.html).
				  */

/* Declares global variables */
extern const char *names[];
extern int speed;
extern double frequency;
extern int format;
extern int stereo;

struct pair
{
	double first;
	double second;
};

void init_decode();
void get_data_from_device();
unsigned char tuner_get_char();
void close_decode();

#endif /*_DECODE_H_*/
