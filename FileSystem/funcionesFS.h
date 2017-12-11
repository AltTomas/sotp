
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
#include <commons/bitarray.h>
#include <math.h>

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
#define tablaArchivos "../../yamaFS/metadata/archivos"
#define tablaBitmaps "../../yamaFS/metadata/bitmaps"
#define	tablaArchivostxt "../../yamaFS/metadata/Archivos.txt"

#define MAX_LEN_PUERTO 6
#define PUNTO_MONTAJE 250

int estadoConexiones;
int estadoFileSystem;
int estadoEjecucion;




typedef struct {
	char* fs_puerto;
	char* punto_montaje;
}t_config_fs;

typedef struct{
	int socket;
	char* ip;
	int puerto;
	char* nombreNodo;
	int bloquesTotales;
	int bloquesLibres;
	t_list* bloqueDN;//t_struct_bloqueDN
	int estado;
	t_bitarray*	bitmap;
}t_info_nodo;

typedef struct{
	char* nombreNodo;
	int bloquesLibres;
}t_bloquesLibres_nodo;

typedef struct{
	int bloqueArchivo;
	char* copia0Nodo;
	int copia0NodoBloque;
	char* copia1Nodo;
	int copia1NodoBloque;
	int bytesOcupados;
}t_info_bloque_archivo;

typedef struct{
	char* pathArchivo;
	char* nombreArchivo;
	int copiasDisponibles;
	int tamanio;
	int indicePadre;
	char* tipo;
	t_list* infoBloques;
}t_info_archivo;

t_log* logger;
t_config_fs* config;

fd_set datanode;
fd_set setDataNodes;
fd_set yama;
fd_set setyama;
int socketEscuchaDataNodes;
int max_fd;

t_list* directorios; // una lista de t_directory
t_Nodos* infoNodos;
t_list* info_DataNodes;
t_list* infoBloquesArchivo; // lista de t_info_bloque_archivo
t_list* archivos; // lista de t_info_archivo

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
long conseguirTamanioArchivo(char*);
int enviarADataNode(t_info_archivo*, char*, int, int);
t_Nodos* leerMetadataNodos(char*);
bool verificarMetadataNodos(t_config*);
int obtenerCantidadElementos(char**);
bool esBinario(const void *, size_t);
int almacenarArchivo(char*, char*, char*);
int almacenarBinario(t_info_archivo*, FILE* );
int almacenarTexto(t_info_archivo*, FILE*);
int buscarSocketNodo(char*);
int guardarCopia(t_info_archivo*, int, t_pedido_almacenar_bloque*, int);
bool masBloquesLibres(t_info_nodo*, t_info_nodo*);
void seDesconectaUnNodo(char*);
int buscarPosicion(char**, char*);
void actualizarTablaNodosBorrar(int);
char* leer(char* path);
void actualizarTablaNodosAgregar(char*, int, int);
void actualizarTablaNodosAsignacion(int);
char* buscarNombreNodo(int);
void crearMetadataArchivo(int, char*, long, char*);
//int existeEnTablaDirectorio(char*);
t_directory* buscarDirectorio(char*);
void actualizarMetadataArchivo(char*);
void actualizarMetadataArchivoBloques(char*, char*, int, int);
int verificarSiEsArchivoODirectorio(char* path);
int verificarExistenciaNombreDirectorio(t_directory*);
int verificarExistenciaNombreArchivo(char* rutaArchivo);
void MD5(char* path_archivo);
void cat(char* path_archivo);
void ls(char* path_archivo);
void info(char* path_archivo);
void formatear();
void liberarDirectorio(t_directory* unDirectorio);
void liberarBloqueArch(bloque* bloqueArch);
void liberarArchivo(t_info_archivo* unArchivo);
void liberarinfoDataNodo(t_Nodos *unNodo);
void liberarBloqueEnNodo(copiaLectura* bloqueEnNodo);
void crearRoot();
t_directory* verificarExistenciaDirectorio(char*, int);
t_directory* crearDirectorio(char*, int);
int buscarIndices(char*);
int pedidoLecturaRuta(char*);
t_list* leerTablaDeDirectorios(char* ruta);
void actualizarMetadataDirectorios(t_directory*);
int borrarDirectorio(char*);
void eliminarDirectorio(t_directory*);
bool tieneSubdirectorio(t_directory*);
t_directory* traerSubdirectorio(t_directory*);
t_directory* buscarPorPadre(int);
void eliminarDirectorioVacio(t_directory*);
void liberarDirectorio(t_directory*);
bool estaVacio(t_directory*);
t_directory* buscarPorIndice(int);
bool verificarMetadataDirectorios(t_config*);
char* sacarNombreArchivoDelPath(char* path);
void mover (char* path_original, char* path_finalCompleto);
int buscarArchivo(char*);
void renombrar(char* path_original, char* path_finalCompleto);
void mv (char*, char*);
bool rutaTieneRaiz(char*);
bool caracteresIguales (char, char);
bool rutaBarrasEstanSeparadas(char*);
bool verificarRuta(char*);
bool archivoExiste(int, char*);
int buscarBloqueDeArchivo(t_info_archivo* , int);
void recuperarArchivo();
void recuperarDirectorio();
void recuperarNodo();

#endif /* FUNCIONESFS_H_ */
