/*
 * tablaDirectorio.h
 *
 *  Created on: 30/9/2017
 *      Author: utnso
 */

#ifndef TABLADIRECTORIOS_H_
#define TABLADIRECTORIOS_H_

#include <assert.h>

//#define DIRECTORIOS_LENGTH 100

#define METADATA_DIRECTORIO_PATH "metadata/directorios.dat"

typedef struct {
	int index;
	char nombre[255];
	int padre;
} t_directory;

static_assert( (t_directory.index) < 100, "debe haber menos de 100 directorios");

#endif /* TABLADIRECTORIOS_H_ */
