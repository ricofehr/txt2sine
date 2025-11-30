#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/soundcard.h>
#include <err.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "iodsp.h"
#include "iofile.h"

/* Dsp file descriptor */
static int desc_dsp;

/*
*	open_dsp_wr - Open dsp device in write mode
*/
static void open_dsp_wr()
{
	desc_dsp = open_file_wr("/dev/dsp", 0);
}

/*
*	open_dsp_ro - Open dsp device in read mode
*/
static void open_dsp_ro()
{
	desc_dsp = open_file_ro("/dev/dsp");
}

/*
*	open_dsp_rwr - Open dsp device in read and write mode
*/
static void open_dsp_rwr()
{
	desc_dsp = open_file_rwr("/dev/dsp");
}

/*
*	open_dsp - Open dsp device with properties
*	@channels: count of channels (1: mono, 2: stereo)
*	@speed: sampling rate (48Khz for hifi sampling)
*	@format: Sample format (quantification)
*	@wr: io mode (0 -> ro, 1 -> wr, 2 -> rwr)
*	@fragments: size of chunk buffered with dsp device
*
*	Open a dsp device and set sample properties
*/
void open_dsp(int channels, int speed, int format, int wr, int fragments)
{
	if (wr == 2)
		open_dsp_rwr();
	else if (wr == 1)
		open_dsp_wr();
	else
		open_dsp_ro();

	ioctl(desc_dsp, SNDCTL_DSP_RESET, 0);
	if (ioctl(desc_dsp, SNDCTL_DSP_SETFMT, &format) == -1) {
		/* fatal error */
		perror("SNDCTL_DSP_SETFMT");
		exit(1);
	}

	if (ioctl(desc_dsp, SNDCTL_DSP_CHANNELS, &channels) == -1) {
		/* Fatal error */
		perror("SNDCTL_DSP_CHANNELS");
		exit(1);
	}

	if (ioctl(desc_dsp, SNDCTL_DSP_SPEED, &speed) == -1) {
		/* Fatal error */
		perror("SNDCTL_DSP_SPEED");
		exit(1);
	}

	if (fragments != 0)
		ioctl(desc_dsp, SNDCTL_DSP_SETFRAGMENT, &fragments);
	ioctl(desc_dsp, SNDCTL_DSP_SYNC, 0);
}

/*
*	read_dsp - Read a chunk from dsp device
*	return count of octets reading
*
*	Read sz characters from dsp device and write
*	them in buffer variable
*/
int read_dsp(short *buffer, int sz)
{
	int ret;

	if ((ret = read_file(desc_dsp, buffer, sz)) == -1) {
		perror("audio read");
		exit(1);
	}

	return ret;
}

/*
*	write_dsp - Write a chunk to dsp device
*	return count of octets writing
*
*	Write sz characters from buffer variable
*	to dsp device
*/
int write_dsp(short *buffer, int sz)
{
	int ret;
	int test;

	if ((ret = write_file(desc_dsp, buffer, sz*sizeof(unsigned char)) == -1)) {
		perror("audio write");
		exit(1);
	}

	if ((test = ioctl(desc_dsp, SOUND_PCM_SYNC, 0)) == -1) {
		perror("audio sync");
		exit(1);
	}

	return ret;
}

/*
*	close_dsp - Close dsp device
*/
void close_dsp()
{
	close_file(desc_dsp);
	desc_dsp = -1;
}
