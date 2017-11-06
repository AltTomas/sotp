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


/*typedef struct {
	int numBloque;
	int numNodo;
	char* ipNodo;
	int puertoNodo;
} bloquePrueba;*/


typedef struct {
	int numNodo;
	int bloqueNodo; //bloque del nodo que compone el archivo
	char* ip;
	int puerto;
} copia;


typedef struct {
	int numBloque; //bloque individual del archivo (0,1,2,...)
	copia* copia0;
	copia* copia1;
	int finBloque; //byte que indica el ultimo byte del bloque
} bloque;


typedef struct {
	char* path;
	int tamanio;
	char* tipo; //lectura o texto
	t_list* bloques; //sruct bloque
} file;



#endif /* TABLAARCHIVOS_H_ */

