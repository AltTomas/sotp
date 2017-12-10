#include "funcionesDataNode.h"
#include <estructuras.h>




void leerArchivoConfig(char* rutaArchivoConfig)
{
	data_DataNode = malloc(sizeof(t_DataNode));

	data_DataNode->IP_FILESYSTEM=strdup("");
	data_DataNode->NOMBRE_NODO=strdup("");
	data_DataNode->RUTA_DATABIN=strdup("");

	t_config *configuracion = config_create(rutaArchivoConfig);

	string_append(&data_DataNode->IP_FILESYSTEM,config_get_string_value(configuracion,"IP_FILESYSTEM"));
	string_append(&data_DataNode->NOMBRE_NODO,config_get_string_value(configuracion,"NOMBRE_NODO"));
	string_append(&data_DataNode->RUTA_DATABIN,config_get_string_value(configuracion,"RUTA_DATABIN"));
	data_DataNode->PUERTO_FILESYSTEM= config_get_int_value(configuracion,"PUERTO_FILESYSTEM");
	data_DataNode->PUERTO_WORKER=config_get_int_value(configuracion,"PUERTO_WORKER");

	config_destroy(configuracion);
}

void conectarConFS(void){

	log_info(logger,"Conectandose a YAMA");

	int puerto = data_DataNode->PUERTO_FILESYSTEM;

	socketConexionFS = crearCliente(data_DataNode->IP_FILESYSTEM, puerto);

	if(socketConexionFS== -1){
		log_error(logger,"No se pudo establecer la conexion con YAMA, cerrando FS...");
		exit(1);
	}

	puts("Me conecte a FS");
	log_info(logger,"Me conecte a FS");

		int cantidadBloquesDN = list_size(blokesDN);

		int i;

		t_bloquesDN* bloqueDataN = list_get(blokesDN,i);

		t_struct_datanode* datanodeParaFS = malloc(sizeof(t_struct_datanode));

		datanodeParaFS->ipDN = DataNodePRUEBA->IP_NODO;
		datanodeParaFS->puertoDN = DataNode->PUERTO_NODO;
		datanodeParaFS->nomDN = DataNode->NOMBRE_N;

		for(i=0;i<cantidadBloquesDN;i++){
			t_bloquesDN* bloque = list_get(data_DataNode->bloquesDN,i)
			datanodeParaFs->bloqueDN-> = DataNode->bloqueDN->numBloque;

		}


		socket_enviar(socketConexionFS, D_STRUCT_DATA_NODE,dataNodeParaFS);
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
