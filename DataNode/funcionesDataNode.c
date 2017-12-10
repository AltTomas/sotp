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

	printf("IP FS: %s\n", config->IP_FILESYSTEM);
	printf("Nombre Nodo: %s\n", config->NOMBRE_NODO);
	printf("Puerto FS: %s\n", config->PUERTO_FILESYSTEM);
	printf("Puerto Nodo: %s\n", config->PUERTO_NODO);
	printf("Ruta DataBin: %s\n", config->RUTA_DATABIN);

	t_struct_numero* handshake = malloc(sizeof(t_struct_numero));

	handshake->numero = ES_DATANODE;

	t_struct_datanode * datanodeParaFS = malloc(sizeof(t_struct_datanode));

	socketConexionFS = crearCliente(config->IP_FILESYSTEM, atoi(config->PUERTO_FILESYSTEM));

	if(socketConexionFS== 1){
		log_error(logger,"No se pudo establecer la conexion con FS...");
		puts("No se pudo establecer la conexion con FS...");
		exit(1);
	}

	socket_enviar(socketConexionFS, D_STRUCT_NUMERO, handshake);

	puts("Me conecte a FS");
	log_info(logger,"Me conecte a FS");

	t_list *listatest =list_create();
	list_add(listatest, "hola");

	datanodeParaFS->bloquesTotales = 1;
	datanodeParaFS->nomDN = malloc(strlen(config->NOMBRE_NODO)+1);
	strcpy(datanodeParaFS->nomDN,config->NOMBRE_NODO);
	datanodeParaFS->bloqueDN = listatest;
	datanodeParaFS->bloquesLibres = 1;
	datanodeParaFS->ipDN = "123";
	datanodeParaFS->puertoDN = atoi(config->PUERTO_NODO);

	socket_enviar(socketConexionFS, D_STRUCT_INFO_NODO,datanodeParaFS);
	//socket_enviar(socketConexionFS, D_STRUCT_NUMERO,enviado);

	log_info(logger,"Handshake enviado a YAMA");

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
