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

#define configuracionNodo "../../Configs/configNodo.config"

typedef struct{
	int numBloque;
	int estado; //ocupado o libre
}t_bloquesDN;

typedef struct{
	char*   IP_FILESYSTEM;
	char*   PUERTO_FILESYSTEM;
	char*   NOMBRE_NODO;
	char*   RUTA_DATABIN;
	char* 	PUERTO_NODO;
} t_config_nodo;

typedef struct{
  char*   IP_FILESYSTEM;
  char*   PUERTO_FILESYSTEM;
  char*   NOMBRE_N;
  char*   PUERTO_WORKER;
  char*   RUTA_DATABIN;
  char* 	  PUERTO_NODO;
  char*   IP_NODO;
} DataNodePRUEBA;

t_list* blokesDN;//t_bloqueDN

char * databin;
int cantidadBloques;
t_config_nodo* config;

bool verificarExistenciaDeArchivo(char*);
t_config_nodo* levantarConfiguracionNodo(char*);
void conectarConFS();
char getBloque(int bloque);
int setBloque(int bloque, char data[DATANODE_BLOCK_SIZE]);
void calcularCantidadDeBloques(int fd);
void mapearDataBin();


#endif /* FUNCIONESDATANODE_H_ */
