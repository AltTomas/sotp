#ifndef ESTRUCTURAS_H_
#define ESTRUCTURAS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdint.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <commons/string.h>

//Estructura definida para Archivo de Configuracion del DataNode.
typedef struct{
  char*   IP_FILESYSTEM;
	int   PUERTO_FILESYSTEM;
  char*   NOMBRE_NODO;
	int   PUERTO_WORKER;
	int   PUERTO_DATANODE;
  char*   RUTA_DATABIN;
} t_DataNode;

typedef uint8_t t_tipoEstructura;

typedef struct Stream {
	int length;
	char* data;
} t_stream;

typedef struct Cabecera {
	uint32_t id;		/* ID de operacion */
	uint32_t longitud; /* Longitud del mensaje, en bytes */
} cabecera_t;


enum{

	//Generales
	D_STRUCT_NUMERO=1,
	D_STRUCT_CHAR=2,
	D_STRUCT_STRING=3,

	D_STRUCT_JOBT=4,
	D_STRUCT_JOBR=5,


	//Handshake
	ES_YAMA=100,
	ES_MASTER=101,
	ES_WORKER=102,
	ES_FILESYSTEM=103,
	ES_DATANODE=104,

	//Errores
	ERR_RCV=40, //Se utiliza para comunicar un error al recibir

	//Respuestas
	JOB_OK=30



} t_operaciones;

// Header de stream
typedef struct {
	uint8_t tipoEstructura;
	uint16_t length;
} __attribute__ ((__packed__)) t_header;


// Estructuras segun tipo de datos a enviar por sockets

typedef struct struct_numero {
	int32_t numero;
} __attribute__ ((__packed__)) t_struct_numero;

typedef struct struct_char {
	char letra;
} __attribute__ ((__packed__)) t_struct_char;

typedef struct struct_string {
	char * string;
} __attribute__ ((__packed__)) t_struct_string;

typedef struct jobT{
	char* scriptTransformacion;
	char* pathOrigen;
	char* pathTemporal;
}__attribute__((__packed__)) t_struct_jobT;

typedef struct jobR{
	char* scriptReduccion;
	char* pathTemp;
	char* pathTempFinal;
}__attribute__((__packed__)) t_struct_jobR;


#endif /* ESTRUCTURAS_H_ */
