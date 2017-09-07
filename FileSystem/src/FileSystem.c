/*
 ============================================================================
 Name        : FileSystem.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "funcionesFS.h"

int main(int argc, char** argv){

	logger = log_create("../logFileSystem","filesystem", 0, LOG_LEVEL_TRACE);
	crearConfig(argc,argv);
	destruirConfig(config);

	return EXIT_SUCCESS;
}
