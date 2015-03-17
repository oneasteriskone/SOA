

#include <pthread.h>
#include "sharedBuffer.h"
#include "utilities.h"
#define MAX_CHILD_PROCESS 20


#define BUFFER_SIZE 100
#define BUFFER_FILE 0

#define BUFFER_SEM_KEY 0xA61532

int serverSocket, clientSocket;
int num_requests;
struct Buffer* buffer;

struct threadInfo
{
  int socket;
  pthread_t id;
  struct sockaddr_in client;
};

void killChilds()
{
  sendKillToChilds(buffer);
  waitForChildsToFinish(buffer);
}

void endServer(int code, char* message)
{
  warn("Finishing Server!!");
  killChilds();
  destroyBuffer(buffer);
  closeSocket(serverSocket);
  if(0 == code)
    warn(message);
  else
    err(code, message);
}

void signalCatcher(int triggeredSignal)
{
	switch(triggeredSignal)
  {
		case SIGINT:
		case SIGTERM:
      signal(SIGINT, SIG_DFL);
      signal(SIGTERM, SIG_DFL);
      endServer(0, "Finished server");
			exit(0);
	}	
}

void* serveConnection(void* data)
{
  struct threadInfo* thread = (struct threadInfo*)data;
  pushValueInBuffer(buffer, thread->id);
  char requestInfo[REQUEST_INFO_LENGHT];
  recv(thread->socket, requestInfo, REQUEST_INFO_LENGHT, 0);
  char *fileRequested = getFileRequest(requestInfo);
  if(fileRequested != 0)
    responseRequest(thread->socket, fileRequested, buffer);
  closeSocket(thread->socket);
  removeValueFromBuffer(buffer, thread->id);
  free(data);
  return 0;
}

void manageConnection(int socket, struct sockaddr_in client)
{
  struct threadInfo* thread = (struct threadInfo*)calloc(1, sizeof(struct threadInfo));
  thread->socket = socket;
  thread->client = client;
  pthread_create(&(thread->id), 0, &serveConnection, (void*)thread);
}

void runServer()
{
  while(1)
  {
    printf("Waiting...\n");
    if(listen(serverSocket, 10) == -1)
    {
      closeSocket(serverSocket);
      err(EXIT_FAILURE, "Error on listening the socket");
    }
    struct sockaddr_in client;
    socklen_t sin_size = sizeof(struct sockaddr_in);
    clientSocket = accept(serverSocket, (struct sockaddr *)&client, &sin_size);
    if(clientSocket < 0)
    {
      closeSocket(serverSocket);
      err(EXIT_FAILURE, "Error on accepting a connection in the socket");
    }
    num_requests++;
    manageConnection(clientSocket, client);
    clientSocket = 0;
  }
}

int main(int argc, char* argv[])
{
  if(argc != 3)
    err(1, "You must give the port and the root path parameters to the server");
  int port = atoi(argv[1]);
  url = argv[2];

  serverSocket = bootServer(port);
  buffer = initBuffer(BUFFER_SIZE, BUFFER_FILE, BUFFER_SEM_KEY);
  printf("Server initialized on port=[%d].\n", port);
  signal(SIGINT, signalCatcher);
  signal(SIGTERM, signalCatcher);
  runServer();
  return 0;
}
