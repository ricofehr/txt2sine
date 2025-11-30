#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include "iofile.h"

/*
*	open_file_ro - Open an file in readonly mode
*	@path: the file path to open
*	return the file descriptor or exit if error occurs
*/
int open_file_ro(char *path)
{
	int ret;
	char buferr[128];

	if ((ret = open(path, O_RDONLY)) == -1) {
		sprintf(buferr, "open_file_ro %s failed", path);
		perror(buferr);
		exit(1);
	}

	return ret;
}

/*
*	create_file - Create a new file
*	@path: the file path to create
*	return the file descriptor or exit if error occurs
*/
int create_file(char *path)
{
	int ret;

	if ((ret = open(path, O_CREAT|O_WRONLY|O_TRUNC, 0777)) == -1) {
		perror("create_file failed");
		exit(1);
	}

	return ret;
}

/*
*	open_file_wr - Open an file in writeonly mode
*	@path: the file path to open
*	@truncate: boollean, if need truncate file
*	return the file descriptor or exit if error occurs
*/
int open_file_wr(char *path, int truncate)
{
	int ret;
	char buferr[128];

	if (!truncate) {
		if ((ret = open(path, O_WRONLY)) == -1) {
			sprintf(buferr, "open_file_wr %s failed", path);
			perror(buferr);
			exit(1);
		}
	} else {
		if ((ret = open(path, O_WRONLY | O_TRUNC)) == -1) {
			perror("open_file_wr failed");
			exit(1);
		}
	}

	return ret;
}

/*
*	open_file_rwr - Open an file in read / write mode
*	@path: the file path to open
*	return the file descriptor or exit if error occurs
*/
int open_file_rwr(char *path)
{
	int ret;

	if ((ret = open(path, O_RDWR)) == -1) {
		perror("open_file_rwr failed");
		exit(1);
	}

	return ret;
}

/*
*	read_file - Read chunk of a file
*	@desc: file descriptor
*	@buffer: buffer to fill with the chunk
*	@sz: size of the chunk
*	return the count of reading characters
*
*	Read a chunk of characters from a file (in current position)
*	and fill buffer with this
*/
int read_file(int desc, signed char *buffer, int sz)
{
	return read(desc, buffer, sz*sizeof(unsigned char));
}

/*
*	write_file - Write chunk into a file
*	@desc: file descriptor
*	@buffer: buffer filled with the chunk to write
*	@sz: size of the chunk
*	return the count of writing characters
*
*	Write sz characters from buffer into a file (in current position)
*/
int write_file(int desc, signed char *buffer, int sz)
{
	return write(desc, buffer, sz*sizeof(unsigned char));
}

/*
*	close_file - Close a file
*	@desc: file descriptor
*/
void close_file(int desc)
{
	close(desc);
}
