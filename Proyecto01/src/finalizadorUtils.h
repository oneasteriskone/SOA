#ifndef FINALIZADOR_UTILS
#define FINALIZADOR_UTILS

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>

#define SHMSZ     27

void createFinalFlag(){
    char c;
    int shmid;
    key_t key = 5678;
    char *shm, *s;

    if ((shmid = shmget(key, SHMSZ, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
    }

    if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat");
        exit(1);
    }

    s = shm;
    *s = 0;
}

int getFinalFlag(){
	char c;
    int shmid;
    key_t key = 5678;
    char *shm, *s;

    if ((shmid = shmget(key, SHMSZ, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
    }

    if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat");
        exit(1);
    }

    return *shm;
}

void raiseFinalFlag(){
	int shmid;
    key_t key = 5678;
    char *shm, *s;

    if ((shmid = shmget(key, SHMSZ, 0666)) < 0) {
        perror("shmget");
        exit(1);
    }

    if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat");
        exit(1);
    }

    *shm = 1;
}
#endif