#ifndef ENCODE_H_
#define ENCODE_H_

#define NSAMPLES (8192)

/* Declares global variables */
extern const int speed;
extern const double frequency;
extern const int format;
extern const int stereo;

void init_encode();
void write_datas(unsigned char *buffer);
void close_encode();

#endif /*ENCODE_H_*/
