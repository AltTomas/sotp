/*
 ============================================================================
 Name        : DataNode.c
 Author      : Christian Monasterios
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "funcionesDataNode.h"

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

//Variables Globales

int fdArchivo;
struct stat mystat;
char *pmapArchivo = NULL;
dataNode_block* BoquesDeDatos;

int cantidadDeBloques;
// Funciones
int levantarArchivo(char * archivoBinario);
char* mappearArchivo(int fdArchivoBinario, char * posArchivoBinario);
void cargar_BloqueDeDatos();

int main(int argc, char *argv[]) {

	printf("%s\n",argv[1]);

	char* path = argv[1];

    leerArchivoConfig(path);

			printf("\n IP_FS: %s",data_DataNode->IP_FILESYSTEM);
			printf("\n PUERTO_FS: %d",data_DataNode->PUERTO_FILESYSTEM);
			printf("\n NOMBRE NODO: %s", data_DataNode->NOMBRE_NODO);
			printf("\n PUERTO WKR: %d", data_DataNode->PUERTO_WORKER);
			printf("\n DATA BIN: %s",data_DataNode->RUTA_DATABIN);

			fdArchivo = levantarArchivo(data_DataNode->RUTA_DATABIN);
			pmapArchivo = mappearArchivo(fdArchivo, pmapArchivo);
			fstat(fdArchivo, &mystat);
			printf("\n Tamanio Data.bin: %u", mystat.st_size);
			cantidadDeBloques = mystat.st_size / DATANODE_BLOCK_SIZE;
			printf("\n CantBloques: %u", cantidadDeBloques);

			//	conectarConFS();

			//INICIO: Recibe Peticiones de FS para Lectura o Escritura



			//FIN: Recibe Peticiones de FS para Lectura o Escritura

			return 0;
}

int levantarArchivo(char * archivoBinario) {
	int fdArchivoBinario = open(archivoBinario, O_RDWR);
	if (fdArchivoBinario == 0) {
		perror("Error en Open");
		//escribir_log("error en open");
		close(fdArchivoBinario);
		exit(1);

	}
	//escribir_log("levantar archivo OK");
	return fdArchivoBinario;
}

char* mappearArchivo(int fdArchivoBinario, char * posArchivoBinario) {

	fstat(fdArchivoBinario, &mystat);
	posArchivoBinario = mmap(0, mystat.st_size, PROT_READ | PROT_WRITE,MAP_SHARED, (int) fdArchivoBinario, 0);

	if (posArchivoBinario == MAP_FAILED) {
		perror("Error en mmap");
	//	escribir_log("error en mmap");
		close(fdArchivoBinario);
		exit(1);
	}
	//escribir_log("mmap OK");
	return posArchivoBinario;

}

void cargar_BloqueDeDatos()
{
	char* posBloqueDeDatos= pmapArchivo;
	BoquesDeDatos = malloc(DATANODE_BLOCK_SIZE * (cantidadDeBloques));

	bzero(BoquesDeDatos, 0);
	memcpy(BoquesDeDatos, posBloqueDeDatos, DATANODE_BLOCK_SIZE * (cantidadDeBloques));
}

