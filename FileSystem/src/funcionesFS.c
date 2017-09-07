#include "funcionesFS.h"

void crearConfig(int32_t argc, char* argv[]){


	if(argc>1){
			if(verificarExistenciaDeArchivo(argv[1])){
				config=levantarConfiguracionFS(argv[1]);
				log_info(logger, "Configuracion levantada correctamente");
			}else{
				log_error(logger,"Ruta incorrecta");
				exit(EXIT_FAILURE);
			}
	}
	else if(verificarExistenciaDeArchivo(configuracionfs)){
		config=levantarConfiguracionFS(configuracionfs);
		log_info(logger,"Configuracion levantada correctamente");

	}
	else if(verificarExistenciaDeArchivo(string_substring_from(configuracionfs,3))){
		config=levantarConfiguracionFS(string_substring_from(configuracionfs,3));
		log_info(logger,"Configuracion levantada correctamente");
	}
	else{
		log_error(logger,"No se pudo levantar el archivo de configuracion");
		exit(EXIT_FAILURE);
	}

}

t_config_fs* levantarConfiguracionFS(char* archivo_conf) {

        t_config_fs* conf = malloc(sizeof(t_config_fs));
        t_config* configfs;

        log_debug(logger,"Path config: %s",archivo_conf);

        configfs = config_create(archivo_conf);
        if(configfs == NULL)
        	log_error(logger,"ERROR");

        if(!verificarConfig(configfs))
        	log_error(logger,"CONFIG NO VALIDA");

        conf->punto_montaje = malloc(PUNTO_MONTAJE);
        strcpy(conf->punto_montaje, config_get_string_value(configfs, "PUNTO_MONTAJE"));

        conf->fs_puerto = malloc(MAX_LEN_PUERTO);
        strcpy(conf->fs_puerto, config_get_int_value(configfs, "FS_PUERTO"));

        config_destroy(configfs);
        printf("Configuracion levantada correctamente.\n");
        return conf;
}

bool verificarConfig(t_config_fs* config){
	return config_has_property(config,"PUNTO_MONTAJE") && config_has_property(config,"FS_PUERTO");
}

bool verificarExistenciaDeArchivo(char* path) {
	FILE * archivoConfig = fopen(path, "r");
	if (archivoConfig!=NULL){
		 fclose(archivoConfig);
		 return true;
	}
	return false;
}

void destruirConfig(t_config_fs* config){
	free(config->punto_montaje);
	free(config->fs_puerto);
	free(config);
}

