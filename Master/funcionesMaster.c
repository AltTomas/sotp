#include "funcionesMaster.h"

pthread_mutex_t mutex_contadorReduccion = PTHREAD_MUTEX_INITIALIZER;

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
	scriptTransformacion = obtenerContenido(argumentos[1]);
	scriptReduccion = obtenerContenido(argumentos[2]);

	argumentosMaster = malloc(sizeof(t_argumentos));

	argumentosMaster->script_transformacion = (char*)malloc(strlen(scriptTransformacion)+1);
	strcpy(argumentosMaster->script_transformacion, scriptTransformacion);

	argumentosMaster->script_reduccion = (char*)malloc(strlen(scriptReduccion)+1);
	strcpy(argumentosMaster->script_reduccion, scriptReduccion);

	argumentosMaster->archivo = (char*)malloc(strlen(argumentos[3])+1);
	strcpy(argumentosMaster->archivo, argumentos[3]);

	argumentosMaster->archivo_resultado = (char*)malloc(strlen(argumentos[4])+1);
	strcpy(argumentosMaster->archivo_resultado, argumentos[4]);

	t_struct_string* enviado = malloc(sizeof(t_struct_string));

	enviado->string = (char*)malloc(strlen(argumentosMaster->archivo)+1);

	strcpy(enviado->string,argumentosMaster->archivo);

	socket_enviar(socketConexionYAMA, D_STRUCT_STRING,enviado);
	free(enviado);

	puts("Solicitando ejecucion de tarea");
	log_info(logger,"Solicitando ejecucion de tarea");

	void* estructuraRecibida;
	t_tipoEstructura tipoEstructura;
	int i;
	///					TRANSFORMACION					///

	// Aca deberiamos recibir los nodos que nos envia YAMA para despues conectarnos
	int recepcion = socket_recibir(socketConexionYAMA, &tipoEstructura, &estructuraRecibida); // Recibimos nodos para transformacion

	if(recepcion == -1){ // YAMA envia enum a ver si me puedo conectar a los nodos

		puts("TRANSFORMACION - No se recibio correctamente la cantidad de nodos");
		log_info(logger,"TRANSFORMACION - No se recibio correctamente la cantidad de nodos");

		puts("Cerrando Master...");
		exit(1);
	}
	else{
		switch (tipoEstructura) {
			case D_STRUCT_NUMERO:
			if(((t_struct_numero*)estructuraRecibida)->numero == YAMA_MASTER_FS_NOT_LOAD) {
				puts("TRANSFORMACION - FS NOT LOAD");
				log_info(logger,"TRANSFORMACION - FS NOT LOAD");
				puts("Cerrando Master...");
				exit(1);
				}
			else{
				pthread_t hiloWorker;
				for(i = 0; i <((t_struct_numero*)estructuraRecibida)->numero; i++){
					recepcion = socket_recibir(socketConexionYAMA, &tipoEstructura, &estructuraRecibida);
					if(recepcion == -1 || tipoEstructura != D_STRUCT_NODO_TRANSFORMACION){
						puts("TRANSFORMACION - No se recibio correctamente la informacion de los nodos");
						log_info(logger,"TRANSFORMACION - No se recibio correctamente la informacion de los nodos");
						puts("Cerrando Master...");
						exit(1);
					}

					pthread_create(&hiloWorker, NULL, (void*)ejecutarTransformacion, (t_infoNodo_transformacion*)estructuraRecibida);
				}
			}
			break;
		}
	}


	///					REDUCCION LOCAL					///

	sem_init(&bin_reduccion,0,0);

	// Debemos hacer un for para recibir todos los nodos

	recepcion = socket_recibir(socketConexionYAMA, &tipoEstructura, &estructuraRecibida); // Recibimos nodos para reduccion local

	if(recepcion == -1 || tipoEstructura != D_STRUCT_NODOS_REDUCCION_LOCAL){ //YAMA_MASTER_DATA_NODO
				log_info(logger,"No se recibio correctamente la informacion de los nodos");
	}

	t_infoNodo_reduccionLocal* info_nodo = ((t_infoNodo_reduccionLocal*)estructuraRecibida);

	pthread_t hiloWorker;

	for(i = 0; i<info_nodo->cantidadTemporales; i++){
		char* temporal = list_get(info_nodo->pathTemp, i);
		}

	pthread_create(&hiloWorker, NULL, (void*)ejecutarReduccionLocal, info_nodo);

	// Esperamos a que se realicen todas las reducciones
	sem_wait(&bin_reduccion);

	///					REDUCCION GLOBAL					///
	recepcion = socket_recibir(socketConexionYAMA, &tipoEstructura, &estructuraRecibida); // Recibimos nodos para reduccion global

	if(recepcion == -1 || tipoEstructura != D_STRUCT_NODOS_REDUCCION_GLOBAL){ //YAMA_MASTER_DATA_NODO
		log_error(logger,"No se recibio correctamente la informacion de los nodos");
	}

	// Primero envia al encargado

	t_infoNodo_reduccionGlobal* info_nodo_encargado = ((t_infoNodo_reduccionGlobal*)estructuraRecibida);

	// Despues envia a los demas nodos (esclavos)
	t_list* nodosEsclavos = list_create();

	recepcion = socket_recibir(socketConexionYAMA, &tipoEstructura, &estructuraRecibida); // Recibimos la cantidad de esclavos
	int cantidadEsclavos = ((t_struct_numero*)estructuraRecibida)->numero;

	for(i=0;i < cantidadEsclavos; i++){

		recepcion = socket_recibir(socketConexionYAMA, &tipoEstructura, &estructuraRecibida); // Recibimos esclavos para reduccion global

		if(recepcion == -1 || tipoEstructura != D_STRUCT_NODO_ESCLAVO){
			log_error(logger,"No se recibio correctamente la informacion de los nodos");
		}

		list_add(nodosEsclavos,((t_struct_nodoEsclavo*)estructuraRecibida));
		}

	 //Parametros son: Nodo Encargado, Nodos Esclavos, Script
	ejecutarReduccionGlobal(info_nodo_encargado,nodosEsclavos, argumentosMaster->script_reduccion);

	///					ALMACENADO FINAL				///

	recepcion = socket_recibir(socketConexionYAMA, &tipoEstructura, &estructuraRecibida); // Recibimos el nodo para realizar el almacenamiento final

	if(recepcion == -1 || tipoEstructura != D_STRUCT_NODOS_REDUCCION_LOCAL){
		log_error(logger,"No se recibio correctamente la informacion del nodo");
	}

	t_infoNodo_Final* nodo = (t_infoNodo_Final*)estructuraRecibida;

	ejecutarAlmacenamientoFinal(nodo);
}


////////////////////////////////// ETAPAS ///////////////////////////////////////////////////////////////////

void ejecutarTransformacion (t_infoNodo_transformacion* worker){
	// Se conecta al worker
	int socketConexionWorker = crearCliente(worker->ip,worker->puerto);

	if(socketConexionWorker == 1){ // Si no se puede conectar tendria que esperar la replanificacion de YAMA
		printf("TRANSFORMACION - No se pudo establecer la conexion con Worker del Nodo %d\n", worker->idNodo);
		log_error(logger,"TRANSFORMACION - No se pudo establecer la conexion con Worker%d", worker->idNodo);

		t_struct_numero* idNodo = malloc(sizeof(t_struct_numero)); // Le deberia mandar a YAMA a que no me conecte
		idNodo->numero = worker->idNodo;
		socket_enviar(socketConexionYAMA,D_STRUCT_NUMERO, idNodo);
		free(idNodo);

		t_struct_numero* confirmacion = malloc(sizeof(t_struct_numero));
		confirmacion->numero=MASTER_YAMA_CONEXION_WORKER_FALLO;
		socket_enviar(socketConexionYAMA,D_STRUCT_NUMERO, confirmacion);
		free(confirmacion);

		// Aca deberiamos recibir de YAMA si puede replanificar este mismo nodo
		// Si puede replanificar tendriamos que recibir los datos del nodo en el hilo y conectarnos a ese nodo y seguimos el flujo
		// Si no puede cierra master
	}
	else{
		t_struct_numero* idNodo = malloc(sizeof(t_struct_numero)); // Le deberia mandar a YAMA a que me conecte
		idNodo->numero = worker->idNodo;
		socket_enviar(socketConexionYAMA,D_STRUCT_NUMERO, idNodo);
		free(idNodo);

		t_struct_numero* confirmacion = malloc(sizeof(t_struct_numero));
		confirmacion->numero=MASTER_YAMA_CONEXION_WORKER_OK;
		socket_enviar(socketConexionYAMA,D_STRUCT_NUMERO, confirmacion);
		free(confirmacion);
	}

	t_struct_jobT* enviado = malloc(sizeof(t_struct_jobT));

	enviado->scriptTransformacion = (char*)malloc(strlen(scriptTransformacion)+1);
	strcpy(enviado->scriptTransformacion, scriptTransformacion);

	enviado->bloque = worker->numBloque;
	enviado->bytesOcupadosBloque = worker->bytesOcupados;

	enviado->pathOrigen = malloc(strlen(argumentosMaster->archivo)+1);
	strcpy(enviado->pathOrigen, argumentosMaster->archivo);

	enviado->pathTemporal = malloc(strlen(worker->nombreTemporal)+1);
	strcpy(enviado->pathTemporal, worker->nombreTemporal);
	socket_enviar(socketConexionWorker, D_STRUCT_JOBT, enviado);
	free(enviado);

	void* estructuraRecibida;
	t_tipoEstructura tipoEstructura;

	int recepcion = socket_recibir(socketConexionWorker, &tipoEstructura, &estructuraRecibida);
	if(recepcion == -1 || tipoEstructura != D_STRUCT_NUMERO){
				log_error(logger,"TRANSFORMACION - No se recibio correctamente la confirmacion del Worker");
	}
	else{
		t_struct_numero* idNodo = malloc(sizeof(t_struct_numero));
		idNodo->numero = worker->idNodo;
		t_struct_numero* confirmacion = malloc(sizeof(t_struct_numero));
		switch(((t_struct_numero*)estructuraRecibida)->numero){
				case WORKER_MASTER_TRANSFORMACION_OK:{
					// Lo agregamos a una lista de sockets listos para la reduccion local?
					confirmacion->numero = MASTER_YAMA_TRANSFORMACION_WORKER_OK;
					socket_enviar(socketConexionYAMA, D_STRUCT_NUMERO,idNodo);
					socket_enviar(socketConexionYAMA, D_STRUCT_NUMERO,confirmacion);
					close(socketConexionWorker);
					free(idNodo);
					free(confirmacion);
					break;
				}

				case WORKER_MASTER_TRANSFORMACION_FALLO:{
					confirmacion->numero = MASTER_YAMA_TRANSFORMACION_WORKER_FALLO;
					socket_enviar(socketConexionYAMA, D_STRUCT_NUMERO,idNodo);
					socket_enviar(socketConexionYAMA, D_STRUCT_NUMERO,confirmacion);
					close(socketConexionWorker);
					// Que YAMA replanifique
				break;
				}
		}
	}
}

void ejecutarReduccionLocal (t_infoNodo_reduccionLocal* worker){
	// Se conecta al worker
	puts("Ejecutando hilo");
	int socketConexionWorker = crearCliente(worker->ip,worker->puerto);
	if(socketConexionWorker == 1){
		puts("REDUCCION LOCAL - No se pudo establecer la conexion con Worker, cerrando Master...");
		log_error(logger,"REDUCCION LOCAL - No se pudo establecer la conexion con Worker, cerrando Master...");

		t_struct_numero* idNodo = malloc(sizeof(t_struct_numero)); // Le deberia mandar a YAMA que no me conecte
		idNodo->numero= worker->idNodo;
		socket_enviar(socketConexionYAMA,D_STRUCT_NUMERO, idNodo);
		free(idNodo);

		t_struct_numero* confirmacion = malloc(sizeof(t_struct_numero)); // Le deberia mandar a YAMA que no me conecte
		confirmacion->numero=MASTER_YAMA_CONEXION_WORKER_FALLO;
		socket_enviar(socketConexionYAMA,D_STRUCT_NUMERO, confirmacion);
		free(confirmacion);
		exit(1);
		//return(0);
	}
	else{
		t_struct_numero* idNodo = malloc(sizeof(t_struct_numero)); // Le deberia mandar a YAMA que me conecte
		idNodo->numero= worker->idNodo;
		socket_enviar(socketConexionYAMA,D_STRUCT_NUMERO, idNodo);
		free(idNodo);

		t_struct_numero* confirmacion = malloc(sizeof(t_struct_numero)); // Le deberia mandar a YAMA a que me conecte
		confirmacion->numero=MASTER_YAMA_CONEXION_WORKER_OK;
		socket_enviar(socketConexionYAMA,D_STRUCT_NUMERO, confirmacion);
		free(confirmacion);
	}

	t_struct_jobR* enviado = malloc(sizeof(t_struct_jobR));
	enviado->cantidadTemporales = worker->cantidadTemporales;
	enviado->scriptReduccion = malloc(strlen(argumentosMaster->script_reduccion)+1);
	strcpy(enviado->scriptReduccion, argumentosMaster->script_reduccion);
	enviado->pathTempFinal = malloc(strlen(worker->pathTempFinal)+1);
	strcpy(enviado->pathTempFinal, worker->pathTempFinal);
	enviado->pathTemp = worker->pathTemp;

	socket_enviar(socketConexionWorker, D_STRUCT_JOBR, enviado);
	free(enviado);

	void* estructuraRecibida;
	t_tipoEstructura tipoEstructura;

	int recepcion = socket_recibir(socketConexionWorker, &tipoEstructura, &estructuraRecibida);
	if(recepcion == -1 || tipoEstructura != D_STRUCT_NUMERO){
				log_error(logger,"REDUCCION LOCAL - No se recibio correctamente la confirmacion del Worker");
	}
	else{
		t_struct_numero* confirmacion = malloc(sizeof(t_struct_numero));
		t_struct_numero* idNodo = malloc(sizeof(t_struct_numero));
		//idNodo->numero= worker->nodo;
		switch(((t_struct_numero*)estructuraRecibida)->numero){
				case WORKER_MASTER_REDUCCIONL_OK:
				// Lo agregamos a una lista de sockets que hicieron la reduccion local exitosamente?
				confirmacion->numero = MASTER_YAMA_REDUCCIONL_WORKER_OK;
				socket_enviar(socketConexionYAMA, D_STRUCT_NUMERO,idNodo);
				socket_enviar(socketConexionYAMA, D_STRUCT_NUMERO,confirmacion);
				close(socketConexionWorker);
				free(confirmacion);
				free(idNodo);

				if(contadorNodosReduccion < cantidadNodosReduccion){
									pthread_mutex_lock(&mutex_contadorReduccion);
									contadorNodosReduccion++;
									pthread_mutex_unlock(&mutex_contadorReduccion);
								}
								else{
									sem_post(&bin_reduccion);
								}

								break;
				break;

				case WORKER_MASTER_REDUCCIONL_FALLO: //Si falla CantidadNodosReduccion--?

					socket_enviar(socketConexionYAMA, D_STRUCT_NUMERO,idNodo);
					socket_enviar(socketConexionYAMA, D_STRUCT_NUMERO,confirmacion);
					close(socketConexionWorker);
					free(confirmacion);
					free(idNodo);
					// Deberiamos cerrar master porque no se puede replanificar?
				break;
		}
	}
}

void ejecutarReduccionGlobal (t_infoNodo_reduccionGlobal* workerEncargado, t_list* nodos, char* rutaReduccion){
	// Se conecta al worker encargado
	puts("Entramos al hilo");
	int socketConexionWorker = crearCliente(workerEncargado->ip,workerEncargado->puerto);
	if(socketConexionWorker == 1){
		puts("REDUCCION GLOBAL - No se pudo establecer la conexion con Worker Encargado, cerrando Master...");
		log_error(logger,"REDUCCION GLOBAL - No se pudo establecer la conexion con Worker Encargado, cerrando Master...");
		exit(1);
	}

	// Aca hacer un list size y con un for ir enviandole al worker encargado los esclavos

	t_struct_jobRG* enviado = malloc(sizeof(t_struct_jobRG));

	enviado->scriptReduccion = malloc (strlen(rutaReduccion));
	strcpy(enviado->scriptReduccion, rutaReduccion);
	enviado->cantidadNodos = nodos->elements_count;
	enviado->nodos = nodos;

	socket_enviar(socketConexionWorker, D_STRUCT_NODO_ESCLAVO, enviado);
	free(enviado);

	void* estructuraRecibida;
	t_tipoEstructura tipoEstructura;

	int recepcion = socket_recibir(socketConexionWorker, &tipoEstructura, &estructuraRecibida);
	if(recepcion == -1 || tipoEstructura != D_STRUCT_NUMERO){
				log_error(logger," REDUCCION GLOBAL - No se recibio correctamente la confirmacion del Worker Encargado");
	}
	else{
		t_struct_numero* confirmacion = malloc(sizeof(t_struct_numero));
		t_struct_numero* idNodo = malloc(sizeof(t_struct_numero));
		switch(((t_struct_numero*)estructuraRecibida)->numero){
				case WORKER_MASTER_REDUCCIONG_OK:
				// Lo agregamos a una lista de sockets que hicieron la reduccion local exitosamente?
					confirmacion->numero = MASTER_YAMA__REDUCCIONG_WORKER_OK;
					socket_enviar(socketConexionYAMA, D_STRUCT_NUMERO,idNodo);
					socket_enviar(socketConexionYAMA, D_STRUCT_NUMERO,confirmacion);
					close(socketConexionWorker);
					free(confirmacion);
					free(idNodo);
				break;

				case WORKER_MASTER_REDUCCIONG_FALLO:
					confirmacion->numero = MASTER_YAMA__REDUCCIONG_WORKER_FALLO;
					socket_enviar(socketConexionYAMA, D_STRUCT_NUMERO,idNodo);
					socket_enviar(socketConexionYAMA, D_STRUCT_NUMERO,confirmacion);
					close(socketConexionWorker);
					free(confirmacion);
					free(idNodo);
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
		t_struct_numero* confirmacion = malloc(sizeof(t_struct_numero));
		t_struct_numero* idNodo = malloc(sizeof(t_struct_numero));
		switch(((t_struct_numero*)estructuraRecibida)->numero){
				case WORKER_MASTER_ALMACENAMIENTO_FINAL_OK:
					confirmacion->numero = MASTER_YAMA_ALMACENAMIENTO_FINAL_WORKER_OK;
					socket_enviar(socketConexionYAMA, D_STRUCT_NUMERO,idNodo);
					socket_enviar(socketConexionYAMA, D_STRUCT_NUMERO,confirmacion);
					close(socketConexionWorker);
					free(confirmacion);
					free(idNodo);
				break;

				case WORKER_MASTER_ALMACENAMIENTO_FINAL_FALLO:
					confirmacion->numero = MASTER_YAMA_ALMACENAMIENTO_FINAL_WORKER_FALLO;
					socket_enviar(socketConexionYAMA, D_STRUCT_NUMERO,idNodo);
					socket_enviar(socketConexionYAMA, D_STRUCT_NUMERO,confirmacion);
					close(socketConexionWorker);
					free(confirmacion);
					free(idNodo);
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

void mostrarMetricas(){
	puts("Metricas");
}
