#!/bin/bash

# Run this script in the source directory of an APP
# In sqlite - xv6fs (instrumented) - ramdisk (instrucmented) experiments 
# on Linux with shared memory IPC (busy polling on different cores),
# the overhead is around 4% ~ 6%.

CC=../../../build/bin/musl-gcc
CFLAGS="-DFUSEFS -Wall -O3 -static -fPIC -D_GNU_SOURCE -g"
target=newfs

for i in `ls *.c`; do
	filename=${i%%.*}.s
	echo "compiling ${filename}";
	${CC} ${CFLAGS} -S $i
	mv ${filename} tmp
	./rewrite tmp ${filename}
	rm -f tmp
done

${CC} ${CFLAGS} -o ${target} *.s
