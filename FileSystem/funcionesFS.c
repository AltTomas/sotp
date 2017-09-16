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


////////////////////Conexiones/////////////////////////////////////////////////////////////////////////////////////////////////////////


void escucharConexiones(void){

	FD_ZERO(&datanode);
	FD_ZERO(&setDataNodes);

	socketEscuchaDataNodes = crearServidor(atoi(config->fs_puerto));

	max_fd = socketEscuchaDataNodes;

	FD_SET(socketEscuchaDataNodes, &datanode);
	fd_set read_fd;
	int iterador_sockets, resultadoHilo;

	while(1){
		read_fd = datanode;

		if(select(max_fd + 1, &read_fd, NULL, NULL, NULL) == -1){
			perror("Error en SELECT");
			exit(1);
		}

		if(FD_ISSET(socketEscuchaDataNodes, &read_fd)){
			aceptarNuevaConexion(socketEscuchaDataNodes, &setDataNodes);
		}

		for(iterador_sockets = 0; iterador_sockets <= max_fd; iterador_sockets++) {

			if(FD_ISSET(iterador_sockets, &setDataNodes) && FD_ISSET(iterador_sockets,&read_fd)){
				FD_CLR(iterador_sockets, &setDataNodes);
				pthread_t hilo;
				resultadoHilo = pthread_create(hilo, NULL, trabajarSolicitudDataNode, iterador_sockets);
				if(resultadoHilo) exit(1);
			}
		}
	}
}

void aceptarNuevaConexion(int socketEscucha, fd_set* set){

	int socketNuevo = aceptarCliente(socketEscucha);

	if(socketNuevo == -1) {
			perror("Error al aceptar conexion entrante");
	}
	else{
		void* estructuraRecibida;
		t_tipoEstructura tipoEstructura;

		int recepcion = socket_recibir(socketNuevo, &tipoEstructura,&estructuraRecibida);

		if(recepcion == -1 || tipoEstructura != D_STRUCT_NUMERO){
			log_info(logger,"No se recibio correctamente a que atendio FS");
		}
		else if(((t_struct_numero*)estructuraRecibida)->numero == ES_DATANODE){
			FD_SET(socketNuevo, &datanode);
			FD_SET(socketNuevo, set);
			if(socketNuevo > max_fd) max_fd = socketNuevo;
				printf("Se acaba de conectar un proceso  en el socket %d\n", socketNuevo);
				log_info(logger,"Se acaba de conectar un proceso  en el socket %d", socketNuevo);
		}
	}
}

void trabajarSolicitudDataNode(int socketDataNode){

	void* estructuraRecibida;
	t_tipoEstructura tipoEstructura;

	int recepcion = socket_recibir(socketDataNode, &tipoEstructura,&estructuraRecibida);

	if(recepcion == -1){
		printf("Se desconecto el Nodo en el socket %d\n", socketDataNode);
		log_info(logger,"Se desconecto el Nodo en el socket %d", socketDataNode);
		close(socketDataNode);
		FD_CLR(socketDataNode, &datanode);
		FD_CLR(socketDataNode, &setDataNodes);
	}
	else if(tipoEstructura != D_STRUCT_JOB){
		puts("Error en la serializacion");
		log_info(logger,"Error en la serializacion");
	}
	else{
		printf("Llego solicitud de tarea del Nodo en el socket %d\n", socketDataNode);
		log_info(logger,"Llego solicitud de tarea del Nodo en el socket %d", socketDataNode);

		printf("Script Transformacion: %s\n",((t_struct_job*)estructuraRecibida)->scriptTransformacion);
		log_info(logger,"Script Transformacion: %s",((t_struct_job*)estructuraRecibida)->scriptTransformacion);
		printf("Script Reduccion: %s\n",((t_struct_job*)estructuraRecibida)->scriptReduccion);
		log_info(logger,"Script Reduccion: %s",((t_struct_job*)estructuraRecibida)->scriptReduccion);
		printf("Archivo Objetivo: %s\n",((t_struct_job*)estructuraRecibida)->archivoObjetivo);
		log_info(logger,"Archivo Objetivo: %s",((t_struct_job*)estructuraRecibida)->archivoObjetivo);
		printf("Archivo Resultado: %s\n",((t_struct_job*)estructuraRecibida)->archivoResultado);
		log_info(logger,"Archivo Resultado: %s",((t_struct_job*)estructuraRecibida)->archivoResultado);
		FD_SET(socketDataNode,&setDataNodes);
	}
}
