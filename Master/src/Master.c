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

/*./Master argv[0]
 *  Script transformador	argv[1]
 *  Script reductor			argv[2]
 *  Archivo  				argv[3]
 *  Resultado				argv[4]
*/
	logger = log_create("../logMaster","Master", 0, LOG_LEVEL_TRACE);

	if(argc != 5){
		log_error(logger,"ERROR: Se intento iniciar el programa con un numero incorrecto de parametros");
		puts("ERROR: Se intento iniciar el programa con un numero incorrecto de parametros");
		exit(1);
	}

	crearConfig();

	conectarConYAMA();

	ejecutarJob(argv);

	while(1){

	}

	destruirConfig(config);
	puts("!!!Hello World!!!"); /* prints !!!Hello World!!! */
	return EXIT_SUCCESS;
}
