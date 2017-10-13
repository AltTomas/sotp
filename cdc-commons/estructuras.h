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

typedef struct controller{
	u_int32_t job;
	u_int32_t master;
	char* nodo;
	u_int32_t bloque;
	char* etapa;
	char* fileTemporal;
	char* status;
} t_controller;

typedef struct Stream {
	int length;
	char* data;
} t_stream;

typedef struct Cabecera {
	uint32_t id;		/* ID de operacion */
	uint32_t longitud; /* Longitud del mensaje, en bytes */
} cabecera_t;

typedef struct Info_Nodo{
	char * ip;
	int puerto;
} t_info_nodo;

typedef struct Info_Bloque{
	int nroBloque;
	t_list* ubicacionBloques;
}t_info_bloque;

enum{

	//Generales
	D_STRUCT_NUMERO=1,
	D_STRUCT_CHAR=2,
	D_STRUCT_STRING=3,

	D_STRUCT_JOBT=4,
	D_STRUCT_JOBR=5,

	D_STRUCT_BLOQUE =6,

	/*OPERACIONES YAMA-FS*/
	YAMA_FS_GET_DATA_BY_FILE = 17,

	/*OPERACIONES FS-YAMA*/
	FS_YAMA_NOT_LOAD = 18,
	FS_YAMA_CANTIDAD_BLOQUES = 19,
	FS_YAMA_UBICACION_BLOQUES = 20,

	/*OPERACIONES YAMA-MASTER*/
	YAMA_MASTER_FS_NOT_LOAD = 11,
	YAMA_MASTER_EJECUTAR_TRANSFORMACION = 12,
	YAMA_MASTER_EJECUTAR_REDUCCION_LOCAL = 13,
	YAMA_MASTER_EJECUTAR_REDUCCION_GLOBAL = 14,
	YAMA_MASTER_CONECTARSE_A = 15,
	YAMA_MASTER_DATA_NODO = 16,
	/* OPERACIONES MASTER-YAMA */
	MASTER_YAMA_SOLICITAR_INFO_NODO=10,
	MASTER_YAMA_CONEXION_WORKER_OK = 20,
	MASTER_YAMA_ESTADO_NODO = 21,

	//Handshake
	ES_YAMA=100,
	ES_MASTER=101,
	ES_WORKER=102,
	ES_FILESYSTEM=103,
	ES_DATANODE=104,

	//Errores
	D_STRUCT_ERR=40,
	ERR_RCV=41



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

typedef struct error{
	int errorid;
}__attribute__ ((__packed__)) t_struct_error;

typedef struct infoNodo{
	char* nombreNodo;
	int   bloquesDisponibles;
}__attribute__((__packed__)) t_infoNodo;

#endif /* ESTRUCTURAS_H_ */
