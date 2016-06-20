#ifndef IOFILE_H_
#define IOFILE_H_

extern int createFile(char *path) ;
extern int openFileRO(char *path) ;
extern int openFileWR(char *path, int truncate) ;
extern int openFileROWR(char *path) ;
extern int readFile(int desc, signed char *buffer, int sz) ;
extern int writeFile(int desc, signed char *buffer, int sz) ;
extern void closeFile(int desc) ;

#endif /*IOFILE_H_*/
