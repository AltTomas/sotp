/*
 ============================================================================
 Name        : DataNode.c
 Author      : Christian Monasterios
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <commons/collections/dictionary.h>
#include <commons/collections/list.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/log.h>
#include <sys/socket.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include "estructuras.h"

t_DataNode* data_DataNode;

void leerArchivoConfig(char* rutaArchivoConfig);



int main(int argc, char *argv[]) {

	printf("%s\n",argv[1]);

	char* path = argv[1];

    leerArchivoConfig(path);

			printf("\n IP_FS: %s",data_DataNode->IP_FILESYSTEM);
			printf("\n PUERTO_FS: %d",data_DataNode->PUERTO_FILESYSTEM);
			printf("\n NOMBRE NODO: %s", data_DataNode->NOMBRE_NODO);
			printf("\n PUERTO WKR: %d", data_DataNode->PUERTO_WORKER);
			printf("\n PUERTO DTND: %d", data_DataNode->PUERTO_DATANODE);
			printf("\n RUTA BIN: %s",data_DataNode->RUTA_DATABIN);

			return 0;
}

void leerArchivoConfig(char* rutaArchivoConfig)
{
	data_DataNode = malloc(sizeof(t_DataNode));

	data_DataNode->IP_FILESYSTEM=strdup("");
	data_DataNode->NOMBRE_NODO=strdup("");
	data_DataNode->RUTA_DATABIN=strdup("");

	t_config *configuracion = config_create(rutaArchivoConfig);

	string_append(&data_DataNode->IP_FILESYSTEM,config_get_string_value(configuracion,"IP_FILESYSTEM"));
	string_append(&data_DataNode->NOMBRE_NODO,config_get_string_value(configuracion,"NOMBRE_NODO"));
	string_append(&data_DataNode->RUTA_DATABIN,config_get_string_value(configuracion,"RUTA_DATABIN"));
	data_DataNode->PUERTO_FILESYSTEM= config_get_int_value(configuracion,"PUERTO_FILESYSTEM");
	data_DataNode->PUERTO_WORKER=config_get_int_value(configuracion,"PUERTO_WORKER");

	config_destroy(configuracion);
}
