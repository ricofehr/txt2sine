#include <math.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>

/* Fourier Transform Header */
#include <fftw3.h>

#include "decode.h"
#include "iodsp.h"
#include "iomixer.h"

/* Define program global variables */
const char *names[SOUND_MIXER_NRDEVICES] = SOUND_DEVICE_NAMES;
int speed = 22050;
double frequency = 1;
int format = AFMT_U8;
int stereo = 1;

/* Global variables only for decode.c */
static double *in, *in_windowed;
static fftw_complex *out;
static double *bins;
static fftw_plan p;
static struct pair peak;
static signed char *bigbuf;
static int freq;


/*
*	read_chunk - Read sound interval
*
*	Read a chunk from dsp device and prepare fftw plan
*/
static void read_chunk()
{
	int ret, descin;
	unsigned to_read = FFT_LENGTH / PAD_FACTOR / OVERLAP, i;
	signed char buf[to_read];

	/** move sound segment **/
	memmove(in, in + to_read, (FFT_LENGTH / PAD_FACTOR - to_read) * sizeof(double));
	memmove(bigbuf, bigbuf + to_read, (FFT_LENGTH / PAD_FACTOR - to_read) * sizeof(signed char));

	ret = read_dsp(buf, to_read * sizeof(signed char));
	if (ret == 0) {
		printf("EOF\n");
		exit(0);
	}

	to_read = ret;
	//printf("to_read:%d\n", to_read);
	if (ret != (int) to_read * sizeof(signed char)) {
		printf("ret:%d, to_read:%d, %d\n\n", ret, to_read*sizeof(signed char), to_read);
		perror("read");
		exit(1);
	}

	for (i = 0; i < to_read; i++) {
		in[i + (FFT_LENGTH / PAD_FACTOR - to_read)] = (double) buf[i];
		bigbuf[i + (FFT_LENGTH / PAD_FACTOR - to_read)] = buf[i];
	}
}

/*
*	apply_window - Apply hamming window
*
*	Compute hamming window on the signal datas before fftw
*/
static void apply_window()
{
	static double *win_data;
	static unsigned win_len;
	static unsigned win_inited = 0;
	unsigned i;

	/* Initialize the window for the first time */
	if (!win_inited) {
		win_len = FFT_LENGTH / PAD_FACTOR;
		win_data = (double *) malloc(win_len * sizeof(double));

		for (i = 0; i < win_len; i++) {
			win_data[i] = 0.42 - 0.5 * cos(2.0 * M_PI * (double) i / (double) (win_len - 1))
			              + 0.08 * cos(4.0 * M_PI * (double) i / (double) (win_len - 1));
		}

		win_inited = 1;
	}

	assert(win_len == FFT_LENGTH / PAD_FACTOR);

	for (i = 0; i < win_len; i++) {
		in_windowed[i] = in[i] * win_data[i];
	}
	for (i = win_len; i < FFT_LENGTH; i++) {
		in_windowed[i] = 0.0;
	}
}

/*
 *	find_peak_magnitudes - Computes magnitude of frequencies
 *
 *	From fftw results, computes the magnitude of frequencies
 */
static void find_peak_magnitudes()
{
	unsigned i;
	for (i = 0; i < FFT_LENGTH / 2+1; i++) {
		bins[i] = sqrt(out[i][0] * out[i][0] + out[i][1] * out[i][1]);
	}
}

/*
*	interpolate_peak - Find peak with more accuracy
*
*	See http://www-ccrma.stanford.edu/~jos/parshl/Peak_Detection_Steps_3.html
*/
static struct pair interpolate_peak(double ym1, double y0, double y1)
{
	struct pair ret;

	ym1 = log10(ym1);
	y0 = log10(y0);
	y1 = log10(y1);

	double a = 0.5 * y1 + 0.5 * ym1 - y0;
	double b = 0.5 * y1 - 0.5 * ym1;
	double c = y0;

	double xmax = (ym1 - y1) / (2.0 * (y1 + ym1 - 2.0 * y0));
	double ymax = 20.0 * (a * xmax * xmax + b * xmax + c) - 90.0;

	ret.first = xmax;
	ret.second = ymax;

	return ret;
}

/*
 *	bin_to_freq - compute frequence from binary index
 *
 *	Compute frequence from the weigth index of this one
 */
static double bin_to_freq(double bin)
{
	return bin * speed / (double)FFT_LENGTH;
}

/*
 *	freq_to_bin - compute weigth index from frequence
 *
 *	Compute weigth index of frequence from the level of this one
 */
static double freq_to_bin(double freq)
{
	return freq * (double)FFT_LENGTH / (double)speed;
}

/*
 *	init_fftw - Init Fourier transform
 *
 *	Prepare the fftw with windowed and complex datas
 */
static void init_fftw()
{
	in = (double *) (fftw_malloc(sizeof(double) * FFT_LENGTH / PAD_FACTOR));
	in_windowed = (double *) (fftw_malloc(sizeof(double) * FFT_LENGTH));
	out = (fftw_complex *) (fftw_malloc(sizeof(fftw_complex) * (FFT_LENGTH / 2 + 1)));
	bins = (double *) (fftw_malloc(sizeof(double) * (FFT_LENGTH / 2 + 1)));
	bigbuf = (signed char*) malloc(sizeof(signed char) * FFT_LENGTH / PAD_FACTOR);

	memset(in, 0, sizeof(double) * FFT_LENGTH / PAD_FACTOR);
	memset(bigbuf, '0', sizeof(signed char) * FFT_LENGTH / PAD_FACTOR);

	p = fftw_plan_dft_r2c_1d(FFT_LENGTH, in_windowed, out, FFTW_ESTIMATE);
}

/*
*	find_peak - From fftw results, find the peak
*/
static struct pair find_peak()
{
	double best_peak = bins[5];
	unsigned best_bin = 5, i;
	struct pair peakret;

	for (i = 2; i < FFT_LENGTH/2+1; i++) {
		if (bins[i] > best_peak) {
			best_peak = bins[i];
			best_bin = i;
		}
	}

	if (best_bin == 0 || best_bin == FFT_LENGTH/2) {
		peakret.first = -1.0;
		peakret.second = 0.0;
		return peakret;
	}

	// see if we might have hit an overtone (set a limit of 5dB)
	/*
	for (unsigned i = 4; i >= 1; --i) {
		if (best_bin != best_bin / i &&
		    20.0 * log10(bins[best_bin] / bins[best_bin / i]) < 5.0f) {
			best_bin /= i;

			// consider sliding one bin up or down
			if (best_bin > 1 && bins[best_bin - 1] > bins[best_bin] && bins[best_bin - 1] > bins[best_bin - 2]) {
				--best_bin;
			} else if (best_bin < num_samples / 2 - 1 && bins[best_bin + 1] > bins[best_bin] && bins[best_bin + 1] > bins[best_bin + 2]) {
				++best_bin;
			}

			break;
		}
	}
*/
	if (best_bin == 0 || best_bin == FFT_LENGTH / 2)
	{
		peakret.first = -1.0;
		peakret.second = 0.0;
		return peakret;
	}

	peakret = interpolate_peak(bins[best_bin - 1],
				 bins[best_bin],
				 bins[best_bin + 1]);
	peakret.first = bin_to_freq((double)best_bin + peakret.first);
	// fprintf(stderr, "Freq: %d, ym1:%d, y0:%d, y1:%d\n",(int)peakret.first, bins[best_bin - 1], bins[best_bin], bins[best_bin + 1]);

	return peakret;
}

/*
 *	tuner_get_char - Compute a character from a chunk of datas signal
 */
unsigned char tuner_get_char() {
	int descin, filefreq, sz;
	char command[128];
	double delta = 0;
	unsigned int i, j=0;

	get_data_from_device();

	if(freq < 40)
		return 255;
	fprintf(stderr, "Freq: %d, Db:%d\n", (int)peak.first, (int)peak.second);

	for(i=0; i<256; i++) {
		if(freq >= (40 + i*40 - 20) && freq < (40 + i*40 + 20)) {
			fprintf(stderr, "Freq: %u ", (int)freq);
			fprintf(stderr, "NSAMPLES:%d:%d:%c\n", speed, i, i);
			descin = open("assets/freq/current", O_CREAT | O_WRONLY | O_TRUNC, 0777);
			while(j++ < (speed / FFT_LENGTH / PAD_FACTOR) + 1)
				write(descin, bigbuf, FFT_LENGTH / PAD_FACTOR * sizeof(signed char));
			close(descin);

			filefreq = 40 + i*40;

			sprintf(command, "if [ ! -f assets/freq/%d ]; then mv assets/freq/current assets/freq/%d; fi", filefreq, filefreq);
			printf("%s\n", command);
			system(command);
			return i;
		}
	}

	return 255;
}

/*
*	get_data_from_device - Read a chunk and decode it
*/
void get_data_from_device()
{
	read_chunk();
	apply_window();
	fftw_execute(p);
	find_peak_magnitudes();
	peak = find_peak();
	//if (peak.first > 1000 && peak.second > 0.0)
	//	printf("first: %f, second: %f\n", peak.first, peak.second);
	if (peak.first < 40.0 || peak.second - log10(FFT_LENGTH) < 0.0)
		freq = 0;
	else
	    freq = (int) peak.first;
}

/*
*	init_decode - Open and init sound device
*/
void init_decode() {
	unsigned int i;
	int max_fragments = 2;
	int frag_shift = ffs(FFT_LENGTH / OVERLAP) - 1;
	int fragments = (max_fragments << 16) | frag_shift;

	//init_vol();
	init_micro();
	init_fftw();
	open_dsp(stereo, speed, format, 0, fragments);
}

/*
*	close_decode - Close sound device
*/
void close_decode()
{
	close_dsp();
}
