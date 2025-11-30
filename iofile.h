#ifndef _IOFILE_H_
#define _IOFILE_H_

int create_file(char *path);
int open_file_ro(char *path);
int open_file_wr(char *path, int truncate);
int open_file_rwr(char *path);
int read_file(int desc, short *buffer, int sz);
int write_file(int desc, short *buffer, int sz);
void close_file(int desc);

#endif /*_IOFILE_H_*/
