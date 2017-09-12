/*
 * estructuras.h
 *
 *  Created on: 9/9/2017
 *      Author: utnso
 */

#ifndef ESTRUCTURAS_H_
#define ESTRUCTURAS_H_

//Estructura definida para Archivo de Configuracion del DataNode.

typedef struct{
  char*   IP_FILESYSTEM;
	int   PUERTO_FILESYSTEM;
  char*   NOMBRE_NODO;
	int   PUERTO_WORKER;
	int   PUERTO_DATANODE;
  char*   RUTA_DATABIN;
} t_DataNode;


#endif /* ESTRUCTURAS_H_ */
