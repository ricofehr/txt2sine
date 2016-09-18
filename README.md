# txt2sine 

Transmit a text file between 2 computers by sound signal.
The sender side encode binary data into sine chunks and writes them into oss sound device.
The receiver listen the sound device (mic recording), get the signal and decode it onto binary data.

## Compile

Requirements
- Linux operating system
- fftw-devel package (libfftw v3)

On sender side, execute "make encode" for compile binary program
```
$ make encode
gcc -c iofile.c
gcc -c iomixer.c
gcc -c iodsp.c
gcc -I/usr/local/include -c encode.c
gcc -c main_encode.c
gcc -L/usr/local/lib -fexpensive-optimizations -funroll-loops -finline-functions  -ffast-math -Wall -g -O2  -o	bin/decode -export-dynamic iofile.o iomixer.o iodsp.o encode.o main_encode.o -lm -lfftw3
```

On receiver side, execute "make decode" for compile binary program
```
$ make decode
gcc -c iofile.c
gcc -c iomixer.c
gcc -c iodsp.c
gcc -I/usr/local/include -c decode.c
gcc -c main_decode.c
gcc -L/usr/local/lib -fexpensive-optimizations -funroll-loops -finline-functions  -ffast-math -Wall -g -O2  -o	bin/decode -export-dynamic iofile.o iomixer.o iodsp.o decode.o main_decode.o -lm -lfftw3
```

## Folders
```
/		Sources are on the root of Repository
+--bin/		Binary folder where encode / decode executable is written
+--assets/      Output sample (file to send), input sample (file received), and frequencies signal chunk files
```

## Run

You can replace the assets/sampleout file if you want transmit other than default text file.

On receiver side, launch decode binary
```
bin/./decode
```
If you have not oss natively (oss is removed from recent linux distro)
```
padsp bin/./decode
```

After that, you can execute the sender side
```
bin/./encode
```
If you have not oss natively (oss is removed from recent linux distro)
```
padsp bin/./encode
```

## Todo

- Add file path parameter to encode binary for choose any file to transmit
- Increase throughtput (currently, no realistic with some o/s)
- Use alsa library instead of deprecated oss
