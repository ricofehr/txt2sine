#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/soundcard.h>
#include <err.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "iofile.h"
#include "iomixer.h"

/* File descriptor for mixer device */
static int desc_mixer;

/*
*	open_mixer - Open the mixer device
*/
static void open_mixer()
{
	desc_mixer = open_file_rwr("/dev/mixer");
}

/*
*	close_mixer - Close mixer device file
*/
static void close_mixer()
{
	close_file(desc_mixer);
}

/*
*	init_micro - Init mic device
*
*	Open mixer device and enable micro record feature
*/
void init_micro()
{
	int indevice;
	int volume = 90;

	open_mixer();
	ioctl(desc_mixer, MIXER_WRITE(SOUND_MIXER_MIC), &volume);
	ioctl(desc_mixer, MIXER_WRITE(SOUND_MIXER_RECLEV), &volume);

	indevice = 1 << 7;
	if (ioctl(desc_mixer, SOUND_MIXER_WRITE_RECSRC, &indevice) == -1) {
		perror("mixer");
		exit(1);
	}
	close_mixer();
}

/*
*	init_vol - Set volume on mixer device
*
*	Send volume ioctl command to mixer device and record value
*/
void init_vol()
{
	int indevice;
	int volume = 90;

	open_mixer();
	ioctl(desc_mixer, MIXER_WRITE(SOUND_MIXER_RECLEV), &volume);

	indevice = 1 << 0;
	if (ioctl(desc_mixer, SOUND_MIXER_WRITE_RECSRC, &indevice) == -1) {
		perror("mixer");
		exit(1);
	}
}
