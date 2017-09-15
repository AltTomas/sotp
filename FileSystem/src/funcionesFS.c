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

		if(linea){
			add_history(linea);
		} else if(!linea){
			linea = readline("Ingrese un comando: ");
		}

		switch(commandParser(linea)){

		default:
			puts("El comando que ingreso no es valido.");
			break;
		case 1:
			puts("formatear FS");
			break;
		case 2:
			puts("Ingresar path del archivo.");
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
		case 13:
			puts("format - Formatear el Filesystem.");
			puts("rm [path_archivo] - Eliminar un Archivo.");
			puts("rm -d [path_directorio] - Eliminar un directorio.");
			puts("rm -b [path_archivo] [nro_bloque] [nro_copia] - Eliminar un bloque");
			puts("rename [path_original] [nombre_final] - Renombra un Archivo o Directorio");
			puts("mv [path_original] [path_final] - Mueve un Archivo o Directorio");
			puts("cat [path_archivo] - Muestra el contenido del archivo como texto plano.");
			puts("mkdir [path_dir] - Crea un directorio. Si el directorio ya existe, el comando deberá informarlo.");
			puts("cpfrom [path_archivo_origen] [directorio_yamafs] - Copiar un archivo local al yamafs");
			puts("cpto [path_archivo_yamafs] [directorio_filesystem] - Copiar un de yamafs al fs local");
			puts("cpblock [path_archivo] [nro_bloque] [id_nodo] - Crea una copia de un bloque de un archivo en el nodo dado.");
			puts("md5 [path_archivo_yamafs] - Solicitar el MD5 de un archivo en yamafs");
			puts("ls [path_directorio] - Lista los archivos de un directorio");
			puts("info [path_archivo] - Muestra toda la información del archivo, incluyendo tamaño, bloques, ubicación de los bloques, etc.");
			break;

		case 14:
			puts("ingrese el path del directorio");
			break;
		case 15:
			puts("ingrese path del archivo, nro bloque y nro copia");
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
	} else if(strcmp(linea, "HELP") == 0){
		return 13;
	} else if(strcmp(linea, "RM -D") == 0){
		return 14;
	} else if(strcmp(linea, "RM -B") == 0){
		return 15;
	} else{
		return 17;
	}

}
