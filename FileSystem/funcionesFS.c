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

	char* crearDirectorioArchivos = string_from_format("mkdir -p %s", "../../yamaFS/metadata/archivos");
	system(crearDirectorioArchivos);

	char* crearDirectorioBitmaps = string_from_format("mkdir -p %s", "../../yamaFS/metadata/bitmaps");
	system(crearDirectorioBitmaps);

	directorios = list_create();
	char* crearTablaDirectorios = string_from_format("touch %s", tablaDirectorios);
	system(crearTablaDirectorios);

	infoNodos = malloc(sizeof(t_Nodos));
	char* crearTablaNodos = string_from_format("touch %s", tablaNodos);
	system(crearTablaNodos);
	log_info(logger,"Se creo la tabla de nodos");

	infoNodos->nodos = list_create();


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
		case 7:{

			char* pathArchivo = readline("Ingrese path del archivo en FS local: ");
			char* pathYAMAFS = readline("Ingrese path de YAMAFS: ");

			//encontrar el archivo en el FS local
			// Si no esta terminar la operacion y avisar, si esta leerlo
			//char* contenidoArchivoLocal = obtenerContenido(pathArchivo);

			// Crear un archivo con el mismo nombre en YAMA FS
			// Copiar el contenido
			// En realidad esto no seria asi - a partir del contenido deberiamos hacer los bloques y guardarlos
			char* comandoCrearArchivo = string_from_format("cat  %s >", pathArchivo);
			string_append(&comandoCrearArchivo, pathYAMAFS);
			system(comandoCrearArchivo);

			//Actualizar tabla de archivos
			file* archivoNuevo = malloc(sizeof(file));

			archivoNuevo->path = (char*)malloc(strlen(pathYAMAFS)+1);
			strcpy(archivoNuevo->path, pathYAMAFS);

			archivoNuevo->tamanio = conseguirTamanioArchivo(pathArchivo);

			archivoNuevo->tipo = (char*)malloc(strlen("tipo")+1);
			strcpy(archivoNuevo->tipo, "tipo");

			//Ahora habria que hacer toda la asignacion de bloques
			if(strcmp(archivoNuevo->tipo, "Binario") == 0){
				// Hay que ir leyendolo de a cachos de 1MB
				// Nos tenemos que fijar
			}
			else if(strcmp(archivoNuevo->tipo, "Texto") == 0){
				// Hay que ir leyendolo y ver cuantos /n entran en 1MB (hacer un split por /n al contenido y ver que onda)
			}
			// Cortar en bloques de 1 MB y repartir entre los nodos


		printf("path archivo copiado: %s\n", archivoNuevo->path);
		printf("tamanio archivo copiado: %d\n", archivoNuevo->tamanio);
		printf("tipo archivo copiado: %s\n", archivoNuevo->tipo);
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
			socket_enviar(socketNuevo,D_STRUCT_NUMERO,enviado); // Este send puede estar de mas, podria enviarlo el DN
			free(enviado);

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
				  // Recibimos la info del nodo que se conecto
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
  }
  else if(tipoEstructura != D_STRUCT_NUMERO){ // Aca puse D_STRUCT_NUMERO porque no se que iria realmente
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
int almacenarArchivo(char* ruta, char* nombreArchivo, char* tipo){// Faltan argumentos

}

void enviarADataNode(char* data, int numeroBloque, int tam, int sizeBloque){ // Aca distribuimos el envio a los datanodes
	// Aca leemos la tabla de nodos para saber cual tiene mas nodos libres
	t_Nodos* metadataNodos = leerMetadataNodos(tablaNodos);

	//Guardamos copia 0

	//Guardamos copia 1

}

t_Nodos* leerMetadataNodos(char* archivoNodos){

	t_Nodos* metadata = malloc(sizeof(t_Nodos));
    t_config* metadataNodos;

    if(!verificarMetadataNodos(metadataNodos))
    log_error(logger,"TABLA DE NODOS NO VALIDA");

    metadata->tamanio_total =  config_get_int_value(metadataNodos, "TAMANIO");
    metadata->libre_total = config_get_int_value(metadataNodos, "LIBRE");
    metadata->nodos = config_get_array_value(metadataNodos,"NODOS");

    nodo* infoNodo = malloc(sizeof(nodo));

    int i;
    for(i = 0; i<metadata->nodos->elements_count; i++){ // list_get y asignar o hacer strcpy del get?
    	 infoNodo->nombreNodo = list_get(metadata->nodos,i);

    	 char* bloquesTotalesNodo = string_new(); // Hacer de otra forma mas eficiente
    	 string_append(&bloquesTotalesNodo,infoNodo->nombreNodo);
    	 string_append(&bloquesTotalesNodo,"Total");

    	 char* bloquesLibresNodo = string_new(); // Hacer de otra forma mas eficiente
    	 string_append(&bloquesLibresNodo,infoNodo->nombreNodo);
    	 string_append(&bloquesLibresNodo,"Libre");

    	 infoNodo->tamTotal = config_get_int_value(metadataNodos, bloquesTotalesNodo);
    	 infoNodo->tamLibre = config_get_int_value(metadataNodos, bloquesLibresNodo);

    	 list_add(metadata->nodos,infoNodo);
    }

    return metadata;
}

bool verificarMetadataNodos(t_config* metadataNodos){
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

int conseguirTamanioArchivo(char* ruta){

	struct stat st;
	stat(ruta, &st);
	return st.st_size;
}
