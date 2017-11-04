#ifndef FUNCIONESMASTER_H_
#define FUNCIONESMASTER_H_

#include <sockets.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/collections/queue.h>
#include <commons/log.h>
#include <commons/collections/dictionary.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <stdbool.h>
#include <pthread.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>

#include <semaphore.h>


#define configuracionMaster "../../Configs/configMaster.config"
#define MAX_LEN_PUERTO 6
#define MAX_LEN_IP 20
#define MAX_LEN_RUTA 100

typedef struct {
	char* YAMA_puerto;
	char* YAMA_ip;
}t_config_master;

typedef struct {
	char* script_transformacion;
	char* script_reduccion;
	char* archivo;
	char* archivo_resultado;
}t_argumentos;

typedef struct {
	char* ip;
	int puerto;
	int idNodo;
	int numeroBloque;
}t_conexionNodo_transformacion;


extern t_log* logger;
t_config_master* config;
int socketConexionYAMA;
t_argumentos* argumentosMaster;
int cantidadNodosReduccion;
int contadorNodosReduccion;
sem_t bin_reduccion;
char* scriptTransformacion;
char* scriptReduccion;

void crearConfig();
t_config_master* levantarConfiguracionMaster(char*);
bool verificarConfig(t_config*);
bool verificarExistenciaDeArchivo(char*);
void destruirConfig(t_config_master*);
void conectarConYAMA(void);
char* obtenerContenido(char*);
void ejecutarJob(char**);
void ejecutarTransformacion (t_infoNodo_transformacion*);
void ejecutarReduccionLocal (t_infoNodo_reduccionLocal*);
void ejecutarReduccionGlobal(t_infoNodo_reduccionGlobal*, t_list*, char*);
bool esEncargado(t_infoNodo_reduccionGlobal* nodo);
t_infoNodo_reduccionGlobal* buscarEncargado(t_list*);
void ejecutarAlmacenamientoFinal(t_infoNodo_Final* );
void mostrarMetricas();

#endif /* FUNCIONESMASTER_H_ */
