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
	unsigned char buff[4096];

	int sz = 4096;
	int desc = open_file_ro("assets/sampleout");

	init_encode();
	/* Trigger decode to start record */
	write_char('h');
	write_char('h');
	do {
		sz = read_file(desc, buff, sz*sizeof(unsigned char));
		write_datas(buff);
	}
	while (sz >= 4096);
	/* Trigger decode to finish record */
	write_char('h');
	write_char('h');

	close_encode();
	close_file(desc);
}
