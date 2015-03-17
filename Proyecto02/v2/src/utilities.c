
#include "utilities.h"

void closeSocket(int socket)
{
  struct sockaddr_in sin;
  socklen_t addrlen = sizeof(sin);
  int local_port = 0;
  if(getsockname(socket, (struct sockaddr *)&sin, &addrlen) == 0)
    local_port = ntohs(sin.sin_port);
  printf("Closing socket=[%d]...\n", local_port);
	if(close(socket) < 0)
    err(1, "Could not close socket");
}

long getFileLength(char* fileRequested)
{
  struct stat st;
  stat(fileRequested, &st);
  return st.st_size;
}

char* ltoa(long integer)
{
  char* buffer = (char*)calloc(50, sizeof(char));
  sprintf(buffer, "%ld", integer);
  return buffer;
}

void sendNotFoundResponse(int socket)
{
  char* extension = getFileExtension(SOFT404);
  char* mime = getMIMEType(extension);
  FILE* file = fopen(SOFT404, "rb");
  if (0 != file)
  {
    long contentLength = getFileLength(SOFT404);
    sendHeader(socket, mime, contentLength);
    sendFile(socket, file, 0);
    close(file);
  }
}

void responseRequest(int socket, char* fileRequested, struct Buffer* buffer)
{
  FILE* file = fopen(fileRequested, "rb");
  if(0 == file)
    sendNotFoundResponse(socket);
  else
  {
    char* extension = 0;
    char* mime = 0;
    extension = getFileExtension(fileRequested);
    mime = getMIMEType(extension);
    if(0 == mime)
      sendNotFoundResponse(socket);
    else
    {
      long contentLength = getFileLength(fileRequested);
      //printf("File requested=[%s] with extension=[%s], mime=[%s], length=[%d].\n", fileRequested, extension, mime, contentLength);

      sendHeader(socket, mime, contentLength);
      sendFile(socket, file, buffer);
    }
    close(file);
  }
  free(fileRequested);
}

void sendFile(int socket, FILE* file, struct Buffer* buffer)
{
  char messageBuffer[RESPONSE_BUFFER_SIZE];
  bzero(messageBuffer, RESPONSE_BUFFER_SIZE);
  int dataLenght;
  int total = 0;
  printf("Transfering file...\n");
  while((dataLenght = fread(messageBuffer, sizeof(char), RESPONSE_BUFFER_SIZE - 1, file)) > 0)
  {
    if(send(socket, messageBuffer, dataLenght, MSG_NOSIGNAL) < 0)
    {
      warn("Failed sending a file");
      break;
    }
    total += dataLenght;
    bzero(messageBuffer, RESPONSE_BUFFER_SIZE);
    if(0 != buffer && 1 == mustDie(buffer))
      break;
  }
  printf("File transfered=[%dkB]\n", total/1024);
}

void sendHeader(int socket, char* mime, long contentLength)
{
  char* contentLengthParsed = ltoa(contentLength);
  char* date = getDate();
  char* httpHeader = "HTTP/1.1 200 OK\r\nDate:%sContent-Type:%s\r\nContent-Lenght:%d\r\n\r\n";
  long buffer_size = strlen(mime) + strlen(httpHeader) + strlen(date) + strlen(contentLengthParsed);
  char* header = (char*)calloc(buffer_size, sizeof(char));
  sprintf(header, httpHeader, date, mime, contentLength);
  send(socket, header, strlen(header), MSG_NOSIGNAL);
  free(header);
  free(contentLengthParsed);
}

int bootServer(int port)
{
  int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

  if(serverSocket == -1)
    err(1, "Can't create the socket in the port=[%d]", port);

  struct sockaddr_in server;
  server.sin_family = AF_INET;
  server.sin_port = htons(port);
  server.sin_addr.s_addr = INADDR_ANY;

  bzero(&(server.sin_zero), 8);

  if(bind(serverSocket, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
    err(1, "Can't bind the socket in the port=[%d]", port);
  
  //struct linger so_linger;
  //so_linger.l_onoff = 1;
  //so_linger.l_linger = 0;
  //int z = setsockopt(serverSocket, SOL_SOCKET, SO_LINGER, &so_linger, sizeof(so_linger));
  //if ( z )
  //{
  //  perror("setsockopt(2)");
  //  close(serverSocket);
  //}
  return serverSocket;
}

char* getDate()
{
  time_t rawtime;
  struct tm * timeinfo;
  time ( &rawtime );
  timeinfo = localtime ( &rawtime );
  return asctime(timeinfo);
}

char* getMIMEType(char* ext)
{
  static char extensions[MIMEFILE_MAX_ENTRIES][EXTENSION_MAX_LENGHT];
  static char mimes[MIMEFILE_MAX_ENTRIES][MIME_MAX_LENGHT];
  static unsigned short loaded = 0;
  if(0 == loaded)
  {
    FILE* file;
    if(!(file = fopen(MIMEFILE, "r")))
    {
      warn("Couldn't open the mimefile");
      return 0;
    }

    char buffer[MIMEFILE_LINE_MAX_LENGHT];
    char* token;
    int i = 0;
    while(!feof(file))
    {
      fgets(buffer, MIMEFILE_LINE_MAX_LENGHT, file);
      if(strlen(buffer) == 0)
        break;
      token = strtok(buffer, " ");
      strcpy(extensions[i], token);
      token = strtok(0, " ");
      if(0 == strchr(token, '\n'))
        strcpy(mimes[i], token);
      else
        strncpy(mimes[i], token, strlen(token)-1);
      if(++i == MIMEFILE_MAX_ENTRIES)
        break;
    }
    fclose(file);
    loaded = 1;
  }
  int i;
  for(i = 0 ; i < MIMEFILE_MAX_ENTRIES ; i++)
  {
    if(strncmp(extensions[i], ext, strlen(ext)) == 0)
      return mimes[i];
  }
  return 0;
}

char* getFileRequest(char* request)
{
  char* fileRequested = extractFileFromRequest(request);
  if(0 != fileRequested)
  {
    char* completePathFile = (char*)calloc(strlen(url) + strlen(fileRequested) + 1, sizeof(char));
    strcpy(completePathFile, url);
    strcat(completePathFile, fileRequested);
    fileRequested = completePathFile;
  }
  return fileRequested;
}

char* extractFileFromRequest(char* request)
{
  char* t;
  int i = 0;
  for ( t = strtok(request," "); t != NULL; t = strtok(NULL, " ") )
  {
    if(i == 1)
      break;
    i++;
  }
  if(i == 1)
    return t;
  else
    return 0;
}

char* getFileExtension(char* fileName)
{
  char* token;
  token = strchr(fileName, '.');
  return (token == 0) ? 0 : strpbrk(token, &token[1]);
}


