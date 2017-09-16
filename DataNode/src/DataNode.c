/*
 ============================================================================
 Name        : DataNode.c
 Author      : Christian Monasterios
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "funcionesDataNode.h"

t_DataNode* data_DataNode;

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

			conectarConFS();

			return 0;
}

