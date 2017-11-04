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
#include <sockets.h>
#include <sys/types.h>
#include <sys/wait.h>

#define configuracionWorker "/home/utnso/workspace/tp-2017-2c-Los-caballeros-del-C-diaco/Configs/configNodo.config"
#define MAX_LEN_PUERTO 6
#define MAX_LEN_NOMBRE 7
#define MAX_RUTA 150
#define MAX_LEN_IP 20

typedef struct {
	int Worker_Puerto;
}t_config_Worker;

t_log* logger;
t_config_Worker* config;
int socketEscucha;
int max_fd;
fd_set maestro;
fd_set setMasters;

void crearConfig();
t_config_Worker* levantarConfiguracionWorker(char*);
bool verificarConfig(t_config*);
bool verificarExistenciaDeArchivo(char*);
void destruirConfig(t_config_Worker*);
void aceptarNuevaConexion(int socketEscucha, fd_set* set);
int doJob_transformacion(t_struct_jobT* job);
int doJob_reduccion(t_struct_jobR* job);
void atenderMaster(int socketMaster);
void escucharConexiones(void);


#endif /* FUNCIONESWORKER_H_ */
