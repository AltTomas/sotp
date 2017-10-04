/*
 ============================================================================
 Name        : Worker.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "funcionesWorker.h"

int main(void) {

	logger = log_create("../logWorker","Worker", 0, LOG_LEVEL_TRACE);
	crearConfig();
	destruirConfig(config);

	crearServidor(config->Worker_Puerto);

	}
