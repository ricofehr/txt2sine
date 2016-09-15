#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/soundcard.h>
#include "decode.h"
#include "iofile.h"

/*
*	Open sound device and write testaudioWR with
*	frequency signal recorded
*/
int main(int argc, char *argv[])
{
	int i;
	char current;
	unsigned char buff[4096];
	int cp = 0;
	int desc;
	int cntz = 0;
	int cnth = 0;

	/*
	printf("%d::Masque CD\n", SOUND_MASK_CD);
	printf("%d::Masque PCM\n", SOUND_MASK_PCM);
	printf("%d::Masque MIC\n", SOUND_MASK_MIC);
	for (i = 0; i < SOUND_MIXER_NRDEVICES; i++) {
		printf("%d::%s\n", i, names[i]);
	}
	*/

	desc = create_file("assets/samplein");
	init_decode();
	while(1) {
		current = tuner_get_char();
		if (current == 'h')
			++cnth;
		else
			cnth = 0;

		if (cnth == 2) {
			cnth = 0;
			while(1) {
				current = tuner_get_char();

				if(current == 'h')
					++cnth;
				else
					cnth = 0;
				if(cnth == 2)
					break;

				buff[cp++] = current;

				if(cp == 4095) {
					buff[cp++] = '\0';
					write(desc, buff, cp*sizeof(unsigned char));
					cp = 0;
				}
			}

			if(cp) {
				buff[cp] = '\0';
				write(desc, buff, (cp-1)*sizeof(unsigned char));
			}

			break;
		}
	}
	close_decode();
}
