/*
 * funcionesYAMA.c
 *
 *  Created on: 10/9/2017
 *      Author: utnso
 */

#include "funcionesYAMA.h"

void crearConfig(){

	if(verificarExistenciaDeArchivo(configuracionYAMA)){
		config=levantarConfiguracionMaster(configuracionYAMA);
		log_info(logger,"Configuracion levantada correctamente");

	}
	else if(verificarExistenciaDeArchivo(string_substring_from(configuracionYAMA,3))){
		config=levantarConfiguracionMaster(string_substring_from(configuracionYAMA,3));
		log_info(logger,"Configuracion levantada correctamente");

	}
	else{
		log_error(logger,"No se pudo levantar el archivo de configuracion");
		exit(EXIT_FAILURE);
	}

}

t_config_YAMA* levantarConfiguracionMaster(char* archivo_conf) {

	t_config_YAMA* conf = malloc(sizeof(t_config_YAMA));
        t_config* configYAMA;

        log_debug(logger,"Path config: %s",archivo_conf);

        configYAMA = config_create(archivo_conf);
        if(configYAMA == NULL)
        	log_error(logger,"ERROR");

        if(!verificarConfig(configYAMA))
        	log_error(logger,"CONFIG NO VALIDA");

        conf->FS_ip = malloc(MAX_LEN_IP);
        strcpy(conf->FS_ip, config_get_string_value(configYAMA, "FS_IP"));

        conf->FS_Puerto = malloc(MAX_LEN_PUERTO);
        strcpy(conf->FS_Puerto, config_get_string_value(configYAMA, "FS_PUERTO"));

       conf->Retardo_Planificacion = config_get_int_value(configYAMA, "RETARDO_PLANIFICACION");

       conf->Algoritmo_Balanceo = malloc(MAX_ALGORITMO);
       strcpy(conf->Algoritmo_Balanceo, config_get_string_value(configYAMA, "ALGORITMO_BALANCEO"));

        config_destroy(configYAMA);
        printf("Configuracion levantada correctamente.\n");
        return conf;
}

bool verificarConfig(t_config* config){
	return 	config_has_property(config,"FS_IP") &&
			config_has_property(config,"FS_PUERTO") &&
			config_has_property(config,"RETARDO_PLANIFICACION") &&
			config_has_property(config,"ALGORITMO_BALANCEO");
}

bool verificarExistenciaDeArchivo(char* path) {
	FILE * archivoConfig = fopen(path, "r");
	if (archivoConfig!=NULL){
		 fclose(archivoConfig);
		 return true;
	}
	return false;
}

void destruirConfig(t_config_YAMA* config){
	free(config->FS_ip);
	free(config->FS_Puerto);
	free(config->Retardo_Planificacion); //Si le meto un "&" cuando intento ejecutar el proceso me tira un error de doble free
	free(config->Algoritmo_Balanceo);
	free(config);
}


