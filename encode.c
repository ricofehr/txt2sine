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
const int speed = 22050;
const double frequency = 1;
const int format = AFMT_U8;
const int stereo=1;

/*
*	write_char - Encode an inputed character to a signal
*	@c: input char to encode
*
*	Take an input character and write to dsp device an uniq
*	chunk of datas for this character
*/
static void write_char(unsigned char c)
{
	int freq = 40 + c*40;
	int desc, sz;
	double cycles = (double)freq/speed;
	unsigned int i;
	signed char buffer[NSAMPLES];
	signed char buffer2[512];
	char *filename;

	sz = 2;
	if (freq > 99)
		sz = 3;
	if (freq > 999)
		sz = 4;
	if (freq > 9999)
		sz = 5;
	sz += 5;
	filename = (char*) malloc((sz+1) * sizeof(char));
	sprintf(filename, "assets/freq/%d", freq);
	filename[sz] = '\0';

	printf("Char:%c, frequence:%d\n", c, freq);

	desc = open_file_ro(filename);
	read_file(desc, buffer2, 512);
	close_file(desc);
        free(filename);

	for(i = 0; i < NSAMPLES; i++)
		buffer[i] = buffer2[i%512];

	write_dsp(buffer, NSAMPLES);
}

/*
*	init_encode - Open dsp device
*/
void init_encode()
{
	open_dsp(stereo, speed, format, 1, 0);
}

/*
*	write_datas - Transmit buffer to dsp device
*	@buffer: datas to encode
*
*	Encode buffer string into chunk of signal
*/
void write_datas(unsigned char *buffer)
{
	unsigned int i;
	for (i = 0; i < strlen(buffer); i++)
		write_char(buffer[i]);
}

/*
*	close_encode - Close dsp device
*/
void close_encode()
{
	close_dsp();
}
