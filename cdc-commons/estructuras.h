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
	  D_STRUCT_NODOS=7,
	  D_STRUCT_TRANSFORMACION=8,
	  D_STRUCT_CONFIRMACION_TRANSFORMACION=9,

	  /*OPERACIONES YAMA-FS*/
	  YAMA_FS_GET_DATA_BY_FILE = 00,
	  YAMA_FS_SOLICITAR_LISTABLOQUES = 01,

	  /*OPERACIONES FS-YAMA*/
	  FS_YAMA_NOT_LOAD = 10,
	  FS_YAMA_CANTIDAD_BLOQUES = 11,
	  FS_YAMA_UBICACION_BLOQUES = 12,
	  FS_YAMA_LISTABLOQUES = 13,

	  /*OPERACIONES YAMA-MASTER*/
	  YAMA_MASTER_FS_NOT_LOAD = 21,
	  YAMA_MASTER_EJECUTAR_TRANSFORMACION = 22,
	  YAMA_MASTER_EJECUTAR_REDUCCION_LOCAL = 23,
	  YAMA_MASTER_EJECUTAR_REDUCCION_GLOBAL = 24,
	  YAMA_MASTER_CONECTARSE_A = 25,
	  YAMA_MASTER_DATA_NODO = 26,

	  /* OPERACIONES MASTER-YAMA */
	  MASTER_YAMA_SOLICITAR_INFO_NODO=30,

	  /* OPERACIONES MASTER-WORKER*/

	  /* OPERACIONES WORKER-MASTER*/
	  WORKER_MASTER_TRANSFORMACION_OK=50,
	  WORKER_MASTER_TRANSFORMACION_FALLO=51,

	  //Handshake
	  ES_YAMA=100,
	  ES_MASTER=101,
	  ES_WORKER=102,
	  ES_FILESYSTEM=103,
	  ES_DATANODE=104,

	  //Errores
	  D_STRUCT_ERR=90,
	  ERR_RCV=91

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

typedef struct infoNodo{ // Lista de nodos con bloques que contienen al archivo pedido por Master
	char* ip;
	int puerto;
	int numBloque;
	int bytesOcupados;
	char* nombreTemporal;
}__attribute__((__packed__)) t_infoNodo;

typedef struct bloques{ // Elementos de la lista anterior
    int numBloque;
    int numNodo;
    int bytesOcupados;
    char* ip;
    int puerto;
}__attribute__((__packed__)) t_struct_bloques;

typedef struct nodos{ // Lista de nodos que recibe master
	t_list* lista_nodos;
}__attribute__((__packed__))t_struct_nodos_transformacion;

typedef struct transformacion{ // Los elementos de la lista recibida por master
	char* ip;
	int puerto;
	int bloque;
	int bytes_ocupados_bloque;
	char* nombre_archivo_temporal;
	char* script_transformacion;
}__attribute__((__packed__))t_struct_transformacion;

typedef struct confirmacionTransformacion{
	int confirmacion;
	int nodo;
}__attribute__((__packed__))t_struct_confirmacion_transformacion;

#endif /* ESTRUCTURAS_H_ */
