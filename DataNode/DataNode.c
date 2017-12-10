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

int main(int argc, char *argv[]) {

	logger = log_create("../../logDataNode","datanode", 0, LOG_LEVEL_TRACE);

	crearConfig();

	conectarConFS();

	return 0;
}


