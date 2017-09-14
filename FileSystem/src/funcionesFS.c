#include "funcionesFS.h"


void crearConfig(){

	if(verificarExistenciaDeArchivo(configuracionFS)){
		config=levantarConfiguracionFS(configuracionFS);
		log_info(logger,"Configuracion levantada correctamente");

	}
	else if(verificarExistenciaDeArchivo(string_substring_from(configuracionFS,3))){
		config=levantarConfiguracionFS(string_substring_from(configuracionFS,3));
		log_info(logger,"Configuracion levantada correctamente");

	}
	else{
		log_error(logger,"No se pudo levantar el archivo de configuracion");
		exit(EXIT_FAILURE);
	}

}

t_config_fs* levantarConfiguracionFS(char* archivo_conf) {

        t_config_fs* conf = malloc(sizeof(t_config_fs));
        t_config* configFs;

        log_debug(logger,"Path config: %s",archivo_conf);

        configFs = config_create(archivo_conf);
        if(configFs == NULL)
        	log_error(logger,"ERROR");

        if(!verificarConfig(configFs))
        	log_error(logger,"CONFIG NO VALIDA");

        conf->punto_montaje = malloc(PUNTO_MONTAJE);
        strcpy(conf->punto_montaje, config_get_string_value(configFs, "PUNTO_MONTAJE"));

        conf->fs_puerto = malloc(MAX_LEN_PUERTO);
        strcpy(conf->fs_puerto, config_get_string_value(configFs, "FS_PUERTO"));

        config_destroy(configFs);
        printf("Configuracion levantada correctamente.\n");
        return conf;
}

bool verificarConfig(t_config* config){
	return config_has_property(config,"PUNTO_MONTAJE") &&
			config_has_property(config,"FS_PUERTO");
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

void commandHandler(){

	char * linea;

	puts("Bienvenido a YAMAFS. Si no conoce los comandos, ingrese HELP para más información.");

	while(1){
		linea = readline("Ingrese un comando: ");
		string_to_upper(linea);

		if(linea){
			add_history(linea);
		} else if(!linea){
			linea = readline("Ingrese un comando: ");
		} else if(strcmp(linea, "HELP") == 0){
			puts("format - Formatear el Filesystem");
			puts("rm [path_archivo] - rm -d [path_directorio] -rm [path_archivo] ");
		}

		switch(commandParser(linea)){
		case 1:
			puts("formatear FS");
			break;
		case 2:
			puts("Ingresar path del archivo, -d path del directorio o -b path archivo, nro bloque y nro copia");
			break;
		case 3:
			puts("Ingresar path original y nombre final.");
			break;
		case 4:
			puts("Ingresar path original y path final");
			break;
		case 5:
			puts("Ingrese path el archivo");
			break;
		case 6:
			puts("Ingre el path del directorio a crear");
			break;
		case 7:
			puts("Ingrese el path del archivo y el directorio donde se encuenta YAMAFS");
			break;
		case 8:
			puts("Ingrese el path del archivo en el YAMAFS y el directorio del FS final");
			break;
		case 9:
			puts("Ingrese el path del archivo, nro de bloque e ID del nodo");
			break;
		case 10:
			puts("Ingrese el path del archivo en el YAMAFS");
			break;
		case 11:
			puts("Ingrese el path de un directorio");
			break;
		case 12:
			puts("Ingrese el path de un archivo");
			break;
		default:
			puts("El comando que ingreso no es valido.");
			break;
		}
		free(linea);

	}
}


int commandParser(char* linea){

	string_to_upper(linea);

	if(strcmp(linea, "FORMAT") == 0){
		return 1;
	} else if (strcmp(linea, "RM") == 0){
		return 2;
	} else if(strcmp(linea, "RENAME") == 0){
		return 3;
	} else if (strcmp(linea, "MV") == 0){
		return 4;
	} else if (strcmp(linea, "CAT") == 0){
		return 5;
	} else if(strcmp(linea, "MKDIR") == 0){
		return 6;
	} else if (strcmp(linea, "CPFROM") == 0){
		return 7;
	} else if (strcmp(linea, "CPTO") == 0){
		return 8;
	} else if(strcmp(linea, "CPBLOCK") == 0){
		return 9;
	} else if (strcmp(linea, "MD5") == 0){
		return 10;
	} else if (strcmp(linea, "LS") == 0){
		return 11;
	} else if(strcmp(linea, "INFO") == 0){
		return 12;
	}
	else{
		return 15;
	}

}
