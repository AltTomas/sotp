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
	int tamanio;
	int libre;
	uint32_t nodos;
} t_Nodos;


#endif /* TABLANODOS_H_ */
