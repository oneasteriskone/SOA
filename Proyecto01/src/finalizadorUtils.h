#ifndef FINALIZADOR_UTILS
#define FINALIZADOR_UTILS

int fin = 0;	

#define FLAG_FIN 0XA61533

void createFinalFlag(){
	int flag = (int) createMmap("/soa_final", sizeof(int));
	flag = 0;
}

int getFinalFlag(){
	int fflag = (int) calloc(1, sizeof(int));
	fflag = (int) getMmap("/soa_final", sizeof(int));
	return fflag;
}

// #define BUFFER_SIZE 150
// #define BUFFER_ROW_SIZE 50
// #define BUFFER_FILE "/soa_buffer"
// #define BUFFER_FILE_SIZE BUFFER_ROW_SIZE * BUFFER_SIZE * sizeof(char)

// #define BUFFER_SEM_KEY 0xA61531

// struct Buffer
// {
//   char* address;
//   int semId;
// };


// void createBuffer()
// {
//   char* buffer = (char*) createMmap(BUFFER_FILE, BUFFER_FILE_SIZE);
//   char* current = buffer;
//   for(int i = 0 ; i < BUFFER_SIZE ; i++)
//   {
//     current = buffer + BUFFER_ROW_SIZE * i;
//     for(int j = 0 ; j < BUFFER_ROW_SIZE ; j++)
//     {
//       current[j] = '\0';
//     }
//   }
//   int semId = createSemaphore(BUFFER_SEM_KEY);
// }

// struct Buffer* getBuffer()
// {
//   struct Buffer* buffer = (struct Buffer*) calloc(1, sizeof(struct Buffer));
//   buffer->address = (char*) getMmap(BUFFER_FILE, BUFFER_FILE_SIZE);
//   buffer->semId = getSemaphore(BUFFER_SEM_KEY);
//   return buffer;

#endif
