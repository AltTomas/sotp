#include "funcionesMaster.h"

void crearConfig(){

	if(verificarExistenciaDeArchivo(configuracionMaster)){
		config=levantarConfiguracionMaster(configuracionMaster);
		log_info(logger,"Configuracion levantada correctamente");

	}
	else if(verificarExistenciaDeArchivo(string_substring_from(configuracionMaster,3))){
		config=levantarConfiguracionMaster(string_substring_from(configuracionMaster,3));
		log_info(logger,"Configuracion levantada correctamente");

	}
	else{
		log_error(logger,"No se pudo levantar el archivo de configuracion");
		exit(EXIT_FAILURE);
	}

}

t_config_master* levantarConfiguracionMaster(char* archivo_conf) {

        t_config_master* conf = malloc(sizeof(t_config_master));
        t_config* configMaster;

        log_debug(logger,"Path config: %s",archivo_conf);

        configMaster = config_create(archivo_conf);
        if(configMaster == NULL)
        	log_error(logger,"ERROR");

        if(!verificarConfig(configMaster))
        	log_error(logger,"CONFIG NO VALIDA");

        conf->YAMA_ip = malloc(MAX_LEN_IP);
        strcpy(conf->YAMA_ip, config_get_string_value(configMaster, "YAMA_IP"));

        conf->YAMA_puerto = malloc(MAX_LEN_PUERTO);
        strcpy(conf->YAMA_puerto, config_get_string_value(configMaster, "YAMA_PUERTO"));

        config_destroy(configMaster);
        printf("Configuracion levantada correctamente.\n");
        return conf;
}

bool verificarConfig(t_config* config){
	return config_has_property(config,"YAMA_IP") &&
			config_has_property(config,"YAMA_PUERTO");
}

bool verificarExistenciaDeArchivo(char* path) {
	FILE * archivoConfig = fopen(path, "r");
	if (archivoConfig!=NULL){
		 fclose(archivoConfig);
		 return true;
	}
	return false;
}

void destruirConfig(t_config_master* config){
	free(config->YAMA_ip);
	free(config->YAMA_puerto);
	free(config);
}

///////////////////////////////////////////////////// CONEXIONES //////////////////////////////////////////////////////////////

void conectarConYAMA(void){

	log_info(logger,"Conectandose a YAMA");

	socketConexionYAMA = crearCliente(config->YAMA_ip, atoi(config->YAMA_puerto));

	if(socketConexionYAMA == 1){
		puts("No se pudo establecer la conexion con YAMA, cerrando Master...");
		log_error(logger,"No se pudo establecer la conexion con YAMA, cerrando Master...");
		exit(1);
	}

	puts("Me conecte a YAMA");
	log_info(logger,"Me conecte a YAMA");

	t_struct_numero* enviado = malloc(sizeof(t_struct_numero));
	enviado->numero = ES_MASTER;

	socket_enviar(socketConexionYAMA, D_STRUCT_NUMERO,enviado);
	free(enviado);
	log_info(logger,"Handshake argumentosMaster a YAMA");

}

void testConexion(){
	int socketCliente = crearCliente("127.0.0.1",7710);
	t_struct_numero* es_master = malloc(sizeof(t_struct_numero));
		es_master->numero = ES_MASTER;
		socket_enviar(socketCliente, D_STRUCT_NUMERO, es_master);

<<<<<<< HEAD
		t_struct_jobT* job = malloc(sizeof(t_struct_jobT));
		job->scriptTransformacion = "/home/utnso/yama-test1/transformador.sh";
		job->pathTemporal = "/home/utnso/temporal/temp";
		job->pathOrigen = "/home/utnso/yama-test1/WBAN.csv";

		socket_enviar(socketCliente, D_STRUCT_JOBT, job);
		free(es_master);


=======
	argumentosMaster = malloc(sizeof(t_argumentos));

	argumentosMaster->script_transformacion = malloc(sizeof(MAX_LEN_RUTA));
	strcpy(argumentosMaster->script_transformacion, argumentos[1]);

	argumentosMaster->script_reduccion = malloc(sizeof(MAX_LEN_RUTA));
	strcpy(argumentosMaster->script_reduccion, argumentos[2]);

	argumentosMaster->archivo = malloc(sizeof(MAX_LEN_RUTA));
	strcpy(argumentosMaster->archivo, argumentos[3]);

	argumentosMaster->archivo_resultado = malloc(sizeof(MAX_LEN_RUTA));
	strcpy(argumentosMaster->archivo_resultado, argumentos[4]);

	t_struct_string* enviado = malloc(sizeof(t_struct_string));
	enviado->string = malloc(sizeof(MAX_LEN_RUTA));
	strcpy(enviado->string,argumentosMaster->archivo);

	socket_enviar(socketConexionYAMA, D_STRUCT_STRING,enviado);
	free(enviado);

	puts("Solicitando ejecucion de tarea");
	log_info(logger,"Solicitando ejecucion de tarea");

	void* estructuraRecibida;
	t_tipoEstructura tipoEstructura;

	// Aca deberiamos recibir los nodos que nos envia YAMA para despues conectarnos
	int recepcion = socket_recibir(socketConexionYAMA, &tipoEstructura, &estructuraRecibida);

	if(recepcion == -1 || tipoEstructura != D_STRUCT_NODOS){ //YAMA_MASTER_DATA_NODO
				log_info(logger,"No se recibio correctamente la informacion de los nodos");
	}

	// Seria una lista de estructuras que vendrian a ser del nodo: IP, puerto, bloque, bytes ocupados y el nombre del temporal
	int i;
	for(i = 0; i<((t_struct_nodos_transformacion*)estructuraRecibida)->lista_nodos->elements_count; i++){ // Por cada worker al que tengamos que conectarnos creamos un hilo que maneje la conexion
		t_struct_transformacion* info_nodo = list_get(((t_struct_nodos_transformacion*)estructuraRecibida)->lista_nodos,i);
		pthread_t hiloWorker;
		pthread_create(&hiloWorker, NULL, (void*)ejecutarTransformacion, info_nodo);
	}

}


void ejecutarTransformacion (t_infoNodo worker){
	// Se conecta al worker
	//int crearCliente(char* ipServidor,int puertoServidor);
	int socketConexionWorker = crearCliente(worker.ip,worker.puerto);
	if(socketConexionWorker == 1){
		puts("No se pudo establecer la conexion con Worker, cerrando Master...");
		log_error(logger,"No se pudo establecer la conexion con Worker, cerrando Master...");
		exit(1);
	}

	t_struct_transformacion* enviado = malloc(sizeof(t_struct_transformacion));
	// Enviado vendria a ser una estructura igual a cada estructura contenida en la lista + el script
	enviado->script_transformacion= malloc(sizeof(MAX_LEN_RUTA));
	enviado->nombre_archivo_temporal = malloc(sizeof(MAX_LEN_RUTA));
	enviado->ip = malloc(sizeof(MAX_LEN_IP));

	enviado->puerto = worker.puerto;
	enviado->bloque = worker.numBloque;
	enviado->bytes_ocupados_bloque = worker.bytesOcupados;
	strcpy(enviado->ip, worker.ip);
	strcpy(enviado->script_transformacion, argumentosMaster->script_transformacion);
	strcpy(enviado->nombre_archivo_temporal, worker.nombreTemporal);

	socket_enviar(socketConexionWorker, D_STRUCT_TRANSFORMACION, enviado);
	free(enviado);

	void* estructuraRecibida;
	t_tipoEstructura tipoEstructura;

	int recepcion = socket_recibir(socketConexionWorker, &tipoEstructura, &estructuraRecibida);
	if(recepcion == -1 || tipoEstructura != D_STRUCT_NUMERO){
				log_info(logger,"No se recibio correctamente la confirmacion del Worker");
	}
	else{
		t_struct_confirmacion_transformacion* confirmacion = malloc(sizeof(t_struct_confirmacion_transformacion));
		switch(((t_struct_confirmacion_transformacion*)estructuraRecibida)->confirmacion){
				case WORKER_MASTER_TRANSFORMACION_OK:
				// Lo agregamos a una lista de sockets listos para la reduccion local?
				socket_enviar(socketConexionYAMA, D_STRUCT_CONFIRMACION_TRANSFORMACION,confirmacion);
				close(socketConexionWorker); // Lo tengo que cerrar o lo puedo dejar abierto para la reduccion local?
				// Aca iria un recibe de YAMA para empezar la reduccion local?
				break;

				case WORKER_MASTER_TRANSFORMACION_FALLO:
				socket_enviar(socketConexionYAMA, D_STRUCT_CONFIRMACION_TRANSFORMACION,confirmacion);
				close(socketConexionWorker);
				// Que YAMA replanifique
				// Hacer recibe de lo que haya replanificado YAMA aca o hacer otro hilo?
				break;
	}
}
>>>>>>> bac1c3cf5de083d5e114fc76b1064775a1a3dbb6
}
//void ejecutarJob(char** argumentos){
//
//	t_struct_job* enviado = malloc(sizeof(t_struct_job));
//
//	enviado->scriptTransformacion = malloc(sizeof(MAX_LEN_RUTA));
//	strcpy(enviado->scriptTransformacion, argumentos[1]);
//
//	printf("script transformacion: %s\n", enviado->scriptTransformacion);
//
//	enviado->scriptReduccion = malloc(sizeof(MAX_LEN_RUTA));
//	strcpy(enviado->scriptReduccion, argumentos[2]);
//
//	printf("script reduccion: %s\n", enviado->scriptReduccion);
//
//	enviado->archivoObjetivo = malloc(sizeof(MAX_LEN_RUTA));
//	strcpy(enviado->archivoObjetivo, argumentos[3]);
//
//	printf("archivo objetivo: %s\n", enviado->archivoObjetivo);
//
//	enviado->archivoResultado = malloc(sizeof(MAX_LEN_RUTA));
//	strcpy(enviado->archivoResultado, argumentos[4]);
//
//	printf("archivo resultado: %s\n", enviado->archivoResultado);
//
//	socket_enviar(socketConexionYAMA, D_STRUCT_JOB,enviado);
//
//	puts("Solicitando ejecucion de tarea");
//	log_info(logger,"Solicitando ejecucion de tarea");
//}
