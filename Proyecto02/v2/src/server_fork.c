

#include "sharedBuffer.h"
#include "utilities.h"
#define MAX_CHILD_PROCESS 20


#define BUFFER_SIZE 100
#define BUFFER_FILE "/soa_buffer_server_forked"

#define BUFFER_SEM_KEY 0xA61531

int serverSocket, clientSocket;
int num_requests;
struct Buffer* buffer;

void killChilds()
{
  sendKillToChilds(buffer);
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
  int pid;
	switch(triggeredSignal)
  {
		case SIGINT:
		case SIGTERM:
      signal(SIGINT, SIG_DFL);
      signal(SIGTERM, SIG_DFL);
      endServer(0, "Finished server");
			exit(0);
		case SIGUSR1:
      pid = getpid();
      printf("finishing process=[%d] with siguser1.\n", pid);
			closeSocket(clientSocket);
			exit(0);
	}	
}

void manageConnection(int socket, struct sockaddr_in client)
{
  int pid = fork();
  char address[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &(client.sin_addr), address, INET_ADDRSTRLEN);
  if(pid < 0)
    err(EXIT_FAILURE, "Failed creating a child process to handle request from [%s]", address);
  else if(pid == 0)
  {
    struct sigaction userSignal;
    memset (&userSignal, '\0', sizeof(userSignal));
    userSignal.sa_handler = &signalCatcher;
    sigaction(SIGUSR1, &userSignal, NULL);
    signal(SIGINT, SIG_DFL);
    signal(SIGTERM, SIG_DFL);
    pid = getpid();
    printf("Client IP Address=[%s] with pid=[%d]\n", address, pid);
    char requestInfo[REQUEST_INFO_LENGHT];
    recv(socket, requestInfo, REQUEST_INFO_LENGHT, 0);
    printf("%s\n", requestInfo);
    char *fileRequested = getFileRequest(requestInfo);
    if(fileRequested != 0)
      responseRequest(socket, fileRequested, buffer);
    closeSocket(socket);
    socket = 0;
    removeValueFromBuffer(buffer, pid);
    exit(0);
  }
  if(0 == pushValueInBuffer(buffer, pid))
    endServer(EXIT_FAILURE, "Couldn't push the process id of the forked process");

  close(socket);
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
