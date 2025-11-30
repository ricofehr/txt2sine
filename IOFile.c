#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include "IOFile.h"

int openFileRO(char *path)
{
	int ret ;
	if((ret = open(path, O_RDONLY)) == -1) 
	{
		perror("openFileRO failed");
		exit(1);
	}
	
	return ret ;
}

int createFile(char *path)
{
	int ret ;
	if((ret = open(path, O_CREAT|O_WRONLY|O_TRUNC, 0777)) == -1) 
	{
		perror("createFile failed");
		exit(1);
	}
	
	return ret ;
}

int openFileWR(char *path, int truncate)
{
	int ret ;
	
	if(!truncate)
	{
		if((ret = open(path, O_WRONLY)) == -1) 
		{
			perror("openFileWR failed");
			exit(1);
		}
	}
	else
	{
		if((ret = open(path, O_WRONLY | O_TRUNC)) == -1) 
		{
			perror("openFileWR failed");
			exit(1);
		}
	}
	
	return ret ;
}

int openFileROWR(char *path)
{
	int ret ;
		
	if((ret = open(path, O_RDWR)) == -1) 
	{
		perror("openFileROWR failed");
		exit(1);
	}
	
	return ret ;
}

int readFile(int desc, signed char *buffer, int sz)
{
	return read(desc, buffer, sz*sizeof(unsigned char)) ;
}

int writeFile(int desc, signed char *buffer, int sz)
{
	return write(desc, buffer, sz*sizeof(unsigned char)) ;
}

void closeFile(int desc)
{
	close(desc) ;	
}