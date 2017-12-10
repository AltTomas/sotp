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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void commandHandler(){

	//todo: Tener cuidado con autocompletar, porque inserta un " " al final por lo que no tomaria bien los path

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
		case 1:{ // FORMAT - Testeado
			puts("formatear FS");
			formatear();
		}
		break;
		case 2:{ // RM archivo - pasarle cosas en formato de directorios contenidos en yamaFS. PE: metadata/archivos/1/asd.asd
			char* pathArchivo = readline("Ingrese path del archivo que se quiera borrar en yamaFS: ");

			// Si borramos un archivo deberiamos avisarle a cada nodo que tiene sus bloques que los libere y borrar el metadata

			char** pathDividido = string_split(pathArchivo, "/");

			int cantidadSubdirectorios = obtenerCantidadElementos(pathDividido);

			// Faltaria una condicion mas, si hay 2 archivos con = nombre cagamos

			// Posible solucion: a t_info_archivo agregar un campo int que sea el indice que cuando
			//se crea el archivo se copia el indice
			bool buscarArchivo(t_info_archivo* archivo){

			return (strcmp(archivo->nombreArchivo,pathDividido[cantidadSubdirectorios-2]) == 0);// -1 seria el null?
			}

			t_info_archivo* metadataArchivo = list_find(archivos,(void*)buscarArchivo);

			int i;
			for(i = 0; i < metadataArchivo->infoBloques->elements_count; i++){

				t_info_bloque_archivo* bloque = list_get(metadataArchivo->infoBloques,i);

				// Si no encuentra los sockets significa que se desconecto
				int socketNodoCopia0 = buscarSocketNodo(bloque->copia0Nodo);
				int socketNodoCopia1 = buscarSocketNodo(bloque->copia1Nodo);

				t_struct_numero* eliminarBloque = malloc(sizeof(t_struct_numero));
				eliminarBloque->numero = FS_DATANODE_ELIMINAR_BLOQUE;

				socket_enviar(socketNodoCopia0, D_STRUCT_NUMERO, eliminarBloque);
				socket_enviar(socketNodoCopia1, D_STRUCT_NUMERO, eliminarBloque);

				// TODO: Manejar Desconexion

				free(eliminarBloque);

				t_struct_numero* bloqueCopia0 = malloc(sizeof(t_struct_numero));
				bloqueCopia0->numero = bloque->copia0NodoBloque;

				t_struct_numero* bloqueCopia1 = malloc(sizeof(t_struct_numero));
				bloqueCopia1->numero = bloque->copia1NodoBloque;

				socket_enviar(socketNodoCopia0, D_STRUCT_NUMERO, bloqueCopia0);
				socket_enviar(socketNodoCopia1, D_STRUCT_NUMERO, bloqueCopia1);

				free(bloqueCopia0);
				free(bloqueCopia1);

				// Manejar Desconexion

			}

			// Una vez que todos los nodos borraron sus bloques pertenecientes al archivo

			char* comandoBorrarArchivo = string_new();
			string_append(&comandoBorrarArchivo, "rm ");
			string_append(&comandoBorrarArchivo, "../../yamaFS/");
			string_append(&comandoBorrarArchivo, pathArchivo);

			system(comandoBorrarArchivo);

		}
		break;
		case 3: // RENAME
			puts("Ingresar path original y nombre final.");
			char* pathOriginalARenombrar = readline("Ingrese path del archivo o carpeta que se quiera renombrar en yamaFS: ");
			char* pathRenombrado = readline("Ingrese pathFinal del archivo o carpeta que se quiera renombrar en yamaFS: ");

			renombrar(pathOriginalARenombrar,pathRenombrado);

			break;

		case 4: // MV
			puts("Ingresar path original y path final");
			char* pathOriginalAMover = readline("Ingrese path del archivo o carpeta que se quiera mover en yamaFS: ");
			char* pathMovido = readline("Ingrese path del archivo o carpeta que se quiera mover en yamaFS: ");

			mv (pathOriginalAMover, pathMovido);

			break;

		case 5: // CAT
			puts("Ingrese path el archivo");
			char* pathCAT = readline("Ingrese path del archivo o carpeta que se quiera mover en yamaFS: ");

			cat (pathCAT);

			break;

		case 6:{ //MKDIR

			char* directorioACrear = readline("Ingrese el path del directorio a crear: ");

			log_info(logger,"Obteniendo indice de la  ruta: %s",directorioACrear);

			char** subdirectorios = string_split(directorioACrear, "/");
			int i;
			int resultado = 0;

			int cantidadSubdirectorios = obtenerCantidadElementos(subdirectorios);

			int indicePadre;

			for(i = 0; i < cantidadSubdirectorios; i++){

				if(i == 0){ // Directorio con root como padre

					t_directory* directorioPadre = verificarExistenciaDirectorio(subdirectorios[i], 0); // Tiene a root como padre

					if(directorioPadre == NULL){ // No lo encontro
						directorioPadre = crearDirectorio(subdirectorios[i], 0);
					}

					indicePadre = directorioPadre->index;
				}
				else{
					t_directory* directorioExistente = verificarExistenciaDirectorio(subdirectorios[i], indicePadre);

					if( (directorioExistente != NULL) && (i == cantidadSubdirectorios - 1)){ // Encontro al ultimo -> siempre existio
						resultado = 1;
					}
					else{
					if(directorioExistente == NULL){ // No lo encontro
						directorioExistente = crearDirectorio(subdirectorios[i], indicePadre);

						if(directorioExistente == NULL){ // No se pudo crear
							resultado = 0;
						}
					}

					indicePadre = directorioExistente->index;
					}
				}
			}

			if(resultado == 1){
				puts("El directorio a crear ya existia");
			}
			else if (resultado == 2){
				puts("El directorio se creo exitosamente");
			}
			else{
				puts("No se puedo crear el directorio");
			}
		}

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

			int resultado = almacenarArchivo(pathArchivo, pathYAMAFS, tipo);// Faltan argumentos?

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

		case 8:{ // CPTO
			char* pathYAMAFS = readline("Ingrese el path del archivo en yamaFS: ");
			char* pathLocal = readline("Ingrese el path que tendria el archivo en el FS local: ");

			//Con el path de yamaFS leemos el archivo (obtenemos su contenido completo)

			char* contenidoArchivo = leer(pathYAMAFS);

			char* comandoCopiar = string_from_format ("echo %s", contenidoArchivo);

			string_append(&comandoCopiar, " >");
			string_append(&comandoCopiar, pathLocal);


			//Creamos un archivo en la ruta desginada del FS local con el contenido
			system(comandoCopiar);


		}
		break;

		case 9:{ // CPBLOCK

			char* pathArchivo = readline("Ingrese path del archivo en yamaFS: ");
			char* numeroBloque = readline("Ingrese el bloque a ser copiado: ");
			char* idNodo = readline("Ingrese el nombre del nodo en el que se va a copiar el bloque: ");

			t_info_bloque_archivo* bloque;

			// Buscar el archivo

			char* nombreArchivo = sacarNombreArchivoDelPath(pathArchivo);

			int posicion = buscarArchivo(nombreArchivo);

			t_info_archivo* archivo = list_get(archivos,posicion);

			// Ver si existe el bloque en cuestion

			bool confirmarBloque(t_info_bloque_archivo* bloquePedido){
				return bloquePedido->bloqueArchivo == atoi(numeroBloque);
			}

			bloque = list_find(archivo->infoBloques, (void*)confirmarBloque);


			if (bloque == NULL){ // NO existe el bloque
				puts("No se encontro el bloque pedido - No se puede realizar el CPBLOCK");
			}
			else{

				//Pedimos contenido a nodo

				int socketNodoAlmacenando = buscarSocketNodo(bloque->copia0Nodo);

				t_struct_numero* enviado = malloc(sizeof(t_struct_numero));

				enviado->numero = FS_DATANODE_PEDIDO_CONTENIDO_BLOQUE;

				socket_enviar(socketNodoAlmacenando,D_STRUCT_NUMERO,enviado);

				enviado->numero = atoi(numeroBloque);

				socket_enviar(socketNodoAlmacenando,D_STRUCT_NUMERO,enviado);

				// Obtener contenido del bloque (habaria que pedirselo al nodo que lo tenga)

				void* estructuraRecibida;
				t_tipoEstructura tipoEstructura;

				int recepcion = socket_recibir(socketNodoAlmacenando, &tipoEstructura,&estructuraRecibida);

				 if(recepcion == -1){
				    printf("Se desconecto el Nodo en el socket %d\n", socketNodoAlmacenando);
				    log_info(logger,"Se desconecto el Nodo en el socket %d", socketNodoAlmacenando);
				    close(socketNodoAlmacenando);
				    FD_CLR(socketNodoAlmacenando, &datanode);
				    FD_CLR(socketNodoAlmacenando, &setDataNodes);

				    //Actualizar tabla de nodos - Borrar
				    actualizarTablaNodosBorrar(socketNodoAlmacenando);
				  }
				  else if(tipoEstructura != D_STRUCT_STRING){
				    puts("Error en la serializacion");
				    log_info(logger,"Error en la serializacion");
				  }
				  else{
				    log_info(logger,"El nodo en el socket %d mando el contenido del bloque pedido", socketNodoAlmacenando);

				    // Enviarle contenido a nodo nuevo para guardar

				    int socketNodoCopiar = buscarSocketNodo(idNodo);

				    t_pedido_almacenar_bloque* datosBloque = malloc(sizeof(t_pedido_almacenar_bloque));

				    datosBloque->contenidoBloque = malloc(strlen(((t_struct_string*)estructuraRecibida)->string)+1);
				    strcpy(datosBloque->contenidoBloque, ((t_struct_string*)estructuraRecibida)->string);
				    datosBloque->bloqueArchivo = atoi(numeroBloque);
				    datosBloque->bytesOcupados = bloque->bytesOcupados;

				    // todo: Ver que onda esto, podemos hacer que se ingrese por parametro si se lo quiere meter de copia 0/1
				    int resultado = guardarCopia(archivo, socketNodoCopiar, datosBloque, 0);
				    if(resultado){
				    	printf("Se guardo el bloque %s del archivo %s en el nodo de nombre: %s\n", numeroBloque, pathArchivo, idNodo);
				    }
				    else{
				    	printf("ERROR: No se pudo guardar el bloque %s del archivo %s en el nodo de nombre: %s\n", numeroBloque, pathArchivo, idNodo);
				    }
				  }
			}

		}
			break;
		case 10: // MD5
			puts("Ingrese el path del archivo en el YAMAFS");
			char* pathMD5 = readline("Ingrese path del archivo para realizar su md5: ");

			MD5 (pathMD5);

			break;

		case 11:// LS
			puts("Ingrese el path de la carpeta en el YAMAFS");
			char* pathls = readline("Ingrese path de una carpeta para realizar su ls: ");

			ls (pathls);

			break;
		case 12:// INFO
			puts("Ingrese el path del archivo en el YAMAFS");
			char* pathinfo = readline("Ingrese path de un archivo para realizar su info: ");

			info (pathinfo);

			break;
		case 13: // HELP
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

		case 14:{ // RM -D

			char* pathDirectorio = readline("Ingrese path del directorio a borrar en yamaFS: ");

			char* comandoBorrarDirectorio = string_new();
			string_append(&comandoBorrarDirectorio,"rm -d  ../../yamaFS/");
			string_append(&comandoBorrarDirectorio,pathDirectorio);

			int resultado = system(comandoBorrarDirectorio);
			//TODO: Si el directorio no esta vacio muestra el prompt de rm de linux, tendremos que de alguna manera catchearlo?
		}
		break;

		case 15:{// RM - B
			char* pathArchivo = readline("ingrese el path del archivo");
			char* numeroBloque = readline("ingrese el numero de bloque bloque");
			char* numeroCopia = readline("ingrese el numero de copia");

			char* nombreArchivo;

			if(atoi(numeroCopia) > 0){
				t_config* metadataArchivo = config_create(pathArchivo);

				if(metadataArchivo == NULL){
						printf("ERROR: No se pudo encontrar el archivo %s", pathArchivo);

				}

				char** rutaDividida = string_split(pathArchivo, "/");
				int cantidadRutas = obtenerCantidadElementos(rutaDividida);

				char* rutaSinArchivo = string_new();
				int i;
				for(i = 0; i < (cantidadRutas - 1); i++){ // No consideramos el NULL
					if(i != cantidadRutas - 2){
						string_append(&rutaSinArchivo,rutaDividida[i]);
					}
					else if(i == cantidadRutas - 2){
						nombreArchivo = (char*)malloc(strlen(rutaDividida[i])+1);
						strcpy(nombreArchivo,rutaDividida[i]);
					}
				}

				bool esArchivoBuscado(t_info_archivo* archivo){
					return((strcmp(archivo->pathArchivo,rutaSinArchivo)==0)&&(strcmp(archivo->nombreArchivo,nombreArchivo)==0));
				}

				t_info_archivo* archivoBuscado = list_find(archivos,(void*)esArchivoBuscado);

				if(archivoBuscado->copiasDisponibles > 1){
					bool esBloqueBuscado(t_info_bloque_archivo* bloque){
						return(bloque->bloqueArchivo == atoi(numeroBloque));
					}

					t_info_bloque_archivo* bloqueBuscado = list_find(archivoBuscado->infoBloques,(void*)esBloqueBuscado);

					if(atoi(numeroCopia) == 0){
						int socketNodoCopia0 = buscarSocketNodo(bloqueBuscado->copia0Nodo);

						t_struct_numero* eliminarBloque = malloc(sizeof(t_struct_numero));
						eliminarBloque->numero = FS_DATANODE_ELIMINAR_BLOQUE;

						socket_enviar(socketNodoCopia0,D_STRUCT_NUMERO,eliminarBloque);

						t_struct_numero* enviado = malloc(sizeof(t_struct_numero));
						enviado->numero = bloqueBuscado->copia0NodoBloque;

						socket_enviar(socketNodoCopia0,D_STRUCT_NUMERO,enviado);

						void* estructuraRecibida;
						t_tipoEstructura tipoEstructura;

						int recepcion = socket_recibir(socketNodoCopia0,&tipoEstructura,&estructuraRecibida);

						if(recepcion == -1){
							printf("Se desconecto el Nodo en el socket %d\n", socketNodoCopia0);
							log_info(logger,"Se desconecto el Nodo en el socket %d", socketNodoCopia0);
							close(socketNodoCopia0);
							FD_CLR(socketNodoCopia0, &datanode);
							FD_CLR(socketNodoCopia0, &setDataNodes);

							//Actualizar tabla de nodos - Borrar
							actualizarTablaNodosBorrar(socketNodoCopia0);

							puts("El nodo que contenia el bloque a borrar se desconecto");
						}

						else if(tipoEstructura != D_STRUCT_NUMERO){
							puts("Hubo un error en la serializacion del mensaje del nodo que contenia el bloque a eliminar");
						}
						else{
							if(((t_struct_numero*)estructuraRecibida)->numero == 1){
								printf("La copia 0 del bloque %s del archivo %s se elimino exitosamente", numeroBloque, nombreArchivo);

								bloqueBuscado->copia0Nodo;
								bloqueBuscado->copia0NodoBloque = -1;
							}
							else{
								printf("No se pudo eliminar la copia 0 del bloque %s del archivo %s", numeroBloque, nombreArchivo);
							}

						}
					}
					else if(atoi(numeroCopia) == 1){

						int socketNodoCopia1 = buscarSocketNodo(bloqueBuscado->copia1Nodo);

						t_struct_numero* eliminarBloque = malloc(sizeof(t_struct_numero));
						eliminarBloque->numero = FS_DATANODE_ELIMINAR_BLOQUE;

						socket_enviar(socketNodoCopia1,D_STRUCT_NUMERO,eliminarBloque);

						t_struct_numero* enviado = malloc(sizeof(t_struct_numero));
						enviado->numero = bloqueBuscado->copia1NodoBloque;

						socket_enviar(socketNodoCopia1,D_STRUCT_NUMERO,enviado);

						void* estructuraRecibida;
						t_tipoEstructura tipoEstructura;

						int recepcion = socket_recibir(socketNodoCopia1,&tipoEstructura,&estructuraRecibida);

						if(recepcion == -1){
							printf("Se desconecto el Nodo en el socket %d\n", socketNodoCopia1);
							log_info(logger,"Se desconecto el Nodo en el socket %d", socketNodoCopia1);
							close(socketNodoCopia1);
							FD_CLR(socketNodoCopia1, &datanode);
							FD_CLR(socketNodoCopia1, &setDataNodes);

							//Actualizar tabla de nodos - Borrar
							actualizarTablaNodosBorrar(socketNodoCopia1);

							puts("El nodo que contenia el bloque a borrar se desconecto");
							}
						else if(tipoEstructura != D_STRUCT_NUMERO){
							puts("Hubo un error en la serializacion del mensaje del nodo que contenia el bloque a eliminar");
						}
						else{
							if(((t_struct_numero*)estructuraRecibida)->numero == 1){
								printf("La copia 1 del bloque %s del archivo %s se elimino exitosamente", numeroBloque, nombreArchivo);
							}
							else{
								printf("No se pudo eliminar la copia 1 del bloque %s del archivo %s", numeroBloque, nombreArchivo);
							}

						}

					}
					else{
						puts("Por el momento nada mas trabajamos con copia 0 y 1");
					}
				}
				else{
					printf("Solo queda una copia del bloque %s del archivo %s, no se puede eliminar", numeroBloque, nombreArchivo);
				}
			}
			else{
				puts("El numero de copia ingresado no existe porque es negativo");
			}
		}
		break;

		}// Llave switch

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
		t_info_archivo* archivo = element;
		return string_equals_ignore_case(archivo->pathArchivo, nombreFile);
	}
	t_info_archivo* archivoEncontrado = list_find(archivos, condition); //me devuelve el archivo, verificar si esta bien hecha

  int cantidadBloques = list_size(archivoEncontrado -> infoBloques);

  t_struct_numero* enviado = malloc(sizeof(t_struct_numero));
  enviado->numero = cantidadBloques;

  socket_enviar(socketConexionYAMA,D_STRUCT_NUMERO,enviado); //FS_YAMA_CANTIDAD_BLOQUES ya estaba definido del lado de funcionesYama

  int i;

  for(i=0;i<cantidadBloques;i++){

  bloque* bloque = list_get(archivoEncontrado->infoBloques,i);

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

/*int determinarEstado() {

	int cantidadArchivosAlmacenados = list_size(archivos);

	int i;

	for(i=0;i<cantidadArchivosAlmacenados;i++){

		t_info_archivo* archivoAlmacenado1 = list_get(archivos,i);

		int cantidadBloquesArchivosAlmacenados = list_size(archivoAlmacenado1->bloques);

		for(i=0;i<cantidadBloquesArchivosAlmacenados;i++){
			//comparo si alguna de las dos copias de ese bloque esta dentro de los Nodos conectados con el md5 con un IF
			//IF -> OK -> esta ese bloque en el Nodo Conectado

		}
		//IF se cumple todo, esta en estado estable
	}
}*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

char* leer (char* path){ //esta marcando error porque no esta hecho el char que devuelve

	//char* nombreArch = sacarNombreArchivoDelPath(path);
	//t_directory* directorioEncontrado = buscarDirectorio(path);

	char* contenidoTotal=string_new();

	void* estructuraRecibida;
	t_tipoEstructura tipoEstructura;

	t_config* archivoTablaArchivos = config_create(path);

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

		char** bloqueLectura= config_get_array_value(archivoTablaArchivos,bloque); //TIENE QUE SER CHAR**

		int socketNodo = buscarSocketNodo(bloqueLectura[0]);

		t_struct_numero* pedidoBloque = malloc(sizeof(t_struct_numero));
		pedidoBloque->numero = FS_DATANODE_PEDIDO_CONTENIDO_BLOQUE;

		socket_enviar(socketNodo, D_STRUCT_NUMERO,pedidoBloque);

		free(pedidoBloque);

		int recepcionOrden = socket_recibir(socketNodo, &tipoEstructura,&estructuraRecibida);

		if (recepcionOrden == -1){

						log_info(logger,"No se recibio la respuesta al pedido");

						close(socketNodo);
						FD_CLR(socketNodo, &datanode);
						FD_CLR(socketNodo, &setDataNodes);

						actualizarTablaNodosBorrar(socketNodo);
		}

		t_struct_numero* enviado = malloc(sizeof(t_struct_numero));
		enviado->numero = atoi(bloqueLectura[1]);

		socket_enviar(socketNodo, D_STRUCT_NUMERO,enviado); //[Nodo1, 33]


		int recepcion = socket_recibir(socketNodo, &tipoEstructura,&estructuraRecibida);

		if (recepcion == -1){

				log_info(logger,"No se recibio la copia0 del Nodo");

				close(socketNodo);
				FD_CLR(socketNodo, &datanode);
				FD_CLR(socketNodo, &setDataNodes);

				actualizarTablaNodosBorrar(socketNodo);


				char* bloqueCopia = "BLOQUE";

				string_append(&bloqueCopia, numero);

				string_append (&bloqueCopia,"COPIA1");

				char** bloqueCopiaLectura= config_get_array_value(archivoTablaArchivos,bloqueCopia);


				int socketNodoCopia = buscarSocketNodo(bloqueCopiaLectura[0]);


				t_struct_numero* pedidoBloqueCopia = malloc(sizeof(t_struct_numero));
				pedidoBloqueCopia->numero = FS_DATANODE_PEDIDO_CONTENIDO_BLOQUE;

				socket_enviar(socketNodoCopia, D_STRUCT_NUMERO,pedidoBloqueCopia);

				free(pedidoBloqueCopia);


				int recepcionOrdenCopia = socket_recibir(socketNodoCopia, &tipoEstructura,&estructuraRecibida);

				if (recepcionOrdenCopia == -1){

								log_info(logger,"No se recibio la respuesta al pedido");

								close(socketNodoCopia);
								FD_CLR(socketNodoCopia, &datanode);
								FD_CLR(socketNodoCopia, &setDataNodes);

								actualizarTablaNodosBorrar(socketNodoCopia);
				}


				t_struct_numero* enviadoCopia = malloc(sizeof(t_struct_numero));
				enviadoCopia->numero = atoi(bloqueLectura[1]);

				socket_enviar(socketNodo, D_STRUCT_NUMERO,enviadoCopia); //[Nodo1, 33]

				int recepcionCopia = socket_recibir(socketNodoCopia, &tipoEstructura,&estructuraRecibida);

				if (recepcionCopia == -1){
					log_info(logger,"No se recibio ni la copia0 ni la copia1 del Nodo");
					close(socketNodoCopia);
					FD_CLR(socketNodoCopia, &datanode);
					FD_CLR(socketNodoCopia, &setDataNodes);
					actualizarTablaNodosBorrar(socketNodoCopia);
				}
		}


	//verifico lo que recibio
				if(tipoEstructura != D_STRUCT_STRING){ //deberia recibir un char con el contenido del bloque
					puts("Error en la serializacion");
					log_error(logger,"Error en la serializacion");
					return 0;
				}

		string_append(&contenidoTotal,estructuraRecibida);
	}

	return contenidoTotal;

}

//////////////////// todo: Fijarse si esta funcion sigue siendo necesaria ////////////////////////////////////////

t_directory* buscarDirectorio(char* path){ //similar a buscarIndice pero devuelve directorio y no crea uno si no lo encuentra

	log_info(logger,"Obteniendo indice de la  ruta: %s",path);

	char** subdirectorios = string_split(path, "/");
	int i;
	t_directory* directorioExistente;

	int cantidadSubdirectorios = obtenerCantidadElementos(subdirectorios);

	int indicePadre;

	for(i = 0; i < cantidadSubdirectorios; i++){

		if(i == 0){ // Directorio con root como padre

			t_directory* directorioPadre = verificarExistenciaDirectorio(subdirectorios[i], 0); // Tiene a root como padre

			//
			if(directorioPadre == NULL){ // No lo encontro
				printf("No se encontro el directorio\n");
			}

			indicePadre = directorioPadre->index;
		}
		else{
			directorioExistente = verificarExistenciaDirectorio(subdirectorios[i], indicePadre);

			//
			if(directorioExistente == NULL){ // No lo encontro
				printf("No se encontro el directorio\n");

			}

			indicePadre = directorioExistente->index;
		}
	}

	char* padreChar = string_itoa(indicePadre);

	bool condition(void* element) {
		t_directory* directorio = element;
		return string_equals_ignore_case (string_itoa(directorio->index), padreChar); //chequear
	}

	t_directory* directorioEncontrado = list_find(directorios, condition); //seria directorios o habria que abrir el metadata directorios.dat
	return directorioEncontrado;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int almacenarArchivo(char* rutaDesde, char* rutaHacia, char* tipoArchivo){

if(verificarRuta(rutaHacia)) {
		log_error(logger,"La ruta hacia donde se almacenaria no es valida");
		return -1;
}

char** rutaDividida = string_split(rutaHacia, "/");
int cantidadRutas = obtenerCantidadElementos(rutaDividida);
char* rutaSinNombreArchivo = string_new();
char* rutaHaciaNombreArchivo;

int i;
for(i = 0; i < (cantidadRutas - 1); i++){ // No consideramos el NULL
	if(i != cantidadRutas - 2){
		string_append(&rutaSinNombreArchivo,rutaDividida[i]);
    }
    else if(i == cantidadRutas - 2){
		rutaHaciaNombreArchivo = malloc(strlen(rutaDividida[i])+1);
		strcpy(rutaHaciaNombreArchivo,rutaDividida[i]);
    }
}

int indiceDirectorio = pedidoLecturaRuta(rutaSinNombreArchivo);

if(indiceDirectorio < 0){
	log_error(logger,"No se pudo obtener el directorio para almacenar el archivo de la ruta %s", rutaHacia);
		return -1;
}

bool tieneIndice(t_directory* directorio){
	return directorio->index == indiceDirectorio;
}

t_directory* directorio = list_find(directorios, (void*)tieneIndice);

if(archivoExiste(directorio->index, rutaHaciaNombreArchivo)) {
	log_error(logger, "Ya existe un archivo con el nombre del que se quiere almacenar");
	return -1;
}

FILE* archivo = fopen(rutaDesde, "r");

if(archivo == NULL) {
	log_error(logger, "El archivo a copiar para el almacenamiento no existe");
	return -1;
}

log_info(logger, "Copiando archivo en yamaFS");
t_info_archivo* archivoCreado = malloc(sizeof(t_info_archivo));

archivoCreado->pathArchivo = malloc(strlen(rutaHacia)+1);
strcpy(archivoCreado->pathArchivo, rutaHacia);
archivoCreado->nombreArchivo = malloc(strlen(rutaHaciaNombreArchivo)+1);
strcpy(archivoCreado->nombreArchivo, rutaHaciaNombreArchivo);
archivoCreado->copiasDisponibles = 0;
archivoCreado->tamanio = conseguirTamanioArchivo(rutaDesde);
archivoCreado->indicePadre = directorio->index;
archivoCreado->tipo = malloc(strlen(tipoArchivo)+1);
strcpy(archivoCreado->tipo, tipoArchivo);
archivoCreado->infoBloques = list_create();

free(rutaHaciaNombreArchivo);
free(rutaDesde);

int estado = -1;

if(strcmp(archivoCreado->tipo, "Binario") == 0){
	estado = almacenarBinario(archivoCreado, archivo);
}
else{
	estado = almacenarTexto(archivoCreado, archivo);
}

fclose(archivo);
return estado;

}

int almacenarBinario(t_info_archivo* archivo, FILE* archivoACopiar){
	char* dataBloque = malloc(sizeBloque);
	size_t bytes;
	int resultado = 1;

	int numeroBloque;
	for(numeroBloque = 0; (bytes = fread(dataBloque, sizeof(char), sizeBloque, archivoACopiar)) == sizeBloque; numeroBloque++){

		resultado = enviarADataNode(archivo, dataBloque, numeroBloque, sizeBloque);
		if(resultado == -1){
			return resultado;
		}
	}
	return resultado;
}

int almacenarTexto(t_info_archivo* archivo, FILE* archivoACopiar){

	char* dataBloque = malloc(sizeBloque+1);
	char* datos = malloc(sizeBloque);
	int bytesDisponibles = sizeBloque;
	int offset = 0;
	int resultado = 1;
	int numeroBloque;

	ssize_t sizeDataBloque;
	size_t limite = sizeBloque;

	while((sizeDataBloque = getline(&dataBloque, &limite, archivoACopiar)) != -1){
		if(sizeDataBloque > sizeBloque){
			resultado = -1;
			log_error(logger,"El registro del archivo de texto es mas grande que 1 MB");
			return -1;
		}
		if(sizeDataBloque <= bytesDisponibles){

			memcpy(datos + offset, dataBloque, sizeDataBloque);
			bytesDisponibles -= sizeDataBloque;
			offset += sizeDataBloque;
		}
		else{

			resultado = enviarADataNode(archivo, datos, numeroBloque, sizeBloque - bytesDisponibles);

			if(resultado == -1){
				return -1;
			}

			free(datos);
			datos = malloc(sizeBloque);
			bytesDisponibles = sizeBloque;
			offset = 0;
			numeroBloque++;

			memcpy(datos+offset, dataBloque, sizeDataBloque);
			bytesDisponibles -= sizeDataBloque;
			offset += sizeDataBloque;
			}
		}

	if(resultado != -1 && bytesDisponibles < sizeBloque){
		resultado = enviarADataNode(archivo, datos, numeroBloque, sizeBloque - bytesDisponibles);
	}

	free(dataBloque);
	free(datos);
	return resultado;

}

int enviarADataNode(t_info_archivo* archivo, char* data, int numeroBloque, int bytesOcupados){ // Aca distribuimos el envio a los datanodes

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

	t_info_bloque_archivo* infoBloque = malloc(sizeof(t_info_bloque_archivo));

	infoBloque->bloqueArchivo = numeroBloque;
	infoBloque->bytesOcupados = bytesOcupados;

	list_add(archivo->infoBloques, infoBloque);

	int resultadoCopia0 = guardarCopia(archivo, socketNodoCopia0, enviado,0);
	int resultadoCopia1 = guardarCopia(archivo, socketNodoCopia1, enviado,1);
	free(enviado);

	if(resultadoCopia0 || resultadoCopia1){ // Se pudo guardar en alguna de las 2 copias

		return 1;

	list_add(archivos,archivo);
	}
	else{ // No se pudo guardar el archivo

		return 0;
	}
	return 0;
}

int guardarCopia(t_info_archivo* archivo, int socketNodo, t_pedido_almacenar_bloque* enviado, int copia){

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

			int indexDirectorio = pedidoLecturaRuta(archivo->pathArchivo);

			char* string_index = string_itoa(indexDirectorio);

			char* rutaMetadata = string_new();
			string_append(&rutaMetadata,tablaArchivos);
			string_append(&rutaMetadata,"/");
			string_append(&rutaMetadata,string_index);
			string_append(&rutaMetadata,"/");
			string_append(&rutaMetadata,archivo->nombreArchivo);

			t_config* metadataArchivo = config_create(rutaMetadata);

			if(metadataArchivo == NULL)
					log_error(logger,"ERROR: No se pudo encontrar el la metadata de archivo %s", rutaMetadata);
					printf("ERROR: No se pudo encontrar el la metadata de archivo %s", rutaMetadata);

			    if(!verificarMetadataNodos(metadataArchivo)){ // verificarMetadataArchivo
			    	log_error(logger,"Fallo en formato de metadata del archivo %s",rutaMetadata);
				   	printf("Fallo en formato de metadata del archivo %s",rutaMetadata);
				}
			    else{

			    }
			    //Agrego los valores de las keys del metadata

			    int posicionBloque = buscarBloqueDeArchivo(archivo, enviado->bloqueArchivo);

			    t_info_bloque_archivo* bloqueArchivo = list_remove(archivo->infoBloques,posicionBloque);

			    if(bloqueArchivo == NULL){
			    	printf("Error almacenando bloque %d del archivo %s", enviado->bloqueArchivo, archivo->pathArchivo);
			    	log_error(logger,"Error almacenando bloque %d del archivo %s", enviado->bloqueArchivo, archivo->pathArchivo);
			    }
			    else{
			    	char* string_bloque = string_itoa(enviado->bloqueArchivo);

			    	char* keyBloqueCopia0 = strdup("BLOQUE");
			    	string_append(&keyBloqueCopia0,string_bloque);
			    	char* keyBloqueCopia1 = strdup(keyBloqueCopia0);
			    	char* keyBytesOcupadosBloque = strdup(keyBloqueCopia0);
			    	string_append(&keyBloqueCopia0,"COPIA0");
			    	string_append(&keyBloqueCopia1,"COPIA1");
			    	string_append(&keyBytesOcupadosBloque,"BYTES");

			    	char* string_bytesOcupados = string_itoa(enviado->bytesOcupados);

			    	//Actualizo los valores de las keys del config
			    	config_set_value(metadataArchivo,keyBytesOcupadosBloque , string_bytesOcupados);

			    	if(copia == 0){
			    		bloqueArchivo->copia0Nodo =((t_almacenar_bloque*)estructuraRecibida)->nombreNodo;
			    		bloqueArchivo->copia0NodoBloque = ((t_almacenar_bloque*)estructuraRecibida)->bloqueNodo;

			    		char* string_nodo_bloque = string_itoa(bloqueArchivo->copia0NodoBloque);

			    		char* arrayBloques = string_new();
			    		string_append(&arrayBloques,"[");
			    		string_append(&arrayBloques, bloqueArchivo->copia0Nodo);
			    		string_append(&arrayBloques,",");
			    		string_append(&arrayBloques, string_nodo_bloque);
			    		string_append(&arrayBloques,"]");

			    		config_set_value(metadataArchivo, keyBloqueCopia0, arrayBloques);

			    		list_add(archivo->infoBloques,bloqueArchivo);
			    		archivo->copiasDisponibles += 1;
			    	}
			    	else{
			    		bloqueArchivo->copia1Nodo =((t_almacenar_bloque*)estructuraRecibida)->nombreNodo;
			    		bloqueArchivo->copia1NodoBloque = ((t_almacenar_bloque*)estructuraRecibida)->bloqueNodo;

			    		char* string_nodo_bloque = string_itoa(bloqueArchivo->copia1NodoBloque);

			    		char* arrayBloques = string_new();
			    		string_append(&arrayBloques,"[");
			    		string_append(&arrayBloques, bloqueArchivo->copia1Nodo);
			    		string_append(&arrayBloques,",");
			    		string_append(&arrayBloques, string_nodo_bloque);
			    		string_append(&arrayBloques,"]");

			    		config_set_value(metadataArchivo, keyBloqueCopia1, arrayBloques);

			    		list_add(archivo->infoBloques,bloqueArchivo);
			    		archivo->copiasDisponibles += 1;
			    	}

			    	config_save(metadataArchivo);
			    		log_info(logger,"La metadata del archivo %s se actualizo exitosamente", archivo->nombreArchivo);
			    }

			    	log_info(logger,"Se  almaceno el bloque %d en el DN de socket: %d", enviado->bloqueArchivo, socketNodo);
			    	return 1;
		}
	}

	puts("Llego a cualquier lado");
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

bool caracteresIguales (char caracter1, char caracter2){
	return caracter1 == caracter2;
}

bool rutaTieneRaiz(char* ruta) {
	char* raiz = string_substring_until(ruta, 8);
	int resultado = strcmp(raiz, "/yamafs:") == 0;
	free(raiz);
	return resultado;
}

bool rutaBarrasEstanSeparadas(char* ruta) {
	int i;
	for(i = 0; i < strlen(ruta); i++) {
		if(caracteresIguales(ruta[i], '/')) {
			if(i==0) {

				if(caracteresIguales(ruta[i+1], '/'))
					return false;
			}
			else {

				if(i==strlen(ruta)-1 && ruta[i-1] != ':'){
					return false;
				}

				else if (caracteresIguales(ruta[i-1], '/') || caracteresIguales(ruta[i+1], '/'))
						return false;
			}

		}
	}
	return true;
}

bool verificarRuta(char* ruta) {
	return rutaTieneRaiz(ruta) && rutaBarrasEstanSeparadas(ruta);
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

bool masBloquesLibres(t_info_nodo* nodo1, t_info_nodo* nodo2){
	return nodo1->bloquesLibres > nodo2->bloquesLibres;
}

/////////////////////////////////////////// FUNCIONES TABLA NODOS /////////////////////////////////////////////////

bool verificarMetadataNodos(t_config* metadataNodos){ // Si o si tiene que tener estos
	return config_has_property(metadataNodos,"TAMANIO") &&
			config_has_property(metadataNodos,"LIBRE") &&
			config_has_property(metadataNodos,"NODOS");
}

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

void crearRoot(){ //Si yo formateo el FS deberia ejecutarse esta funcion

	t_directory* root = malloc(sizeof(t_directory));

	root->index = 0;
	root->nombre = malloc(strlen("root")+1);
	strcpy(root->nombre, "root");
	root->indexPadre = -1;

	list_add(directorios,root);

	//Se persiste en directorios.dat
	actualizarMetadataDirectorios(root);

}

bool verificarMetadataDirectorios(t_config* metadataDirectorios){
	return config_has_property(metadataDirectorios,"INDEX") &&
			config_has_property(metadataDirectorios,"DIRECTORIO") &&
			config_has_property(metadataDirectorios,"PADRE");
}

t_list* leerTablaDeDirectorios(char* ruta){

	t_list* directorios = list_create();

	 t_config* metadataDirectorios = config_create(ruta);

	 if(metadataDirectorios == NULL)
	    	puts("ERROR");


	    if(!verificarMetadataDirectorios(metadataDirectorios)){
	    	puts("Fallo en formato de tabla de directorios");
	    }

	   char** indices =  config_get_array_value(metadataDirectorios, "INDEX");
	   char** nombres = config_get_array_value(metadataDirectorios, "DIRECTORIO");
	   char** padres = config_get_array_value(metadataDirectorios,"PADRE");

	   int i;
	   int cantidadDirectorios = obtenerCantidadElementos(indices);

	   if(cantidadDirectorios > 0){

		   for(i = 0; i < cantidadDirectorios; i ++){

			   t_directory* directorio = malloc(sizeof(t_directory));

			   directorio->index = atoi(indices[i]);
			   directorio->nombre = malloc(strlen(nombres[i])+1);
			   strcpy(directorio->nombre, nombres[i]);
			   directorio->indexPadre = atoi(padres[i]);

			   list_add(directorios,directorio);
		   }
	   }

	   return directorios;
}

t_directory* verificarExistenciaDirectorio(char* directorio, int indicePadre){

	log_info(logger,"Verificando existencia de: %s\n", directorio);

	directorios = leerTablaDeDirectorios(tablaDirectorios);

	bool estaEnLista(t_directory* elemento){
		return (strcmp(elemento->nombre,directorio) == 0 && (elemento->indexPadre == indicePadre) );
	}

	t_directory* directorioEncontrado = list_find(directorios,(void*)estaEnLista);

	if(directorioEncontrado == NULL){
		printf("El directorio %s no existe, se procede a crearlo\n", directorio);
		log_info(logger,"El directorio %s no existe, se procede a crearlo", directorio);
	}
	else{
		log_info(logger,"El directorio %s con padre en indice %d existe!\n", directorio, indicePadre);
		log_info(logger,"Indice directorio: %d\n", directorioEncontrado->index);
		log_info(logger,"Nombre directorio: %s\n", directorioEncontrado->nombre);
		log_info(logger,"Indice Padre: %d\n", directorioEncontrado->indexPadre);
	}

	return directorioEncontrado;
}

t_directory* buscarPorIndice(int indice){

	bool esIndice(t_directory* elemento){
		return elemento->index == indice;
	}

	t_directory* directorioBuscado = list_find(directorios,(void*)esIndice);

	return directorioBuscado;
}

bool estaVacio(t_directory* directorioPadre){

	bool tieneHijos(t_directory* directorio){
		return (directorio->indexPadre == directorioPadre->index);
	}

	bool cumple = list_any_satisfy(directorios, (void*)tieneHijos);

	return(!cumple);
}

void eliminarDirectorioVacio(t_directory* directorioAEliminar) {

	bool indiceBuscado(t_directory* directorio) {
		return directorioAEliminar->index == directorio->index;
	}
	list_remove_and_destroy_by_condition(directorios,(void*) indiceBuscado, (void*) liberarDirectorio);
}

t_directory* buscarPorPadre(int indicePadre){

	bool esHijo(t_directory* elemento){
		return elemento->indexPadre == indicePadre;
	}

	t_directory* directorioHijo = list_find(directorios,(void*)esHijo);

	return directorioHijo;
}

t_directory* traerSubdirectorio(t_directory* directorio) {
	return buscarPorPadre(directorio->index);
}

bool tieneSubdirectorio(t_directory* directorio) {
	return traerSubdirectorio(directorio) != NULL;
}

void eliminarDirectorio(t_directory* directorioAEliminar) {

	t_directory* directorio = directorioAEliminar;

	void eliminarDirRecursivamente(t_directory* directorio) {

		if (estaVacio(directorio) && directorio != directorioAEliminar) {

			t_directory *directorioPadre = buscarPorIndice(directorio->indexPadre);

			eliminarDirectorioVacio(directorio);

			eliminarDirRecursivamente(directorioPadre);

		}
		 else if (tieneSubdirectorio(directorio)) {

			 t_directory* subdirectorio = traerSubdirectorio(directorio);

			eliminarDirRecursivamente(subdirectorio);

		}
		else {	//esta vacio y es el directorio a eliminar

			eliminarDirectorioVacio(directorioAEliminar);

			printf("Se elimino todo el directorio con su contenido\n");
		}

	}
	eliminarDirRecursivamente(directorio);
}

int borrarDirectorio(char* ruta){ // todo: Se deberia poder borrar el root?

	char** subdirectorios = string_split(ruta, "/");

	int i;
	int cantidadSubdirectorios = obtenerCantidadElementos(subdirectorios);

	int indicePadre;

	t_directory* directorioABorrar;

	for(i = 0; i < cantidadSubdirectorios; i++){

		if(cantidadSubdirectorios == 1){ // Directorio con root como padre

			log_info(logger,"La ruta a eliminar (%s) es hijo de root y no tiene hijos", ruta);

			directorioABorrar = verificarExistenciaDirectorio(subdirectorios[i], 0);
		}
		else{ // Es un directorio mas alla de algun hijo de root

			if(i == 0){ // Directorio con root como padre

				t_directory* directorioPadre = verificarExistenciaDirectorio(subdirectorios[i], 0); // Tiene a root como padre

				indicePadre = directorioPadre->index;
			}
			else{ // Directorios que le siguen

				directorioABorrar = verificarExistenciaDirectorio(subdirectorios[i], indicePadre);

				indicePadre = directorioABorrar->index;
			}
		}
	}

	eliminarDirectorio(directorioABorrar);

	// Borramos del metadata todos los que estan en la lista

	t_config* metadataDirectorios = config_create(tablaDirectorios);

	if(metadataDirectorios == NULL)
		puts("ERROR");

	if(!verificarMetadataDirectorios(metadataDirectorios)){
		puts("Fallo en formato de tabla de directorios");
	}

	int cantidadFinalDirectorios = directorios->elements_count;

	////////////////////////////// INDICES ///////////////////////////////////////////////
	char** copiaArrayIndices = malloc(sizeof(char*) * (cantidadFinalDirectorios));

	for(i = 0; i < cantidadFinalDirectorios; i++){

		t_directory* directorio = list_get(directorios,i);

		char* string_indice = string_itoa(directorio->index);

		copiaArrayIndices[i]=(char*)malloc(strlen(string_indice)+1);
		strcpy(copiaArrayIndices[i],string_indice);

	}

	// Le damos formato al array asi modificamos el archivo de metadata

	char* arrayIndices = string_new();
	string_append(&arrayIndices,"[");

	for(i = 0; i < cantidadFinalDirectorios; i++){
		if( i == cantidadFinalDirectorios -1){
			string_append(&arrayIndices,copiaArrayIndices[i]);
		}
		else{
			string_append(&arrayIndices,copiaArrayIndices[i]);
			string_append(&arrayIndices,",");
		}

	}
	string_append(&arrayIndices,"]");

	////////////////////////////// DIRECTORIOS ///////////////////////////////////////////////
	char** copiaArrayNombres = (char*)malloc(sizeof(char*) * (cantidadFinalDirectorios));

	for(i = 0; i < cantidadFinalDirectorios; i++){

		t_directory* directorio = list_get(directorios,i);

		copiaArrayNombres[i]=(char*)malloc(strlen(directorio->nombre)+1);
		strcpy(copiaArrayNombres[i],directorio->nombre);

	}

	char* arrayNombres = string_new();
	string_append(&arrayNombres,"[");

	for(i = 0; i < cantidadFinalDirectorios; i++){
		if( i == cantidadFinalDirectorios -1){
			string_append(&arrayNombres,copiaArrayNombres[i]);
		}
		else{
			string_append(&arrayNombres,copiaArrayNombres[i]);
			string_append(&arrayNombres,",");
		}

	}
	string_append(&arrayNombres,"]");

	////////////////////////////// PADRES ///////////////////////////////////////////////
	char** copiaArrayPadres = (char*)malloc(sizeof(char*) * (cantidadFinalDirectorios));

	for(i = 0; i < cantidadFinalDirectorios; i++){

		t_directory* directorio = list_get(directorios,i);

		char* string_padre = string_itoa(directorio->indexPadre);

		copiaArrayPadres[i]=(char*)malloc(strlen(string_padre)+1);
		strcpy(copiaArrayPadres[i],string_padre);
	}

	char* arrayPadres = string_new();
	string_append(&arrayPadres,"[");

	for(i = 0; i < cantidadFinalDirectorios; i++){
		if( i == cantidadFinalDirectorios -1){
			string_append(&arrayPadres,copiaArrayPadres[i]);
		}
		else{
			string_append(&arrayPadres,copiaArrayPadres[i]);
			string_append(&arrayPadres,",");
		}

	}

	string_append(&arrayPadres,"]");

	//Actualizo los valores de las keys del config
	config_set_value(metadataDirectorios, "INDEX", arrayIndices);
	config_set_value(metadataDirectorios, "DIRECTORIO", arrayNombres);
	config_set_value(metadataDirectorios, "PADRE", arrayPadres);

	config_save(metadataDirectorios);
	log_info(logger,"Guardamos cambios en tabla de directorios - BORRADO");

	return 1;
}

void actualizarMetadataDirectorios(t_directory* directorio){

	t_config* metadataDirectorios = config_create(tablaDirectorios);

		if(metadataDirectorios == NULL)
			puts("ERROR: No se pudo encontrar el archivo directorios.dat");

	    if(!verificarMetadataDirectorios(metadataDirectorios)){
		   	puts("Fallo en formato de tabla de directorios");
		}
	    else{

	    	char** indices =  config_get_array_value(metadataDirectorios, "INDEX");
	    	char** nombres = config_get_array_value(metadataDirectorios, "DIRECTORIO");
	    	char** padres = config_get_array_value(metadataDirectorios,"PADRE");

	    	int i;
	    	int cantidadDirectorios = obtenerCantidadElementos(indices);

	    	char** nuevoArrayIndices = (char*)malloc(sizeof(char*) * (cantidadDirectorios+1));

	    	for(i = 0; i < cantidadDirectorios; i++){

	    		nuevoArrayIndices[i]=(char*)malloc(strlen(indices[i])+1);
	    		strcpy(nuevoArrayIndices[i],indices[i]);
	    	}

	    	char** nuevoArrayNombres = (char*)malloc(sizeof(char*) * (cantidadDirectorios+1));

	    	for(i = 0; i < cantidadDirectorios; i++){

	    		nuevoArrayNombres[i]=(char*)malloc(strlen(nombres[i])+1);
	    		 strcpy(nuevoArrayNombres[i],nombres[i]);
	    	}

	    	char** nuevoArrayPadres = (char*)malloc(sizeof(char*) * (cantidadDirectorios+1));

	    	for(i = 0; i < cantidadDirectorios; i++){

	    		nuevoArrayPadres[i]=(char*)malloc(strlen(padres[i])+1);
	    		 strcpy(nuevoArrayPadres[i],padres[i]);
	    	}

	    char* string_indice = string_itoa(directorio->index);
	    char* string_indicePadre = string_itoa(directorio->indexPadre);

	    int cantidadDirectoriosArrayNuevo  = cantidadDirectorios + 1;

	    /////////////////////////////////////////////// INDICES ///////////////////////////////////////////////
	    char* arrayIndices = string_new();
	    string_append(&arrayIndices,"[");

	    for(i = 0; i < cantidadDirectoriosArrayNuevo; i++){
	    	if( i == cantidadDirectoriosArrayNuevo -1){
	    	string_append(&arrayIndices,string_indice);
	    	}
	    	else{
	    		string_append(&arrayIndices,nuevoArrayIndices[i]);
	    		string_append(&arrayIndices,",");
	    	}
	    }
	    string_append(&arrayIndices,"]");

	    /////////////////////////////////////////////// NOMBRES ///////////////////////////////////////////////

	    char* arrayNombres = string_new();
	    string_append(&arrayNombres,"[");

	    for(i = 0; i < cantidadDirectoriosArrayNuevo; i++){
	    	if( i == cantidadDirectoriosArrayNuevo -1){
	    	string_append(&arrayNombres, directorio->nombre);
	    	}
	    	else{
	    		string_append(&arrayNombres,nuevoArrayNombres[i]);
	    		string_append(&arrayNombres,",");
	    	}
	    }
	    string_append(&arrayNombres,"]");

	    /////////////////////////////////////////////// PADRES ///////////////////////////////////////////////

	    char* arrayPadres = string_new();
	    string_append(&arrayPadres,"[");

	    for(i = 0; i < cantidadDirectoriosArrayNuevo; i++){
	    	if( i == cantidadDirectoriosArrayNuevo -1){
	    	string_append(&arrayPadres,string_indicePadre);
	    	}
	    	else{
	    		string_append(&arrayPadres,nuevoArrayPadres[i]);
	    		string_append(&arrayPadres,",");
	    	}
	    }
	    string_append(&arrayPadres,"]");

	    config_set_value(metadataDirectorios, "INDEX", arrayIndices);
	    config_set_value(metadataDirectorios, "DIRECTORIO", arrayNombres);
	    config_set_value(metadataDirectorios, "PADRE", arrayPadres);


	      config_save(metadataDirectorios);
	      log_info(logger,"Guardamos cambios en tabla de directorios - ACTUALIZACION");

	    }
}

t_directory* crearDirectorio(char* directorio, int indicePadre){

	log_info(logger,"Se quiere crear el directorio %s", directorio);

	t_directory* directorioNuevo = malloc(sizeof(t_directory));

	if(directorios->elements_count <=100){

		bool mayorIndice(t_directory* directorio1, t_directory* directorio2){
			return directorio1->index > directorio2->index;
		}

		t_list* directoriosOrdenadosPorIndice = list_create();
		list_add_all(directoriosOrdenadosPorIndice,directorios);
		list_sort(directoriosOrdenadosPorIndice, mayorIndice); // Tira warning pero funciona

		int i;

		t_directory* directorioMayorIndice = list_get(directoriosOrdenadosPorIndice,0);

		directorioNuevo->index = directorioMayorIndice->index + 1;

		free(directorioMayorIndice);

		for(i = 0; i > directoriosOrdenadosPorIndice->elements_count; i++){
			t_directory* directorioCopia = list_remove(directoriosOrdenadosPorIndice, i);
			free(directorioCopia);
		}

		free(directoriosOrdenadosPorIndice);

		directorioNuevo->nombre = malloc(strlen(directorio)+1);
		strcpy(directorioNuevo->nombre, directorio);
		directorioNuevo->indexPadre = indicePadre;

		list_add(directorios, directorioNuevo);

		actualizarMetadataDirectorios(directorioNuevo);

		return directorioNuevo;
	}
	return NULL;
}

int buscarIndices(char* ruta){

	log_info(logger,"Obteniendo indice de la  ruta: %s",ruta);

	char** subdirectorios = string_split(ruta, "/");
	int i;
	t_directory* directorioExistente;

	int cantidadSubdirectorios = obtenerCantidadElementos(subdirectorios);

	int indicePadre;

	for(i = 0; i < cantidadSubdirectorios; i++){

		if(i == 0){ // Directorio con root como padre

			t_directory* directorioPadre = verificarExistenciaDirectorio(subdirectorios[i], 0); // Tiene a root como padre

			//
			if(directorioPadre == NULL){ // No lo encontro
				directorioPadre = crearDirectorio(subdirectorios[i], 0);
			}
			// MANEJAR ERROR NO EXISTE

			indicePadre = directorioPadre->index;
		}
		else{
			directorioExistente = verificarExistenciaDirectorio(subdirectorios[i], indicePadre);

			//
			if(directorioExistente == NULL){ // No lo encontro
				directorioExistente = crearDirectorio(subdirectorios[i], indicePadre);

				if(directorioExistente == NULL){ // No se pudo crear
					return -1;
				}
			}
			//

			indicePadre = directorioExistente->index;
		}
	}
	return directorioExistente->index;
}

int pedidoLecturaRuta(char* ruta){

	log_info(logger, "Trabajando pedido de la ruta %s",ruta);

	int resultado = buscarIndices(ruta);

	if(resultado >= 0){
		log_info(logger, "Se pudo acceder a los directorios de la ruta %s", ruta);
		return resultado;
	}
	else{
		log_error(logger, "Hubo con el acceso a los directorios de la ruta %s", ruta);
		return -1;
	}
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
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void renombrar(char* path_original, char* path_finalCompleto){//ejemplo renombrar(lalala/so/2/tp.txt, lalala/so/2/op.txt)

	t_directory* ultimoSubdirectorio = buscarDirectorio(path_original);
	t_directory* ultimoSubdirectorioFinal = buscarDirectorio(path_finalCompleto);

	char* path_final = ultimoSubdirectorioFinal->nombre; //deberia ser op.txt


	if(verificarSiEsArchivoODirectorio(path_original)!=0){

		int posicion = ultimoSubdirectorio->index;
		char* indiceChar = string_itoa (posicion);
		char* rutaArchivo = string_new();
		string_append (&rutaArchivo,tablaArchivos);
		string_append (&rutaArchivo,"/");
		string_append (&rutaArchivo,indiceChar);
		string_append (&rutaArchivo,"/");
		string_append (&rutaArchivo, path_final);

				if(verificarExistenciaNombreArchivo(rutaArchivo)!=0){
					char* mvChar = string_new();
					string_append (&mvChar,"mv "); //necesita directorio completo original y directorio completo final
					string_append (&mvChar,path_original);
					string_append (&mvChar," "); //espacio para separar ambos paths
					string_append (&mvChar,path_finalCompleto);
					system(mvChar);
				}else{
					log_error(logger,"Ya existe ese nombre de Archivo");
				}

	}else{
	//char* nombreUltimoSubdirectorio = ultimoSubdirectorio -> nombre;
	//int padreUltimoSubdirectorio = ultimoSubdirectorio -> indexPadre;

	if (verificarExistenciaNombreDirectorio(ultimoSubdirectorio)==1){ //si es 1 no existe el mismo nombre con ese padre
		free(ultimoSubdirectorio->nombre);
		ultimoSubdirectorio->nombre = malloc(strlen(path_final) + 1);
		strcpy(ultimoSubdirectorio->nombre, path_final);
		printf("Directorio fue renombrado correctamente\n");
	}else{
		log_error(logger,"Ya existe ese nombre de Directorio");
	}
	}
}



int verificarSiEsArchivoODirectorio(char* path){
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISDIR(path_stat.st_mode);// si es !=0 es un archivo
} //podria usarse un fopen, si abre es archivo y sino directorio, o con string_contains que se fijaria el "." para que sea archivo



int verificarExistenciaNombreDirectorio(t_directory* directorioAVerificar){ //podria usar un strcmp

	char* nombreDirectorio=directorioAVerificar->nombre;
	int padreDirectorio=directorioAVerificar->indexPadre;


	int cantidadDirectorios = list_size(directorios);

	int indice;

	for(indice = 0; indice < cantidadDirectorios; indice++) {
			t_directory* directorioEncontrado = list_get(directorios, indice);

			if((directorioEncontrado->nombre == nombreDirectorio) && (directorioEncontrado->indexPadre == padreDirectorio)) {
				return 0; //devuelve 0 si ya existe
			}
			}

	return 1;
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

bool archivoExiste(int indiceDirectorioPadre, char* nombreArchivo) {

	bool existeNombre(t_info_archivo* archivo) {
		return (indiceDirectorioPadre == archivo->indicePadre && (strcmp(nombreArchivo, archivo->nombreArchivo) == 0));
	}

	bool resultado = list_any_satisfy(archivos, (void*)existeNombre);
	return resultado;
}

/////////////////////////////////////////////////////////


void MD5(char* path_archivo) { //chequear si es char* o void (por system)

	bool condition(void* element) {
		t_info_archivo* archivo = element;
			return string_equals_ignore_case(archivo->pathArchivo, path_archivo);
		}
	t_info_archivo* archivoEncontrado = list_find(archivos, condition);

	 if (archivoEncontrado!=NULL){
		 	char* md5Char = string_new();
		 	string_append (&md5Char,"md5sum ");
		 	string_append (&md5Char,path_archivo);
			system(md5Char); //para que me quede md5sum utn/hola.txt
			printf("\n");
	 } else {
		 log_error(logger,"error al realizar el md5");
	 }

}

void cat(char* path_archivo) { //chequear si es char* o void (por system)

	bool condition(void* element) {
		t_info_archivo* archivo = element;
			return string_equals_ignore_case(archivo->pathArchivo, path_archivo);
		}
	t_info_archivo* archivoEncontrado = list_find(archivos, condition);

	 if (archivoEncontrado!=NULL){
		 	char* catChar = string_new();
		 	string_append (&catChar, "cat ");
		 	string_append (&catChar,path_archivo);
			system(catChar); //para que me quede cat utn/hola.txt, me devolveria el contenido ejemplo: adsasdasd
			printf("\n");
	 } else {
		 log_error(logger,"error al realizar el cat");
	 }

}

void ls(char* path_carpeta) { //chequear si es char* o void (por system)
		 	char* lsChar = string_new();
		 	string_append (&lsChar,"ls ");
		 	string_append (&lsChar,path_carpeta);
			system(lsChar);
			printf("\n");
}

void info(char* path_archivo) { //chequear si es char* o void (por system)

	bool condition(void* element) {
		t_info_archivo* archivo = element;
			return string_equals_ignore_case(archivo->pathArchivo, path_archivo);
		}
	t_info_archivo* archivoEncontrado = list_find(archivos, condition);

	 if (archivoEncontrado!=NULL){
		 	int tamanioDelArchivo = archivoEncontrado->tamanio;
		 	char* tipoDelArchivo = archivoEncontrado->tipo;
		 	int cantidadBloques = list_size(archivoEncontrado -> infoBloques);

		 	 printf("El tamanio Del Archivo es %d\n", tamanioDelArchivo);
		 	 printf("El tipo Del Archivo del archivo es %s\n", tipoDelArchivo);
		 	 printf("La cantidad de bloques es %d\n", cantidadBloques);


		 	int i;
		 	for(i=0;i<cantidadBloques;i++){
		 	  bloque* bloque = list_get(archivoEncontrado->infoBloques,i);
		 	  int numeroBloque = bloque -> numBloque;
		 	  int fin = bloque -> finBloque;
		 	  char* nodoOriginal = bloque -> copia0-> numNodo;
		 	  int bloqueNodoOriginal = bloque -> copia0-> bloqueNodo;
		 	  char* nodoCopia = bloque -> copia1 -> numNodo;
		 	  int bloqueNodoCopia = bloque -> copia1-> bloqueNodo;

		 	 printf("Contiene al bloque numero %d\n", numeroBloque);
		 	 printf("Finaliza en %d\n", fin);
		 	 printf("La copia original se ubica en el nodo %s\n", nodoOriginal);
		 	 printf("En el bloque %d\n", bloqueNodoOriginal);
		 	 printf("La copia1 se ubica en el nodo %s\n", nodoCopia);
		 	 printf("En el bloque %d\n", bloqueNodoCopia);

		 	}


	 }
}

/////////////////////////////////////////////////////

void formatear() {

		puts("Empezamos el formateo");

		list_destroy_and_destroy_elements(archivos, (void*) liberarArchivo);
		list_destroy_and_destroy_elements(info_DataNodes, (void*) liberarinfoDataNodo);
		list_destroy_and_destroy_elements(directorios,(void*) liberarDirectorio);

		info_DataNodes = list_create();
		archivos = list_create();
		directorios = list_create();

		//Borramos los metadatas

		char* comandoBorrarMetadatasArchivos = string_new();

		string_append(&comandoBorrarMetadatasArchivos,"rm -r ");
		string_append(&comandoBorrarMetadatasArchivos,tablaArchivos);
		string_append(&comandoBorrarMetadatasArchivos,"/*");

		system(comandoBorrarMetadatasArchivos);

		free(comandoBorrarMetadatasArchivos);

		char* comandoBorrarBitmaps = string_new();

		string_append(&comandoBorrarBitmaps,"rm -r ");
		string_append(&comandoBorrarBitmaps,"../../yamaFS/metadata/bitmaps/*");

		system(comandoBorrarBitmaps);

		free(comandoBorrarBitmaps);

		char* comandoBorrarNodos = string_new();

		string_append(&comandoBorrarNodos,"rm ");
		string_append(&comandoBorrarNodos,tablaNodos);

		system(comandoBorrarNodos);

		free(comandoBorrarNodos);

		char* comandoBorrarDirectorios = string_new();

		string_append(&comandoBorrarDirectorios,"rm ");
		string_append(&comandoBorrarDirectorios,tablaDirectorios);

		system(comandoBorrarDirectorios);

		free(comandoBorrarDirectorios);

		// Creamos nodos.bin y directorios.dat

		char* crearMetadaNodos = string_new();
		string_append(&crearMetadaNodos,"touch ");
		string_append(&crearMetadaNodos,tablaNodos);

		system(crearMetadaNodos);

		free(crearMetadaNodos);

		t_config* metadataNodos = config_create(tablaNodos);

		if(metadataNodos == NULL)
			printf("ERROR: No se pudo encontrar el archivo %s", tablaNodos);

		//Seteo las keys del config
		config_set_value(metadataNodos, "TAMANIO", "0");
		config_set_value(metadataNodos, "LIBRE", "0");
		config_set_value(metadataNodos, "NODOS", "[]");

		config_save(metadataNodos);


		char* crearMetadaDirectorios = string_new();
		string_append(&crearMetadaDirectorios,"touch ");
		string_append(&crearMetadaDirectorios,tablaDirectorios);

		system(crearMetadaDirectorios);

		free(crearMetadaDirectorios);

		t_config* metadataDirectorios = config_create(tablaDirectorios);

		if(metadataDirectorios == NULL)
			printf("ERROR: No se pudo encontrar el archivo %s", tablaDirectorios);

		//Seteo las keys del config
		config_set_value(metadataDirectorios, "INDEX", "[]");
		config_set_value(metadataDirectorios, "DIRECTORIO", "[]");
		config_set_value(metadataDirectorios, "PADRE", "[]");

		config_save(metadataDirectorios);

		info_DataNodes = list_create();
		archivos = list_create();
		directorios = list_create(); //elimina el root

		crearRoot(); //lo vuelvo a crear

	puts("El file system ha sido formateado.");
}

void liberarBloqueEnNodo(copiaLectura* bloqueEnNodo) {
	free(bloqueEnNodo);
}

void liberarBloqueArch(bloque* bloqueArch) { //verificar si elimina copias
	free(bloqueArch);
}

void liberarArchivo(t_info_archivo* unArchivo) {
	list_destroy_and_destroy_elements((unArchivo->infoBloques),(void*) liberarBloqueArch);
	free(unArchivo);
}

void liberarinfoDataNodo(t_Nodos *unNodo) {
	list_destroy_and_destroy_elements(unNodo->bloquesTotalesyLibres, (void*) free); //eliminar char**Nodos?
	free(unNodo);
}

void liberarDirectorio(t_directory* unDirectorio) {
	free(unDirectorio->nombre);
	free(unDirectorio);
}

////////////////////////////////////////////////////////////////

char* sacarNombreArchivoDelPath(char* path){

	char** pathDividido = string_split(path, "/"); //Me quedaria una lista de cada subdirectorio dividido por "/", termina con un valor NULL
	int i=0;
	char* nombreArchivo;

	while(pathDividido[i]!=NULL){
		if(pathDividido[i+1]==NULL){ //Si la siguiente posicion es NULL entonces es el archivo
		nombreArchivo = malloc(strlen(pathDividido[i])+1);
		strcpy(nombreArchivo, pathDividido[i]);
		}
		else{
			i++;
		}
		}

	return nombreArchivo;
}



void mv (char* path_original, char* path_finalCompleto){ //deberia ser user/juan/datos user/marcelo

	if(verificarSiEsArchivoODirectorio(path_original)!=0){ //si es !=0 es un archivo

			bool condition(void* element) {
				t_info_archivo* archivo = element;
						return string_equals_ignore_case(archivo->pathArchivo, path_original);
				}

			t_info_archivo* archivoOriginal = list_find(archivos, condition);

	if(verificarExistenciaNombreArchivo(path_original)==1) { // ==0 ya existe archivo con ese nombre en el mismo index

		char* nombreNuevoSoloArchivo = sacarNombreArchivoDelPath(path_finalCompleto); // si

		char* nombreNuevoCompleto=string_new();

		string_append(&nombreNuevoCompleto, path_finalCompleto);

		string_append(&nombreNuevoCompleto, nombreNuevoSoloArchivo);

		(archivoOriginal ->pathArchivo) = nombreNuevoCompleto;


	}else{
		printf("Ya existe el mismo nombre de archivo en ese directorio\n");
	}

	}else{ //es directorio

	t_directory* ultimoSubdirectorio = buscarDirectorio(path_original);

	t_directory* ultimoSubdirectorioFinal = buscarDirectorio(path_finalCompleto);


	int padreOriginal = ultimoSubdirectorio -> indexPadre;

	int indiceFinal = ultimoSubdirectorioFinal -> index;


	if (padreOriginal != -1){ //verifico que no sea el root

		if (indiceFinal != padreOriginal){ //verifico que no esten en el mismo lugar, seria al pedo moverlo

			padreOriginal = indiceFinal; //el padre del original tendria que pasar a ser = al indice del nuevo directorio donde se va a mover

		} else printf("No se puede mover un Directorio adentro de si mismo\n");

	} else printf("No se puede mover el root\n");
	}
}


////////////////


char* stringCrear(int tamanio) {
	char* string = malloc(tamanio);
	memset(string, '\0', tamanio);
	return string;
}


t_bitarray* crearBitmap(int cantidadBloques) {
	int tamanioBytes = (int)ceil((double)cantidadBloques/(double)8); //para que ande habia que agregar lm en las libraries
	char* bits = malloc(tamanioBytes);
	t_bitarray* bitmap = bitarray_create_with_mode(bits, tamanioBytes, LSB_FIRST);
	int indice;
	for(indice = 0; indice < tamanioBytes*8; indice++)
		bitarray_clean_bit(bitmap, indice);
	return bitmap;
}




void recuperarArchivo() { //verificar si no falta lo del indicePadre en el archivo

	FILE* archivo = fopen(tablaArchivostxt, "r");

		if(archivo == NULL) {
	printf("no se encontro el archivo\n");
	return;
		}

	fclose(archivo);
	archivos = list_create();
	t_config* config = config_create(tablaArchivostxt);
	int cantidadArchivos = config_get_int_value(config, "ARCHIVOS");
	int indice;

	for(indice = 0; indice < cantidadArchivos; indice++) {

		char* nombreDentroConfig = string_from_format("NOMBRE%i", indice);
		char* padreDentroConfig = string_from_format("PADRE%i", indice);
		char* nombre = stringCrear(255);

		int padre = config_get_int_value(config, padreDentroConfig);
		strcpy(nombre,config_get_int_value(config, nombreDentroConfig));

		free(nombreDentroConfig);
		free(padreDentroConfig);

		char* ruta = string_from_format("%s/%i/%s", tablaArchivos, padre, nombre);
		t_config* config = config_create(ruta);
		free(ruta);
		free(nombre);

		t_info_archivo* archivo = malloc(sizeof(t_info_archivo));
		strcpy(archivo->nombreArchivo, config_get_string_value(config, "NOMBRE"));
		archivo->indicePadre = config_get_int_value(config, "ID_PADRE");
		strcpy(archivo->tipo, config_get_string_value(config, "TIPO"));

		int indiceBloques;
		archivo->infoBloques = list_create();
		int cantidadBloques = config_get_int_value(config, "BLOQUES");

		for(indiceBloques = 0; indiceBloques < cantidadBloques; indiceBloques++) {
			t_info_bloque_archivo* bloque = malloc(sizeof(bloque));
			char* lineaBloque = string_from_format("BLOQUE%i_BYTES", indiceBloques);
			bloque->bloqueArchivo = indiceBloques;
			bloque->bytesOcupados = config_get_int_value(config, lineaBloque);
			free(lineaBloque);
			list_add(archivo->infoBloques, bloque);
			//faltaria copia0 y copia1
		}
		list_add(archivos,archivo);
		config_destroy(config);
	}
	config_destroy(config);
}



void recuperarNodo() {

	t_config* config = config_create(tablaNodos);
	int cantidadNodos = config_get_int_value(config, "NODOS");
	int indice;

	for(indice = 0; indice < cantidadNodos; indice++) {
		char* nombreDentroConfig = string_from_format("NOMBRE%i", indice);
		char* lineaTotales = string_from_format("BLOQUES_TOTALES%i", indice);
		char* lineaLibres = string_from_format("BLOQUES_LIBRES%i", indice);
		t_info_nodo* nodo = malloc(sizeof(nodo));
		strcpy(nodo->nombreNodo, config_get_string_value(config, nombreDentroConfig));
		nodo->bloquesTotales = config_get_int_value(config, lineaTotales);
		nodo->bloquesLibres = config_get_int_value(config, lineaLibres);
		nodo->estado = 0;
		nodo->socket = 0;
		//nodo->mensaje?
		nodo->puerto = 0;
		strcpy(nodo->ip, "-");
		//nodo->tareas?
		free(nombreDentroConfig);
		free(lineaTotales);
		free(lineaLibres);
		list_add(info_DataNodes,nodo);
		char* ruta = string_from_format("%s/%s", tablaBitmaps, nodo->nombreNodo); //append =

		FILE* file = fopen(ruta, "r");
		if(file == NULL) {
				printf("no se encontro el archivo\n");
				free(ruta);
				return;
		}

		nodo->bitmap = crearBitmap(nodo->bloquesTotales);
		fread(nodo->bitmap->bitarray, sizeof(char), nodo->bitmap->size, file);
		free(ruta);
		fclose(file);
		config_destroy(config);
		}

	config_destroy(config);
	}



void recuperarDirectorio() {

	FILE* file = fopen(tablaArchivostxt, "r");
	crearBitmap(100);
		if(file == NULL) {
			printf("no se encontro el archivo\n");
			return;
		}

	fclose(file);

	crearBitmap(100);
	directorios = list_create();
	t_config* config = config_create(tablaDirectorios);
	int cantidadDirectorios = config_get_int_value(config, "DIRECTORIOS");

	int indice;
	for(indice = 0; indice < cantidadDirectorios; indice++) {
		char* lineaIdentificador = string_from_format("IDENTIFICADOR%i", indice);
		char* nombreDentroConfig = string_from_format("NOMBRE%i", indice);
		char* padreDentroConfig = string_from_format("PADRE%i", indice);
		t_directory* directorio = malloc(sizeof(t_directory));
		directorio->index = config_get_int_value(config, lineaIdentificador);
		directorio->indexPadre = config_get_int_value(config, padreDentroConfig);
		strcpy(directorio->nombre, config_get_string_value(config, nombreDentroConfig));
		list_add(directorios, directorio);
		free(lineaIdentificador);
		free(nombreDentroConfig);
		free(padreDentroConfig);
	}

	config_destroy(config);
	for(indice = 0; indice < cantidadDirectorios; indice++)
		bitarray_set_bit(tablaBitmaps, indice);
}



