#ifndef UTILITIES_H
#define UTILITIES_H

#define MIMEFILE "TablaMIME.list"
#define MIMEFILE_MAX_ENTRIES 700
#define EXTENSION_MAX_LENGHT 10
#define MIME_MAX_LENGHT 70
#define MIMEFILE_LINE_MAX_LENGHT EXTENSION_MAX_LENGHT + MIME_MAX_LENGHT + 1
#define REQUEST_INFO_LENGHT 1024

#define RESPONSE_BUFFER_SIZE 256

#define SOFT404 "soft404.html"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <err.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <sys/stat.h>

struct Buffer;
int mustDie(struct Buffer* buffer);


struct threadInfo
{
  int socket;
  pthread_t id;
  struct sockaddr_in client;
};

char* url;

long getFileLength(char* fileRequested);
char* ltoa(long integer);
void sendNotFoundResponse(int socket);
void responseRequest(int socket, char* fileRequested, struct Buffer* buffer);
void closeSocket(int socket);
int bootServer(int port);
void sendFile(int socket, FILE* file, struct Buffer* buffer);
void sendHeader(int socket, char* mime, long contentLength);
char* getFileRequest(char* http);
char* extractFileFromRequest(char* request);
char* getFileExtension(char* fileName);
char* getMIMEType(char* ext);
char* getDate();

#endif
