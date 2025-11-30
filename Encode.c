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

#include "Encode.h"
#include "IODsp.h"
#include "IOMixer.h"

extern int speed;
extern double frequency;
extern int format;
extern int stereo;

void initEncode()
{
	openDSP(stereo, speed, format, 1, 0);
}

void writeDatas(unsigned char *buffer)
{
	unsigned int i;
	for (i = 0; i < strlen(buffer); i++)
		writeChar(buffer[i]);
}


void writeChar(unsigned char c)
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
	filename = (char*) malloc((sz+1)*sizeof(char));		
	sprintf(filename, "freq/%d", freq);
	filename[sz] = '\0';

	printf("Char:%c, frequence:%d\n", c, freq);
	desc = openFileRO(filename);
	readFile(desc, buffer2, 512);
	closeFile(desc);
	
	for(i = 0; i < NSAMPLES; i++)
		buffer[i] = buffer2[i%512];
	
	writeDSP(buffer, NSAMPLES);			
}

void closeEncode()
{
	closeDSP();	
}
