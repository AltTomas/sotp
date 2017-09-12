/*
 * funcionesMaster.h
 *
 *  Created on: 6/9/2017
 *      Author: utnso
 */

#ifndef FUNCIONESMASTER_H_
#define FUNCIONESMASTER_H_

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
#include "sockets.h"

#define configuracionMaster "../../Configs/configMaster.config"
#define MAX_LEN_PUERTO 6
#define MAX_LEN_IP 20
#define MAX_LEN_RUTA 100

typedef struct {
	char* YAMA_puerto;
	char* YAMA_ip;
}t_config_master;


extern t_log* logger;
t_config_master* config;
int socketConexionYAMA;


void crearConfig();
t_config_master* levantarConfiguracionMaster(char*);
bool verificarConfig(t_config*);
bool verificarExistenciaDeArchivo(char*);
void destruirConfig(t_config_master*);
void conectarConYAMA(void);
void ejecutarJob(char**);

#endif /* FUNCIONESMASTER_H_ */
