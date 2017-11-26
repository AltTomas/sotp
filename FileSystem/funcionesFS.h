
#
#ifndef FUNCIONESFS_H_
#define FUNCIONESFS_H_

#include <estructuras.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/collections/queue.h>
#include <commons/collections/dictionary.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <stdbool.h>
#include <pthread.h>
#include <readline/readline.h>
#include <sockets.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>

#include "tablaArchivos.h"
#include "tablaDirectorios.h"
#include "tablaNodos.h"

#define configuracionFS "../../Configs/configFileSystem.config"
#define tablaDirectorios "../../metadata/directorios.dat"
#define tablaNodos "../../yamaFS/metadata/nodos.bin"
#define tablaArchivos "../../metadata/archivos/"

#define MAX_LEN_PUERTO 6
#define PUNTO_MONTAJE 250



typedef struct {
	char* fs_puerto;
	char* punto_montaje;
}t_config_fs;

typedef struct{
	int socket;
	char* nombreNodo;
}t_sockets_nodo;

typedef struct{
	char* nombreNodo;
	int bloquesLibres;
}t_bloquesLibres_nodo;

t_list* filesAlmacenados; // una lista de t_directory?
t_list* directorios; //repetido linea 70

t_log* logger;
t_config_fs* config;

fd_set datanode;
fd_set setDataNodes;
fd_set yama;
fd_set setyama;
int socketEscuchaDataNodes;
int max_fd;


t_list* directorios;
t_list* nodosConectados;
t_Nodos* infoNodos;
t_list* info_DataNodes;
t_list* bloquesLibresPorNodo;

char estado; //estable o no estable

void crearConfig();
void crearEstructurasAdministrativas();
t_config_fs* levantarConfiguracionFS(char*);
bool verificarConfig(t_config*);
bool verificarExistenciaDeArchivo(char*);
void destruirConfig(t_config_fs*);
void commandHandler();
void escucharConexiones(void);
void aceptarNuevaConexion(int, fd_set* );
void trabajarSolicitudDataNode(int);
void buscarBloquesArchivo(char*, int);
char* obtenerContenido(char*);
int conseguirTamanioArchivo(char*);
int enviarADataNode(char*, int, int);
t_Nodos* leerMetadataNodos(char*);
bool verificarMetadataNodos(t_config*);
int obtenerCantidadElementos(char**);
bool esBinario(const void *, size_t);
int almacenarArchivo(char*, char*, char*);
int buscarSocketNodo(char*);
int guardarCopia(int, t_almacenar_bloque*);
bool masBloquesLibres(t_bloquesLibres_nodo*, t_bloquesLibres_nodo*);
void seDesconectaUnNodo(char*);
int buscarPosicion(char**, char*);
void actualizarTablaNodosBorrar(int);

#endif /* FUNCIONESFS_H_ */
