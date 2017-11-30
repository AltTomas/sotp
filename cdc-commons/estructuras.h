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
	D_STRUCT_BLOQUE_FS_YAMA=6,
	D_STRUCT_NODOS_REDUCCION_LOCAL=7,
	D_STRUCT_NODOS_REDUCCION_GLOBAL=8,
	D_STRUCT_INFO_BLOQUE=9,
	D_STRUCT_DATA_NODE= 111,
	D_STRUCT_INFO_NODO= 112,

	D_STRUCT_NODO_TRANSFORMACION= 71,
	D_STRUCT_NODO_ESCLAVO= 61,

	  /*OPERACIONES YAMA-FS*/
	  YAMA_FS_GET_DATA_BY_FILE = 00,
	  YAMA_FS_SOLICITAR_LISTABLOQUES = 12,

	  /*OPERACIONES FS-YAMA*/
	  FS_YAMA_NOT_LOAD = 10,
	  FS_YAMA_LISTABLOQUES = 11,

	  /*OPERACIONES YAMA-MASTER*/
	  YAMA_MASTER_FS_NOT_LOAD = 21,
	  YAMA_MASTER_EJECUTAR_TRANSFORMACION = 22,
	  YAMA_MASTER_EJECUTAR_REDUCCION_LOCAL = 23,
	  YAMA_MASTER_EJECUTAR_REDUCCION_GLOBAL = 24,
	  YAMA_MASTER_CONECTARSE_A = 25,
	  YAMA_MASTER_DATA_NODO = 26,

	  /* OPERACIONES MASTER-YAMA */
	  MASTER_YAMA_SOLICITAR_INFO_NODO=30,
	  MASTER_YAMA_TRANSFORMACION_WORKER_OK=31,
	  MASTER_YAMA_TRANSFORMACION_WORKER_FALLO=32,
	  MASTER_YAMA_REDUCCIONL_WORKER_OK=32,
	  MASTER_YAMA_REDUCCIONL_WORKER_FALLO=32,
	  MASTER_YAMA__REDUCCIONG_WORKER_OK=33,
	  MASTER_YAMA__REDUCCIONG_WORKER_FALLO=34,
	  MASTER_YAMA_ALMACENAMIENTO_FINAL_WORKER_OK=35,
	  MASTER_YAMA_ALMACENAMIENTO_FINAL_WORKER_FALLO=36,
	  MASTER_YAMA_CONEXION_WORKER_OK = 38,
	  MASTER_YAMA_CONEXION_WORKER_FALLO=39,

	  /* OPERACIONES MASTER-WORKER*/

	  /* OPERACIONES WORKER-MASTER*/
	  WORKER_MASTER_TRANSFORMACION_OK=50,
	  WORKER_MASTER_TRANSFORMACION_FALLO=51,
	  WORKER_MASTER_REDUCCIONL_OK=52,
	  WORKER_MASTER_REDUCCIONL_FALLO=53,
	  WORKER_MASTER_REDUCCIONG_OK=54,
	  WORKER_MASTER_REDUCCIONG_FALLO=55,
	  WORKER_MASTER_ALMACENAMIENTO_FINAL_OK=56,
	  WORKER_MASTER_ALMACENAMIENTO_FINAL_FALLO=57,
	  JOB_OK=58,

	  /* OPERACIONES FS-DATANODE*/
	  FS_DATANODE_PEDIDO_INFO=80,
	  FS_DATANODE_ALMACENAR_BLOQUE=81,

	  //Handshake
	  ES_YAMA=100,
	  ES_MASTER=101,
	  ES_WORKER=102,
	  ES_FILESYSTEM=103,
	  ES_DATANODE=104,

	  //Errores
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
	int bloque;
	int bytesOcupadosBloque;
	char* pathOrigen;
	char* pathTemporal;
}__attribute__((__packed__)) t_struct_jobT;

typedef struct jobR{
	char* scriptReduccion;
	uint16_t cantidadTemporales;
	t_list* pathTemp; // Lista de temporales de los cuales vamos a generar 1 archivo final
	char* pathTempFinal;
}__attribute__((__packed__)) t_struct_jobR;

typedef struct jobRG{
	char* scriptReduccion;
	uint32_t cantidadNodos;
	t_list* nodos; // Lista de t_struct_nodoEsclavo
}__attribute__((__packed__)) t_struct_jobRG;

typedef struct nodoEsclavo{
	char* ip;
	int puerto;
	uint32_t idNodo; // Hara falta aca?
	char* nombreTemporal;
}__attribute__((__packed__)) t_struct_nodoEsclavo;

typedef struct infoNodoF{
	char* ip;
	int puerto;
	char* nombreResultadoRG;
}__attribute__((__packed__)) t_infoNodo_Final;

typedef struct error{
	int errorid;
}__attribute__ ((__packed__)) t_struct_error;

typedef struct infoNodoT{ // Lista de nodos con bloques que contienen al archivo pedido por Master
	char* ip;
	int puerto;
	uint32_t idNodo;
	int numBloque;
	int bytesOcupados;
	char* nombreTemporal;
}__attribute__((__packed__)) t_infoNodo_transformacion;

typedef struct infoNodoR{ // Lista de nodos con bloques que contienen al archivo pedido por Master
	char* ip;
	int puerto;
	uint32_t idNodo;
	uint32_t cantidadTemporales;
	t_list* pathTemp; // Lista de temporales de los cuales vamos a generar 1 archivo final
	char* pathTempFinal;
}__attribute__((__packed__)) t_infoNodo_reduccionLocal;

typedef struct infoNodoRG{
	char* ip;
	int puerto;
	uint32_t idNodo;
	bool encargado;
	char* pathTemporal;
	char* pathFinal;
}__attribute__((__packed__)) t_infoNodo_reduccionGlobal;

typedef struct bloques{ // Elementos de la lista anterior
    int finalBloque;
    int numNodoOriginal;
    int numBloqueOriginal;
    char* ipNodoOriginal;
    int puertoNodoOriginal;
    int numNodoCopia;
    int numBloqueCopia;
    char* ipNodoCopia;
    int puertoNodoCopia;
}__attribute__((__packed__)) t_struct_bloques;

typedef struct t_tablaEstados{
	u_int32_t job;
	u_int32_t master;
	u_int32_t nodo;
	u_int32_t bloque;
	u_int32_t etapa;
	char* fileTemporal;
	u_int32_t status;
} t_tablaEstados;

typedef struct bloqueDN{
	int numBloque;
	int estado; //ocupado o libre
}__attribute__((__packed__)) t_struct_bloqueDN;


typedef struct dataNode{
	char* ipDN;
	int puertoDN;
	char* nomDN;
	t_list* bloqueDN;//t_struct_bloqueDN
}__attribute__((__packed__)) t_struct_datanode;

typedef struct balanceoCargas{
	int availability;
	int worker;
	t_list* bloques;
}__attribute__((__packed__)) balanceoCargas;

typedef struct listaUbicacionBloques{
	int finalBloque;
	int idNodoOriginal;
	int numeroBloqueOriginal;
	char* ipOriginal;
	int puertoOriginal;
	int idNodoCopia;
	int numeroBloqueCopia;
	char* ipCopia;
	int puertoCopia;
}__attribute__((__packed__)) listaUbicacionBloques;

typedef struct almacenarBloque{
	char* contenidoBloque;
	int bloqueArchivo;
	int bytesOcupados;
}__attribute__((__packed__)) t_almacenar_bloque;

#endif /* ESTRUCTURAS_H_ */
