#include "funcionesDataNode.h"
#include <estructuras.h>

t_DataNode* data_DataNode;

void leerArchivoConfig(char* rutaArchivoConfig)
{
	data_DataNode = malloc(sizeof(t_DataNode));

	t_config *configuracion = config_create(rutaArchivoConfig);

	char* ipFS = config_get_string_value(configuracion,"IP_FILESYSTEM");

	data_DataNode->IP_FILESYSTEM = malloc(strlen(ipFS)+1);
	strcpy(data_DataNode->IP_FILESYSTEM,ipFS);

	data_DataNode->NOMBRE_NODO= strdup(config_get_string_value(configuracion,"NOMBRE_NODO"));
	data_DataNode->RUTA_DATABIN=strdup(config_get_string_value(configuracion,"RUTA_DATABIN"));
	data_DataNode->PUERTO_FILESYSTEM= config_get_int_value(configuracion,"PUERTO_FILESYSTEM");
	data_DataNode->PUERTO_WORKER=config_get_int_value(configuracion,"PUERTO_WORKER");

	config_destroy(configuracion);
}

void conectarConFS(void){

	t_struct_numero* handshake = malloc(sizeof(t_struct_numero));

	handshake->numero = ES_DATANODE;

	log_info(logger,"Conectandose a FS");

	printf("IP FS: %s\n", data_DataNode->IP_FILESYSTEM);

	t_struct_datanode * datanodeParaFS = malloc(sizeof(t_struct_datanode));

	socketConexionFS = crearCliente(data_DataNode->IP_FILESYSTEM, data_DataNode->PUERTO_FILESYSTEM);

	if(socketConexionFS== 1){
		log_error(logger,"No se pudo establecer la conexion con FS...");
		puts("No se pudo establecer la conexion con FS...");
		exit(1);
	}

	socket_enviar(socketConexionFS, D_STRUCT_NUMERO, handshake);

	puts("Me conecte a FS");
	log_info(logger,"Me conecte a FS");


	datanodeParaFS->bloquesTotales = cantidadBloques;
	datanodeParaFS->nomDN = data_DataNode->NOMBRE_NODO;

	socket_enviar(socketConexionFS, D_STRUCT_INFO_NODO,datanodeParaFS);
	//socket_enviar(socketConexionFS, D_STRUCT_NUMERO,enviado);

	log_info(logger,"Handshake enviado a YAMA");

}



char getBloque(int bloque){

	char data[DATANODE_BLOCK_SIZE];

    if(bloque < 0){
        return 0;
    }

    if(bloque>cantidadBloques){
        	log_error(logger, "No existe el bloque pedido");
        	return -2;
        }

    int fd = open(data_DataNode->RUTA_DATABIN, O_RDONLY);

    if(fd == -1){
        log_error(logger, "No se encontró el data.bin en la ruta %s\n", data_DataNode->RUTA_DATABIN);
        return -1;
    }

    databin = mmap(NULL, DATANODE_BLOCK_SIZE,
                        PROT_READ, MAP_PRIVATE, fd, DATANODE_BLOCK_SIZE * bloque);

    if(databin == MAP_FAILED){
        log_error(logger, "No se pudo mapear el data.bin en la ruta\n");
        return -1;
    }

    memcpy(data, databin, DATANODE_BLOCK_SIZE);

    munmap(databin, DATANODE_BLOCK_SIZE);
    close(fd);

    return data;
}

int setBloque(int bloque, char data[DATANODE_BLOCK_SIZE]){
    int fd = open(data_DataNode->RUTA_DATABIN, O_RDWR);

    if(fd == -1){
        log_error(logger, "No se encontró el data.bin en la ruta %s\n", data_DataNode->RUTA_DATABIN);
        return -1;
    }

    if(bloque>cantidadBloques){
    	log_error(logger, "No hay más espacio para escribir el bloque pedido");
    	return -2;
    }

    databin = mmap(NULL, DATANODE_BLOCK_SIZE,
                        PROT_READ | PROT_WRITE, MAP_SHARED, fd, DATANODE_BLOCK_SIZE * bloque);

    if(databin == MAP_FAILED){
        log_error(logger, "No se pudo mapear el data.bin en la ruta\n");
        return -1;
    }


    memcpy(databin, data, DATANODE_BLOCK_SIZE);

    if(msync(databin, DATANODE_BLOCK_SIZE, MS_SYNC) == -1){
        return -1;
    }

    munmap(databin, DATANODE_BLOCK_SIZE);
    close(fd);

    return 0;
}

void calcularCantidadDeBloques(int fd){

	struct stat stats;
	fstat(fd, &stats);
	cantidadBloques = stats.st_size / DATANODE_BLOCK_SIZE;

	log_info(logger, "La cantidad de bloques es: %d", cantidadBloques);
}
