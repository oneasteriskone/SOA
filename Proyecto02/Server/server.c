//fuente http://es.tldp.org/Tutoriales/PROG-SOCKETS/prog-sockets.html#STRUCT

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <time.h>

//structs

//declaracion de metodos 
int bootServer(char *);
void ReadAndWrite(void *);
void FIFOStyle();
void Threaded();
void PreThreaded(void *);
void ForkStyle();
void PreForkStyle(int);
char *getFileRequest(char *);
char *getFileExtension(char *);
char *getMIMEType(char *);
char *getText(char *);
char *getContent(char *);
char *getDate();

//constantes
#define MIMEFILE "TablaMIME.c"
#define SOFT404 "soft404.html"

//variables
int server_socket;
int sin_size;
struct sockaddr_in server;
int num_requests;
char *url;
pthread_t *hilos;
int procesos;

//main
main(int argc, char *argv[])
{
	sin_size = sizeof(struct sockaddr_in);

	//se revisa que se haya ingresado un puerto

	if(argv[1] == NULL){
		printf("Digite numero de modo de trabajo de servidor\n");
		exit(0);
	}//end if

	if(bootServer(argv[2]) == -1)
		exit(0);
	else
		printf("Se ha iniciado el servidor.\n");

	if(argv[3] == NULL){
		printf("Digite direccion de los archivos a trabajar\n");
		exit(0);
	}//end if
	else
		url = argv[3];

	//se inicia el servidor

	switch(atoi(argv[1])){
		case 1://Estilo FIFO
			printf("Modo de trabajo: FIFO\n");
			FIFOStyle();
			break;
		case 2://Estilo Threaded
			printf("Modo de trabajo: Threaded\n");
			Threaded();
			break;
		case 3:
			printf("Modo de trabajo: Pre-Threaded\n");
			printf("Digite la cantidad de procesos a crear:");
			scanf("%d", &procesos);
			hilos = (pthread_t *) malloc(sizeof(pthread_t)*procesos);
			int i;
			for(i=0;i<procesos;i++)
				pthread_create(&hilos[i], NULL, &PreThreaded, &i);

			for(i=0;i<procesos;i++)
				pthread_join(hilos[i], NULL);
			break;
		case 4://Estilo Fork
			printf("Modo de trabajo: Fork\n");
			ForkStyle();
			break;
		case 5://Estilo Pre-Forked
			printf("Modo de trabajo: PreForked\n");
			int p_num;
			printf("Digite cantidad de procesos a crear:");
			scanf("%d", &p_num);
			PreForkStyle(p_num);
			scanf("%d", &p_num);
			break;
		default://Ninguno
			printf("La opcion que ha elegido no existe\n");
			break;
	}//end switch
	
}//end main


//metodos---------------------------------------------------------------------
int bootServer(char *argv){

	server_socket = socket(AF_INET, SOCK_STREAM, 0);

	//se revisa que haya funcionado la llamada al socket
	if(server_socket == -1){
		perror("error en socket()\n");
		return -1;
	}//fin de if
	
	//se llena la informacion del servidor
	server.sin_family = AF_INET;
	
	/*htons() sirve para convertir una ordenacion de bytes para nodos a una ordenacióe bytes para redes,
	este convierte a variable corta, htonl() convierte a variable larga*/
	server.sin_port = htons(atoi(argv));

	//INADDR_ANY coloca la direccióP automaticamente
	server.sin_addr.s_addr = INADDR_ANY;

	//se escriben 0 en al estructura
	bzero(&(server.sin_zero), 8);

	if(bind(server_socket, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1){
		perror("error en bind");
		return -1;
	}//end if

	return 1;
}//end bootServer

//---------------------------------------------------------------------------
void FIFOStyle(){
	while(1){
		printf("Esperando cliente\n");
		if(listen(server_socket, 10) == -1){
			perror("error en listen.");
			exit(-1);
		}//end if
		else
			printf("Atendiendo al cliente\n");
				
		//se acepta la conexion
		int est_acc;
		struct sockaddr_in client;
		est_acc = accept(server_socket, (struct sockaddr *)&client, &sin_size);
		//se revisa que se haya aceptado
		if(est_acc == -1){
			perror("error a la hora de aceptar.");
			exit(-1);
		}//end if
		else
	           printf("Se acepto la conexion\n");	

		ReadAndWrite((void *)est_acc);
       	close(est_acc);
	}//end while
}//end FIFOStyle

//---------------------------------------------------------------------------
void Threaded()
{
	//se obtiene primero la cantidad de veces que se van a pedir los archivos
	//int cantreq = getCantReq();
	//printf("numero de archivos ? %d\n",cantreq);
    
	while(1){
		printf("Esperando cliente\n");
		if(listen(server_socket, 10) == -1){
			perror("error en listen.");
			exit(-1);
		}//end if
		else
			printf("Atendiendo al cliente\n");
				
		//se acepta la conexion
		int est_acc;
		struct sockaddr_in client;
		est_acc = accept(server_socket, (struct sockaddr *)&client, &sin_size);

		//se revisa que se haya aceptado
		if(est_acc == -1){
			perror("error a la hora de aceptar.");
			exit(-1);
		}//end if
		else
			printf("Se acepto la conexion\n");				

			pthread_t Hilo;
			printf("creacion del hilo\n");
			int i;
	      //for(i = 0; i < num_requests; i++)			
			pthread_create(&Hilo, NULL, &ReadAndWrite, (void *)est_acc);		
	}//end while
}//end Threaded

//--------------------------------------------------------------------------------------
void PreThreaded(void *h) {
	int hl = *((int*)h);
	while(1) {
		
		printf("Esperando cliente\n");
		if(listen(server_socket, 1) == -1){
			perror("error en listen.");
			exit(-1);
		}//end if
		else
			printf("Atendiendo al cliente\n");
				
		//se acepta la conexion
		int est_acc;
		struct sockaddr_in client;
		est_acc = accept(server_socket, (struct sockaddr *)&client, &sin_size);

		//se revisa que se haya aceptado
		if(est_acc == -1){
			perror("error a la hora de aceptar.");
			exit(-1);
		}//end if
		else
			printf("Se acepto la conexion\n");
		printf("Hilo %d atendiendo\n", hl);
		ReadAndWrite((void *)est_acc);
		close(est_acc);
	}//end while
}//end PreThreaded

//---------------------------------------------------------------------------
void ForkStyle(){

	//int num_requests = getCantReq();

	//el servidor espera conexiones, cuando llega una se atiende 
	//creando un fork para poder seguir esperando mas conexiones
	
	while(1){
		int est;
		struct sockaddr_in cli;

		printf("Se espera cliente\n");
		if(listen(server_socket, 50) == -1){
			perror("Error en listen");
			exit(-1);
		}//end if
		
		est = accept(server_socket, (struct sockaddr *)&cli, &sin_size);
		if(est == -1){
			perror("error en accept");
			exit(-1);
		}//end if
		else
			printf("Se acepta cliente\n");

		pid_t pid;
		if((pid = fork()) == 0){
            //close(server_socket);
            //int i;
            //for(i = 0; i < num_requests; i++)
    			ReadAndWrite((void *)est);
            exit(0);
		}//end if
        else
            close(est);
	}//end while

}//end ForkStyle

//---------------------------------------------------------------------------
void PreForkStyle(int limit){
	pid_t *pid = (pid_t *) malloc(sizeof(pid_t) * limit);
	int *status = (int *) malloc(sizeof(int) * limit);
	int i;
	for(i = 0; i < limit; i++){
		if((pid[i] = fork()) == 0){
			while(1){
				int est;
				struct sockaddr_in cli;
		
				printf("Se espera cliente\n");
				if(listen(server_socket, limit) == -1){
					perror("Error en listen");
					exit(-1);
				}//end if
				
				est = accept(server_socket, (struct sockaddr *)&cli, &sin_size);
				if(est == -1){
					perror("error en accept");
					exit(-1);
				}//end if
				else
					printf("Se acepta cliente\n");
				printf("El subproceso[%d] esta trabajando\n", getpid());
    				ReadAndWrite((void *)est);
			}//end while
		}//end if
		else{}
	}//end for
}//end ForkStyle

//---------------------------------------------------------------------------
void ReadAndWrite(void *socket){
	//se obtiene la informacion enviada por el cliente
printf("dsadasdsa");
	char info [512];
	//se hace un llamado a read(int fd, void *buf, size_t count)
	recv(((int)socket), info, 512, 0);
	char *extension = NULL;
	char *mime = NULL;
	char *content = NULL;
	
	char *name = getFileRequest(info);
	//revisa que haya extenison en el request del archivo, si lo hay se procede de manera normal
	if((strchr(name, '.')) != NULL){
		extension = getFileExtension(name);
		mime = getMIMEType(extension);	
		content = NULL;
	}//fin de if
	//aqui va metodo para sacar el array del archivo o texto
	if(mime != NULL && extension != NULL){
		if(strstr(mime, "text") != NULL){
			content = getText(name);
}
		else{

			content = getContent(name);
}


}

	if(content == NULL){
		extension = getFileExtension(SOFT404);
		mime = getMIMEType(extension);
		content = getText(SOFT404);
	}//end if
	//se escribe el http
	char *date = getDate();
	char *http_body = "HTTP/1.1 200 OK\nDate:%sContent-Type:%sContent-Lenght:%d\n\n%s";
	long buffer_size = strlen(mime) + strlen(content) + strlen(http_body) + strlen(date);
	
	char* http = (char *) malloc(sizeof(char) * buffer_size);
	sprintf(http, http_body, date, mime, strlen(content), content);
//	printf("%s\n\n", http);
printf("%d\n", strlen(content));
	//se hace un llamado a write
	send(((int)socket), http, strlen(http), 0);
	
	close(((int)socket));
	//free(http);
}//end ReadAndWrite

//---------------------------------------------------------------------------
char *getFileRequest(char *http){
	char *t;
	int i = 0;
	for ( t = strtok(http," "); t != NULL; t = strtok(NULL, " ") ){
		if(i == 1) break;
		i++;
	}//end for
	return strpbrk(t, &t[1]);
}//end getFileName

//---------------------------------------------------------------------------
char *getFileExtension(char *fileName){
	char *t;
	t = strchr(fileName, '.');
	return (t == NULL) ? NULL : strpbrk(t, &t[1]);
}//end getFileExtension

//---------------------------------------------------------------------------
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

//-------------------------------------------------------------------------
char *getText(char *fileName){
	FILE *file;
	char *buffer;
	long fsize;
	char *completeURL = (char *) malloc(sizeof(char) * strlen(url) + strlen(fileName)); 
	if(strcmp(fileName, "soft404.html") != 0){
		strcpy(completeURL, url);
		strcat(completeURL,"/");
		strcat(completeURL, fileName);
	}//fin de if
	else
{
		strcpy(completeURL, fileName);
}
	if(!(file = fopen(completeURL , "r")))
		return NULL;

	//obtiene tamañel archivo
	fseek(file, 0, SEEK_END);
	fsize = ftell(file);
	rewind(file);

	//se pide el espacio de memoria donde se guardara la informacion
	buffer = (char *) malloc(sizeof(char *) * fsize);
	
	//se copia el archivo en el buffer
	fread(buffer, 1, fsize, file);
	fclose(file);
	free(completeURL);
	return buffer;
}//end getText

//--------------------------------------------------------------------------
char *getContent(char* fileName){
	FILE *file;
        printf("%s",fileName);
	char *completeURL = (char *) malloc(sizeof(char) * strlen(url) + strlen(fileName));


       if(strcmp(fileName, "soft404.html") != 0){
                strcpy(completeURL, url);
                strcat(completeURL,"/");
                strcat(completeURL, fileName);
        }//fin de if
        else
{
                strcpy(completeURL, fileName);
}

	if (!(file = fopen(completeURL, "rb")))
		return NULL;
	
	//obtiene tamañrchivo
	fseek(file, 0, SEEK_END);  
	long int size = ftell(file); 
	rewind(file);

	unsigned char caracter [60];
	char *buffer = (char *) malloc(sizeof(char) * size);
	int c;

	while((c=getc(file))!= EOF){
		sprintf(caracter, "%c", c);
		strcat(buffer, caracter);
	}//end while
fread(buffer, size, 1, file);
	
int s = read(file,  buffer, size);

	printf("Tamañtell()]:%d\nTamañtrlen()]:%d\n", size, strlen(buffer));

	fclose(file);
	return buffer;
}//end getContent

//--------------------------------------------------------------------------
char *getDate(){
	time_t rawtime;
  	struct tm * timeinfo;
	char buffer [50];
  	time ( &rawtime );
  	timeinfo = localtime ( &rawtime );
	return asctime(timeinfo);
}//end getDate

//---------------------------------------------------------------------------
int getCantReq(){
	if(listen(server_socket, 10) == -1){
		perror("Error en listen");
		exit(-1);
	}//end if

	int est_acc;
	struct sockaddr_in client;
	est_acc = accept(server_socket, (struct sockaddr *)&client, &sin_size);

	if(est_acc == -1){
		perror("error en accept");
		exit(-1);
	}//end if
	
	char num [4];

	recv(est_acc, num, 4, 0);
	num_requests = atoi(num);
	printf("numrequest=%s\n", num);
    	close(est_acc);

	return atoi(num);
}//end getCantReq


