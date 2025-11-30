# Makefile for encode and decode binary executable
# -------------------

objd = iofile.o iomixer.o iodsp.o decode.o main_decode.o
obje = iofile.o iomixer.o iodsp.o encode.o main_encode.o

"all" : encode
"encode" : encode
"decode" : decode

encode : $(obje)
	$(shell test -f assets/sampleout || cp assets/sampleout.dist assets/sampleout)
	gcc -L/usr/local/lib -fexpensive-optimizations -funroll-loops -finline-functions  -ffast-math -Wall -g -O2  -o	bin/encode -export-dynamic $(obje) -lm
decode : $(objd)
	gcc -L/usr/local/lib -fexpensive-optimizations -funroll-loops -finline-functions  -ffast-math -Wall -g -O2  -o	bin/decode -export-dynamic $(objd) -lm -lfftw3
iofile.o: iofile.c iofile.h
	gcc -c iofile.c
iomixer.o: iomixer.c iomixer.h iofile.h
	gcc -c iomixer.c
iodsp.o: iodsp.c iodsp.h iofile.h
	gcc -c iodsp.c
decode.o: decode.c decode.h iodsp.h iomixer.h
	gcc -I/usr/local/include -c decode.c
main_decode.o:main_decode.c decode.h iofile.h
	gcc -c main_decode.c
encode.o: encode.c encode.h iodsp.h iomixer.h
	gcc -I/usr/local/include -c encode.c
main_encode.o: main_encode.c encode.h iofile.h
	gcc -c main_encode.c
