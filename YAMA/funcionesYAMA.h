#ifndef FUNCIONESYAMA_H_
#define FUNCIONESYAMA_H_

#include <sockets.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/collections/queue.h>
#include <commons/collections/dictionary.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <stdbool.h>
#include <pthread.h>
#include <commons/temporal.h>

#define configuracionYAMA "../../Configs/configYAMA.config"
#define MAX_LEN_PUERTO 6
#define MAX_ALGORITMO 6
#define MAX_LEN_IP 20
#define ESTADO_EN_PROCESO 800
#define ESTADO_ERROR 801
#define ESTADO_FINALIZADO_OK 802
#define ETAPA_TRANSFORMACION 803
#define ETAPA_REDUCCION_LOCAL 804
#define ETAPA_REDUCCION_GLOBAL 805

typedef struct {
	char* FS_ip;
	char* FS_Puerto;
	char* YAMA_Puerto;
	int Retardo_Planificacion;
	char* Algoritmo_Balanceo;
	u_int32_t Disp_Base;
}t_config_YAMA;


extern t_log* logger;
t_config_YAMA* config;
int socketConexionFS;
fd_set maestro;
fd_set setMasters;
int socketEscuchaMasters;
int max_fd;


void crearConfig();
t_config_YAMA* levantarConfiguracionMaster(char*);
bool verificarConfig(t_config*);
bool verificarExistenciaDeArchivo(char*);
void destruirConfig(t_config_YAMA*);
int conectarConFS(void);
void escucharConexiones(void);
void aceptarNuevaConexion(int, fd_set* );
void trabajarSolicitudMaster(int);
char* generarNombreTemporal(int, int);
void getNodoByFile(char*, int);
bool workerTieneBloque(int indexWorkerActual, int numBloque);
void asignarBloquesALosWorkers();

/* Variables Globales */
int socketConexionFS;

t_list * tablaEstados;
t_list* listaInfoBloques;
t_list* listaBalanceoCargas;
t_dictionary* ubicacionBloques;

#endif /* FUNCIONESYAMA_H_ */
