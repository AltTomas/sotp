#include "funcionesDataNode.h"
#include <estructuras.h>

void crearConfig(){

	if(verificarExistenciaDeArchivo(configuracionNodo)){
		config = levantarConfiguracionNodo(configuracionNodo);
		log_info(logger,"Configuracion levantada correctamente");

	}
	else if(verificarExistenciaDeArchivo(string_substring_from(configuracionNodo,3))){
		config=levantarConfiguracionNodo(string_substring_from(configuracionNodo,3));
		log_info(logger,"Configuracion levantada correctamente");

	}
	else{
		log_error(logger,"No se pudo levantar el archivo de configuracion");
		exit(EXIT_FAILURE);
	}

}

bool verificarConfig(t_config* config){
	return 	config_has_property(config,"IP_FILESYSTEM") &&
			config_has_property(config,"PUERTO_FILESYSTEM") &&
			config_has_property(config,"NOMBRE_NODO") &&
			config_has_property(config,"PUERTO_NODO") &&
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

void destruirConfig(t_config_nodo* config){
	free(config->IP_FILESYSTEM);
	free(config->NOMBRE_NODO);
	free(config->PUERTO_NODO);
	free(config->PUERTO_FILESYSTEM);
	free(config->RUTA_DATABIN);
	free(config);
}

t_config_nodo* levantarConfiguracionNodo(char* archivo_conf) {

        t_config_nodo* conf = malloc(sizeof(t_config_nodo));
        t_config* configNodo;

        log_debug(logger,"Path config: %s",archivo_conf);

        configNodo = config_create(archivo_conf);
        if(configNodo == NULL)
        	log_error(logger,"ERROR");

        if(!verificarConfig(configNodo))
        	log_error(logger,"CONFIG NO VALIDA");

        conf->IP_FILESYSTEM = malloc(strlen(config_get_string_value(configNodo, "IP_FILESYSTEM"))+1);
        strcpy(conf->IP_FILESYSTEM, config_get_string_value(configNodo, "IP_FILESYSTEM"));

        conf->PUERTO_FILESYSTEM = malloc(strlen(config_get_string_value(configNodo, "PUERTO_FILESYSTEM"))+1);
        strcpy(conf->PUERTO_FILESYSTEM, config_get_string_value(configNodo, "PUERTO_FILESYSTEM"));

        conf->NOMBRE_NODO = malloc(strlen(config_get_string_value(configNodo, "NOMBRE_NODO"))+1);
        strcpy(conf->NOMBRE_NODO, config_get_string_value(configNodo, "NOMBRE_NODO"));

        conf->PUERTO_NODO = malloc(strlen(config_get_string_value(configNodo, "PUERTO_NODO"))+1);
        strcpy(conf->PUERTO_NODO, config_get_string_value(configNodo, "PUERTO_NODO"));

        conf->RUTA_DATABIN = malloc(strlen(config_get_string_value(configNodo, "RUTA_DATABIN"))+1);
        strcpy(conf->RUTA_DATABIN, config_get_string_value(configNodo, "RUTA_DATABIN"));

        config_destroy(configNodo);
        printf("Configuracion levantada correctamente.\n");
        return conf;
}

void conectarConFS(void){

	puts("Conectandose a FS");
	log_info(logger,"Conectandose a FS");

	t_struct_numero* handshake = malloc(sizeof(t_struct_numero));

	handshake->numero = ES_DATANODE;

	t_struct_datanode * datanodeParaFS = malloc(sizeof(t_struct_datanode));

	socketConexionFS = crearCliente(config->IP_FILESYSTEM, atoi(config->PUERTO_FILESYSTEM));

	if(socketConexionFS== 1){
		log_error(logger,"No se pudo establecer la conexion con FS...");
		puts("No se pudo establecer la conexion con FS...");
		exit(1);
	}

	puts("Me conecte a FS");
	log_info(logger,"Me conecte a FS");

	socket_enviar(socketConexionFS, D_STRUCT_NUMERO, handshake);

	log_info(logger,"Handshake enviado a FS");

	t_list *listatest =list_create();
	t_struct_bloqueDN* bloque = malloc(sizeof(t_struct_bloqueDN));
	bloque->numBloque = 1;
	bloque->estado = 1;
	list_add(listatest, bloque);

	datanodeParaFS->bloquesTotales = 1;
	datanodeParaFS->nomDN = malloc(strlen(config->NOMBRE_NODO)+1);
	strcpy(datanodeParaFS->nomDN,config->NOMBRE_NODO);
	datanodeParaFS->bloqueDN = listatest;
	datanodeParaFS->bloquesLibres = 1;
	datanodeParaFS->ipDN = malloc(strlen("123")+1);
	strcpy(datanodeParaFS->ipDN,"123");
	datanodeParaFS->puertoDN = atoi(config->PUERTO_NODO);

	socket_enviar(socketConexionFS, D_STRUCT_INFO_NODO,datanodeParaFS);

	puts("Aca muere datanode porque falta comportamiento");
}

void manejarFS(){

	t_tipoEstructura tipoEstructura;
	void * structRecibido;


	while(socket_recibir(socketConexionFS,&tipoEstructura,&structRecibido) > 0){

			switch(tipoEstructura){

			case FS_DATANODE_ALMACENAR_BLOQUE: ;

			t_pedido_almacenar_bloque*  archivoEsc = malloc(sizeof(t_pedido_almacenar_bloque* ));
			t_struct_numero*  respuesta = malloc(sizeof(t_struct_numero* ));

				socket_recibir(socketConexionFS, FS_DATANODE_ALMACENAR_BLOQUE , archivoEsc);

				if(setBloque(archivoEsc->bloqueArchivo, archivoEsc->contenidoBloque) >0){

					respuesta->numero = archivoEsc->bloqueArchivo;

					socket_enviar(socketConexionFS, D_STRUCT_NUMERO, respuesta);
				}

				free(archivoEsc);

			break;

			case FS_DATANODE_LEER_BLOQUE: ;

			t_pedido_almacenar_bloque*   archivoLec = malloc(sizeof(t_pedido_almacenar_bloque* ));

					socket_recibir(socketConexionFS, FS_DATANODE_ALMACENAR_BLOQUE , archivoLec);

					archivoLec->contenidoBloque = getBloque(archivoEsc->bloqueArchivo);

					socket_enviar(socketConexionFS, FS_DATANODE_ALMACENAR_BLOQUE , archivoLec);

					free(archivoLec);

			break;
			}


	}


	if (socket_recibir(socketConexionFS,&tipoEstructura,&structRecibido) == -1) {
		log_info(logger,"El FS %d cerró la conexión.",socketConexionFS);
	}
}



char getBloque(int bloque){

	char data[DATANODE_BLOCK_SIZE];

    if(bloque < 0){
        return 0;
    }

    if(bloque>cantidadBloques){
        	log_error(logger, "No existe el bloque pedido");
        	return -2;
        }

    int fd = open(config->RUTA_DATABIN, O_RDONLY);

    if(fd == -1){
        log_error(logger, "No se encontró el data.bin en la ruta %s\n", config->RUTA_DATABIN);
        return -1;
    }

    databin = mmap(NULL, DATANODE_BLOCK_SIZE,
                        PROT_READ, MAP_PRIVATE, fd, DATANODE_BLOCK_SIZE * bloque);

    if(databin == MAP_FAILED){
        log_error(logger, "No se pudo mapear el data.bin en la ruta\n");
        return -1;
    }

    memcpy(data, databin, DATANODE_BLOCK_SIZE);

    munmap(databin, DATANODE_BLOCK_SIZE);
    close(fd);

    return data;
}

int setBloque(int bloque, char data[DATANODE_BLOCK_SIZE]){
    int fd = open(config->RUTA_DATABIN, O_RDWR);

    if(fd == -1){
        log_error(logger, "No se encontró el data.bin en la ruta %s\n", config->RUTA_DATABIN);
        return -1;
    }

    if(bloque>cantidadBloques){
    	log_error(logger, "No hay más espacio para escribir el bloque pedido");
    	return -2;
    }

    databin = mmap(NULL, DATANODE_BLOCK_SIZE,
                        PROT_READ | PROT_WRITE, MAP_SHARED, fd, DATANODE_BLOCK_SIZE * bloque);

    if(databin == MAP_FAILED){
        log_error(logger, "No se pudo mapear el data.bin en la ruta\n");
        return -1;
    }


    memcpy(databin, data, DATANODE_BLOCK_SIZE);

    if(msync(databin, DATANODE_BLOCK_SIZE, MS_SYNC) == -1){
        return -1;
    }

    munmap(databin, DATANODE_BLOCK_SIZE);
    close(fd);

    return 0;
}

void calcularCantidadDeBloques(int fd){

	struct stat stats;
	fstat(fd, &stats);
	cantidadBloques = stats.st_size / DATANODE_BLOCK_SIZE;

	log_info(logger, "La cantidad de bloques es: %d", cantidadBloques);
}
