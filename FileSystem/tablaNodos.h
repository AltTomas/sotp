/*
 * tablaNodos.h
 *
 *  Created on: 30/9/2017
 *      Author: utnso
 */

#ifndef TABLANODOS_H_
#define TABLANODOS_H_

#include <stdint.h>

#define METADATA_NODOS_PATH "metadata/nodos.bin"


typedef struct {
	char* nombreNodo;
	int tamTotal;
	int tamLibre;
} nodo;

typedef struct {
	int tamanio_total;
	int libre_total;
	t_list* nodos; //struct nodo
} t_Nodos;



#endif /* TABLANODOS_H_ */
