
#include "sharedBuffer.h"
#include "utilities.h"
#define MAX_CHILD_PROCESS 20


#define BUFFER_FILE "/soa_buffer_server_preforked"
#define POOL_SIZE_DEFAULT 20

#define BUFFER_SEM_KEY 0xA61532

#define MAXLINE 4096


#include <sys/socket.h>
#define CONTROLLEN  CMSG_LEN(sizeof(int))

static struct cmsghdr   *cmptr = NULL;

int send_fd(int fd, int fd_to_send)
{
    struct iovec    iov[1];
    struct msghdr   msg;
    char            buf[2];

    iov[0].iov_base = buf;
    iov[0].iov_len  = 2;
    msg.msg_iov     = iov;
    msg.msg_iovlen  = 1;
    msg.msg_name    = NULL;
    msg.msg_namelen = 0;
    if (fd_to_send < 0) {
        msg.msg_control    = NULL;
        msg.msg_controllen = 0;
        buf[1] = -fd_to_send;
        if (buf[1] == 0)
            buf[1] = 1;
    } else {
        if (cmptr == NULL && (cmptr = malloc(CONTROLLEN)) == NULL)
            return(-1);
        cmptr->cmsg_level  = SOL_SOCKET;
        cmptr->cmsg_type   = SCM_RIGHTS;
        cmptr->cmsg_len    = CONTROLLEN;
        msg.msg_control    = cmptr;
        msg.msg_controllen = CONTROLLEN;
        *(int*)CMSG_DATA(cmptr) = fd_to_send;
        buf[1] = 0;
    }
    buf[0] = 0;
    if (sendmsg(fd, &msg, 0) != 2)
        return(-1);
    return(0);
}

int recv_fd(int fd)
{
   int             newfd, nr, status;
   char            *ptr;
   char            buf[MAXLINE];
   struct iovec    iov[1];
   struct msghdr   msg;

   status = -1;
   for ( ; ; ) {
       iov[0].iov_base = buf;
       iov[0].iov_len  = sizeof(buf);
       msg.msg_iov     = iov;
       msg.msg_iovlen  = 1;
       msg.msg_name    = NULL;
       msg.msg_namelen = 0;
       if (cmptr == NULL && (cmptr = malloc(CONTROLLEN)) == NULL)
           return(-1);
       msg.msg_control    = cmptr;
       msg.msg_controllen = CONTROLLEN;
       if ((nr = recvmsg(fd, &msg, 0)) < 0) {
           err(0, "recvmsg error");
       } else if (nr == 0) {
           warn("connection closed by server");
           return(-1);
       }
       printf("recv nr=[%d]\n", nr);
       for (ptr = buf; ptr < &buf[nr]; ) {
           if (*ptr++ == 0) {
               if (ptr != &buf[nr-1])
                   err(0, "message format error");
               status = *ptr & 0xFF;
               if (status == 0) {
                   if (msg.msg_controllen != CONTROLLEN)
                       warn("status = 0 but no fd");
                   newfd = *(int *)CMSG_DATA(cmptr);
               } else {
                   newfd = -status;
               }
               nr -= 2;
           }
        }
        if (status >= 0)
            return(newfd);
   }
}

int serverSocket;
int clientSocket;
int num_requests;
struct Buffer* buffer;

int semFreeSlots;
int* childPids;
int* serverPipes;
int* clientPipes;
int poolSize;
int* childCommunicationChannel;

int getIndexForChildId(int poolSize, int childId)
{
  int i;
  for(i = 0 ; i < poolSize ; i++)
    if(childPids[i] == childId)
      return i;
  return -1;
}

void killChilds()
{
  int i;
  for(i = 0; i < poolSize ; i++)
  {
    printf("i=[%d] pid=[%d]\n", i, childPids[i]);
    kill(childPids[i], SIGUSR1);
  }
  for(i = 0; i < poolSize ; i++)
  {
    int status;
    waitpid(childPids[i], &status, 0);
    printf("pid=[%d] with status=[%d] \n", childPids[i], status);
  }
  free(childPids);
  //sendKillToChilds(buffer);
}

void endServer(int code, char* message)
{
  warn("Finishing Server...");
  killChilds();
  destroyBuffer(buffer);
  closeSocket(serverSocket);
  if(0 == code)
    warn(message, 0); // 0 servers as junk to prevent warning
  else
    err(code, message, 0); // 0 servers as junk to prevent warning
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
      removeValueFromBuffer(buffer, pid);
      if(0 != clientSocket)
        closeSocket(clientSocket);
			exit(0);
	}	
}

void serveConnection(int id, int channel, struct Buffer* buffer, int semFreeSlots)
{
  int pid = getpid();
  struct sigaction userSignal;
  memset (&userSignal, '\0', sizeof(userSignal));
  userSignal.sa_handler = signalCatcher;
  userSignal.sa_flags = 0;
  sigemptyset(&userSignal.sa_mask);
  if(-1 == sigaction(SIGUSR1, &userSignal, NULL))
  {
    removeValueFromBuffer(buffer, pid);
    err(EXIT_FAILURE, "Couln't set the signal handling for process=[%d]", id);
  }
  pushValueInBuffer(buffer, pid);
  while(1)
  {
    clientSocket = recv_fd(channel);
    char requestInfo[REQUEST_INFO_LENGHT];
    recv(clientSocket, requestInfo, REQUEST_INFO_LENGHT, 0);
    printf("%s\n", requestInfo);
    char *fileRequested = getFileRequest(requestInfo);
    //printf("fileRequested=[%s]\n", fileRequested);
    if(fileRequested != 0)
      responseRequest(clientSocket, fileRequested, buffer);
    closeSocket(clientSocket);
    clientSocket = 0;
    pushValueInBuffer(buffer, pid);
    signalSemaphore(semFreeSlots);
  }
  exit(0);
}

void initPool(struct Buffer* buffer, int poolSize)
{
  int i;
  int pid;
  semFreeSlots = createSemaphore(BUFFER_SEM_KEY + 1, poolSize);
  childPids = (int*)calloc(poolSize, sizeof(int));
  childCommunicationChannel = (int*)calloc(poolSize, sizeof(int));
  int pair[2];
  for(i = 0 ; i < poolSize ; i++)
  {
    socketpair(PF_LOCAL, SOCK_STREAM, 0, pair);
    pid = fork();
    if(pid < 0)
      endServer(EXIT_FAILURE, "Couldn't create the pool");
    else if(pid > 0)
    {
      close(pair[1]);
      childCommunicationChannel[i] = pair[0];
      childPids[i] = pid;
    }
    else
    {
      close(pair[0]);
      serveConnection(i, pair[1], buffer, semFreeSlots);
    }
  }
}

void manageConnection(int socket, struct sockaddr_in client)
{
  waitSemaphore(semFreeSlots);
  int childPid = popValueFromBuffer(buffer);
  int childIndex = getIndexForChildId(poolSize, childPid);
  printf("Found index=[%d] with pid=[%d].\n", childIndex, childPid);
  send_fd(childCommunicationChannel[childIndex], socket);
  close(socket);
}

void runServer()
{
  while(1)
  {
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
  buffer = initBuffer(poolSize, BUFFER_FILE, BUFFER_SEM_KEY);
  initPool(buffer, poolSize);
  printf("Server initialized on port=[%d].\n", port);
  signal(SIGINT, signalCatcher);
  signal(SIGTERM, signalCatcher);
  runServer();
  return 0;
}
