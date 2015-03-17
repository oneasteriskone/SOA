
#include "sharedBuffer.h"
#include "utilities.h"
#define MAX_CHILD_PROCESS 20


#define BUFFER_FILE "/soa_buffer_server_preforked"
#define POOL_SIZE_DEFAULT 20

#define BUFFER_SEM_KEY 0xA61532

int serverSocket;
//int clientSocket;
int num_requests;
struct Buffer* buffer;

int semGo;
int semActive;
int* childPids;
int* serverPipes;
int* clientPipes;
int poolSize;

void killChilds()
{
  sendKillToChilds(buffer);
}

void endServer(int code, char* message)
{
  warn("Finishing Server...");
  killChilds();
  destroyBuffer(buffer);
  destroySemaphore(semGo);
  destroySemaphore(semActive);
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

void serveConnection(void* data)
{
  int index = *((int*)data);
  free(data);
  //struct sigaction userSignal;
  //memset (&userSignal, '\0', sizeof(userSignal));
  //userSignal.sa_handler = &signalCatcher;
  //sigaction(SIGUSR1, &userSignal, NULL);
  while(1)
  {
    waitSemaphore(semGo);
    int clientSocket = popValueFromBuffer(buffer);
    char requestInfo[REQUEST_INFO_LENGHT];
    recv(clientSocket, requestInfo, REQUEST_INFO_LENGHT, 0);
    printf("%s\n", requestInfo);
    char *fileRequested = getFileRequest(requestInfo);
    printf("fileRequested=[%s]\n", fileRequested);
    if(fileRequested != 0)
      responseRequest(clientSocket, fileRequested, buffer);
    closeSocket(clientSocket);
    signalSemaphore(semActive);
  }
  exit(0);
}

void initPool(struct Buffer* buffer, int poolSize)
{
  int i;
  int pid;
  semGo = createSemaphore(BUFFER_SEM_KEY + 1, 0);
  semActive = createSemaphore(BUFFER_SEM_KEY + 2, poolSize);
  childPids = (int*)calloc(poolSize, sizeof(int));
  for(i = 0 ; i < poolSize ; i++)
  {
    pthread_t id;
    int* index = (int*)calloc(1, sizeof(int));
    (*index) = i;
    pthread_create(&id, 0, &serveConnection, (void*)index);
    childPids[i] = id;
  }
}

void manageConnection(int socket, struct sockaddr_in client)
{
  pushValueInBuffer(buffer, socket);
  signalSemaphore(semGo);
  //close(socket);
  waitSemaphore(semActive);
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
    int clientSocket = accept(serverSocket, (struct sockaddr *)&client, &sin_size);
    fcntl(clientSocket, F_SETFD, fcntl(serverSocket, F_GETFD) | FD_CLOEXEC);
    if(clientSocket < 0)
    {
      closeSocket(serverSocket);
      err(EXIT_FAILURE, "Error on accepting a connection in the socket");
    }
    num_requests++;
    manageConnection(clientSocket, client);
  }
}

int main(int argc, char* argv[])
{
  poolSize = POOL_SIZE_DEFAULT;
  if(argc < 3)
    err(1, "You must give the port, the root path and the size of the pool of process to the server");
  if(argc == 4)
    poolSize = atoi(argv[3]);
  else
    printf("No pool size given in the parameter, using %d as default.\n", POOL_SIZE_DEFAULT);
  int port = atoi(argv[1]);
  url = argv[2];

  serverSocket = bootServer(port);
  buffer = initBuffer(poolSize, 0, BUFFER_SEM_KEY);
  initPool(buffer, poolSize);
  printf("Server initialized on port=[%d].\n", port);
  signal(SIGINT, signalCatcher);
  signal(SIGTERM, signalCatcher);
  runServer();
  return 0;
}
