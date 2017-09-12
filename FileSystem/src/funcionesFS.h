
#ifndef FUNCIONESFS_H_
#define FUNCIONESFS_H_

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

#define configuracionFS "../../Configs/configFileSystem.config"
#define MAX_LEN_PUERTO 6
#define PUNTO_MONTAJE 250

typedef struct {
	char* fs_puerto;
	char* punto_montaje;
}t_config_fs;


t_log* logger;
t_config_fs* config;


void crearConfig();
t_config_fs* levantarConfiguracionFS(char*);
bool verificarConfig(t_config*);
bool verificarExistenciaDeArchivo(char*);
void destruirConfig(t_config_fs*);

#endif /* FUNCIONESFS_H_ */
