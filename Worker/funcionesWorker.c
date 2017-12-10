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

       conf->Worker_Puerto = config_get_int_value(configWorker, "PUERTO_WORKER");
       conf->databinPath = strdup(config_get_string_value(config, "RUTA_DATABIN"));
       conf->Puerto_DataNode = config_get_int_value(configWorker,"PUERTO_DATANODE");

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
	free(config);
}

int conectarADataNode (){

	//Genera el socket cliente y lo conecta al kernel
	int socketCliente = crearCliente(config->Worker_Puerto,config->Puerto_DataNode);

	//Se realiza el handshake con el kernel
	t_struct_numero* es_datanode = malloc(sizeof(t_struct_numero));
	es_datanode->numero = ES_DATANODE;
	socket_enviar(socketCliente, D_STRUCT_NUMERO, es_datanode);
	free(es_datanode);

	return socketCliente;

}

void escucharConexiones(void){

	FD_ZERO(&maestro);
	FD_ZERO(&setMasters);

	socketEscucha = crearServidor(config->Worker_Puerto);

	max_fd = socketEscucha;

	FD_SET(socketEscucha, &maestro);
	fd_set read_fd;
	int iterador_sockets;
	pid_t childPID;

	while(1){
		read_fd = maestro;

		if(select(max_fd + 1, &read_fd, NULL, NULL, NULL) == -1){
			perror("Error en SELECT");
			exit(1);
		}

		if(FD_ISSET(socketEscucha, &read_fd)){ //un master quiere conectarse
			aceptarNuevaConexion(socketEscucha, &setMasters);
		}

		for(iterador_sockets = 0; iterador_sockets <= max_fd; iterador_sockets++) {

			if(FD_ISSET(iterador_sockets, &setMasters) && FD_ISSET(iterador_sockets,&read_fd)){ //una master realiza una operacion
				FD_CLR(iterador_sockets, &setMasters);
				childPID = fork();

				if(childPID == 0){
					atenderMaster(iterador_sockets);
				}

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
			log_info(logger,"No se recibio correctamente al Master");
		}
		else if(((t_struct_numero*)estructuraRecibida)->numero == ES_MASTER){
			FD_SET(socketNuevo, &maestro);
			FD_SET(socketNuevo, set);
			if(socketNuevo > max_fd) max_fd = socketNuevo;
				printf("Se acaba de conectar un proceso Master en el socket %d\n", socketNuevo);
				log_info(logger,"Se acaba de conectar un proceso Master en el socket %d", socketNuevo);
		}
	}
}

void atenderMaster(int socketMaster){

	t_tipoEstructura estructura;
	void* job;
	t_struct_numero * response = malloc(sizeof(t_struct_numero));
	int res_ok;

	if (socket_recibir(socketMaster, &estructura, &job)){

		if(estructura==D_STRUCT_JOBT){

			log_info(logger, "Se recibio un job de transformación del master %d", socketMaster);

			res_ok = doJob_transformacion(job);

		}else if(estructura==D_STRUCT_JOBR){
			log_info(logger, "Se recibio un job de reducción del master %d", socketMaster);

			res_ok = doJob_reduccion(job);

		}

		if(res_ok == 1){
			log_info(logger, "El job pedido por el master %d, fue realizado exitosamente", socketMaster);

			response->numero = JOB_OK;
			socket_enviar(socketMaster, D_STRUCT_NUMERO, response);
			_exit(1);

		}else {

		log_info(logger, "El job pedido por el master %d no se realizó", socketMaster);
		_exit(1);
		}
	}

	log_info(logger, "Error al recibir job del master %d", socketMaster);

	response->numero=ERR_RCV;

	socket_enviar(socketMaster, D_STRUCT_NUMERO, response);

	free(response);

	_exit(1);


}

int doJob_transformacion(t_struct_jobT* job){

	char pathScriptT[200];
	char pathBloqueT[200];

	pathScriptT = createScript(job->scriptTransformacion);
	pathBloqueT = createBloque(job->bloque);

	char command[500];
	int resultado;
	sprintf(command, "cat %s | %s > %s", job->pathOrigen, pathScriptT, job->pathTemporal);

	resultado = system(command);

	if(resultado == 1){
		return 1;
	}else return 0;



}

int doJob_reduccion(t_struct_jobR* job){

	char command[500];

		sprintf(command, "cat %s | %s >> %s", job->pathTemp, job->scriptReduccion, job->pathTempFinal);

		if(system(command)){
			return 1;
		}else return 0;
}

char createScript(char script){

	char path[200];
	FILE * fd;

	sprintf(path, "Scripts/%d", numScriptT);

	fd = fopen(path, "w");

	if(pwrite(fd, script, sizeof(script), 0)<0){
					log_error(logger, "Error de escritura (%s) del archivo en el path: %s ", strerror(errno), path);
	}

	return path;
}

char createBloque(int bloque){
	char path[200];
	char * bloqueData;
		FILE * fd;

		sprintf(path, "Scripts/%d", numScriptT);

		fd = fopen(path, "w");

		bloqueData = obtenerBloque(bloque);

		if(pwrite(fd, bloqueData, sizeof(bloqueData), 0)<0){
						log_error(logger, "Error de escritura (%s) del archivo en el path: %s ", strerror(errno), path);
		}

		return path;

}
void initializeFolders(){

	mkdir("Scripts", S_IRWXU | S_IRWXG | S_IRWXO);
	mkdir("BloquesTemp", S_IRWXU | S_IRWXG | S_IRWXO);
	mkdir("Reducidos", S_IRWXU | S_IRWXG | S_IRWXO);
	mkdir("Transformados", S_IRWXU | S_IRWXG | S_IRWXO);
	mkdir("Final", S_IRWXU | S_IRWXG | S_IRWXO);
}

char obtenerBloque(int bloque){

		char data[DATANODE_BLOCK_SIZE];

		char databin;

	    if(bloque < 0){
	        return 0;
	    }

	    int fd = open(config->databinPath, O_RDONLY);

	    if(fd == -1){
	        log_error(logger, "No se encontró el data.bin en la ruta %s\n", config->databinPath);
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

