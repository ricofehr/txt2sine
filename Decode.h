#ifndef DECODE_H_
#define DECODE_H_

#define NSAMPLES (22050)
#define FFT_LENGTH      4096     /* in samples */
#define PAD_FACTOR      2        /* 1/pf of the FFT samples are real samples, the rest are padding */
#define OVERLAP         4        /* 1/ol samples will be replaced in the buffer every frame. Should be
				  * a multiple of 2 for the Hamming window (see
				  * http://www-ccrma.stanford.edu/~jos/parshl/Choice_Hop_Size.html).
				  */

typedef struct paireDouble
{
	double first;
	double second;
}pair;


extern void initDecode();
void getDataFromDevice();
unsigned char tunerGetChar();
void readChunk();
void applyWindow();
void findPeakMagnitudes();
pair findPeak();
pair interpolatePeak(double ym1, double y0, double y1);
//std::pair<double, double> adjustForOvertones(std::pair<double, double> base, unsigned num_samples);
double binToFreq(double bin);
double freqToBin(double freq);
void initFFTW();
extern void closeDecode();

#endif /*DECODE_H_*/
