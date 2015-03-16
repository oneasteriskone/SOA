#include "sharedBuffer.h"

#include <stropts.h>
#define MAXLINE 4096

/*
 * Pass a file descriptor to another process.
 * If fd<0, then -fd is sent back instead as the error status.
 */
int send_fd(int fd, int fd_to_send)
{
    char    buf[2];     /* send_fd()/recv_fd() 2-byte protocol */
    
    buf[0] = 0;         /* null byte flag to recv_fd() */
    if (fd_to_send < 0) {
        buf[1] = -fd_to_send;   /* nonzero status means error */
        if (buf[1] == 0)
            buf[1] = 1; /* -256, etc. would screw up protocol */
    } else {
        buf[1] = 0;     /* zero status means OK */
    }

    if (write(fd, buf, 2) != 2)
        return(-1);
    if (fd_to_send >= 0)
        if (ioctl(fd, I_SENDFD, fd_to_send) < 0)
            return(-1);
    return(0);
}
/*
 * Receive a file descriptor from another process (a server).
 * In addition, any data received from the server is passed
 * to (*userfunc)(STDERR_FILENO, buf, nbytes). We have a
 * 2-byte protocol for receiving the fd from send_fd().
 */
int recv_fd(int fd)
{
    int                 newfd, nread, flag, status;
    char                *ptr;
    char                buf[MAXLINE];
    struct strbuf       dat;
    struct strrecvfd    recvfd;

    status = -1;
    for ( ; ; ) {
        dat.buf = buf;
        dat.maxlen = MAXLINE;
        flag = 0;
        if (getmsg(fd, NULL, &dat, &flag) < 0)
            err(0, "getmsg error");
        nread = dat.len;
        if (nread == 0) {
            warn("connection closed by server");
            return(-1);
        }
        /*
         * See if this is the final data with null & status.
         * Null must be next to last byte of buffer, status
         * byte is last byte. Zero status means there must
         * be a file descriptor to receive.
         */
        for (ptr = buf; ptr < &buf[nread]; ) {
            if (*ptr++ == 0) {
                if (ptr != &buf[nread-1])
                    warn("message format error");
                 status = *ptr & 0xFF;   /* prevent sign extension */
                 if (status == 0) {
                     if (ioctl(fd, I_RECVFD, &recvfd) < 0)
                         return(-1);
                     newfd = recvfd.fd;  /* new descriptor */
                 } else {
                     newfd = -status;
                 }
                 nread -= 2;
            }
        }

        if (status >= 0)    /* final data has arrived */
            return(newfd);  /* descriptor, or -status */
    }
}

void sendKillToChilds(struct Buffer* buffer)
{
  waitSemaphore(buffer->semId);
  ((int*)buffer->address)[buffer->entries] = 1;
  signalSemaphore(buffer->semId);
  if(((int*)buffer->address)[buffer->entries+1] > 0)
  {
    warn("%d childs are still alive... waiting them", ((int*)buffer->address)[buffer->entries+1]);
    while(((int*)buffer->address)[buffer->entries+1] > 0)
      sleep(1);
  }
}

int mustDie(struct Buffer* buffer)
{
  waitSemaphore(buffer->semId);
  int die = ((int*)buffer->address)[buffer->entries];
  signalSemaphore(buffer->semId);
  return die;
}

int pushValueInBuffer(struct Buffer* buffer, int value)
{
  waitSemaphore(buffer->semId);

  int* address = (int*)buffer->address;
  address[buffer->entries+1]++;
  int i;
  for(i = 0; i < buffer->entries ; i++)
  {
    if(0 == address[i])
    {
      printf("pushing with i=[%d] value=[%d]\n", i, value);
      address[i] = value;
      break;
    }
  }
  signalSemaphore(buffer->semId);
  return (i == buffer->entries) ? 0 : 1; 
}

void removeValueFromBuffer(struct Buffer* buffer, int value)
{
  waitSemaphore(buffer->semId);

  int* address = (int*)buffer->address;
  address[buffer->entries+1]--;
  int i;
  for(i = 0; i < buffer->entries ; i++)
  {
    if(value == address[i])
    {
      address[i] = 0;
      break;
    }
  }

  signalSemaphore(buffer->semId);
}

int popValueFromBuffer(struct Buffer* buffer)
{
  waitSemaphore(buffer->semId);

  int* address = (int*)buffer->address;
  address[buffer->entries+1]--;
  int i;
  int value;
  for(i = 0; i < buffer->entries ; i++)
  {
    if(0 != address[i])
    {
      value = address[i];
      address[i] = 0;
      break;
    }
  }

  signalSemaphore(buffer->semId);
  return value;
}

int* getValuesFromBuffer(struct Buffer* buffer, int release)
{
  waitSemaphore(buffer->semId);
  int* pids = (int*)calloc(buffer->entries - 2, buffer->typeSize);
  int i;
  int j = 0;
  for(i = 2 ; i < buffer->entries; i++)
  {
    if(0 != ((int*)buffer->address)[i])
      pids[j++] = ((int*)buffer->address)[i];
  }
  if(release)
    signalSemaphore(buffer->semId);
  return pids;
}

struct Buffer* initBuffer(int size, char* path, int semKey)
{
  struct Buffer* buffer = createBuffer(path, (size + 2) * sizeof(int), semKey);
  int i;
  int* address = (int*)buffer->address;
  for(i = 0 ; i < size + 2 ; i++)
    address[i] = 0;
  buffer->entries = size;
  buffer->typeSize = sizeof(int);
  buffer->path = path;
  return buffer;
}

struct Buffer* createBuffer(char* path, int size, int semKey)
{
  struct Buffer* buffer = (struct Buffer*)calloc(1, sizeof(struct Buffer));
  if(0 != path)
    buffer->address = createMmap(path, size);
  else
    buffer->address = calloc(size, 1);
  buffer->semId = createSemaphore(semKey, 1);
  return buffer;
}

void destroyBuffer(struct Buffer* buffer)
{
  if(0 != buffer->path)
    destroyMmap(buffer->address, (buffer->entries+2) * buffer->typeSize);
  else
    free(buffer->address);
  destroySemaphore(buffer->semId);
  free(buffer);
}

void* createMmap(char* path, int size)
{
  int handler = shm_open(path, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
  if(handler < 0)
    err(EXIT_FAILURE, "Error creating handler for the file=[%s]", path);
  int result = ftruncate(handler, size);
  if (result < 0) {
    close(handler);
    err(EXIT_FAILURE, "Error the file=[%s]", path);
  }

  void* address = (void*) mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, handler, 0);
  if(address == MAP_FAILED)
    err(EXIT_FAILURE, "Can't create the shared shared memory on file=[%s]", path);
  bzero(address, size);

  return address;
}

void destroyMmap(char* address, int size)
{
  int valueReturned = munmap(address, size);
  if(valueReturned < 0)
    err(EXIT_FAILURE, "Couldn't destroy the shared memory");
  //int handler = shm_open(path, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
  //if(handler < 0)
  //  err(EXIT_FAILURE, "Error creating handler for the file=[%s]", path);
  //close(handler);
}

int createSemaphore(int key, int initValue)
{
  union semun valor;
  int semId = semget(key, 1, 0600|IPC_CREAT);
  valor.val = initValue;
  int valueReturned = semctl(semId, 0, SETVAL, valor);
  if(valueReturned < 0)
    err(EXIT_FAILURE, "Couldn't set de semaphore with the key=[%d] and id=[%d]", key, semId);
  return semId;
}

void destroySemaphore(int semId)
{
  int valueReturned = semctl(semId, 0, IPC_RMID);
  if(valueReturned < 0)
    err(EXIT_FAILURE, "Couldn't destroy de semaphore with the id=[%d]", semId);
}

void waitSemaphore(int semId)
{
  struct sembuf buffer;
  buffer.sem_num = 0;
  buffer.sem_op = -1;
  buffer.sem_flg = 0;
  int valueReturned = semop(semId, &buffer, 1);
  if(valueReturned < 0 )
    err(EXIT_FAILURE, "Couldn't get de semaphore with the id=[%d]", semId);
}

void signalSemaphore(int semId)
{
  struct sembuf buffer;
  buffer.sem_num = 0;
  buffer.sem_op = 1;
  buffer.sem_flg = 0;
  int valueReturned = semop(semId, &buffer, 1);
  if(valueReturned < 0 )
    err(EXIT_FAILURE, "Couldn't release de semaphore with the id=[%d]", semId);
}