#ifndef FUNCIONESDATANODE_H_
#define FUNCIONESDATANODE_H_


#include <commons/collections/dictionary.h>
#include <commons/collections/list.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/log.h>
#include <sys/socket.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>


extern t_log* logger;
int socketConexionFS;

typedef struct{
  char*   IP_FILESYSTEM;
	int   PUERTO_FILESYSTEM;
  char*   NOMBRE_NODO;
	int   PUERTO_WORKER;
	int   PUERTO_DATANODE;
  char*   RUTA_DATABIN;
} t_DataNode;

t_DataNode* data_DataNode;

void leerArchivoConfig(char* rutaArchivoConfig);
void conectarConFS(void);

#endif /* FUNCIONESDATANODE_H_ */
