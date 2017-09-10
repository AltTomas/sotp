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

int main(void) {

	logger = log_create("../logYAMA","YAMA", 0, LOG_LEVEL_TRACE);
	crearConfig();
	destruirConfig(config);
	puts("!!!Hello World!!!"); /* prints !!!Hello World!!! */
	return EXIT_SUCCESS;
	}
