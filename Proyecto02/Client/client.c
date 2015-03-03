#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define BUFSIZE 18196
char *getFileExtension(char *);
char *getMIMEType(char *);
#define MIMEFILE "TablaMIME.c"

pexit(char * msg)
{
	perror(msg);
	exit(1);
}

main(int argc , char *argv[])
{
int i,sockfd;
int port=atoi(argv[2]);
char* ip_address=argv[1];
char buffer[BUFSIZE];
char* file=argv[3];
//char *extension=getFileExtension(file);
//char *mime= getMIMEType(extension);
char* extension="txt";
char* mime="text/plain"; 
static struct sockaddr_in serv_addr;

	printf("client trying to connect to %s and port %d\n",ip_address,port);
	if((sockfd = socket(AF_INET, SOCK_STREAM,0)) <0) 
		pexit("socket() failed");

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(ip_address);
	serv_addr.sin_port = htons(port);

	/* Connect tot he socket offered by the web server */
	if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) <0) 
		pexit("connect() failed");

	/* Now the sockfd can be used to communicate to the server the GET request */
char *command=(char *) malloc(sizeof(char) * BUFSIZE);
//char *command="GET /s.txt HTTP/1.0 \r\n\r\n";
//char *get_body = "GET %s HTTP/1.1 Host: %s\r\nAccept: %s\r\n\r\n";
char *get_body = "GET /%s HTTP/1.0 \r\n\r\n" ;
sprintf(command, get_body, file);
	printf("Send bytes=%d %s\n",strlen(command), command);
	write(sockfd, command, strlen(command));

	/* This displays the raw HTML file (if index.html) as received by the browser */
	while( (i=read(sockfd,buffer,BUFSIZE)) > 0)
		write(1,buffer,i);
}
char *getMIMEType(char *ext){
        FILE *file;
        char p [60];
        char *buffer;
        char *t;
        if(!(file = fopen(MIMEFILE, "r")))
                return NULL;

        while(!feof(file)){
                fgets(p, 60, file);
                int flag = 0;

                for ( t = strtok(p," "); t != NULL; t = strtok(NULL, " ") ){
                        if(strcmp(t, ext) == 0)
                                flag = 1;
                        else
                                break;
                }//fin for

                if(flag){
                        char *m = (char *) malloc(sizeof(char) * strlen(t));
                        strcpy(m ,t);
                        fclose(file);
                        return m;
                }//fin de if
        }//fin de while

        fclose(file);
        return NULL;
}//end getMIMETYPE
char *getFileExtension(char *fileName){
        char *t;
        t = strchr(fileName, '.');
        return (t == NULL) ? NULL : strpbrk(t, &t[1]);
}//end getFileExtension


