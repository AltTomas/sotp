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

extern t_log* logger;
int socketConexionFS;


#define DATANODE_BLOCK_SIZE 1048576
typedef unsigned char dataNode_block[DATANODE_BLOCK_SIZE];
typedef uint32_t dataNode_block_pointer;

typedef struct{
  char*   IP_FILESYSTEM;
	int   PUERTO_FILESYSTEM;
  char*   NOMBRE_NODO;
	int   PUERTO_WORKER;
 char*   RUTA_DATABIN;
} t_DataNode;

t_DataNode* data_DataNode;


void leerArchivoConfig(char* rutaArchivoConfig);
void conectarConFS(void);

#endif /* FUNCIONESDATANODE_H_ */
