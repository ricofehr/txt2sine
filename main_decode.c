#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/soundcard.h>
#include "decode.h"
#include "iofile.h"


/* Synchronisation pattern */
static unsigned char sync_datas[] = "hkh";
/* Separator between octet */
static unsigned char separator = ';';

/*
*	Open sound device and write testaudioWR with
*	frequency signal recorded
*/
int main(int argc, char *argv[])
{
	int i;
	unsigned char current, after;
	unsigned char buff[4096];
	unsigned int octet[256];
	int cp = 0;
	int desc;
	int iswrited = 1;
	int cnth = 0, cntb = 0, cntb2 = 0;
	int max = 0;
	int begin = 0;

	desc = create_file("assets/samplein");
	init_decode();

	/* Init count char array */
	for (i = 0; i < 256; i++)
		octet[i] = 0;

	while(1) {
		after = tuner_get_char();
		if (after != separator) {
			/* Count character if no more separator */
			if (begin && cntb > 4) {
				octet[(int)after]++;
				iswrited = 0;
				cntb2 = 0;
			}

			if (begin)
				++cntb;
			continue;
		}

		/* Begin transmission afer first separator char */
		begin = 1;
		cntb = 0;
		++cntb2;
		/* Already recorded or no more 'separator' frames received */
		if (iswrited || cntb2 < 4)
			continue;

		/* Filered the most received character */
		for (i = 0; i < 256; i++) {
			if (max < octet[i]) {
				current = (unsigned char)i;
				max = octet[i];
			}
			octet[i] = 0;
		}

		if (debug)
			printf("%d::%d::%c\n", cnth, max, current);

		/* Ensure at least 8 frames received for this character */
		if (max < 8)
			continue;
		max = 0;

		/* Synchronisation steps */
		if (cnth % 3 == 2 && current == sync_datas[2]) {
			cnth++;
			if (cnth == 3) {
				iswrited = 1;
				continue;
			}

			/* cnth == 6, then end of transmitt */
			break;
		} else if ((cnth % 3 != 1 && current == sync_datas[0])
			    || (cnth % 3 == 1 && current == sync_datas[1])) {
			++cnth;
			continue;
		} else {
			if (cnth > 3) {
				buff[cp++] = 'h';
				if (cnth > 4)
					buff[cp++] = 'k';
			}

			cnth = (cnth / 3) * 3;
		}

		if (cnth < 3) {
			iswrited = 1;
			continue;
		}

		/* Record character */
		buff[cp++] = current;
		printf("%s\n", buff);
		if(cp >= 4091) {
			buff[cp++] = '\0';
			write(desc, buff, cp * sizeof(unsigned char));
			cp = 0;
		}

		iswrited = 1;
	}

	if(cp) {
		buff[cp++] = '\0';
		write(desc, buff, cp * sizeof(unsigned char));
	}

	close_decode();
}
