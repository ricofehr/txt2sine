#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/soundcard.h>
#include "encode.h"
#include "iofile.h"

/*
*	Open "testaudioRO" file and transmit to dsp device
*/
int main(int argc, char *argv[])
{
	short buff[4096];

	int sz = 4096;
	int desc = open_file_ro("assets/sampleout");

	sleep(5);
	init_encode();
	/* Trigger decode to start record */
	write_datas("bhkh");
	do {
		sz = read_file(desc, buff, sz*sizeof(unsigned char));
		write_datas((unsigned char *)buff);
	}
	while (sz >= 4096);
	/* Trigger decode to finish record */
	write_datas("hkh");

	close_encode();
	close_file(desc);
}
