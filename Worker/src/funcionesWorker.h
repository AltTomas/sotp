/*
 * funcionesWorker.h
 *
 *  Created on: 10/9/2017
 *      Author: utnso
 */

#ifndef FUNCIONESWORKER_H_
#define FUNCIONESWORKER_H_

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

#define configuracionWorker "../../Configs/configNodo.config"
#define MAX_LEN_PUERTO 6
#define MAX_LEN_NOMBRE 7
#define MAX_RUTA 150
#define MAX_LEN_IP 20

typedef struct {
	char* FS_ip;
	char* FS_Puerto;
	char* Nombre_Nodo;
	char* Worker_Puerto;
	char* Ruta_Databin;
}t_config_Worker;

t_log* logger;
t_config_Worker* config;


void crearConfig();
t_config_Worker* levantarConfiguracionWorker(char*);
bool verificarConfig(t_config*);
bool verificarExistenciaDeArchivo(char*);
void destruirConfig(t_config_Worker*);


#endif /* FUNCIONESWORKER_H_ */
