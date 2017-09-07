/*
 ============================================================================
 Name        : Master.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "funcionesMaster.h"

int main(int argc, char** argv){

	logger = log_create("../logKernel","kernel", 0, LOG_LEVEL_TRACE);
	crearConfig(argc,argv);
	destruirConfig(config);
	puts("!!!Hello World!!!"); /* prints !!!Hello World!!! */
	return EXIT_SUCCESS;
}
