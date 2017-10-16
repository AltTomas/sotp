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

	if(socketConexionYAMA == -1){
		log_error(logger,"No se pudo establecer la conexion con YAMA, cerrando Master...");
		exit(1);
	}

	puts("Me conecte a YAMA");
	log_info(logger,"Me conecte a YAMA");

	t_struct_numero* enviado = malloc(sizeof(t_struct_numero));
	enviado->numero = ES_MASTER;

	socket_enviar(socketConexionYAMA, D_STRUCT_NUMERO,enviado);

	log_info(logger,"Handshake enviado a YAMA");

}

void testConexion(){
	int socketCliente = crearCliente("127.0.0.1",7710);
	t_struct_numero* es_master = malloc(sizeof(t_struct_numero));
		es_master->numero = ES_MASTER;
		socket_enviar(socketCliente, D_STRUCT_NUMERO, es_master);

		t_struct_jobT* job = malloc(sizeof(t_struct_jobT));
		job->scriptTransformacion = "/home/utnso/yama-test1/transformador.sh";
		job->pathTemporal = "/home/utnso/temporal/temp";
		job->pathOrigen = "/home/utnso/yama-test1/WBAN.csv";

		socket_enviar(socketCliente, D_STRUCT_JOBT, job);
		free(es_master);


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
