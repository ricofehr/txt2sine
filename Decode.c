#include <math.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>

/* Fourier Transform Header */
#include <fftw3.h>

#include "Decode.h"
#include "IODsp.h"
#include "IOMixer.h"

extern int speed;
extern double frequency;
extern int format;
extern int stereo;

/** Global variables **/
double *in, *in_windowed;
fftw_complex *out;
double *bins;
fftw_plan p;
pair peak;
signed char *bigbuf;

int freq;

/** Main function **/
void getDataFromDevice()
{
    readChunk();
    applyWindow();
    fftw_execute(p);
    findPeakMagnitudes();
	peak = findPeak();
	if (peak.first < 40.0 || peak.second - log10(FFT_LENGTH) < 0.0) {
		freq = 0;
	}
	else {
	    freq = (int)peak.first;
	}
}

/** Read sound interval **/
void readChunk()
{
	int ret,descin;
	unsigned to_read = FFT_LENGTH / PAD_FACTOR / OVERLAP, i;
	signed char buf[to_read];

	/** move sound segment **/
	memmove(in, in + to_read, (FFT_LENGTH / PAD_FACTOR - to_read) * sizeof(double));
	memmove(bigbuf, bigbuf + to_read, (FFT_LENGTH / PAD_FACTOR - to_read) * sizeof(signed char));

	ret = readDSP(buf, to_read * sizeof(signed char));
	if (ret == 0) {
		printf("EOF\n");
		exit(0);
	}

	if (ret != (int) to_read * sizeof(signed char)) {
		printf("ret:%d, to_read:%d, %d\n\n", ret, to_read*sizeof(signed char), to_read);
		// blah
		perror("read");
		exit(1);
	}

	for (i = 0; i < to_read; i++)
	{
		in[i + (FFT_LENGTH / PAD_FACTOR - to_read)] = (double) buf[i];
		bigbuf[i + (FFT_LENGTH / PAD_FACTOR - to_read)] = buf[i];
	}
}

/*
 * Apply hamming window
 *
 */
void applyWindow()
{
	static double *win_data;
	static unsigned win_len;
	static unsigned win_inited = 0;
	unsigned i;

	// Initialize the window for the first time
	if (!win_inited) {
		win_len = FFT_LENGTH / PAD_FACTOR;
		win_data = (double *)malloc(win_len*sizeof(double));

		for (i = 0; i < win_len; i++) {
			//win_data[i] = 0.54 - 0.46 * cos(2.0 * M_PI * double(i) / double(win_len - 1));
			win_data[i] = 0.42 - 0.5*cos(2.0*M_PI*(double) i / (double) (win_len - 1)) + 0.08*cos(4.0*M_PI*(double)i / (double)(win_len - 1));
		}

		win_inited = 1;
	}

	assert(win_len == FFT_LENGTH / PAD_FACTOR);

	for (i = 0; i < win_len; i++) {
		//out[i] = in[i] * win_data[i];
		in_windowed[i] = in[i] * win_data[i];
	}
	for (i = win_len; i < FFT_LENGTH; i++) {
		//out[i] = 0.0;
		in_windowed[i] = 0.0;
	}
}

/*
 * Calcule le module de la sortie de la transformé.
 * On a alors les amplitudes des différentes fréquence.
 */
void findPeakMagnitudes()
{
	unsigned i;
	for (i = 0; i < FFT_LENGTH / 2+1; i++)
	{
		//calcul module du ieme complexe du tableau out
		bins[i] = sqrt(out[i][0]*out[i][0]+out[i][1]*out[i][1]);
	}
}

/*
 * Cherche le peak.
 * bins contient les amplitudes des harmoniques.
 * A partir de l index du peak, on retrouve la fréquence associé aux amplitudes (voir fction binTofreq())
 */
pair findPeak()
{
	double best_peak = bins[5];
	unsigned best_bin = 5, i;
	pair peakret;

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

	peakret = interpolatePeak(bins[best_bin - 1],
				 bins[best_bin],
				 bins[best_bin + 1]);
	peakret.first = binToFreq((double)best_bin + peakret.first);
	//fprintf(stderr, "Freq: %d, ym1:%d, y0:%d, y1:%d\n",(int)peakret.first, bins[best_bin - 1], bins[best_bin], bins[best_bin + 1]);

	return peakret;
}

/*
 * lors de l execution de la fft on a trouvé le peak en y0
 * Or le delta entre deux echantillons est de fs/N où fs est la fréquence d echnatillonage et N la taille de la FFT.
 * On est donc à une précision de +- (fs/N)/2 par rapport à y0 pour le peak réel.
 * On forme alors un repère orthonormal avec f(0)=y0, f(-1)=ym1 et f(1)=y1.
 * Par rapport à ce repère, on cherche à trouver les coordonnées du point maximum tel que Y= ax^2 + bx + c.
 * Cette fonction pose donc le repère, calcule a,b,c puis xmax et ymax et retourne la paire (xmax,ymax) avec ymax convertit en décibel (d'où le 20*log10()).
 * La peak se situe donc à l'index xoriginal+xmax (effectuer bintofreq pour retrouver la fréquence correcpondante) pour une amplitude de ymax décibels.
 *
 * Voir http://www-ccrma.stanford.edu/~jos/parshl/Peak_Detection_Steps_3.html
 */
pair interpolatePeak(double ym1, double y0, double y1)
{
	pair ret;

	ym1 = log10(ym1);
	y0 = log10(y0);
	y1 = log10(y1);

#if 0
	assert(y0 >= y1);
	assert(y0 >= ym1);
#endif

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
 * Obtient la fréquence à partir de l'index du poids de cette dernière.
 */
double binToFreq(double bin)
{
	return bin * speed / (double)FFT_LENGTH;
}

/*
 * Obtient l'index du poids de la fréquence à partir de cette dernière.
 */
double freqToBin(double freq)
{
	return freq * (double)FFT_LENGTH / (double)speed;
}

/*
 * initialise le decodeur
 */
void initDecode() {
	unsigned int i;
	int max_fragments = 2;
	int frag_shift = ffs(FFT_LENGTH / OVERLAP) - 1;
	int fragments = (max_fragments << 16) | frag_shift;

	//initVol();
	initMicro();
	initFFTW();
	openDSP(stereo, speed, format, 0, fragments);
}

/*
 * Init Fourier transform
 */
void initFFTW()
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
 * Freq to character function
 */
unsigned char tunerGetChar() {
	int descin, filefreq, sz;
	char *command;
	double delta = 0;
	unsigned int i, j=0;

	getDataFromDevice();

	if(freq < 40)return 255;
	fprintf(stderr, "Freq: %d, Db:%d\n",(int)peak.first, (int)peak.second);

	for(i=0; i<256; i++)
	{
		if(freq >= (40+i*40-20) && freq < (40+(i)*40+20))
		{
			fprintf(stderr, "Freq: %u ",(int)freq);
			fprintf(stderr, "NSAMPLES:%d:%d:%c\n", speed,i,i);

			descin = open("freq/current", O_CREAT | O_WRONLY | O_TRUNC, 0777);
			while(j++<(speed/FFT_LENGTH/PAD_FACTOR)+1)write(descin, bigbuf, FFT_LENGTH / PAD_FACTOR*sizeof(signed char));
			close(descin);

			filefreq = 40+i*40;

			sz = 2*2;
			if(filefreq > 99)sz = 3*2;
			if(filefreq > 999)sz = 4*2;
			if(filefreq > 9999)sz = 5*2;
			sz += 49;
			command = (char*)malloc((sz+1)*sizeof(char));
			sprintf(command, "if [ ! -f freq/%d ]; then mv freq/current freq/%d; fi", filefreq, filefreq);
			command[sz] = '\0';

			printf("%s\n", command);
			system(command);
			free(command);

			return i;
		}
	}

	return 255;
}

/** Close sound device **/
void closeDecode()
{
	closeDSP();
}
