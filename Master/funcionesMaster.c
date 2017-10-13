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

void ejecutarJob(char** argumentos){

	t_argumentos* argumentosMaster = malloc(sizeof(t_argumentos));

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

	if(recepcion == -1 || tipoEstructura != D_STRUCT_NUMERO){ //YAMA_MASTER_DATA_NODO
				log_info(logger,"No se recibio correctamente la informacion de los nodos");
	}
	// Manejamos el tipo de estructura y estructura recibida
	// Por cada worker al que tengamos que conectarnos
	pthread_t hiloWorker;
	pthread_create(&hiloWorker, NULL, (void*)ejecucionJob, NULL);
}


void ejecucionJob (){
	//Aca nos conectamos al worker y manejamos todos los aspectos de la comunicacion
}

