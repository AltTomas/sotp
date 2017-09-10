/*
 * funcionesYAMA.h
 *
 *  Created on: 10/9/2017
 *      Author: utnso
 */

#ifndef FUNCIONESYAMA_H_
#define FUNCIONESYAMA_H_

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

#define configuracionYAMA "../../Configs/configYAMA.config"
#define MAX_LEN_PUERTO 6
#define MAX_ALGORITMO 3
#define MAX_LEN_IP 20

typedef struct {
	char* FS_ip;
	char* FS_Puerto;
	int Retardo_Planificacion;
	char* Algoritmo_Balanceo;
}t_config_YAMA;


t_log* logger;
t_config_YAMA* config;


void crearConfig();
t_config_YAMA* levantarConfiguracionMaster(char*);
bool verificarConfig(t_config*);
bool verificarExistenciaDeArchivo(char*);
void destruirConfig(t_config_YAMA*);


#endif /* FUNCIONESYAMA_H_ */
