/*
 * tablaArchivos.h

 *
 *  Created on: 30/9/2017
 *      Author: utnso
 */


#ifndef TABLAARCHIVOS_H_
#define TABLAARCHIVOS_H_

#define METADATA_FILE_PATH "metadata/archivos" //Deberia ser metadata/archivos/(index del directorio)/ejemplo.csv
#define ARCH_EXT ".csv" //Verificar
#define PATH_DIV "/"

typedef struct {
	int tamanio;
	char* tipo; //lectura o texto
} sadica_file;

#endif /* TABLAARCHIVOS_H_ */
