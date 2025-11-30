#ifndef ENCODE_H_
#define ENCODE_H_

#define NSAMPLES (22050)

extern void initEncode() ;
extern void writeDatas(unsigned char *buffer) ;
void writeChar(unsigned char c) ;
extern void closeEncode() ;

#endif /*ENCODE_H_*/
