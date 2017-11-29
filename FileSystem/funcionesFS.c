#include "funcionesFS.h"

int sizeBloque = 1048576;

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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void crearEstructurasAdministrativas(){

	info_DataNodes = list_create();
	archivos = list_create();

	char* crearDirectorioArchivos = string_from_format("mkdir -p %s", tablaArchivos);
	system(crearDirectorioArchivos);

	char* crearDirectorioBitmaps = string_from_format("mkdir -p %s", "../../yamaFS/metadata/bitmaps");
	system(crearDirectorioBitmaps);

	directorios = list_create();

	t_directory* directorioRoot = malloc(sizeof(t_directory));
	directorioRoot->index = 0;
	directorioRoot->nombre = (char*)malloc(strlen("root")+1);
	strcpy(directorioRoot->nombre ,"root");
	directorioRoot->padre = -1;

	list_add(directorios,directorioRoot);

	char* crearTablaDirectorios = string_from_format("touch %s", tablaDirectorios);
	system(crearTablaDirectorios);

	// todo: Persistir root en archivo tabla de directorios

	char* crearTablaNodos = string_from_format("touch %s", tablaNodos);
	system(crearTablaNodos);
	log_info(logger,"Se creo la tabla de nodos");


}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
		case 7:{ // CPFROM

			char* pathArchivo = readline("Ingrese path del archivo en FS local: ");
			char* pathYAMAFS = readline("Ingrese path de YAMAFS: ");

			void* contenido = obtenerContenido(pathArchivo);
			int tamanioArchivo = conseguirTamanioArchivo(pathArchivo);
			char* tipo;

			if(esBinario(contenido,tamanioArchivo)){
				tipo = strdup("Binario");
			}
			else{
				tipo = strdup("Texto");
			}

			int resultado = almacenarArchivo(pathArchivo, pathYAMAFS, tipo);// Faltan argumentos

			if(resultado){
				puts("Almacenamiento exitoso!");
				log_info(logger,"Almacenamiento exitoso!");
			}
			else{
				puts("No se pudo almacenar el archivo");
				log_error(logger,"No se pudo almacenar el archivo");
			}

		}
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
		else if(((t_struct_numero*)estructuraRecibida)->numero == ES_DATANODE){ // Aca deberiamos rebotar nodos si el FS esta formateado
			FD_SET(socketNuevo, &datanode);
			FD_SET(socketNuevo, set);
			if(socketNuevo > max_fd) max_fd = socketNuevo;
				printf("Se acaba de conectar un DATANODE  en el socket %d\n", socketNuevo);
				log_info(logger,"Se acaba de conectar un DATANODE  en el socket %d", socketNuevo);

			t_struct_numero* enviado = malloc(sizeof(t_struct_numero));
			enviado->numero = FS_DATANODE_PEDIDO_INFO;
			socket_enviar(socketNuevo,D_STRUCT_NUMERO,enviado);
			free(enviado);// Este send puede estar de mas, podria enviarlo el DN directamente

			void* estructuraRecibida;
			t_tipoEstructura tipoEstructura;

			 int recepcion = socket_recibir(socketNuevo, &tipoEstructura,&estructuraRecibida);

			  if(recepcion == -1){
			    printf("Se desconecto el Nodo en el socket %d\n", socketNuevo);
			    log_info(logger,"Se desconecto el Nodo en el socket %d", socketNuevo);
			    close(socketNuevo);
			    FD_CLR(socketNuevo, &datanode);
			    FD_CLR(socketNuevo, &setDataNodes);
			  }
			  else if(tipoEstructura != D_STRUCT_INFO_NODO){
			    puts("Se esperaba recibir una estructura D_STRUCT_INFO_NODO");
			    log_error(logger,"Se esperaba recibir una estructura D_STRUCT_INFO_NODO");
			  }
			  else{

				  // Guardamos esa info en nuestra lista de nodos conectados
				  list_add(info_DataNodes, (t_struct_datanode*)estructuraRecibida);

				  // Con la info recibida del nodo que se conecto  actualizamos nodos.bin

				  actualizarTablaNodosAgregar( ((t_struct_datanode*)estructuraRecibida)->nomDN,
						  	  	  	  	  	   ((t_struct_datanode*)estructuraRecibida)->bloquesTotales,
											   ((t_struct_datanode*)estructuraRecibida)->bloquesLibres
				  	  	  	  	  	 	 	 );
			  }
		}
		else if(1){ // estado Estable
			// Pueden conectarse YAMA y workers
		}
		else{ // estado NO estable
			// No puede conectarse nada que no sea datanode
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

    //Actualizar tabla de nodos - Borrar
    actualizarTablaNodosBorrar(socketDataNode);
  }
  else if(tipoEstructura != D_STRUCT_NUMERO){
    puts("Error en la serializacion");
    log_info(logger,"Error en la serializacion");
  }
  else{
    printf("Llego solicitud de tarea del Nodo en el socket %d\n", socketDataNode);
    log_info(logger,"Llego solicitud de tarea del Nodo en el socket %d", socketDataNode);

    FD_SET(socketDataNode,&setDataNodes);
  }
}

///////////////////////////////////CONEXION YAMA-FS//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void trabajarSolicitudYama(int socketYAMA){

  void* estructuraRecibida;
  t_tipoEstructura tipoEstructura;

  int recepcion = socket_recibir(socketYAMA, &tipoEstructura,&estructuraRecibida);

  if(recepcion == -1){
    printf("Se desconecto el YAMA en el socket %d\n", socketYAMA);
    log_info(logger,"Se desconecto el YAMA en el socket %d", socketYAMA);
    close(socketYAMA);
    FD_CLR(socketYAMA, &yama);
    FD_CLR(socketYAMA, &setyama);
  }
  else{
    switch(tipoEstructura){
      case D_STRUCT_NUMERO:
      switch(((t_struct_numero*)estructuraRecibida)->numero){
        case YAMA_FS_SOLICITAR_LISTABLOQUES :
          printf("Llego solicitud de bloques del YAMA en el socket %d\n", socketYAMA);
          log_info(logger,"Llego solicitud de bloques del YAMA en el socket %d", socketYAMA);

          printf("Archivo Objetivo: %s\n",((t_struct_string*)estructuraRecibida)->string);
          log_info(logger,"Archivo Objetivo: %s",((t_struct_string*)estructuraRecibida)->string);

          buscarBloquesArchivo(((t_struct_string*)estructuraRecibida)->string,socketYAMA);

          FD_SET(socketYAMA,&setyama);
          break;
      }
    }
  }
}

void buscarBloquesArchivo(char* nombreFile, int socketConexionYAMA) {

	bool condition(void* element) {
		file* archivo = element;
		return string_equals_ignore_case(archivo->path, nombreFile);
	}
  file* archivoEncontrado = list_find(filesAlmacenados, condition); //me devuelve el archivo, verificar si esta bien hecha

  int cantidadBloques = list_size(archivoEncontrado -> bloques);

  socket_enviar(socketConexionYAMA,D_STRUCT_NUMERO,cantidadBloques); //FS_YAMA_CANTIDAD_BLOQUES ya estaba definido del lado de funcionesYama

  int i;

  for(i=0;i<cantidadBloques;i++){

  bloque* bloque = list_get(archivoEncontrado->bloques,i);

  t_struct_bloques* bloquesFile = malloc(sizeof(t_struct_bloques));  //bloquesFile seria la informacion de cada Bloque del arch
  bloquesFile->finalBloque = bloque -> finBloque;
  bloquesFile->numNodoOriginal = bloque -> copia0-> numNodo;
  bloquesFile->numBloqueOriginal = bloque -> copia0-> bloqueNodo;
  bloquesFile->ipNodoOriginal = bloque -> copia0 -> ip;
  bloquesFile->puertoNodoOriginal = bloque -> copia0-> puerto;
  bloquesFile->numNodoCopia = bloque -> copia1 -> numNodo;
  bloquesFile->numBloqueCopia = bloque -> copia1-> bloqueNodo;
  bloquesFile->ipNodoCopia = bloque -> copia1-> ip;
  bloquesFile->puertoNodoCopia = bloque -> copia1 -> puerto;
  socket_enviar(socketConexionYAMA,D_STRUCT_BLOQUE_FS_YAMA,bloquesFile);
  free (bloquesFile);

  }
}

int determinarEstado() {

	int cantidadArchivosAlmacenados = list_size(filesAlmacenados);

	int i;

	for(i=0;i<cantidadArchivosAlmacenados;i++){

		file* archivoAlmacenado1 = list_get(filesAlmacenados,i);

		int cantidadBloquesArchivosAlmacenados = list_size(archivoAlmacenado1->bloques);

		for(i=0;i<cantidadBloquesArchivosAlmacenados;i++){
			//comparo si alguna de las dos copias de ese bloque esta dentro de los Nodos conectados con el md5 con un IF
			//IF -> OK -> esta ese bloque en el Nodo Conectado

		}
		//IF se cumple todo, esta en estado estable
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void leer(char* path,char* nombreArch){

	char* posicion;

	t_directory* directorioEncontrado = buscarDirectorio(path);

	posicion = string_itoa(directorioEncontrado->index);

	char* rutaArchivo = string_new();

	string_append (&rutaArchivo,tablaArchivos);
	string_append (&rutaArchivo,"/");
	string_append (&rutaArchivo,posicion);
	string_append (&rutaArchivo,"/");
	string_append (&rutaArchivo,nombreArch);


	t_config* archivoTablaArchivos = config_create(rutaArchivo); // con o sin &&??

	if(archivoTablaArchivos == NULL)
			log_error(logger,"ERROR: No se pudo encontrar el archivo nombreArch.csv");
			puts("ERROR: No se pudo encontrar el archivo nombreArch.csv");


	int cantidadKeys = config_keys_amount(archivoTablaArchivos);


	int cantidadBloques = (cantidadKeys-2)/3;

	int j;

	for (j=0;j<cantidadBloques;j++){

		char* numero = string_itoa(j);

		char* bloque = "BLOQUE";

		string_append(&bloque, numero);

		string_append (&bloque,"COPIA0");

		copiaLectura* bloqueLectura= config_get_array_value(archivoTablaArchivos,bloque); //TIENE QUE SER CHAR**

		//busco Nodo y su bloque interno que contiene a la copia0 del bloque del archivo

		bool condition(void* element) {
			t_info_nodo* nodo = element;
				return string_equals_ignore_case(nodo->nombreNodo, bloqueLectura->numNodo);
			}

		t_info_nodo* nodoEncontrado = list_find(info_DataNodes,condition);

		int socketNodoEncontrado = nodoEncontrado -> socket;

		// crear t_struct_numero* enviado, asignarle valor a enviado->numero (bloqueLectura->bloqueNodo)
		t_struct_numero* enviado = bloqueLectura->bloqueNodo;
		// Ejemeplo: socket_enviar(socketNodoEncontrado, D_STRUCT_NUMERO,enviado);
		socket_enviar(socketNodoEncontrado, D_STRUCT_NUMERO,enviado); //[Nodo1, 33]

		void* estructuraRecibida;
		t_tipoEstructura tipoEstructura;

		int recepcion = socket_recibir(socketNodoEncontrado, &tipoEstructura,&estructuraRecibida);

		if (recepcion == -1){

				log_info(logger,"No se recibio la copia0 del Nodo");

				char* numeroBloqueSinNumCopia = string_substring_until(numero,12);//BLOQUE0COPIA faltaria el 1

				string_append (&numeroBloqueSinNumCopia,"1"); //corregir a COPIA1

				copiaLectura* bloqueCopiaLectura= config_get_array_value(archivoTablaArchivos,numeroBloqueSinNumCopia); //TIENE QUE SER CHAR**


				bool condition(void* element) {
					t_info_nodo* nodo = element;
					return string_equals_ignore_case(nodo->nombreNodo, bloqueCopiaLectura->numNodo);
						}

				t_info_nodo* nodocopia1Encontrado = list_find(info_DataNodes,condition);

				int socketNodocopia1Encontrado = nodocopia1Encontrado -> socket;

				t_struct_numero* enviado = bloqueLectura->bloqueNodo;

				// Lo mismo que con el enviar de arriba
				socket_enviar(socketNodocopia1Encontrado,D_STRUCT_NUMERO,enviado); //[Nodo1, 33]

				void* estructuraCopiaRecibida;
				t_tipoEstructura tipoEstructuraCopia;

				int recepcionCopia = socket_recibir(socketNodoEncontrado, &tipoEstructuraCopia,&estructuraCopiaRecibida);

				if (recepcionCopia == -1){
					log_info(logger,"No se recibio ni la copia0 ni la copia1 del Nodo");
				}



		}

		j++;

	}
}


t_directory* buscarDirectorio(char* path){

char** pathDividido = string_split(path, "/"); //Me quedaria una lista de cada subdirectorio dividido por "/", termina con un valor NULL
int i=0;
char* ultimoSubdirectorio;

while(pathDividido[i]!=NULL){
	if(pathDividido[i+1]==NULL){ //Si la siguiente posicion es NULL entonces es el ultimo subdirectorio
	ultimoSubdirectorio = malloc(strlen(pathDividido[i])+1);
	strcpy(ultimoSubdirectorio, pathDividido[i]);  //obtengo subdirectorio donde estara el archivo
	}
	else{
		i++;
	}
}

bool condition(void* element) {
	t_directory* directorio = element;
	return string_equals_ignore_case(directorio->nombre, ultimoSubdirectorio);
}

t_directory* directorioEncontrado = list_find(directorios, condition); //seria directorios o habria que abrir el metadata directorios.dat
return directorioEncontrado;
}







int almacenarArchivo(char* ruta, char* nombreArchivo, char* tipo){// Faltan argumentos

	long tamanioFile = conseguirTamanioArchivo(ruta);
	void* data = obtenerContenido(ruta);
	int numeroBloque = 0;

	// todo: Yo cuando quiero almacenar un archivo me fijo si tengo que crearle una ruta o puedo usar una existente?
	int indexDirectorio;

	if(existeEnTablaDirectorio(ruta)){
		t_directory* directorio = buscarDirectorio(ruta);
	}
	else{ // Hay que crearle un directorio
		indexDirectorio = asignarPosicionEnTablaDirectorios(ruta, nombreArchivo);

		if(indexDirectorio < 0){ // Tabla llena
				log_error(logger,"ERROR: Como la tabla de directorios tiene 100 entradas no se pueden agregar mas");
				return 0; // No se pudo almacenar
			}
	}

	crearMetadataArchivo(indexDirectorio, nombreArchivo, tamanioFile, tipo);

	if(strcmp(tipo,"Binario") == 0){

		int cantidadBloques = tamanioFile/sizeBloque + (tamanioFile % sizeBloque !=0);
		int offset = 0;

		for(numeroBloque = 0; numeroBloque < cantidadBloques; numeroBloque++){ // Fijarse bien como cortar el void*
			if(tamanioFile > sizeBloque){
				char* dataBloque = string_substring(data,offset,sizeBloque);
				offset += sizeBloque;
				int resultadoEnvio = enviarADataNode(nombreArchivo, dataBloque, numeroBloque, sizeBloque); // Considerando que los binarios no tiene basura
				if(!resultadoEnvio){ // Fallo el almacenamiento en nodos de algun bloque
								return 0;
							}
			}
			else{ // Puede estar de mas esto
				enviarADataNode(nombreArchivo, data, numeroBloque, tamanioFile);
			}

		}
	}
	else if(strcmp(tipo,"Texto") == 0){

		char **registros = string_split((char*)data, "\n");

		int cantidadRegistros = obtenerCantidadElementos(registros);

		for(numeroBloque = 0; numeroBloque< cantidadRegistros; numeroBloque++){
			int resultadoEnvio = enviarADataNode(nombreArchivo, registros[numeroBloque],numeroBloque,strlen(registros[numeroBloque]));
			if(!resultadoEnvio){ // Fallo el almacenamiento en nodos de algun bloque
				return 0;
			}
		}// habra que poner el +1 en el strlen?

	}
	//Contemplar que no se pueda almacenar porque faltan bloques

	return 1; // Exito
}

int enviarADataNode(char* nombreArchivo, char* data, int numeroBloque, int bytesOcupados){ // Aca distribuimos el envio a los datanodes

	//t_Nodos* metadataNodos = leerMetadataNodos(tablaNodos); // Llamo la funcion para ver si hubo cambios en nodos.bin
	// Ya no se usaria, pero por las dudas la dejo

	list_sort(info_DataNodes, masBloquesLibres); // Tira warning pero funciona

	t_info_nodo* nodoMasLibre = list_get(info_DataNodes,0);
	t_info_nodo* segundoNodoMasLibre = list_get(info_DataNodes,1);

	int socketNodoCopia0 = buscarSocketNodo(nodoMasLibre->nombreNodo);
	int socketNodoCopia1 = buscarSocketNodo(segundoNodoMasLibre->nombreNodo);

	t_pedido_almacenar_bloque* enviado = malloc(sizeof(t_pedido_almacenar_bloque));
	enviado->contenidoBloque = (char*)malloc(strlen(data)+1);
	enviado->bloqueArchivo = numeroBloque;
	enviado->bytesOcupados = bytesOcupados;

	int resultadoCopia0 = guardarCopia(nombreArchivo, socketNodoCopia0,enviado,0);
	int resultadoCopia1 = guardarCopia(nombreArchivo, socketNodoCopia1,enviado,1);
	free(enviado);

	if(resultadoCopia0 || resultadoCopia1){ // Se pudo guardar en alguna de las 2 copias

		return 1;
	}
	else{ // No se pudo guardar el archivo

		return 0;
	}
	return 0;
}

t_Nodos* leerMetadataNodos(char* archivoNodos){

	t_Nodos* metadata = malloc(sizeof(t_Nodos));
	metadata->bloquesTotalesyLibres = list_create();

    t_config* metadataNodos;

    metadataNodos = config_create(archivoNodos);
    if(metadataNodos == NULL)
    	puts("ERROR - nodos.bin inexistente");


    if(!verificarMetadataNodos(metadataNodos)){
    	puts("Fallo en formato de tabla de nodos");
    }

    metadata->tamanio_total =  config_get_int_value(metadataNodos, "TAMANIO");
    metadata->libre_total = config_get_int_value(metadataNodos, "LIBRE");
    metadata->nodos = config_get_array_value(metadataNodos,"NODOS");

    int i;
    int cantidadNodos = obtenerCantidadElementos(metadata->nodos);
    for(i = 0; i< cantidadNodos; i++){

    	t_bloquesLibres_nodo* bloquesLibresNodos = malloc(sizeof(t_bloquesLibres_nodo));

    	nodo* infoNodo = malloc(sizeof(nodo));
    	char* bloquesTotalesNodo = string_new(); // Hacer de otra forma mas eficiente
    	string_append(&bloquesTotalesNodo,metadata->nodos[i]);
    	string_append(&bloquesTotalesNodo,"Total");

    	char* bloquesLibresNodo = string_new(); // Hacer de otra forma mas eficiente
    	string_append(&bloquesLibresNodo,metadata->nodos[i]);
    	string_append(&bloquesLibresNodo,"Libre");

    	infoNodo->tamTotalNodo = config_get_int_value(metadataNodos, bloquesTotalesNodo);
    	infoNodo->tamLibreNodo = config_get_int_value(metadataNodos, bloquesLibresNodo);

    	bloquesLibresNodos->nombreNodo = (char*)malloc(strlen(metadata->nodos[i])+1);
    	strcpy(bloquesLibresNodos->nombreNodo, metadata->nodos[i]);

    	bloquesLibresNodos->bloquesLibres = infoNodo->tamLibreNodo;
    }


    return metadata;
}

bool verificarMetadataNodos(t_config* metadataNodos){ // Si o si tiene que tener estos
	return config_has_property(metadataNodos,"TAMANIO") &&
			config_has_property(metadataNodos,"LIBRE") &&
			config_has_property(metadataNodos,"NODOS");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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

long conseguirTamanioArchivo(char* ruta){

	struct stat st;
	stat(ruta, &st);
	return st.st_size;
}

int obtenerCantidadElementos(char** array){
	int cantidadElementos;
	for (cantidadElementos = 0; array[cantidadElementos] != NULL; cantidadElementos++);
	return cantidadElementos;
}

bool esBinario(const void *data, size_t len){// revisar porque puede fallar
	    return memchr(data, '\0', len) != NULL;
	}

int buscarSocketNodo(char* nombreNodo){ // Revisar

	bool seEncuentraEnlista(t_info_nodo* element) {
		return strcmp(element->nombreNodo, nombreNodo)==0;
	}
	t_info_nodo * nodo = list_find(info_DataNodes, (void*)seEncuentraEnlista);
	return nodo->socket;
}

char* buscarNombreNodo(int socketNodo){ // Revisar

	bool seEncuentraEnlista(t_info_nodo* element) {
		return element->socket == socketNodo;
	}
	t_info_nodo * nodo = list_find(info_DataNodes, (void*)seEncuentraEnlista);
	return nodo->nombreNodo;
}

int buscarArchivo(char* nombreArchivo){

	int i;

	for(i = 0; i < archivos->elements_count ; i++){
		t_info_archivo* archivo = list_get(archivos,i);
		if(strcmp(archivo->nombreArchivo,nombreArchivo)==0){
			return i;
		}
	}
	return -1;
}

int buscarBloqueDeArchivo(t_info_archivo* archivo, int bloqueArchivo){

	int i;

	for(i = 0; i < archivo->infoBloques->elements_count ; i++){
		t_almacenar_bloque* bloque = list_get(archivo->infoBloques,i);
		if(bloqueArchivo == bloque->bloqueNodo){
			return i;
		}
	}
	return -1;
	}

int guardarCopia(char* nombreArchivo, int socketNodo, t_pedido_almacenar_bloque* enviado, int copia){

	socket_enviar(socketNodo, FS_DATANODE_ALMACENAR_BLOQUE,enviado);

	void* estructuraRecibida;
	t_tipoEstructura tipoEstructura;

	int recepcion = socket_recibir(socketNodo, &tipoEstructura,&estructuraRecibida);

	if(recepcion == -1){
		printf("Se desconecto el Nodo en el socket %d\n", socketNodo);
		log_info(logger,"Se desconecto el Nodo en el socket %d", socketNodo);
		close(socketNodo);
		FD_CLR(socketNodo, &datanode);
		FD_CLR(socketNodo, &setDataNodes);

		//Actualizar tabla de nodos - Borrar
		actualizarTablaNodosBorrar(socketNodo);

		return 0;
	}
	else if(tipoEstructura != D_STRUCT_NUMERO){
		puts("Error en la serializacion");
		log_error(logger,"Error en la serializacion");
		return 0;
	}
	else{
		if(((t_almacenar_bloque*)estructuraRecibida)->bloqueNodo >= 0){ // Se guardo bien, me enviaria el bloque donde se guardo

	   		//Actualizar tabla de nodos - Reducir bloques libres del nodo y bloques libres totales
		    actualizarTablaNodosAsignacion(socketNodo);

		    // Actualizar metadata de archivo - Bloque en bloque nodo
		   int posicion = buscarArchivo(nombreArchivo); // Verificamos si el archivo existe

		   if(posicion < 0){ // No existia el archivo

			   // todo: Crear el archivo (buscar index, hacer directorios, crear archivo usando nombre)

			   t_info_archivo* infoArchivo = malloc(sizeof(t_info_archivo));

			   infoArchivo->nombreArchivo = (char*)malloc(strlen(nombreArchivo)+1);
			   infoArchivo->infoBloques = list_create();

			   t_info_bloque_archivo* infoBloque = malloc(sizeof(t_info_bloque_archivo));

			   infoBloque->bloqueArchivo = enviado->bloqueArchivo;
			   infoBloque->bytesOcupados = enviado->bytesOcupados;

			   if(copia == 0){
				   infoBloque->copia0Nodo =((t_almacenar_bloque*)estructuraRecibida)->nombreNodo;
				   infoBloque->copia0NodoBloque = ((t_almacenar_bloque*)estructuraRecibida)->bloqueNodo;
			   }
			   else{
				   infoBloque->copia1Nodo =((t_almacenar_bloque*)estructuraRecibida)->nombreNodo;
				   infoBloque->copia1NodoBloque = ((t_almacenar_bloque*)estructuraRecibida)->bloqueNodo;
			   }

			   // Actualizar archivo metadata
			   actualizarMetadataArchivo(nombreArchivo);
		   }
		   else{ // Significa que ya existia el archivo, buscar por bloque en la lista
			   t_info_archivo* archivo = list_get(archivos,posicion);


			   int posicionBloque = buscarBloqueDeArchivo(archivo, enviado->bloqueArchivo);// Validacion < 0 ?

			   t_info_bloque_archivo* bloque = list_get(archivo->infoBloques, posicionBloque);

			   bloque->bloqueArchivo = enviado->bloqueArchivo;
			   bloque->bytesOcupados = enviado->bytesOcupados;

			   if(copia == 0){
				   bloque->copia0Nodo =((t_almacenar_bloque*)estructuraRecibida)->nombreNodo;
				   bloque->copia0NodoBloque = ((t_almacenar_bloque*)estructuraRecibida)->bloqueNodo;

				   // Actualizar archivo metadata
				   actualizarMetadataArchivoBloques(nombreArchivo,bloque->copia0Nodo,bloque->copia0NodoBloque, 0);
			   }
			   else{
				   bloque->copia1Nodo =((t_almacenar_bloque*)estructuraRecibida)->nombreNodo;
				   bloque->copia1NodoBloque = ((t_almacenar_bloque*)estructuraRecibida)->bloqueNodo;

				   // Actualizar archivo metadata
				   actualizarMetadataArchivoBloques(nombreArchivo,bloque->copia1Nodo,bloque->copia1NodoBloque, 1);
			   }
		   }

	   		log_info(logger,"Se  almaceno el bloque %d en el DN de socket: %d", enviado->bloqueArchivo, socketNodo);
	   		return 1;
	  	}

	}
	puts("Llego a cualquier lado");
	return 0;
}

bool masBloquesLibres(t_info_nodo* nodo1, t_info_nodo* nodo2){
	return nodo1->bloquesLibres > nodo2->bloquesLibres;
}

/////////////////////////////////////////// FUNCIONES TABLA NODOS /////////////////////////////////////////////////

void seDesconectaUnNodo(char* nombreNodo){

	log_info(logger,"Se desconecto un nodo - Actualizando tabla de nodos");
	t_Nodos* infoNodos = leerMetadataNodos(tablaNodos);

	int indexNodo = buscarPosicion(infoNodos->nodos, nombreNodo);
	if(indexNodo < 0){
		log_error(logger,"AVISO: No se encontro el nodo %s en la tabla de nodos", nombreNodo);
		log_info(logger,"Dejamos la tabla de nodos como estaba");
	}
	else{

	nodo* nodoABorrar = list_get(infoNodos->bloquesTotalesyLibres, indexNodo);

	if(nodoABorrar == NULL){
		log_error(logger,"AVISO: No hay nodos en la posicion %d", indexNodo);
		log_info(logger,"Dejamos la tabla de nodos como estaba");
	}
	else{
		log_info(logger,"Bloques Nodo que se elimina: %d\n", nodoABorrar->tamTotalNodo);
		log_info(logger,"Bloques Libres Nodo que se elimina: %d\n", nodoABorrar->tamLibreNodo);

	char* bloquesNodoEliminado = strdup(infoNodos->nodos[indexNodo]);
	char* bloquesLibresNodoEliminado = strdup(infoNodos->nodos[indexNodo]);
	string_append(&bloquesNodoEliminado,"Total");
	string_append(&bloquesLibresNodoEliminado,"Libre");

	int i = 0;
	int j = 0;
	int cantidadNodos = obtenerCantidadElementos(infoNodos->nodos);

	char** nuevoArray= malloc(sizeof(char*) * (cantidadNodos-1));

	for(i = 0; i < cantidadNodos; i++){

		if(i == indexNodo){

		}
		else{
			nuevoArray[j]=(char*)malloc(strlen(infoNodos->nodos[i])+1);
			strcpy(nuevoArray[j],infoNodos->nodos[i]);
			j++;
		}

	}

	t_config* archivoTablaNodos = config_create(tablaNodos);

	if(archivoTablaNodos == NULL)
		log_error(logger,"ERROR: No se pudo encontrar el archivo nodos.bin");
		puts("ERROR: No se pudo encontrar el archivo nodos.bin");

    if(!verificarMetadataNodos(archivoTablaNodos)){
    	log_error(logger,"Fallo en formato de tabla de nodos");
	   	puts("Fallo en formato de tabla de nodos");
	}
    else{

    int sizeTotal = config_get_int_value(archivoTablaNodos, "TAMANIO"); //Obtengo la cantidad de Nodos totales del archivo
    int sizeLibres = config_get_int_value(archivoTablaNodos, "LIBRE"); //Obtengo la cantidad de Nodos libres del archivo
    //char** nodos= config_get_array_value(archivoTablaNodos, "NODOS"); //Por las duda lo comente porque no se usaba

    sizeTotal -= nodoABorrar->tamTotalNodo;
    sizeLibres -= nodoABorrar->tamLibreNodo;

    char* string_sizeTotal = string_itoa(sizeTotal);
    char* string_sizeLibre = string_itoa(sizeLibres);

    int cantidadNodosArrayNuevo  = cantidadNodos - 1;

    char* arrayNodos = string_new();
    string_append(&arrayNodos,"[");

    for(i = 0; i < cantidadNodosArrayNuevo; i++){
    	if( i == cantidadNodosArrayNuevo -1){
    	string_append(&arrayNodos,nuevoArray[i]);
    	}
    	else{
    		string_append(&arrayNodos,nuevoArray[i]);
    		string_append(&arrayNodos,",");
    	}

    }
    string_append(&arrayNodos,"]");

    //Actualizo los valores de las keys del config
    config_set_value(archivoTablaNodos, "TAMANIO", string_sizeTotal);
    config_set_value(archivoTablaNodos, "LIBRE", string_sizeLibre);
    config_set_value(archivoTablaNodos, "NODOS", arrayNodos);

    config_set_value(archivoTablaNodos, bloquesNodoEliminado, "-");
    config_set_value(archivoTablaNodos, bloquesLibresNodoEliminado, "-");

    config_save(archivoTablaNodos);
    log_info(logger,"El archivo nodos.bin se actualizo exitosamente");
    puts("El archivo nodos.bin se actualizo exitosamente");
    }
	}
	}

}

int buscarPosicion(char** nodos, char* nombreNodo){

	int i;
	int cantidadNodos = obtenerCantidadElementos(nodos);

	for(i = 0; i < cantidadNodos ; i++){
		if(strcmp(nodos[i],nombreNodo)==0){
			return i;
		}
	}
	return -1;
}


void actualizarTablaNodosBorrar(int socketDataNode){

	int encontrarPosicionNodo(int socket){
	int i;
	for(i = 0; i < info_DataNodes->elements_count; i++ ){
		t_info_nodo* nodo = list_get(info_DataNodes,i);
		if(nodo->socket == socket){
			return i;
		}
	}
	return -1;
}
int posicion = encontrarPosicionNodo(socketDataNode);
if(posicion < 0){
	puts("No se encontro rastro del nodo eliminado en el sistema");
	log_error(logger,"No se encontro rastro del nodo eliminado en el sistema");
}
else{
	t_info_nodo* nodoDesconectado = list_remove(info_DataNodes,posicion);
	seDesconectaUnNodo(nodoDesconectado->nombreNodo);
}
}

void actualizarTablaNodosAgregar(char* nombreNodo, int bloquesTotalesNodo, int bloquesLibresNodo){

	// Habria que contemplar que se quieran agregar 2 nodos con el mismo nombre?
	log_info(logger,"Se conecto %s, actualizando Tabla de Nodos",nombreNodo);

	t_Nodos* infoNodos = leerMetadataNodos(tablaNodos);

	int i = 0;
	int cantidadNodos = obtenerCantidadElementos(infoNodos->nodos);

	char** nuevoArray= malloc(sizeof(char*) * (cantidadNodos+1));

	for(i = 0; i < cantidadNodos; i++){

			nuevoArray[i]=(char*)malloc(strlen(infoNodos->nodos[i])+1);
			strcpy(nuevoArray[i],infoNodos->nodos[i]);
		}

	char* keybloquesTotalesNodo = strdup(nombreNodo);
	char* keybloquesLibresNodo = strdup(nombreNodo);
	string_append(&keybloquesTotalesNodo,"Total");
	string_append(&keybloquesLibresNodo,"Libre");

	t_config* archivoTablaNodos = config_create(tablaNodos);

	if(archivoTablaNodos == NULL)
		puts("ERROR: No se pudo encontrar el archivo nodos.bin");

    if(!verificarMetadataNodos(archivoTablaNodos)){
	   	puts("Fallo en formato de tabla de nodos");
	}
    else{

    int sizeTotal = config_get_int_value(archivoTablaNodos, "TAMANIO"); //Obtengo la cantidad de Nodos totales del archivo
    int sizeLibres = config_get_int_value(archivoTablaNodos, "LIBRE"); //Obtengo la cantidad de Nodos libres del archivo
    //char** nodos= config_get_array_value(archivoTablaNodos, "NODOS"); //Por las dudas lo comente porque no se usaba

    sizeTotal += bloquesTotalesNodo;
    sizeLibres += bloquesLibresNodo;

    char* string_sizeTotal = string_itoa(sizeTotal);
    char* string_sizeLibre = string_itoa(sizeLibres);
    char* string_NodoNuevoBloquesTotal = string_itoa(bloquesTotalesNodo);
    char* string_NodoNuevoBloquesLibre = string_itoa(bloquesLibresNodo);

    int cantidadNodosArrayNuevo  = cantidadNodos + 1;

    char* arrayNodos = string_new();
    string_append(&arrayNodos,"[");

    for(i = 0; i < cantidadNodosArrayNuevo; i++){
    	if( i == cantidadNodosArrayNuevo -1){
    	string_append(&arrayNodos,nombreNodo);
    	}
    	else{
    		string_append(&arrayNodos,nuevoArray[i]);
    		string_append(&arrayNodos,",");
    	}

    }
    string_append(&arrayNodos,"]");

    //Actualizo los valores de las keys del config
    config_set_value(archivoTablaNodos, "TAMANIO", string_sizeTotal);
    config_set_value(archivoTablaNodos, "LIBRE", string_sizeLibre);
    config_set_value(archivoTablaNodos, "NODOS", arrayNodos);

    config_set_value(archivoTablaNodos, keybloquesTotalesNodo, string_NodoNuevoBloquesTotal);
    config_set_value(archivoTablaNodos, keybloquesLibresNodo, string_NodoNuevoBloquesLibre);

    config_save(archivoTablaNodos);
    log_info(logger,"Se agrego el %s a la Tabla de Nodos",nombreNodo);
    }

}

void actualizarTablaNodosAsignacion(int socketNodo){

	t_Nodos* infoNodos = leerMetadataNodos(tablaNodos);

	char* nombreNodo = buscarNombreNodo(socketNodo);


	int indexNodo = buscarPosicion(infoNodos->nodos, nombreNodo);
	if(indexNodo < 0){
		printf("AVISO: No se encontro el nodo %s en la tabla de nodos\n", nombreNodo);
		puts("Dejamos la tabla de nodos como estaba");
		}
	else{
	char* keyBloquesLibresNodo = string_new();
	string_append(&keyBloquesLibresNodo, nombreNodo);
	string_append(&keyBloquesLibresNodo, "Libre");

	t_config* archivoTablaNodos = config_create(tablaNodos);

	if(archivoTablaNodos == NULL)
		puts("ERROR: No se pudo encontrar el archivo nodos.bin");

    if(!verificarMetadataNodos(archivoTablaNodos)){
	   	puts("Fallo en formato de tabla de nodos");
	}
    else{

    int sizeLibres = config_get_int_value(archivoTablaNodos, "LIBRE"); //Obtengo la cantidad de Nodos libres del archivo
    int bloquesLibresNodo = config_get_int_value(archivoTablaNodos,keyBloquesLibresNodo);

    sizeLibres -= 1;
    bloquesLibresNodo -= 1;

	int encontrarPosicionNodo(char* nombreNodo){
		int i;
		for(i = 0; i < info_DataNodes->elements_count; i++ ){
			t_info_nodo* nodo = list_get(info_DataNodes,i);
			if(strcmp(nodo->nombreNodo,nombreNodo) == 0){
				return i;
			}
		}
	return -1;
	}

	int posicion = encontrarPosicionNodo(nombreNodo);
    t_struct_datanode* infoNodo = list_remove(info_DataNodes, posicion);
    infoNodo->bloquesLibres = bloquesLibresNodo;
    list_add(info_DataNodes,infoNodo);

    char* string_sizeLibre = string_itoa(sizeLibres);
    char* string_NodoNuevoBloquesLibre = string_itoa(bloquesLibresNodo);

    //Actualizo los valores de las keys del config
    config_set_value(archivoTablaNodos, "LIBRE", string_sizeLibre);
    config_set_value(archivoTablaNodos, keyBloquesLibresNodo, string_NodoNuevoBloquesLibre);

    config_save(archivoTablaNodos);
    log_info(logger,"Se asigno un bloque  del %s\n",nombreNodo);
    }
	}
}
//////////////////////////////////// FUNCIONES TABLA DE DIRECTORIOS /////////////////////////////////////

int asignarPosicionEnTablaDirectorios(char* rutaArchivo, char* nombreArchivo){

}

int existeEnTablaDirectorio(char* rutaArchivo){

}

//////////////////////////////////// FUNCIONES TABLA DE ARCHIVOS ///////////////////////////////////////

void crearMetadataArchivo(int indexDirectorio, char* nombreArchivo, long tamanioArchivo,char* tipoArchivo){

	char* string_index = string_itoa(indexDirectorio);

	char* rutaArchivo = string_new();
	string_append(&rutaArchivo,tablaArchivos);
	string_append(&rutaArchivo,"/");
	string_append(&rutaArchivo,string_index);
	string_append(&rutaArchivo,"/");
	string_append(&rutaArchivo,nombreArchivo);

	char* crearMetadaArchivo = string_from_format("touch %s", rutaArchivo);

	system(crearMetadaArchivo);

	log_info(logger,"Se creo el archivo de metadata del archivo %s",nombreArchivo);


		t_config* metadataArchivo = config_create(rutaArchivo);

		if(metadataArchivo == NULL)
			printf("ERROR: No se pudo encontrar el archivo %s", rutaArchivo);

		char* string_tamanio = string_itoa(tamanioArchivo);

	    //Actualizo los valores de las keys del config
	    config_set_value(metadataArchivo, "TAMANIO", string_tamanio);
	    config_set_value(metadataArchivo, "TIPO", tipoArchivo);

	    config_save(metadataArchivo);
	    log_info(logger,"Se creo exitosamente archivo de metada %s",rutaArchivo);
	    }

//todo: Implementar funciones
void actualizarMetadataArchivo(char* nombreArchivo){

}

void actualizarMetadataArchivoBloques(char* nombreArchivo, char* nombreNodo, int bloqueNodo, int copia){

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void renombrar(char* path_original, char* path_final){//ejemplo renombrar(lalala/so/2/tp.txt, op.txt)

	t_directory* ultimoSubdirectorio = buscarDirectorio(path_original);

	if(verificarSiEsArchivoODirectorio(path_original)!=0){

		int posicion = ultimoSubdirectorio->index;
		char* indiceChar = string_itoa (posicion);
		char* rutaArchivo = string_new();
		string_append (&rutaArchivo,tablaArchivos);
		string_append (&rutaArchivo,"/");
		string_append (&rutaArchivo,indiceChar);
		string_append (&rutaArchivo,"/");
		string_append (&rutaArchivo, path_final);

				if(verificarExistenciaNombreArchivo(path_final)!=0){
					path_original = rutaArchivo;//CHEQUEAR
				}else{
					log_error(logger,"Ya existe ese nombre de Archivo");
				}

	}else{
	char* nombreUltimoSubdirectorio = ultimoSubdirectorio -> nombre;
	int padreUltimoSubdirectorio = ultimoSubdirectorio -> padre;

	if (verificarExistenciaNombreDirectorio(nombreUltimoSubdirectorio,padreUltimoSubdirectorio)==1){
		(ultimoSubdirectorio -> nombre) = path_final; //CHEQUEAR
	}else{
		log_error(logger,"Ya existe ese nombre de Directorio");
	}
	}
}



int verificarSiEsArchivoODirectorio(char* path){
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);// si es !=0 es un archivo
} //podria usarse un fopen, si abre es archivo y sino directorio, o con string_contains que se fijaria el "." para que sea archivo



int verificarExistenciaNombreDirectorio(char* nombre, int padre){ //podria usar un strcmp

	int respuesta;
	char* padreChar = string_itoa(padre);

	bool condition(void* element) {
		t_directory* directorio = element;
		return string_equals_ignore_case(string_itoa(directorio->padre), padreChar) && string_equals_ignore_case(directorio->nombre, nombre) ;
	}
	t_directory* directorioConMismoPadreYNombre = list_find(directorios, condition);//seria directorios o habria que abrir el metadata directorios.dat

	if (directorioConMismoPadreYNombre == NULL){
		respuesta=1;
	}else{
		respuesta=0;
	}
	return respuesta; //    =0 ya existe el directorio con ese nombre en el mismo padre
}



int verificarExistenciaNombreArchivo(char* rutaArchivo){
	int respuesta;

	FILE* archivo = fopen(rutaArchivo, "r");

	if (archivo) {
	fclose(archivo);
	 respuesta = 0; //    =0 ya existe archivo con ese nombre en el mismo index
	} else {
	 respuesta=1;
	}

	return respuesta;
	}

/////////////////////////////////////////////////////////


void MD5(char* path_archivo) { //chequear si es char* o void (por system)

	bool condition(void* element) {
			file* archivo = element;
			return string_equals_ignore_case(archivo->path, path_archivo);
		}
	 file* archivoEncontrado = list_find(filesAlmacenados, condition);

	 if (archivoEncontrado!=NULL){
		 	char* md5Char = "md5sum ";
		 	string_append (&md5Char,path_archivo);
			system(md5Char); //para que me quede md5sum utn/hola.txt
	 } else {
		 log_error(logger,"error al realizar el md5");
	 }

}

void cat(char* path_archivo) { //chequear si es char* o void (por system)

	bool condition(void* element) {
			file* archivo = element;
			return string_equals_ignore_case(archivo->path, path_archivo);
		}
	 file* archivoEncontrado = list_find(filesAlmacenados, condition);

	 if (archivoEncontrado!=NULL){
		 	char* catChar = "cat ";
		 	string_append (&catChar,path_archivo);
			system(catChar); //para que me quede cat utn/hola.txt, me devolveria el contenido ejemplo: adsasdasd
	 } else {
		 log_error(logger,"error al realizar el cat");
	 }

}

void ls(char* path_archivo) { //chequear si es char* o void (por system)

	bool condition(void* element) {
			file* archivo = element;
			return string_equals_ignore_case(archivo->path, path_archivo);
		}
	 file* archivoEncontrado = list_find(filesAlmacenados, condition);

	 if (archivoEncontrado!=NULL){
		 	char* lsChar = "ls ";
		 	string_append (&lsChar,path_archivo);
			system(lsChar); //para que me quede ls utn/hola.txt
	 } else {
		 log_error(logger,"error al realizar el ls");
	 }

}

void info(char* path_archivo) { //chequear si es char* o void (por system)

	bool condition(void* element) {
			file* archivo = element;
			return string_equals_ignore_case(archivo->path, path_archivo);
		}
	 file* archivoEncontrado = list_find(filesAlmacenados, condition);

	 if (archivoEncontrado!=NULL){
		 	int tamanioDelArchivo = archivoEncontrado->tamanio;
		 	char* tipoDelArchivo = archivoEncontrado->tipo;
		 	int cantidadBloques = list_size(archivoEncontrado -> bloques);

		 	 printf("El tamanio Del Archivo es %d\n", tamanioDelArchivo);
		 	 printf("El tipo Del Archivo del archivo es %s\n", tipoDelArchivo);
		 	 printf("La cantidad de bloques es %d\n", cantidadBloques);


		 	int i;
		 	for(i=0;i<cantidadBloques;i++){
		 	  bloque* bloque = list_get(archivoEncontrado->bloques,i);
		 	  int numeroBloque = bloque -> numBloque;
		 	  int fin = bloque -> finBloque;
		 	  int nodoOriginal = bloque -> copia0-> numNodo;
		 	  int bloqueNodoOriginal = bloque -> copia0-> bloqueNodo;
		 	  int nodoCopia = bloque -> copia1 -> numNodo;
		 	  int bloqueNodoCopia = bloque -> copia1-> bloqueNodo;

		 	 printf("Contiene al bloque numero %d\n", numeroBloque);
		 	 printf("Finaliza en %d\n", fin);
		 	 printf("La copia original se ubica en el nodo %d\n", nodoOriginal);
		 	 printf("En el bloque %d\n", bloqueNodoOriginal);
		 	 printf("La copia1 se ubica en el nodo %d\n", nodoCopia);
		 	 printf("En el bloque %d\n", bloqueNodoCopia);

		 	}


	 }
}



