# Makefile for encode and decode binary executable
# -------------------

objd = IOFile.o IOMixer.o IODsp.o Decode.o maindecode.o
obje = IOFile.o IOMixer.o IODsp.o Encode.o mainencode.o

"all" : encode
"encode" : encode
"decode" : decode

encode : $(obje)
	gcc -L/usr/local/lib -fexpensive-optimizations -funroll-loops -finline-functions  -ffast-math -Wall -g -O2  -o	bin/decode -export-dynamic $(obje) -lm -lfftw3
decode : $(objd)
	gcc -L/usr/local/lib -fexpensive-optimizations -funroll-loops -finline-functions  -ffast-math -Wall -g -O2  -o	bin/decode -export-dynamic $(objd) -lm -lfftw3
IOFile.o: IOFile.c IOFile.h
	gcc -c IOFile.c
IOMixer.o: IOMixer.c IOMixer.h IOFile.h
	gcc -c IOMixer.c
IODsp.o: IODsp.c IODsp.h IOFile.h
	gcc -c IODsp.c
Decode.o: Decode.c Decode.h IODsp.h IOMixer.h
	gcc -I/usr/local/include -c Decode.c
maindecode.o:maindecode.c Decode.h IOFile.h
	gcc -c maindecode.c
Encode.o: Encode.c Encode.h IODsp.h IOMixer.h
	gcc -I/usr/local/include -c Decode.c
mainencode.o: mainencode.c Encode.h IOFile.h
	gcc -c mainencode.c
