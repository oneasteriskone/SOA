all:
	gcc -o server_fifo src/server_fifo.c src/sharedBuffer.c src/utilities.c -lrt
	gcc -o server_fork src/server_fork.c src/sharedBuffer.c src/utilities.c -lrt
	gcc -o server_thread src/server_pthread.c src/sharedBuffer.c src/utilities.c -lrt -pthread
	gcc -o server_prefork src/server_prefork.c src/sharedBuffer.c src/utilities.c -lrt

