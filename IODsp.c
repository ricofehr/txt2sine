#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/soundcard.h>
#include <err.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "IODsp.h"
#include "IOFile.h"

int descDSP;

void openDspWR()
{
	descDSP = openFileWR("/dev/dsp",0);
}

void openDspRO()
{
	descDSP = openFileRO("/dev/dsp");
}

void openDspROWR()
{
	descDSP = openFileROWR("/dev/dsp");
}

void openDSP(int channels, int speed, int format, int wr, int fragments)
{
	if (wr == 2)
		openDspROWR();
	else if (wr == 1)
		openDspWR();
	else
		openDspRO();

	ioctl(descDSP, SNDCTL_DSP_RESET, 0);
	if (ioctl(descDSP, SNDCTL_DSP_SETFMT, &format) == -1) {
		/* fatal error */
		perror("SNDCTL_DSP_SETFMT");
		exit(1);
	}

	if (ioctl(descDSP, SNDCTL_DSP_CHANNELS, &channels) == -1) {
		/* Fatal error */
		perror("SNDCTL_DSP_CHANNELS");
		exit(1);
	}

	if (ioctl(descDSP, SNDCTL_DSP_SPEED, &speed) == -1) {
		/* Fatal error */
		perror("SNDCTL_DSP_SPEED");
		exit(1);
	}

	if (fragments != 0)
		ioctl(descDSP, SNDCTL_DSP_SETFRAGMENT, &fragments);
	ioctl(descDSP, SNDCTL_DSP_SYNC, 0);
}

int readDSP(signed char *buffer, int sz)
{
	int ret;

	if ((ret = readFile(descDSP, buffer, sz)) == -1) {
		perror("audio read");
		exit(1);
	}

	return ret;
}

int writeDSP(signed char *buffer, int sz)
{
	int ret;
	int test;

	if ((ret = writeFile(descDSP, buffer, sz*sizeof(signed char)) == -1)) {
		perror("audio write");
		exit(1);
	}

	if ((test = ioctl(descDSP, SOUND_PCM_SYNC, 0)) == -1) {
		perror("audio sync");
		exit(1);
	}

	return ret;
}

void closeDSP()
{
	closeFile(descDSP);
	descDSP = -1;
}
