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
#include <stdbool.h>
#include <pthread.h>

#define configuracionMaster "../configMaster.config"
#define MAX_LEN_PUERTO 6
#define MAX_LEN_IP 20

typedef struct {
	char* YAMA_puerto;
	char* YAMA_ip;
}t_config_master;


t_log* logger;
t_config_master* config;


void crearConfig(int, char**);
t_config_master* levantarConfiguracionMaster(char*);
bool verificarConfig(t_config_master*);
bool verificarExistenciaDeArchivo(char*);
void destruirConfig(t_config_master*);

#endif /* FUNCIONESMASTER_H_ */
