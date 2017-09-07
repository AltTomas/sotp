
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
#include <stdbool.h>
#include <pthread.h>

#define configuracionfs "../configFileSystem.config"
#define MAX_LEN_PUERTO 6
#define PUNTO_MONTAJE "PUNTO_MONTAJE"

typedef struct {
	char* fs_puerto;
	char* punto_montaje;
}t_config_fs;


t_log* logger;
t_config_fs* config;


void crearConfig(int, char**);
t_config_fs* levantarConfiguracionfs(char*);
bool verificarConfig(t_config_fs*);
bool verificarExistenciaDeArchivo(char*);
void destruirConfig(t_config_fs*);

#endif /* FUNCIONESFS_H_ */