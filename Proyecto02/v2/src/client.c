//*******************************************************************************
//***  	INSTITUTO TECNOLOGICO DE COSTA RICA
//***	MAESTRIA EN CIENCIAS DE LA COMPUTACION
//***   WEB SERVER
//***   CLIENT
//*******************************************************************************

#include <stdio.h>
#include <ctype.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/timeb.h>
#include <time.h>
#include <fcntl.h>

#define BUFSIZE 81960

// Parameters of the thread 
struct threadPar { 
    char* ipAdd; // ipAdd: ip address
    char* fileReq; // fileReq: requested file
    int   nRequests; // nRequests: number of Requests
	int nThreads; // nThreads: number of threads
	int port; // port: used port
};

// Error codes of http
struct errorCod {
    int code;
    char *messCod; // message according to the error code
} serverErrors [] = { // Errors from the server
    {400,"Bad Request"},
    {401,"Unauthorized"},
    {402,"Payment Required"},
    {403,"Forbidden"},
    {404,"Not found"},
    {405,"Method Not Allowed"},
    {406,"Not Acceptable"},
    {407,"Proxy Authentication Required"},
    {408,"Request Timeout"},
    {409,"Conflict"},
    {410,"Gone"},
    {411,"Length Required"},
    {412,"Precondition Failed"},
    {413,"Request Entity Too Large"},
    {414,"Request-URI Too Long"},
    {415,"Unsupported Media Type"},
    {416,"Requested Range Not Satisfiable"},
    {417,"Expectation Failed"},
    {500,"Internal Server Error"},
    {501,"Not Implemented"},
    {502,"Bad Gateway"},
    {503,"Service Unavailable"},
    {504,"Gateway Timeout"},
    {505,"HTTP Version Not Supported"}};

// Statistical variables	
static unsigned int recBytes = 0;
static unsigned int numErrors = 0;
static unsigned int numCycles = 0;
static unsigned int throughput = 0;
static float timePerRequest = 0.0;
static float averageTimePerRequest = 0.0;

// validate the IP Address entered by users
int valIPADDR(const char* buffer)
{
    if (NULL == buffer) return -1;

    register const      char*   ptrPosition     = buffer;
    register unsigned   char    ch      = *ptrPosition;
    register unsigned   short   count   = 0;

    while (ch != '\0')
    {
        if (!((ch >= '0' && ch <= '9') || ch == '.')) return -1;
            if (ch == '.')
                if (++count > 3) return -1;
                ch = *++ptrPosition;
    }

    if (count == 3 && *--ptrPosition != '.') return 0;
    return -1;
}

 // validate the port, the number of cycles and threads
int numeralValidation(const char* buffer)
{
    register const char* ptrPosition = buffer;
    register unsigned char ch = *ptrPosition;
    while (ch != '\0')
    {
        if (isdigit(ch) == 0){
            return -1;
        }
            ch = *++ptrPosition;
    }
   return 0;
}

// check if there is a HTTP related error
int httpError(char* code) {
    int codeNumber = atoi(code);
    int i=0;
    ///look for the code in the error code table
    int errorArray = sizeof(serverErrors)/sizeof(struct errorCod);
    for(i=0;i<errorArray;i++) {
        if (serverErrors[i].code == codeNumber)
            return -1;
    }
    return 0;
}

// client execution
void *clientExecution(void *threargs)
{
    // Define variables
    int nRequests, sockfd, i, flags, diff, port;
    char* ipAdd  = "";
    char* fileReq = "";
    char* isTextContent = "";
    char http_code[4];
    struct timeb start, end;
    unsigned int bytesReadByRequest;
	
    // Initialize input data
    struct threadPar *params;
    params = (struct threadPar *) threargs;
    nRequests = params -> nRequests;
    ipAdd = params      -> ipAdd;
    port = params         -> port;
    fileReq = params     -> fileReq;

    // Set the HTTP information
    char command[50] = "GET /";
    strcat(command, fileReq);
    strcat(command, " HTTP \r\n\r\n"); // Important: the space in HTTP/1.0 cannot be removed
    printf("*************************************************************************************\n");
    printf("Command: %s\n\n", command);
    char buffer[BUFSIZE];
    static struct sockaddr_in serv_addr;

    for (i = 0; i < nRequests; i++) {
        printf("*************************************************************************************\n");
        printf("Thread: %u\n", (unsigned int)pthread_self());
        printf("Cycle: %d\n", i);
        bytesReadByRequest = 0;
        // Adding the total number of cycles
	    numCycles++;
        // Start time
        ftime(&start);

        ///create a request and receive the file requested
		printf("client trying to connect to %s and port %d to receive %s\n",ipAdd, port, fileReq);
        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            perror("The function socket has failed!");
            // Increasing the number of errors
            numErrors++;
            continue;
        }        

        /// set the server name, ip address and the port number
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = inet_addr(ipAdd);
        serv_addr.sin_port = htons(port);

	   // Connect to the socket of the the web server
        if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		   perror("The conexion to the server has failed!");
		   //Increasing the number of errors
           numErrors++;
           continue;
        }

	    /* Now the sockfd can be used to communicate to the server the GET request */
	    printf("Bytes sent= %zd %s\n\n", strlen(command), command);
	    write(sockfd, command, strlen(command));
        // Set socket file descriptor as non-blocking
        flags = fcntl(sockfd, F_GETFL);
        fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
        
	    // amount of bytes transferred per request
        int readBytes, retval, parts = 0;
        // Set a timeout
        fd_set rfds;
        struct timeval tv;   
        /*Four seconds timeout */
        tv.tv_sec = 4;
        tv.tv_usec = 0;       

        do {
            // Watch stdin (fd 0) to see when it has input
            FD_ZERO(&rfds);
            FD_SET(sockfd, &rfds);
            // Check if there is something to read
            retval = select(sockfd + 1, &rfds, NULL, NULL, &tv);
            /* Don't rely on the value of tv now! */
            if (retval == -1)                
                perror("select()");
            else if (retval){               
                readBytes = recv(sockfd, buffer, BUFSIZE, 0);
                recBytes += readBytes;
                bytesReadByRequest += readBytes;
                // Capture any HTTP related error
                parts++;
                    if(parts == 1) { ///the code reads header...
                        memcpy(http_code, &buffer[9], 3);///extracts http code
                        http_code[3] = '\0';
                        if (httpError(http_code) == -1)//if the code is not 200 we count it as an error
                            // Increasing the number of errors
                            numErrors++;
                        ///check if type is text
                        isTextContent = strstr(buffer,"text");
                    }
                if (isTextContent) {
                    fprintf(stdout,"%s", buffer);
                } else {///if the file is binary the application displays amount of data read
                    printf("\nRead bytes (Thread %u - Cycle %d): %d\n",(unsigned int)pthread_self(), i+1,bytesReadByRequest);
                }
            }else{
                ///printf("No data within 4 seconds.\n");
            break;
            }
            // In this section we capture any socket exception
            if (readBytes == -1)
            {
                perror("Socket Error");
		// Increasing the number of errors
	        numErrors++;
        	break;
            }
        }while (readBytes > 0);
       
	// Stop watch
        ftime(&end);
        // Calculate difference
        diff = difftime(end.time,start.time);
		// Cumulating the execution time
        averageTimePerRequest += diff;

    }
    // Close threads
    pthread_exit(NULL);
    // Close sockets
    close(sockfd);

}

/*
 * Shows error messCod in a friendly way...
 */
int pexit(char * msg)
{
	perror(msg);
	exit(1);
}

/*
 * Main function
 */
int main(int argc, char *argv[]) {
	int nThreads, i, rc = 0;
    pthread_attr_t attr;
	struct threadPar params;
    struct timeb start, end;
    void *status;

	if (argc != 6) {
		printf("%s <Machine IP address> <port> <file> <number of threads> <number of cycles>\n argv[0] = ./client\n argv[1] = 192.0.3.15\n argv[2] = 8083\n argv[3] = picture.jpg\n argv[4] = 3\n argv[5] = 1\n", argv[0]);
	}
	else {

        if (valIPADDR(argv[1]) == -1)
        {
            printf("Invalid Machine. Please enter a valid IP address.\n");
            return 0;
        }

        if (numeralValidation(argv[2]) == -1)
        {
            printf("Invalid port. Please enter a valid port number.\n");
            return 0;
        }

        if (numeralValidation(argv[4]) == -1 || atoi(argv[4]) < 1)
        {
            printf("Invalid number of threads. Please enter a valid number.\n");
            return 0;
        }

        if (numeralValidation(argv[5]) == -1 || atoi(argv[5]) < 1)
        {
            printf("Invalid number of cycles. Please enter a valid number.\n");
            return 0;
        }

	    //Set all arguments 
	    params.ipAdd      = argv[1];
	    params.port         = atoi(argv[2]);
	    params.fileReq     = argv[3];
        nThreads         = atoi(argv[4]);
        params.nThreads  = atoi(argv[4]);
        params.nRequests = atoi(argv[5]);

        //Initialize and set thread detached attribute
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
        pthread_t threads[nThreads];

	    //Start timer
        ftime(&start);

      	// Create threads
	    for (i = 0; i < nThreads; i++) {
            rc = pthread_create(&threads[i], NULL, clientExecution, (void *)&params);
            if (rc) {
                    printf("The thread creation has failed, code: %d\n", rc);
                    exit(-1);
        	}
        }

        // Free attribute and wait for the other threads
        pthread_attr_destroy(&attr);
        for(i=0; i<nThreads; i++) {
            rc = pthread_join(threads[i], &status);
            if (rc) {
                printf("Thread join id: %d\n", rc);
                exit(-1);
            }
        }

	   //Stop timer
       ftime(&end);
	   //Set the total execution time
       timePerRequest = difftime(end.time,start.time);
	   //throughput
       throughput = (((numCycles - numErrors) / numCycles) * 100);
	   //average time per request
       averageTimePerRequest = timePerRequest / numCycles;
	   //Print all the statistics
        printf("\n**********Client Statistics**********");
		printf("\nBytes read: %d",recBytes);
        printf("\nRequests successfully executed: %d",numCycles);
        printf("\nServer throughput: %d%%",throughput);
        printf("\nExecution time: ~%.2f secs",timePerRequest);
        printf("\nTime per thread: ~%.2f secs",averageTimePerRequest);
        printf("\n************************************\n\n");
       pthread_exit(NULL);
	}
    return 0;
}
