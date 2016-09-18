#include <math.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>
#include <string.h>

#include "encode.h"
#include "iofile.h"
#include "iodsp.h"
#include "iomixer.h"

/* Defines global variables */
const int speed = 44100;
const int format = AFMT_S16_LE;
const int stereo = 1;

/*
*	write_char - Encode an inputed character to a signal
*	@c: input char to encode
*
*	Take an input character and write to dsp device an uniq
*	chunk of datas for this character
*/
static void write_char(unsigned char c)
{
	int freq = 400 + c * 60;
	unsigned int i;
	float value;
	short buffer[NSAMPLES];

	printf("Char:%c, frequence:%d\n", c, freq);

	/* Compute sinusoidal signal for encode char */
    	i = 0;
	while (i < NSAMPLES) {
      		/* i is the sample index
      		 Multiply by 2*pi -> one cycle per sample:
      		 Multiply by freq samples per second
      		 Divide by 2050 samples per second
		*/
      		value = 32768 * sin(freq * (2 * M_PI) * i / speed);

		if (value > 32767.0f)
			value = 32767.0f;
		if (value < -32768.0f)
			value = -32768.0f;
		//buffer[i++] = (signed char)value;

		/* first send the low byte then the high byte */
		//buffer[i++] = (unsigned char)((int)value & 0xff);
		//buffer[i++] = (unsigned char)(((int)value >> 8) & 0xff);
		buffer[i++] = (short)value;
    	}


	write_dsp(buffer, NSAMPLES);
}

/*
*	init_encode - Open dsp device
*/
void init_encode()
{
	int max_fragments = 2;
	int frag_shift = ffs(NSAMPLES) - 1;
	int fragments = (max_fragments << 16) | frag_shift;

	fragments = 0x0004000a;
	open_dsp(stereo, speed, format, 1, fragments);
}

/*
*	write_datas - Transmit buffer to dsp device
*	@buffer: datas to encode
*
*	Encode buffer string into chunk of signal
*/
void write_datas(unsigned char *buffer)
{
	unsigned char between = '/';
	unsigned int i;
	for (i = 0; i < strlen(buffer); i++) {
		write_char(buffer[i]);
		write_char(between);
	}
}

/*
*	close_encode - Close dsp device
*/
void close_encode()
{
	close_dsp();
}
