/*
 ============================================================================
 Name        : YAMA.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "funcionesYAMA.h"

t_list * listaProcesos;

int main(void) {
	listaProcesos = list_create();
	logger = log_create("../logYAMA","YAMA", 0, LOG_LEVEL_TRACE);
	crearConfig();
	conectarConFS();
	escucharConexiones();

	destruirConfig(config);
	return EXIT_SUCCESS;
}
