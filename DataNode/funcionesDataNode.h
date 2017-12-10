#ifndef FUNCIONESDATANODE_H_
#define FUNCIONESDATANODE_H_


#include <commons/collections/dictionary.h>
#include <commons/collections/list.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/log.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sockets.h>
#include <serializacion.h>
extern t_log* logger;
int socketConexionFS;


#define DATANODE_BLOCK_SIZE 1048576
typedef unsigned char dataNode_block[DATANODE_BLOCK_SIZE];
typedef uint32_t dataNode_block_pointer;

typedef struct{
	int numBloque;
	int estado; //ocupado o libre
}t_bloquesDN;

typedef struct{
  char*   IP_FILESYSTEM;
	int   PUERTO_FILESYSTEM;
  char*   NOMBRE_NODO;
	int   PUERTO_WORKER;
	int   PUERTO_DATANODE;
  char*   RUTA_DATABIN;
} t_DataNode;

typedef struct{
  char*   IP_FILESYSTEM;
  int   PUERTO_FILESYSTEM;
  int   NOMBRE_N;
  int   PUERTO_WORKER;
  char*   RUTA_DATABIN;
  int 	  PUERTO_NODO;
  char*   IP_NODO;
} DataNodePRUEBA;

t_list* blokesDN;//t_bloqueDN

char * databin;
int cantidadBloques;

void leerArchivoConfig(char* rutaArchivoConfig);
void conectarConFS(void);
char getBloque(int bloque);
int setBloque(int bloque, char data[DATANODE_BLOCK_SIZE]);
void calcularCantidadDeBloques(int fd);
void mapearDataBin();



#endif /* FUNCIONESDATANODE_H_ */
