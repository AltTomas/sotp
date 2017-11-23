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
	int tamTotalNodo;
	int tamLibreNodo;
} nodo;

typedef struct {
	int tamanio_total;
	int libre_total;
	char** nodos; //struct nodo
	t_list* bloquesTotalesyLibres;
} t_Nodos;



#endif /* TABLANODOS_H_ */
