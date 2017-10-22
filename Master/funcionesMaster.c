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

///////////////////////////////////////////////////// EJECUTAR UN JOB ////////////////////////////////////////////////////////

void ejecutarJob(char** argumentos){

	// Obtenemos el codigo de los scripts
	char* scriptTransformacion = obtenerContenido(argumentos[1]);
	char* scriptReduccion = obtenerContenido(argumentos[2]);

	argumentosMaster = malloc(sizeof(t_argumentos));

	argumentosMaster->script_transformacion = malloc(sizeof(MAX_LEN_RUTA));
	strcpy(argumentosMaster->script_transformacion, scriptTransformacion);

	argumentosMaster->script_reduccion = malloc(sizeof(MAX_LEN_RUTA));
	strcpy(argumentosMaster->script_reduccion, scriptReduccion);

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

	///					TRANSFORMACION					///

	// Aca deberiamos recibir los nodos que nos envia YAMA para despues conectarnos
	int recepcion = socket_recibir(socketConexionYAMA, &tipoEstructura, &estructuraRecibida); // Recibimos nodos para transformacion

	if(recepcion == -1 || tipoEstructura != D_STRUCT_NODOS){ //YAMA_MASTER_DATA_NODO
				log_info(logger,"No se recibio correctamente la informacion de los nodos");
	}

	// Seria una lista de estructuras que vendrian a ser del nodo: IP, puerto, bloque, bytes ocupados y el nombre del temporal
	int i;
	for(i = 0; i<((t_struct_nodos*)estructuraRecibida)->lista_nodos->elements_count; i++){ // Por cada worker al que tengamos que conectarnos creamos un hilo que maneje la conexion
		t_infoNodo_transformacion* info_nodo = list_get(((t_struct_nodos*)estructuraRecibida)->lista_nodos,i);
		pthread_t hiloWorker;
		pthread_create(&hiloWorker, NULL, (void*)ejecutarTransformacion, info_nodo);
	}



	///					REDUCCION LOCAL					///
	recepcion = socket_recibir(socketConexionYAMA, &tipoEstructura, &estructuraRecibida); // Recibimos nodos para reduccion local

	if(recepcion == -1 || tipoEstructura != D_STRUCT_NODOS){ //YAMA_MASTER_DATA_NODO
				log_info(logger,"No se recibio correctamente la informacion de los nodos");
	}

	// Seria una lista de estructuras que vendrian a ser del nodo: IP, puerto, bloque, bytes ocupados y el nombre del temporal

	t_list* listaNodosParaReduccionL = ((t_struct_nodos*)estructuraRecibida)->lista_nodos;
	cantidadNodosReduccion = listaNodosParaReduccionL->elements_count;
	contadorNodosReduccion = 0;
	sem_init(&bin_reduccion,0,0);
	sem_init(&mutex_contadorReduccion,0,1);

	for(i = 0; i<cantidadNodosReduccion; i++){ // Por cada worker al que tengamos que conectarnos creamos un hilo que maneje la conexion
		t_infoNodo_reduccionLocal* info_nodo = list_get(((t_struct_nodos*)estructuraRecibida)->lista_nodos,i);
		pthread_t hiloWorker;
		pthread_create(&hiloWorker, NULL, (void*)ejecutarReduccionLocal, info_nodo);
	}

	// Esperamos a que se realicen todas las reducciones
	sem_wait(&bin_reduccion);

	///					REDUCCION GLOBAL					///
	recepcion = socket_recibir(socketConexionYAMA, &tipoEstructura, &estructuraRecibida); // Recibimos nodos para reduccion global

	if(recepcion == -1 || tipoEstructura != D_STRUCT_NODOS){ //YAMA_MASTER_DATA_NODO
		log_error(logger,"No se recibio correctamente la informacion de los nodos");
	}
	t_infoNodo_reduccionGlobal* info_nodo_encargado = buscarEncargado(((t_struct_nodos*)estructuraRecibida)->lista_nodos);
	list_remove_by_condition(((t_struct_nodos*)estructuraRecibida)->lista_nodos, (void*)esEncargado); // Sacamos al nodo encargado de la lista

	t_list* nodosEsclavos = list_create();
	for(i = 0; i<((t_struct_nodos*)estructuraRecibida)->lista_nodos->elements_count; i++){
			t_infoNodo_reduccionGlobal* info_nodo = list_get(((t_struct_nodos*)estructuraRecibida)->lista_nodos,i);

			t_struct_nodoEsclavo* nodo = malloc(sizeof(t_struct_nodoEsclavo));
			nodo->ip = malloc(sizeof(MAX_LEN_IP));
			nodo->nombreTemporal = malloc(sizeof(MAX_LEN_RUTA));

			strcpy(nodo->ip,info_nodo->ip);
			strcpy(nodo->nombreTemporal, info_nodo->pathTemporal);
			nodo->puerto = info_nodo->puerto;

			list_add(nodosEsclavos,nodo);
			}
	// El nodo encargado deberia conocer su temporal
	// Crear una nueva lista solo con la info necesaria de los nodos: IP,Puerto,RutaTemporal (t_info_NodoEsclavo)
	ejecutarReduccionGlobal(info_nodo_encargado,nodosEsclavos, argumentosMaster->script_reduccion);

	///					ALMACENADO FINAL				///

	recepcion = socket_recibir(socketConexionYAMA, &tipoEstructura, &estructuraRecibida); // Recibimos el nodo para realizar el almacenamiento final

	if(recepcion == -1 || tipoEstructura != D_STRUCT_NODOS){
		log_error(logger,"No se recibio correctamente la informacion del nodo");
	}

	t_infoNodo_Final* nodo = (t_infoNodo_Final*)estructuraRecibida;

	ejecutarAlmacenamientoFinal(nodo);
}


////////////////////////////////// ETAPAS ///////////////////////////////////////////////////////////////////

void ejecutarTransformacion (t_infoNodo_transformacion worker){
	// Se conecta al worker
	int socketConexionWorker = crearCliente(worker.ip,worker.puerto);
	if(socketConexionWorker == 1){
		puts("TRANSFORMACION - No se pudo establecer la conexion con Worker, cerrando Master...");
		log_error(logger,"TRANSFORMACION - No se pudo establecer la conexion con Worker, cerrando Master...");
		exit(1);
	}

	t_struct_jobT* enviado = malloc(sizeof(t_struct_jobT));
	// Enviado vendria a ser una estructura igual a cada estructura contenida en la lista + el script
	enviado->scriptTransformacion= malloc(sizeof(MAX_LEN_RUTA));
	enviado->pathTemporal = malloc(sizeof(MAX_LEN_RUTA));
	//enviado->ip = malloc(sizeof(MAX_LEN_IP));

	//enviado->puerto = worker.puerto;
	enviado->bloque = worker.numBloque;
	enviado->bytesOcupadosBloque = worker.bytesOcupados;
	//strcpy(enviado->ip, worker.ip);
	strcpy(enviado->scriptTransformacion, argumentosMaster->script_transformacion);
	strcpy(enviado->pathTemporal, worker.nombreTemporal);

	socket_enviar(socketConexionWorker, D_STRUCT_JOBT, enviado);
	free(enviado);

	void* estructuraRecibida;
	t_tipoEstructura tipoEstructura;

	int recepcion = socket_recibir(socketConexionWorker, &tipoEstructura, &estructuraRecibida);
	if(recepcion == -1 || tipoEstructura != D_STRUCT_NUMERO){
				log_error(logger,"TRANSFORMACION - No se recibio correctamente la confirmacion del Worker");
	}
	else{
		t_struct_confirmacion* confirmacion = malloc(sizeof(t_struct_confirmacion));
		switch(((t_struct_confirmacion*)estructuraRecibida)->confirmacion){
				case WORKER_MASTER_TRANSFORMACION_OK:
				// Lo agregamos a una lista de sockets listos para la reduccion local?
				socket_enviar(socketConexionYAMA, D_STRUCT_CONFIRMACION_TRANSFORMACION,confirmacion);
				close(socketConexionWorker);
				break;

				case WORKER_MASTER_TRANSFORMACION_FALLO:
				socket_enviar(socketConexionYAMA, D_STRUCT_CONFIRMACION_TRANSFORMACION,confirmacion);
				close(socketConexionWorker);
				// Que YAMA replanifique
				break;
		}
	}
}

void ejecutarReduccionLocal (t_infoNodo_reduccionLocal worker){
	// Se conecta al worker
	int socketConexionWorker = crearCliente(worker.ip,worker.puerto);
	if(socketConexionWorker == 1){
		puts("REDUCCION LOCAL - No se pudo establecer la conexion con Worker, cerrando Master...");
		log_error(logger,"REDUCCION LOCAL - No se pudo establecer la conexion con Worker, cerrando Master...");
		exit(1);
	}

	t_struct_jobR* enviado = malloc(sizeof(t_struct_jobR));
	enviado->scriptReduccion = malloc(sizeof(MAX_LEN_RUTA));
	enviado->pathTempFinal = malloc(sizeof(MAX_LEN_RUTA));

	strcpy(enviado->pathTempFinal, worker.pathTempFinal);
	strcpy(enviado->scriptReduccion, argumentosMaster->script_reduccion);
	list_add_all(enviado->pathTemp,worker.pathTemp); // Enviar

	socket_enviar(socketConexionWorker, D_STRUCT_JOBR, enviado);
	free(enviado);

	void* estructuraRecibida;
	t_tipoEstructura tipoEstructura;

	int recepcion = socket_recibir(socketConexionWorker, &tipoEstructura, &estructuraRecibida);
	if(recepcion == -1 || tipoEstructura != D_STRUCT_NUMERO){
				log_error(logger,"REDUCCION LOCAL - No se recibio correctamente la confirmacion del Worker");
	}
	else{
		t_struct_confirmacion* confirmacion = malloc(sizeof(t_struct_confirmacion));
		switch(((t_struct_confirmacion*)estructuraRecibida)->confirmacion){
				case WORKER_MASTER_REDUCCIONL_OK:
				// Lo agregamos a una lista de sockets que hicieron la reduccion local exitosamente?
				socket_enviar(socketConexionYAMA, D_STRUCT_CONFIRMACION_REDUCCIONL,confirmacion);
				close(socketConexionWorker);

				if(contadorNodosReduccion != cantidadNodosReduccion){
					sem_wait(&mutex_contadorReduccion);
					contadorNodosReduccion++;
					sem_post(&mutex_contadorReduccion);
				}
				else{
					sem_post(&bin_reduccion);
				}

				break;

				case WORKER_MASTER_REDUCCIONL_FALLO: //Si falla CantidadNodosReduccion--?
				socket_enviar(socketConexionYAMA, D_STRUCT_CONFIRMACION_REDUCCIONL,confirmacion);
				close(socketConexionWorker);
				break;
		}
	}
}

void ejecutarReduccionGlobal (t_infoNodo_reduccionGlobal* workerEncargado, t_list* nodos, char* rutaReduccion){
	// Se conecta al worker encargado
	int socketConexionWorker = crearCliente(workerEncargado->ip,workerEncargado->puerto);
	if(socketConexionWorker == 1){
		puts("REDUCCION GLOBAL - No se pudo establecer la conexion con Worker Encargado, cerrando Master...");
		log_error(logger,"REDUCCION GLOBAL - No se pudo establecer la conexion con Worker Encargado, cerrando Master...");
		exit(1);
	}

	t_struct_jobRG* enviado = malloc(sizeof(t_struct_jobRG));
	enviado->scriptReduccion = malloc (strlen(rutaReduccion));
	strcpy(enviado->scriptReduccion, rutaReduccion);
	// Habria que crear la lista?
	list_add_all(enviado->nodos,nodos);
	enviado->cantidadNodos = list_size(enviado->nodos);

	socket_enviar(socketConexionWorker, D_STRUCT_NODOS_ESCLAVOS, enviado);
	free(enviado);

	void* estructuraRecibida;
	t_tipoEstructura tipoEstructura;

	int recepcion = socket_recibir(socketConexionWorker, &tipoEstructura, &estructuraRecibida);
	if(recepcion == -1 || tipoEstructura != D_STRUCT_NUMERO){
				log_error(logger," REDUCCION GLOBAL - No se recibio correctamente la confirmacion del Worker Encargado");
	}
	else{
		t_struct_confirmacion* confirmacion = malloc(sizeof(t_struct_confirmacion));
		switch(((t_struct_confirmacion*)estructuraRecibida)->confirmacion){
				case WORKER_MASTER_REDUCCIONG_OK:
				// Lo agregamos a una lista de sockets que hicieron la reduccion local exitosamente?
				socket_enviar(socketConexionYAMA, D_STRUCT_CONFIRMACION_REDUCCIONG,confirmacion);
				close(socketConexionWorker);
				break;

				case WORKER_MASTER_REDUCCIONG_FALLO:
				socket_enviar(socketConexionYAMA, D_STRUCT_CONFIRMACION_REDUCCIONG,confirmacion);
				close(socketConexionWorker);
				break;
		}
	}
}

void ejecutarAlmacenamientoFinal (t_infoNodo_Final* workerEncargado){
	// Se conecta al worker encargado
	int socketConexionWorker = crearCliente(workerEncargado->ip,workerEncargado->puerto);
	if(socketConexionWorker == 1){
		puts("ALMACENAMIENTO FINAL - No se pudo establecer la conexion con Worker Encargado, cerrando Master...");
		log_error(logger,"ALMACENAMIENTO FINAL - No se pudo establecer la conexion con Worker Encargado, cerrando Master...");
		exit(1);
	}

	t_struct_string* enviado = malloc(sizeof(t_struct_string));
	enviado->string = malloc(sizeof(MAX_LEN_RUTA));
	strcpy(enviado->string, workerEncargado->nombreResultadoRG);

	// El nombre del archivo que le da el FS se lo pasaria Master tambien?
	socket_enviar(socketConexionWorker, D_STRUCT_STRING, enviado);

	void* estructuraRecibida;
	t_tipoEstructura tipoEstructura;

	int recepcion = socket_recibir(socketConexionWorker, &tipoEstructura, &estructuraRecibida);
	if(recepcion == -1 || tipoEstructura != D_STRUCT_NUMERO){
				log_error(logger," ALMACENAMIENTO FINAL - No se recibio correctamente la confirmacion del Worker Encargado");
	}
	else{
		t_struct_confirmacion* confirmacion = malloc(sizeof(t_struct_confirmacion));
		switch(((t_struct_confirmacion*)estructuraRecibida)->confirmacion){
				case WORKER_MASTER_ALMACENAMIENTO_FINAL_OK:
				// Lo agregamos a una lista de sockets que hicieron la reduccion local exitosamente?
				socket_enviar(socketConexionYAMA, D_STRUCT_CONFIRMACION_ALMACENAMIENTO_FINAL,confirmacion);
				close(socketConexionWorker);
				break;

				case WORKER_MASTER_ALMACENAMIENTO_FINAL_FALLO:
				socket_enviar(socketConexionYAMA, D_STRUCT_CONFIRMACION_REDUCCIONG,confirmacion);
				close(socketConexionWorker);
				break;
		}
	}

}

//////////////////////////////////////// OTRAS FUNCIONES ///////////////////////////////////////////////////
char* obtenerContenido(char* ruta){
	char *data;
	struct stat sbuf;

	int fd = open(ruta, O_RDONLY);

	if (fd == -1){
		perror("open");
		exit(1);
	}

	 if (stat(ruta, &sbuf) == -1) {
	     perror("stat");
	     exit(1);
	}

	data = mmap((caddr_t)0, sbuf.st_size, PROT_READ, MAP_SHARED, fd, 0);

	if (data == (caddr_t)(-1)) {
		perror("mmap");
		exit(1);
	}

	return data;
}

bool esEncargado(t_infoNodo_reduccionGlobal* nodo) {
		if(nodo->encargado == 1){
		return 1;
		log_info(logger,"REDUCCION GLOBAL - Se encontro al nodo encargado");
		}
		puts("REDUCCION GLOBAL - No se encontro al nodo encargado");
		log_error(logger,"REDUCCION GLOBAL - No se encontro al nodo encargado");
		return 0;
}

t_infoNodo_reduccionGlobal* buscarEncargado(t_list* nodos){

	t_infoNodo_reduccionGlobal* nodoEncargado = list_find(nodos, (void*)esEncargado);
	return nodoEncargado;
}

void mostrarMetricas(){
	puts("Metricas");
}
