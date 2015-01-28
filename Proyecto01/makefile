
all:
	gcc -g -m32 -std=c11 src/creador.c -o creador -D_XOPEN_SOURCE=600 -lrt
	gcc -m32 -std=c11 src/semsUtils_test.c -o semsUtils_test -D_SVID_SOURCE=1
	gcc -g -m32 -std=c11 src/productor.c -o productor -lm -D_XOPEN_SOURCE=600 -lrt
	gcc -g -m32 -std=c11 src/consumidor.c -o consumidor -lm -D_XOPEN_SOURCE=600 -lrt
