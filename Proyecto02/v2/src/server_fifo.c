

//#include "sharedBuffer.h"
#include "utilities.h"

int serverSocket, clientSocket;
int num_requests;

void signalCatcher(int triggeredSignal)
{
	switch(triggeredSignal)
  {
		case SIGINT:
			closeSocket(serverSocket);
      if(clientSocket != 0)
        closeSocket(clientSocket);
			exit(0);
	}
}

void manageConnection(int socket, struct sockaddr_in client)
{
  char address[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &(client.sin_addr), address, INET_ADDRSTRLEN);
  printf("Client IP Address=[%s]\n", address);
  char requestInfo[REQUEST_INFO_LENGHT];
  recv(socket, requestInfo, REQUEST_INFO_LENGHT, 0);
	char *fileRequested = getFileRequest(requestInfo);
  if(fileRequested != 0)
    responseRequest(socket, fileRequested, 0);
  closeSocket(socket);
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
  printf("Server initialized on port=[%d].\n", port);
  signal(SIGINT, signalCatcher);
  runServer();
  return 0;
}
