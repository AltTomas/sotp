/*
 * funcionesYAMA.c
 *
 *  Created on: 10/9/2017
 *      Author: utnso
 */

#include "funcionesYAMA.h"

int cantidadDeBloques = 0;

void crearConfig() {
	if (verificarExistenciaDeArchivo(configuracionYAMA)) {
		config = levantarConfiguracionMaster(configuracionYAMA);
		log_info(logger, "Configuracion levantada correctamente");

	} else if (verificarExistenciaDeArchivo(
			string_substring_from(configuracionYAMA, 3))) {
		config = levantarConfiguracionMaster(
				string_substring_from(configuracionYAMA, 3));
		log_info(logger, "Configuracion levantada correctamente");

	} else {
		log_error(logger, "No se pudo levantar el archivo de configuracion");
		exit(EXIT_FAILURE);
	}

}

t_config_YAMA* levantarConfiguracionMaster(char* archivo_conf) {

	t_config_YAMA* conf = malloc(sizeof(t_config_YAMA));
	t_config* configYAMA;

	log_debug(logger, "Path config: %s", archivo_conf);

	configYAMA = config_create(archivo_conf);
	if (configYAMA == NULL)
		log_error(logger, "ERROR");

	if (!verificarConfig(configYAMA))
		log_error(logger, "CONFIG NO VALIDA");

	conf->FS_ip = malloc(MAX_LEN_IP);
	strcpy(conf->FS_ip, config_get_string_value(configYAMA, "FS_IP"));

	conf->FS_Puerto = malloc(MAX_LEN_PUERTO);
	strcpy(conf->FS_Puerto, config_get_string_value(configYAMA, "FS_PUERTO"));

	conf->YAMA_Puerto = malloc(MAX_LEN_PUERTO);
	strcpy(conf->YAMA_Puerto,config_get_string_value(configYAMA, "YAMA_PUERTO"));

	conf->Retardo_Planificacion = config_get_int_value(configYAMA, "RETARDO_PLANIFICACION");

	conf->Algoritmo_Balanceo = malloc(MAX_ALGORITMO);
	strcpy(conf->Algoritmo_Balanceo,config_get_string_value(configYAMA, "ALGORITMO_BALANCEO"));

	conf->Disp_Base = config_get_int_value(configYAMA, "DISP_BASE");

	config_destroy(configYAMA);
	printf("Configuracion levantada correctamente.\n");
	return conf;
}

bool verificarConfig(t_config* config) {
	return config_has_property(config, "FS_IP")
			&& config_has_property(config, "FS_PUERTO")
			&& config_has_property(config, "YAMA_PUERTO")
			&& config_has_property(config, "RETARDO_PLANIFICACION")
			&& config_has_property(config, "ALGORITMO_BALANCEO");
}

bool verificarExistenciaDeArchivo(char* path) {
	FILE * archivoConfig = fopen(path, "r");
	if (archivoConfig != NULL) {
		fclose(archivoConfig);
		return true;
	}
	return false;
}

void destruirConfig(t_config_YAMA* config) {
	free(config->FS_ip);
	free(config->FS_Puerto);
	free(config->YAMA_Puerto);
	free(config->Retardo_Planificacion); //Si le meto un "&" cuando intento ejecutar el proceso me tira un error de doble free
	free(config->Algoritmo_Balanceo);
	free(config->Disp_Base);
	free(config);
}

///////////////////////////////////////////////////// CONEXIONES //////////////////////////////////////////////////////////////////////////
int conectarConFS() {

	log_info(logger, "Conectandose a FS");

	socketConexionFS = crearCliente(config->FS_ip, atoi(config->FS_Puerto));

	if (socketConexionFS == -1) {
		log_error(logger,
				"No se pudo establecer la conexion con FS, cerrando YAMA...");
		return 0;
	}

	log_info(logger, "Conexión exitosa a FS");
	return 1;

}

void escucharConexiones(void) {

	FD_ZERO(&maestro);
	FD_ZERO(&setMasters);

	socketEscuchaMasters = crearServidor(atoi(config->YAMA_Puerto));

	max_fd = socketEscuchaMasters;

	FD_SET(socketEscuchaMasters, &maestro);
	fd_set read_fd;
	int iterador_sockets, resultadoHilo;

	while (1) {
		read_fd = maestro;

		if (select(max_fd + 1, &read_fd, NULL, NULL, NULL) == -1) {
			perror("Error en SELECT");
			exit(1);
		}

		if (FD_ISSET(socketEscuchaMasters, &read_fd)) { //un master quiere conectarse
			aceptarNuevaConexion(socketEscuchaMasters, &setMasters);
		}

		for (iterador_sockets = 0; iterador_sockets <= max_fd;
				iterador_sockets++) {

			if (FD_ISSET(iterador_sockets,
					&setMasters) && FD_ISSET(iterador_sockets,&read_fd)) { //una master realiza una operacion
				FD_CLR(iterador_sockets, &setMasters);
				pthread_t hilo;
				resultadoHilo = pthread_create(&hilo, NULL,
						(void*) trabajarSolicitudMaster, iterador_sockets);
				if (resultadoHilo)
					exit(1);
			}
		}
	}
}

void aceptarNuevaConexion(int socketEscucha, fd_set* set) {

	int socketNuevo = aceptarCliente(socketEscucha);

	if (socketNuevo == -1) {
		perror("Error al aceptar conexion entrante");
	} else {
		void* estructuraRecibida;
		t_tipoEstructura tipoEstructura;

		int recepcion = socket_recibir(socketNuevo, &tipoEstructura,
				&estructuraRecibida);

		if (recepcion == -1 || tipoEstructura != D_STRUCT_NUMERO) {
			log_info(logger, "No se recibio correctamente a que atendio YAMA");
		} else if (((t_struct_numero*) estructuraRecibida)->numero
				== ES_MASTER) {
			FD_SET(socketNuevo, &maestro);
			FD_SET(socketNuevo, set);
			if (socketNuevo > max_fd)
				max_fd = socketNuevo;
			printf("Se acaba de conectar un proceso Master en el socket %d\n",
					socketNuevo);
			log_info(logger,
					"Se acaba de conectar un proceso Master en el socket %d",
					socketNuevo);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void trabajarSolicitudMaster(int socketMaster) {

	void* estructuraRecibida;
	t_tipoEstructura tipoEstructura;

	int recepcion = socket_recibir(socketMaster, &tipoEstructura,
			&estructuraRecibida);

	if (recepcion == -1) {
		printf("Se desconecto el Master en el socket %d\n", socketMaster);
		log_info(logger, "Se desconecto el Master en el socket %d",
				socketMaster);
		close(socketMaster);
		FD_CLR(socketMaster, &maestro);
		FD_CLR(socketMaster, &setMasters);
	} else {
		switch (tipoEstructura) {
		case D_STRUCT_STRING:
			printf("Llego solicitud de tarea del Master en el socket %d\n",socketMaster);
			log_info(logger,"Llego solicitud de tarea del Master en el socket %d",socketMaster);

			printf("Archivo Objetivo: %s\n",((t_struct_string*) estructuraRecibida)->string);
			log_info(logger, "Archivo Objetivo: %s",((t_struct_string*) estructuraRecibida)->string);
			getNodoByFile(((t_struct_string*) estructuraRecibida)->string,socketMaster);
			FD_SET(socketMaster, &setMasters);

			break;
		}
	}
}

void getNodoByFile(char* nombreFile, int socketConexionMaster) {

	void* estructuraRecibida;
	uint16_t i;
	t_tipoEstructura tipoEstructura;
	t_struct_string* stringFile = malloc(sizeof(t_struct_string));
	strcpy(stringFile->string, nombreFile);


	socket_enviar(socketConexionFS, D_STRUCT_STRING, stringFile);
	free(stringFile);

	int recepcion = socket_recibir(socketConexionFS, &tipoEstructura, &estructuraRecibida);
	if (recepcion == -1) {
		printf("Se desconecto el FS en el socket %d\n", socketConexionFS);
		log_info(logger, "Se desconecto el Master en el socket %d",
				socketConexionFS);
		close(socketConexionFS);
	} else {

		switch (tipoEstructura) {
		case D_STRUCT_NUMERO:
			switch (((t_struct_numero*) estructuraRecibida)->numero) {

				case FS_YAMA_NOT_LOAD: {
					t_struct_numero * numero2 = malloc(sizeof(t_struct_numero));
					numero2->numero = YAMA_MASTER_FS_NOT_LOAD;
					socket_enviar(socketConexionMaster, D_STRUCT_NUMERO, numero2);
					free(numero2);
					break;
				}

				case FS_YAMA_LISTABLOQUES:{
					socket_recibir(socketConexionFS, D_STRUCT_NUMERO,&estructuraRecibida);
					cantidadDeBloques =((t_struct_numero*) estructuraRecibida)->numero;
					t_struct_bloques* bloqueEnviar = malloc(sizeof(t_struct_bloques));
					for (i = 0; i < cantidadDeBloques; i++) {
						/* Recibo Bloques y los agrego a la lista */
						socket_recibir(socketConexionFS, D_STRUCT_BLOQUE_FS_YAMA,&estructuraRecibida);

						bloqueEnviar->numNodoOriginal=	((t_struct_bloques*) estructuraRecibida)->numNodoOriginal;
						bloqueEnviar->numBloqueOriginal = ((t_struct_bloques*) estructuraRecibida)->numBloqueOriginal;
						bloqueEnviar->ipNodoOriginal = ((t_struct_bloques*) estructuraRecibida)->ipNodoOriginal;
						bloqueEnviar->puertoNodoOriginal = ((t_struct_bloques*) estructuraRecibida)->puertoNodoOriginal;
						bloqueEnviar->numNodoCopia=	((t_struct_bloques*) estructuraRecibida)->numNodoCopia;
						bloqueEnviar->numBloqueCopia= ((t_struct_bloques*) estructuraRecibida)->numBloqueCopia;
						bloqueEnviar->ipNodoCopia= ((t_struct_bloques*) estructuraRecibida)->ipNodoCopia;
						bloqueEnviar->puertoNodoCopia= ((t_struct_bloques*) estructuraRecibida)->puertoNodoCopia;
						bloqueEnviar->finalBloque= ((t_struct_bloques*) estructuraRecibida)->finalBloque;

						//agregarBloqueALaLista(bloqueEnviar);

						agregarNodoAlDiccionario(bloqueEnviar->numBloqueOriginal,bloqueEnviar->numNodoOriginal);
						agregarNodoAlDiccionario(bloqueEnviar->numBloqueCopia,bloqueEnviar->numNodoCopia);

						agregarALaListaBalanceoCarga(bloqueEnviar->numNodoOriginal);
						agregarALaListaBalanceoCarga(bloqueEnviar->numNodoCopia);
					}
					iniciarPlanificacion();
				break;
				}
			}
			break;

			case D_STRUCT_STRING:
			break;
		}
	}
}

void init() {
	tablaEstados = list_create();
}

int getAvailability() {
	int pwl;
	if(!strcmp(config->Retardo_Planificacion, "CLOCK")){
		pwl = 0;
	} else {
		//pwl =
	}
	return config->Disp_Base + pwl;
}

char* generarNombreTemporal(int socketMaster, int nroBloque) {
	char* nombre = string_from_format("/tmp/Master%d-temp", socketMaster);
	char* bloque = string_itoa(nroBloque);
	string_append(&nombre, bloque);
	return nombre;
}

void agregarNodoAlDiccionario(int idWorker, int numBloque){

	if(!dictionary_has_key(ubicacionBloques, idWorker)){
		dictionary_put(ubicacionBloques,idWorker,list_create());
	}

	t_list* listaBloques = dictionary_get(ubicacionBloques, idWorker);
	list_add(listaBloques, numBloque);
}

void agregarBloqueALaLista(t_struct_bloques* bloqueAgregar){
	listaUbicacionBloques* nuevoBloque = malloc(sizeof(listaUbicacionBloques));

	/* Datos del nodo Original*/
	nuevoBloque->idNodoOriginal = bloqueAgregar->numNodoOriginal;
	nuevoBloque->numeroBloqueOriginal = bloqueAgregar->numBloqueOriginal;
	nuevoBloque->ipOriginal = bloqueAgregar->ipNodoOriginal;
	nuevoBloque->puertoOriginal =bloqueAgregar->puertoNodoOriginal;
	/* Datos del nodo Copia*/
	nuevoBloque->idNodoCopia = bloqueAgregar->numNodoCopia;
	nuevoBloque->numeroBloqueCopia = bloqueAgregar->numBloqueCopia;
	nuevoBloque->ipCopia = bloqueAgregar->ipNodoCopia;
	nuevoBloque->puertoCopia = bloqueAgregar->puertoNodoCopia;
	nuevoBloque->finalBloque = bloqueAgregar->finalBloque;

	list_add(listaInfoBloques,nuevoBloque); /* lista var global */
}

void asignarBloquesALosWorkers(){

	int cantBloques = dictionary_size(ubicacionBloques);
	int indexCursor = 0;
	int indexWorkerActual = 0;
	int pasos = 1;
	int numBloque = 0;
	for(numBloque; numBloque < cantBloques; numBloque++){
		while(!workerTieneBloque(indexWorkerActual, numBloque)){
			indexWorkerActual++;
			pasos++;

			if(pasos == cantBloques){
				pasos = 1;
				aumentarAvaibility();
			}
			if(indexWorkerActual >= cantBloques) indexWorkerActual = 0;
		}
		indexCursor++;
		indexWorkerActual = indexCursor;
	}
}

bool workerTieneBloque(int indexWorkerActual, int numBloque){

	balanceoCargas* infoWorker = list_get(listaBalanceoCargas, indexWorkerActual);
	t_list* bloquesDelWorker = dictionary_get(ubicacionBloques, infoWorker->worker);

	bool tieneBloque(int bloque){
		return numBloque == bloque ? 1 : 0;
	}

	bool resultado =  list_any_satisfy(bloquesDelWorker, tieneBloque);

	if(resultado){
		if(infoWorker->availability == 0) return 0;
		infoWorker->availability--;
		list_add(bloquesDelWorker, numBloque);
	}

	return resultado;
}

void aumentarAvaibility(){
	void aumentar(balanceoCargas* elemento){
		elemento->availability += getAvailability();
	}
	list_iterate(listaBalanceoCargas, aumentar);
}

void iniciarPlanificacion(){
	/* Asigno los bloques a la lista de Balanceo Cargas*/
	asignarBloquesALosWorkers();

	/* ****************************************************************************** */

	/* Cuando inicias planificacion modificas Tabla de Estados que seria el controller */

	/* ****************************************************************************** */


	/* Me fijo en la lista de balanceo de cargas en que worker esta */

	/* ****************************************************************************** */

	/* Me fijo en la lista de estados si hay algo y sino agrego el estado en ejecucion */

	/* ****************************************************************************** */

	/* Mando a Ejecutar el bloque */

	/* ****************************************************************************** */

}

void agregarALaListaBalanceoCarga(int idWorker){

	bool existeWorker(balanceoCargas* elemento){
		return elemento->worker == idWorker ? 1 : 0;
	}
	if(!list_any_satisfy(listaBalanceoCargas,existeWorker)){
		balanceoCargas* elementoAgregar = malloc(sizeof(balanceoCargas));
		elementoAgregar->availability = getAvailability();
		elementoAgregar->worker = idWorker;
		elementoAgregar->bloques = list_create();
		list_add(listaBalanceoCargas,elementoAgregar);
	}

}
