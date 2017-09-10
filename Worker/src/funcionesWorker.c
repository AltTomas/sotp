/*
 * funcionesWorker.c
 *
 *  Created on: 10/9/2017
 *      Author: utnso
 */

#include "funcionesWorker.h"

void crearConfig(){

	if(verificarExistenciaDeArchivo(configuracionWorker)){
		config=levantarConfiguracionWorker(configuracionWorker);
		log_info(logger,"Configuracion levantada correctamente");

	}
	else if(verificarExistenciaDeArchivo(string_substring_from(configuracionWorker,3))){
		config=levantarConfiguracionWorker(string_substring_from(configuracionWorker,3));
		log_info(logger,"Configuracion levantada correctamente");

	}
	else{
		log_error(logger,"No se pudo levantar el archivo de configuracion");
		exit(EXIT_FAILURE);
	}

}

t_config_Worker* levantarConfiguracionWorker(char* archivo_conf) {

	t_config_Worker* conf = malloc(sizeof(t_config_Worker));
        t_config* configWorker;

        log_debug(logger,"Path config: %s",archivo_conf);

        configWorker = config_create(archivo_conf);
        if(configWorker == NULL)
        	log_error(logger,"ERROR");

        if(!verificarConfig(configWorker))
        	log_error(logger,"CONFIG NO VALIDA");

        conf->FS_ip = malloc(MAX_LEN_IP);
        strcpy(conf->FS_ip, config_get_string_value(configWorker, "IP_FILESYSTEM"));

        conf->FS_Puerto = malloc(MAX_LEN_PUERTO);
        strcpy(conf->FS_Puerto, config_get_string_value(configWorker, "PUERTO_FILESYSTEM"));

        conf->Nombre_Nodo = malloc(MAX_LEN_NOMBRE);
        strcpy(conf->Nombre_Nodo, config_get_string_value(configWorker, "NOMBRE_NODO"));

        conf->Worker_Puerto = malloc(MAX_LEN_PUERTO);
        strcpy(conf->Worker_Puerto, config_get_string_value(configWorker, "PUERTO_WORKER"));

        conf->Ruta_Databin = malloc(MAX_RUTA);
        strcpy(conf->Ruta_Databin, config_get_string_value(configWorker, "RUTA_DATABIN"));

        config_destroy(configWorker);
        printf("Configuracion levantada correctamente.\n");
        return conf;
}

bool verificarConfig(t_config* config){
	return config_has_property(config,"IP_FILESYSTEM") &&
			config_has_property(config,"PUERTO_FILESYSTEM") &&
			config_has_property(config,"NOMBRE_NODO") &&
			config_has_property(config,"PUERTO_WORKER") &&
			config_has_property(config,"RUTA_DATABIN");
}

bool verificarExistenciaDeArchivo(char* path) {
	FILE * archivoConfig = fopen(path, "r");
	if (archivoConfig!=NULL){
		 fclose(archivoConfig);
		 return true;
	}
	return false;
}

void destruirConfig(t_config_Worker* config){
	free(config->FS_ip);
	free(config->FS_Puerto);
	free(config->Nombre_Nodo);
	free(config->Worker_Puerto);
	free(config->Ruta_Databin);
	free(config);
}
