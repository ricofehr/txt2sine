#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/soundcard.h>
#include <err.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "IOFile.h"
#include "IOMixer.h"

int descMixer;

void openMixer()
{
	descMixer = openFileROWR("/dev/mixer");
}


void initMicro()
{
	int indevice;
	int volume = 90;

	openMixer();
	ioctl(descMixer, MIXER_WRITE(SOUND_MIXER_MIC), &volume);
	ioctl(descMixer, MIXER_WRITE(SOUND_MIXER_RECLEV), &volume);

	indevice = 1<<7;
	if (ioctl(descMixer, SOUND_MIXER_WRITE_RECSRC, &indevice) == -1) {
		perror("mixer");
		exit(1);
	}
	closeMixer();
}


void initVol()
{
	int indevice;
	int volume = 90;

	openMixer();
	ioctl(descMixer, MIXER_WRITE(SOUND_MIXER_RECLEV), &volume);

	indevice = 1<<0;
	if (ioctl(descMixer, SOUND_MIXER_WRITE_RECSRC, &indevice) == -1) {
		perror("mixer");
		exit(1);
	}
}

void closeMixer()
{
	closeFile(descMixer);
}
