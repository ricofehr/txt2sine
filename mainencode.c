#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/soundcard.h>
#include "Encode.h"
#include "IOFile.h"

const char *names[SOUND_MIXER_NRDEVICES] = SOUND_DEVICE_NAMES;
const int speed = 22050;
const double frequency = 1 ;
const int format = AFMT_U8 ;
const int stereo=1 ;

int main()
{
	unsigned char buff[4096] ;
	
	int sz = 4096 ;
	int desc = openFileRO("/home/rico/Desktop/testaudioRO") ;
	
	initEncode() ;
	do
	{
		sz = readFile(desc, buff, sz*sizeof(unsigned char)) ;
		writeDatas(buff) ;
	}
	while(sz>=4096) ;
	closeEncode() ;
	closeFile(desc) ;
}
