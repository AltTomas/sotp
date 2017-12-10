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

	logger = log_create("../../logFileSystem","filesystem", 0, LOG_LEVEL_TRACE);
	crearConfig();

	info_DataNodes = list_create();
	archivos = list_create();
	directorios = list_create();
	int estadoConexiones = 1; //1 es activado, 0 es desactivado
	int estadoFileSystem = 0; //0 es inestable, 1 es estable

	if(argc == 2){
		if(strcmp(argv[1],"--clean")==0){
			formatear(); // Esto si se inicio con --clean
		}
	} else{
		recuperarArchivo();
		recuperarDirectorio();
		recuperarNodo();
		int estadoEjecucion = 0; //0 es normal, 1 es si el fs es nuevo
	}


	destruirConfig(config);

	void escucharConexiones();

	commandHandler();
	return EXIT_SUCCESS;
}
