#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/soundcard.h>
#include "Decode.h"
#include "IOFile.h"

const char *names[SOUND_MIXER_NRDEVICES] = SOUND_DEVICE_NAMES;
int speed = 22050;
double frequency = 1;//2.691650391 ;//speed/NSAMPLES ;
int format = AFMT_U8 ;
int stereo=1 ;

int main()
{
	int i ;
	printf("%d::Masque CD\n", SOUND_MASK_CD);
	printf("%d::Masque PCM\n", SOUND_MASK_PCM);
	printf("%d::Masque MIC\n", SOUND_MASK_MIC);
	for (i = 0; i < SOUND_MIXER_NRDEVICES; i++)
	{
		printf("%d::%s\n", i, names[i]);
	}

	//exit(0) ;
	char current ;
	unsigned char buff[4096] ;
	int cp = 0 ;
	int desc ;

	desc = createFile("/home/rico/Desktop/testaudioWR") ;
	initDecode() ;
	while(1)
	{
		current = tunerGetChar() ;

		/*
		if(current == 'h')
		{
			while(1)
			{
				current = tunerGetChar() ;
				if(current == 'h')continue ;
				if(current == 'z')break ;
				buff[cp++] = current ;

				if(cp == 4095)
				{
					buff[cp++] = '\0' ;
					write(desc, buff, cp*sizeof(unsigned char)) ;
					cp = 0 ;
				}
			}

			if(cp)
			{
				buff[cp++] = '\0' ;
				write(desc, buff, cp*sizeof(unsigned char)) ;
			}

			break ;
		}
		*/
	}
	closeDecode() ;
}
