Servidor tiene los siguientes tipos de solicitudes:
1. FIFO
2. Threaded
3. Pre-threaded
4. Forked
5. Pre-forked

Entonces para poder correr el servidor se tiene que especificar cual de los tipos anteriores se quiere usar 
Para llamar al servidor utilice el comando:
./server <tipo> <puerto> <carpeta donde se sirven los archivos >

Para correr el cliente utilice el comando:
./client <direccion IP> <puerto> <archivo>

Falta agregar: N-threads y n-ciclos

Cada uno tiene su Makefile correspondiente
