#include "serializacion.h"

t_header crearHeader(uint8_t tipoEstructura, uint16_t lengthDatos){
	t_header header;
	header.tipoEstructura = tipoEstructura;
	header.length = lengthDatos;
	return header;
}

char * crearDataConHeader(uint8_t tipoEstructura, int length){
	char * data = malloc(length);

	uint16_t lengthDatos = length - sizeof(t_header);

	t_header header = crearHeader(tipoEstructura, lengthDatos); //creo el header

	int tamanoTotal = 0, tamanoDato = 0;

	memcpy(data, &header.tipoEstructura, tamanoDato = sizeof(uint8_t)); //copio el tipoEstructura del header a data
	tamanoTotal = tamanoDato;
	memcpy(data + tamanoTotal, &header.length, tamanoDato = sizeof(uint16_t)); //copio el length del header a data

	return data;
}

t_stream * serialize(int tipoEstructura, void * estructuraOrigen){
	t_stream * paquete=NULL;

	switch (tipoEstructura){
			case D_STRUCT_NUMERO:
				paquete = serializeStruct_numero((t_struct_numero *) estructuraOrigen, D_STRUCT_NUMERO);
				break;
			case D_STRUCT_CHAR:
				paquete = serializeStruct_char((t_struct_char *) estructuraOrigen, D_STRUCT_CHAR);
				break;
			case D_STRUCT_STRING:
				paquete = serializeStruct_string((t_struct_string *) estructuraOrigen, D_STRUCT_STRING);
				break;
			case D_STRUCT_MALC:
				paquete = serializeStruct_malc((t_struct_malloc *) estructuraOrigen);
				break;
			case D_STRUCT_IMPR:
				paquete = serializeStruct_string((t_struct_string *) estructuraOrigen, D_STRUCT_IMPR);
				break;
			case D_STRUCT_FIN_PROG:
				paquete = serializeStruct_numero((t_struct_numero *) estructuraOrigen, D_STRUCT_FIN_PROG);
				break;
			case D_STRUCT_ABORTAR_EJECUCION:
				paquete = serializeStruct_numero((t_struct_numero *) estructuraOrigen, D_STRUCT_ABORTAR_EJECUCION);
				break;
			case D_STRUCT_CONTINUAR_EJECUCION:
				paquete = serializeStruct_numero((t_struct_numero *) estructuraOrigen, D_STRUCT_CONTINUAR_EJECUCION);
				break;
			case D_STRUCT_FIN_QUANTUM:
				paquete = serializeStruct_numero((t_struct_numero *) estructuraOrigen, D_STRUCT_FIN_QUANTUM);
				break;
			case D_STRUCT_SOLICITAR_CODIGO:
				paquete = serializeStruct_numero((t_struct_numero *) estructuraOrigen, D_STRUCT_SOLICITAR_CODIGO);
				break;
			case D_STRUCT_PID:
				paquete = serializeStruct_numero((t_struct_numero *) estructuraOrigen, D_STRUCT_PID);
				break;
			case D_STRUCT_LIBERAR_MEMORIA:
				paquete = serializeStruct_numero((t_struct_numero *) estructuraOrigen, D_STRUCT_LIBERAR_MEMORIA);
				break;
			case D_STRUCT_LECT:
				paquete = serializeStruct_solLect((t_struct_sol_lectura *) estructuraOrigen, D_STRUCT_LECT);
				break;
			case D_STRUCT_LECT_VAR:
				paquete = serializeStruct_solLect((t_struct_sol_lectura *) estructuraOrigen, D_STRUCT_LECT_VAR);
				break;
			case D_STRUCT_ESCRIBIR_HEAP:
				paquete = serializeStruct_solEscr((t_struct_sol_escritura *) estructuraOrigen, D_STRUCT_ESCRIBIR_HEAP);
				break;
			case D_STRUCT_LIBERAR_HEAP:
				paquete = serializeStruct_solEscr((t_struct_sol_escritura *) estructuraOrigen, D_STRUCT_LIBERAR_HEAP);
				break;
			case D_STRUCT_ERROR_HEAP_MAX:
				paquete = serializeStruct_numero((t_struct_numero *) estructuraOrigen, D_STRUCT_ERROR_HEAP_MAX);
				break;
			case D_STRUCT_ERROR_HEAP:
				paquete = serializeStruct_numero((t_struct_numero *) estructuraOrigen, D_STRUCT_ERROR_HEAP);
				break;
			case D_STRUCT_ABORT:
				paquete = serializeStruct_numero((t_struct_numero *) estructuraOrigen, D_STRUCT_ABORT);
				break;
			case D_STRUCT_SOL_ESCR:
				paquete = serializeStruct_solEscr((t_struct_sol_escritura *) estructuraOrigen, D_STRUCT_SOL_ESCR);
				break;
			case D_STRUCT_ESCRITURA_CODIGO:
				paquete = serializeStruct_solEscr((t_struct_sol_escritura *) estructuraOrigen, D_STRUCT_ESCRITURA_CODIGO);
				break;
			case D_STRUCT_COMPACTAR_HEAP:
				paquete = serializeStruct_solEscr((t_struct_sol_escritura *) estructuraOrigen, D_STRUCT_COMPACTAR_HEAP);
				break;
			case D_STRUCT_WAIT:
				paquete = serializeStruct_string((t_struct_string *) estructuraOrigen, D_STRUCT_WAIT);
				break;
			case D_STRUCT_SIGNAL:
				paquete = serializeStruct_string((t_struct_string *) estructuraOrigen, D_STRUCT_SIGNAL);
				break;
			case D_STRUCT_OBTENER_COMPARTIDA:
				paquete = serializeStruct_string((t_struct_string *) estructuraOrigen, D_STRUCT_OBTENER_COMPARTIDA);
				break;
			case D_STRUCT_ARCHIVO_ESC:
				paquete = serializeStruct_archivo_esc((t_struct_archivo *) estructuraOrigen, D_STRUCT_ARCHIVO_ESC);
				break;
			case D_STRUCT_ARCHIVO_LEC:
				paquete = serializeStruct_archivo_esc((t_struct_archivo *) estructuraOrigen, D_STRUCT_ARCHIVO_LEC);
				break;
			case D_STRUCT_RTA_HEAP:
				paquete = serializeStruct_numero((t_struct_numero *) estructuraOrigen, D_STRUCT_RTA_HEAP);
				break;
			case D_STRUCT_ARCHIVO_ABR:
				paquete = serializeStruct_archivo_esc((t_struct_archivo *) estructuraOrigen, D_STRUCT_ARCHIVO_ABR);
				break;
			case D_STRUCT_ARCHIVO_BOR:
				paquete = serializeStruct_solLect((t_struct_sol_lectura *) estructuraOrigen, D_STRUCT_ARCHIVO_BOR);
				break;
			case D_STRUCT_ARCHIVO_BORRAR:
				paquete = serializeStruct_archivo_esc((t_struct_archivo *) estructuraOrigen, D_STRUCT_ARCHIVO_BORRAR);
				break;
			case D_STRUCT_ARCHIVO_CER:
				paquete = serializeStruct_solLect((t_struct_sol_lectura *) estructuraOrigen, D_STRUCT_ARCHIVO_CER);
				break;
			case D_STRUCT_ARCHIVO_MOV:
				paquete = serializeStruct_solLect((t_struct_sol_lectura *) estructuraOrigen, D_STRUCT_ARCHIVO_MOV);
				break;
			case D_STRUCT_BORRAR:
				paquete = serializeStruct_borrar((t_struct_borrar *) estructuraOrigen);
				break;
			case D_STRUCT_OBTENER:
				paquete = serializeStruct_obtener((t_struct_obtener *) estructuraOrigen);
				break;
			case D_STRUCT_JOB:
				paquete = serializeStruct_job((t_struct_job *) estructuraOrigen,D_STRUCT_JOB);
				break;
		}

	return paquete;
}

t_stream * serializeStruct_numero(t_struct_numero * estructuraOrigen, int headerOperacion){

	t_stream * paquete = malloc(sizeof(t_stream));		//creo el paquete

	paquete->length = sizeof(t_header) + sizeof(int32_t);

	char * data = crearDataConHeader(headerOperacion, paquete->length); //creo el data

	memcpy(data + sizeof(t_header), estructuraOrigen, sizeof(t_struct_numero));		//copio a data el numero.

	paquete->data = data;

	return paquete;
}

t_stream * serializeStruct_char(t_struct_char * estructuraOrigen, int headerOperacion){

	t_stream * paquete = malloc(sizeof(t_stream));		//creo el paquete

	paquete->length = sizeof(t_header) + sizeof(unsigned int);

	char * data = crearDataConHeader(headerOperacion, paquete->length); //creo el data

	memcpy(data + sizeof(t_header), &estructuraOrigen->letra, sizeof(char));		//copio a data el char.

	paquete->data = data;

	return paquete;
}

t_stream * serializeStruct_string(t_struct_string * estructuraOrigen, int headerOperacion){

	t_stream * paquete = malloc(sizeof(t_stream));		//creo el paquete

	paquete->length = sizeof(t_header) + strlen(estructuraOrigen->string) + 1;

	char * data = crearDataConHeader(headerOperacion, paquete->length); //creo el data

	int tamanoTotal = sizeof(t_header);

	memcpy(data + tamanoTotal, estructuraOrigen->string, strlen(estructuraOrigen->string)+1);		//copio a data el string.

	paquete->data = data;

	return paquete;
}

t_stream * serializeStruct_malc(t_struct_malloc * estructuraOrigen){

	t_stream * paquete = malloc(sizeof(t_stream));		//creo el paquete

	paquete->length = sizeof(t_header) + 2*sizeof(uint32_t);

	char * data = crearDataConHeader(D_STRUCT_MALC, paquete->length); //creo el data

	int tamanoTotal = sizeof(t_header), tamanoDato = 0;

	memcpy(data + tamanoTotal, &estructuraOrigen->PID, tamanoDato= sizeof(uint32_t));

	tamanoTotal+=tamanoDato;

	memcpy(data + tamanoTotal, &estructuraOrigen->tamano_segmento, tamanoDato= sizeof(uint32_t));

	tamanoTotal+=tamanoDato;

	paquete->data = data;

	return paquete;
}

t_stream * serializeStruct_lect(t_posicion_memoria * estructuraOrigen, int headerOperacion){

	t_stream* paquete = malloc(sizeof(t_stream));

	paquete->length = sizeof(t_header) + sizeof(uint32_t) + sizeof(uint32_t) +sizeof(uint32_t);

	char* data = crearDataConHeader(headerOperacion, paquete->length);

	int tamanoTotal = sizeof(t_header), tamanoDato = 0;

	memcpy(data + tamanoTotal, &estructuraOrigen->pagina, tamanoDato= sizeof(uint32_t));

	tamanoTotal+=tamanoDato;

	memcpy(data + tamanoTotal, &estructuraOrigen->offsetInstruccion, tamanoDato= sizeof(uint32_t));

	tamanoTotal+=tamanoDato;

	memcpy(data + tamanoTotal, &estructuraOrigen->longitudInstruccion, tamanoDato= sizeof(uint32_t));

	tamanoTotal+=tamanoDato;

	paquete->data = data;

	return paquete;

}

t_stream * serializeStruct_solEscr(t_struct_sol_escritura * estructuraOrigen, int headerOperacion){

	t_stream* paquete = malloc(sizeof(t_stream));

	paquete->length = sizeof(t_header) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + estructuraOrigen->tamanio;

	char* data = crearDataConHeader(headerOperacion, paquete->length);

	int tamanoTotal = sizeof(t_header), tamanoDato = 0;

	memcpy(data + tamanoTotal, &estructuraOrigen->pagina, tamanoDato= sizeof(uint32_t));

	tamanoTotal+=tamanoDato;

	memcpy(data + tamanoTotal, &estructuraOrigen->offset, tamanoDato= sizeof(uint32_t));

	tamanoTotal+=tamanoDato;

	memcpy(data + tamanoTotal, &estructuraOrigen->tamanio, tamanoDato= sizeof(uint32_t));

	tamanoTotal+=tamanoDato;

	memcpy(data + tamanoTotal, &estructuraOrigen->PID, tamanoDato= sizeof(uint32_t));

	tamanoTotal+=tamanoDato;

	memcpy(data + tamanoTotal, estructuraOrigen->contenido, tamanoDato = estructuraOrigen->tamanio);

	tamanoTotal+=tamanoDato;

	paquete->data = data;

	return paquete;

}

t_stream * serializeStruct_solLect(t_struct_sol_lectura * estructuraOrigen, int headerOperacion){

	t_stream* paquete = malloc(sizeof(t_stream));

	paquete->length = sizeof(t_header) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t);

	char* data = crearDataConHeader(headerOperacion, paquete->length);

	int tamanoTotal = sizeof(t_header), tamanoDato = 0;

	memcpy(data + tamanoTotal, &estructuraOrigen->pagina, tamanoDato= sizeof(uint32_t));

	tamanoTotal+=tamanoDato;

	memcpy(data + tamanoTotal, &estructuraOrigen->offset, tamanoDato= sizeof(uint32_t));

	tamanoTotal+=tamanoDato;

	memcpy(data + tamanoTotal, &estructuraOrigen->contenido, tamanoDato= sizeof(uint32_t));

	tamanoTotal+=tamanoDato;

	memcpy(data + tamanoTotal, &estructuraOrigen->PID, tamanoDato= sizeof(uint32_t));

	tamanoTotal+=tamanoDato;

	paquete->data = data;

	return paquete;

}

t_stream* serializeStruct_archivo_esc(t_struct_archivo * estructuraOrigen, int headerOperacion){

	t_stream* paquete = malloc(sizeof(t_stream));

	paquete->length = sizeof(t_header) + sizeof(uint32_t) + estructuraOrigen->tamanio + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(t_flags) ;

	char* data = crearDataConHeader(headerOperacion, paquete->length);

	int tamanoTotal = sizeof(t_header), tamanoDato = 0;

	memcpy(data + tamanoTotal, &estructuraOrigen->fileDescriptor, tamanoDato= sizeof(uint32_t));

	tamanoTotal+=tamanoDato;

	memcpy(data + tamanoTotal, &estructuraOrigen->tamanio, tamanoDato= sizeof(uint32_t));

	tamanoTotal+=tamanoDato;

	memcpy(data + tamanoTotal, estructuraOrigen->informacion, tamanoDato = (estructuraOrigen->tamanio));

	tamanoTotal+=tamanoDato;

	memcpy(data + tamanoTotal, &estructuraOrigen->pid, tamanoDato= sizeof(uint32_t));

	tamanoTotal+=tamanoDato;

	memcpy(data + tamanoTotal, &estructuraOrigen->flags.creacion, tamanoDato= sizeof(bool));

	tamanoTotal+=tamanoDato;

	memcpy(data + tamanoTotal, &estructuraOrigen->flags.escritura, tamanoDato= sizeof(bool));

	tamanoTotal+=tamanoDato;

	memcpy(data + tamanoTotal, &estructuraOrigen->flags.lectura, tamanoDato= sizeof(bool));

	tamanoTotal+=tamanoDato;

	paquete->data = data;

	return paquete;

}

t_stream * serializeStruct_borrar(t_struct_borrar * estructuraOrigen){

	t_stream * paquete = malloc(sizeof(t_stream));		//creo el paquete

		paquete->length = sizeof(t_header) + strlen(estructuraOrigen->path) + 1 + sizeof(uint32_t);

		char * data = crearDataConHeader(D_STRUCT_BORRAR, paquete->length); //creo el data

		int tamanoTotal = sizeof(t_header), tamanoDato = 0;

		memcpy(data + tamanoTotal, estructuraOrigen->path, tamanoDato = strlen(estructuraOrigen->path)+1);		//copio a data el string.

		tamanoTotal+=tamanoDato;

		memcpy(data + tamanoTotal, &estructuraOrigen->confirmacion, tamanoDato = sizeof(uint32_t));

		tamanoTotal+=tamanoDato;

		paquete->data = data;

		return paquete;
}

t_stream * serializeStruct_obtener(t_struct_obtener * estructuraOrigen){

	t_stream * paquete = malloc(sizeof(t_stream));		//creo el paquete

		paquete->length = sizeof(t_header) + strlen(estructuraOrigen->path) + 1 + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + estructuraOrigen->size;

		char * data = crearDataConHeader(D_STRUCT_OBTENER, paquete->length); //creo el data

		int tamanoTotal = sizeof(t_header), tamanoDato = 0;

		memcpy(data + tamanoTotal, estructuraOrigen->path, tamanoDato = strlen(estructuraOrigen->path)+1);		//copio a data el string.

		tamanoTotal+=tamanoDato;

		memcpy(data + tamanoTotal, &estructuraOrigen->confirmacion, tamanoDato = sizeof(uint32_t));

		tamanoTotal+=tamanoDato;

		memcpy(data + tamanoTotal, &estructuraOrigen->modo_lectura, tamanoDato = sizeof(uint32_t));

		tamanoTotal+=tamanoDato;

		memcpy(data + tamanoTotal, &estructuraOrigen->offset, tamanoDato = sizeof(uint32_t));

		tamanoTotal+=tamanoDato;

		memcpy(data + tamanoTotal, &estructuraOrigen->size, tamanoDato = sizeof(uint32_t));

		tamanoTotal+=tamanoDato;

		memcpy(data + tamanoTotal, &estructuraOrigen->obtenido, tamanoDato = estructuraOrigen->size);

		tamanoTotal+=tamanoDato;

		paquete->data = data;

		return paquete;
}

t_stream * serializeStruct_job(t_struct_job * estructuraOrigen, int headerOperacion){

	t_stream * paquete = malloc(sizeof(t_stream));

	paquete->length = sizeof(t_header) 	+ strlen(estructuraOrigen->scriptTransformacion)
										+ strlen(estructuraOrigen->scriptReduccion)
										+ strlen(estructuraOrigen->archivoObjetivo)
										+ strlen(estructuraOrigen->archivoResultado)
										+ 1;

	char * data = crearDataConHeader(headerOperacion, paquete->length);

	int tamanoTotal = sizeof(t_header), tamanoDato = 0;

	memcpy(data + tamanoTotal, estructuraOrigen->scriptTransformacion , tamanoDato = strlen(estructuraOrigen->scriptTransformacion)+1);

	tamanoTotal+=tamanoDato;

	memcpy(data + tamanoTotal, estructuraOrigen->scriptReduccion , tamanoDato = strlen(estructuraOrigen->scriptReduccion)+1);

	tamanoTotal+=tamanoDato;

	memcpy(data + tamanoTotal, estructuraOrigen->archivoObjetivo , tamanoDato = strlen(estructuraOrigen->archivoObjetivo)+1);

	tamanoTotal+=tamanoDato;

	memcpy(data + tamanoTotal, estructuraOrigen->archivoResultado , tamanoDato = strlen(estructuraOrigen->archivoResultado)+1);

	tamanoTotal+=tamanoDato;

	paquete->data = data;

	return paquete;
}

t_header desempaquetarHeader(char * header){
	t_header estructuraHeader;

	int tamanoTotal = 0, tamanoDato = 0;
	memcpy(&estructuraHeader.tipoEstructura, header + tamanoTotal, tamanoDato = sizeof(uint8_t));
	tamanoTotal = tamanoDato;
	memcpy(&estructuraHeader.length, header + tamanoTotal, tamanoDato = sizeof(uint16_t));

	return estructuraHeader;
}

void * deserialize(uint8_t tipoEstructura, char * dataPaquete, uint16_t length){

	void * estructuraDestino;

	switch (tipoEstructura){
			case D_STRUCT_NUMERO:
				estructuraDestino = deserializeStruct_numero(dataPaquete, length);
				break;
			case D_STRUCT_CHAR:
				estructuraDestino = deserializeStruct_char(dataPaquete, length);
				break;
			case D_STRUCT_STRING:
				estructuraDestino = deserializeStruct_string(dataPaquete, length);
				break;
			case D_STRUCT_MALC:
				estructuraDestino = deserializeStruct_malc(dataPaquete, length);
				break;
			case D_STRUCT_IMPR:
				estructuraDestino = deserializeStruct_string(dataPaquete, length);
				break;
			case D_STRUCT_FIN_PROG:
				estructuraDestino = deserializeStruct_numero(dataPaquete, length);
				break;
			case D_STRUCT_ABORTAR_EJECUCION:
				estructuraDestino = deserializeStruct_numero(dataPaquete, length);
				break;
			case D_STRUCT_CONTINUAR_EJECUCION:
				estructuraDestino = deserializeStruct_numero(dataPaquete, length);
				break;
			case D_STRUCT_FIN_QUANTUM:
				estructuraDestino = deserializeStruct_numero(dataPaquete, length);
				break;
			case D_STRUCT_SOLICITAR_CODIGO:
				estructuraDestino = deserializeStruct_numero(dataPaquete, length);
				break;
			case D_STRUCT_PID:
				estructuraDestino = deserializeStruct_numero(dataPaquete, length);
				break;
			case D_STRUCT_LIBERAR_MEMORIA:
				estructuraDestino = deserializeStruct_numero(dataPaquete, length);
				break;
			case D_STRUCT_LECT:
				estructuraDestino = deserializeStruct_solLect(dataPaquete, length);
				break;
			case D_STRUCT_LECT_VAR:
				estructuraDestino = deserializeStruct_solLect(dataPaquete, length);
				break;
			case D_STRUCT_ESCRIBIR_HEAP:
				estructuraDestino = deserializeStruct_solEscr(dataPaquete, length);
				break;
			case D_STRUCT_LIBERAR_HEAP:
				estructuraDestino = deserializeStruct_solEscr(dataPaquete, length);
				break;
			case D_STRUCT_COMPACTAR_HEAP:
				estructuraDestino = deserializeStruct_solEscr(dataPaquete, length);
				break;
			case D_STRUCT_ERROR_HEAP_MAX:
				estructuraDestino = deserializeStruct_numero(dataPaquete, length);
				break;
			case D_STRUCT_ERROR_HEAP:
				estructuraDestino = deserializeStruct_numero(dataPaquete, length);
				break;
			case D_STRUCT_ABORT:
				estructuraDestino = deserializeStruct_numero(dataPaquete, length);
				break;
			case D_STRUCT_SOL_ESCR:
				estructuraDestino = deserializeStruct_solEscr(dataPaquete, length);
				break;
			case D_STRUCT_ESCRITURA_CODIGO:
				estructuraDestino = deserializeStruct_solEscr(dataPaquete, length);
				break;
			case D_STRUCT_WAIT:
				estructuraDestino = deserializeStruct_string(dataPaquete, length);
				break;
			case D_STRUCT_SIGNAL:
				estructuraDestino = deserializeStruct_string(dataPaquete, length);
				break;
			case D_STRUCT_OBTENER_COMPARTIDA:
				estructuraDestino = deserializeStruct_string(dataPaquete, length);
				break;
			case D_STRUCT_ARCHIVO_ESC:
				estructuraDestino = deserializeStruct_archivo_esc(dataPaquete, length);
				break;
			case D_STRUCT_ARCHIVO_LEC:
				estructuraDestino = deserializeStruct_archivo_esc(dataPaquete, length);
				break;
			case D_STRUCT_ARCHIVO_ABR:
				estructuraDestino = deserializeStruct_archivo_esc(dataPaquete, length);
				break;
			case D_STRUCT_ARCHIVO_BORRAR:
				estructuraDestino = deserializeStruct_archivo_esc(dataPaquete, length);
				break;
			case D_STRUCT_ARCHIVO_CER:
				estructuraDestino = deserializeStruct_solLect(dataPaquete, length);
				break;
			case D_STRUCT_ARCHIVO_BOR:
				estructuraDestino = deserializeStruct_solLect(dataPaquete, length);
				break;
			case D_STRUCT_ARCHIVO_MOV:
				estructuraDestino = deserializeStruct_solLect(dataPaquete, length);
				break;
			case D_STRUCT_BORRAR:
				estructuraDestino = deserializeStruct_borrar(dataPaquete, length);
				break;
			case D_STRUCT_OBTENER:
				estructuraDestino = deserializeStruct_obtener(dataPaquete, length);
				break;
			case D_STRUCT_JOB:
				estructuraDestino = deserializeStruct_job(dataPaquete, length);
	}

	return estructuraDestino;
}

t_struct_numero * deserializeStruct_numero(char * dataPaquete, uint16_t length){
	t_struct_numero * estructuraDestino = malloc(sizeof(t_struct_numero));

	memcpy(estructuraDestino, dataPaquete, sizeof(int32_t)); //copio el data del paquete a la estructura.

	return estructuraDestino;
}

t_struct_char * deserializeStruct_char(char * dataPaquete, uint16_t length){
	t_struct_char * estructuraDestino = malloc(sizeof(t_struct_char));

	memcpy(&estructuraDestino->letra, dataPaquete, sizeof(char)); //copio la letra a la estructura

	return estructuraDestino;
}

t_struct_string * deserializeStruct_string(char * dataPaquete, uint16_t length){
	t_struct_string * estructuraDestino = malloc(sizeof(t_struct_string));

	int tamanoTotal = 0, tamanoDato = 0;

	tamanoTotal = tamanoDato;

	for(tamanoDato = 1; (dataPaquete + tamanoTotal)[tamanoDato -1] != '\0';tamanoDato++); 	//incremento tamanoDato, hasta el tamaño del nombre.

	estructuraDestino->string = malloc(tamanoDato);
	memcpy(estructuraDestino->string, dataPaquete + tamanoTotal, tamanoDato); //copio el string a la estructura

	return estructuraDestino;
}

t_struct_malloc * deserializeStruct_malc(char * dataPaquete, uint16_t length){
	t_struct_malloc * estructuraDestino = malloc(sizeof(t_struct_malloc));

	int tamanoDato = 0, tamanoTotal = 0;

	memcpy(&estructuraDestino->PID,dataPaquete+tamanoTotal,tamanoDato=sizeof(uint32_t));

	tamanoTotal+= tamanoDato;

	memcpy(&estructuraDestino->tamano_segmento,dataPaquete+tamanoTotal,tamanoDato=sizeof(uint32_t));

	tamanoTotal+= tamanoDato;

	return estructuraDestino;

}

t_posicion_memoria * deserializeStruct_lect(char* dataPaquete, uint16_t length){
	t_posicion_memoria* estructuraDestino = malloc(sizeof(t_posicion_memoria));

	int tamanoDato = 0, tamanoTotal = 0;

	memcpy(&estructuraDestino->pagina,dataPaquete+tamanoTotal,tamanoDato=sizeof(uint32_t));

	tamanoTotal+= tamanoDato;

	memcpy(&estructuraDestino->offsetInstruccion,dataPaquete+tamanoTotal,tamanoDato=sizeof(uint32_t));

	tamanoTotal+= tamanoDato;

	memcpy(&estructuraDestino->longitudInstruccion,dataPaquete+tamanoTotal,tamanoDato=sizeof(uint32_t));

	tamanoTotal+= tamanoDato;

	return estructuraDestino;
}

t_struct_sol_escritura * deserializeStruct_solEscr(char* dataPaquete, uint16_t length){
	t_struct_sol_escritura* estructuraDestino = malloc(sizeof(t_struct_sol_escritura));

	int tamanoDato = 0, tamanoTotal = 0;

	memcpy(&estructuraDestino->pagina,dataPaquete+tamanoTotal,tamanoDato=sizeof(uint32_t));

	tamanoTotal+= tamanoDato;

	memcpy(&estructuraDestino->offset,dataPaquete+tamanoTotal,tamanoDato=sizeof(uint32_t));

	tamanoTotal+= tamanoDato;

	memcpy(&estructuraDestino->tamanio,dataPaquete+tamanoTotal,tamanoDato=sizeof(uint32_t));

	tamanoTotal+= tamanoDato;

	memcpy(&estructuraDestino->PID,dataPaquete+tamanoTotal,tamanoDato=sizeof(uint32_t));

	tamanoTotal+= tamanoDato;

	estructuraDestino->contenido = malloc(estructuraDestino->tamanio);
	memcpy(estructuraDestino->contenido,dataPaquete+tamanoTotal,tamanoDato = estructuraDestino->tamanio);

	tamanoTotal+= tamanoDato;

	return estructuraDestino;
}

t_struct_sol_lectura * deserializeStruct_solLect(char* dataPaquete, uint16_t length){
	t_struct_sol_lectura* estructuraDestino = malloc(sizeof(t_struct_sol_lectura));

	int tamanoDato = 0, tamanoTotal = 0;

	memcpy(&estructuraDestino->pagina,dataPaquete+tamanoTotal,tamanoDato=sizeof(uint32_t));

	tamanoTotal+= tamanoDato;

	memcpy(&estructuraDestino->offset,dataPaquete+tamanoTotal,tamanoDato=sizeof(uint32_t));

	tamanoTotal+= tamanoDato;

	memcpy(&estructuraDestino->contenido,dataPaquete+tamanoTotal,tamanoDato=sizeof(uint32_t));

	tamanoTotal+= tamanoDato;

	memcpy(&estructuraDestino->PID,dataPaquete+tamanoTotal,tamanoDato=sizeof(uint32_t));

	tamanoTotal+= tamanoDato;

	return estructuraDestino;
}

t_struct_archivo * deserializeStruct_archivo_esc(char* dataPaquete, uint16_t length){
	t_struct_archivo* estructuraDestino = malloc(sizeof(t_struct_archivo));

	int tamanoDato = 0, tamanoTotal = 0;

	memcpy(&estructuraDestino->fileDescriptor,dataPaquete+tamanoTotal,tamanoDato=sizeof(uint32_t));

	tamanoTotal+= tamanoDato;

	memcpy(&estructuraDestino->tamanio,dataPaquete+tamanoTotal,tamanoDato=sizeof(uint32_t));

	tamanoTotal+= tamanoDato;

	estructuraDestino->informacion= malloc(estructuraDestino->tamanio);
	memcpy(estructuraDestino->informacion, dataPaquete + tamanoTotal, tamanoDato = estructuraDestino->tamanio);

	tamanoTotal+= tamanoDato;

	memcpy(&estructuraDestino->pid,dataPaquete+tamanoTotal,tamanoDato=sizeof(uint32_t));

	tamanoTotal+= tamanoDato;

	memcpy(&estructuraDestino->flags.creacion,dataPaquete+tamanoTotal,tamanoDato=sizeof(bool));

	tamanoTotal+= tamanoDato;

	memcpy(&estructuraDestino->flags.escritura,dataPaquete+tamanoTotal,tamanoDato=sizeof(bool));

	tamanoTotal+= tamanoDato;

	memcpy(&estructuraDestino->flags.lectura,dataPaquete+tamanoTotal,tamanoDato=sizeof(bool));

	tamanoTotal+= tamanoDato;

	return estructuraDestino;
}

t_struct_borrar * deserializeStruct_borrar(char * dataPaquete, uint16_t length){

	t_struct_borrar * estructuraDestino = malloc(sizeof(t_struct_borrar));

	int tamanoTotal = 0, tamanoDato = 0;

	tamanoTotal = tamanoDato;

	for(tamanoDato = 1; (dataPaquete + tamanoTotal)[tamanoDato -1] != '\0';tamanoDato++); 	//incremento tamanoDato, hasta el tamaño del nombre.

	estructuraDestino->path = malloc(tamanoDato);
	memcpy(estructuraDestino->path, dataPaquete + tamanoTotal, tamanoDato); //copio el string a la estructura

	tamanoTotal+= tamanoDato;

	memcpy(&estructuraDestino->confirmacion,dataPaquete+tamanoTotal,tamanoDato=sizeof(uint32_t));

	tamanoTotal+= tamanoDato;

	return estructuraDestino;
}

t_struct_obtener * deserializeStruct_obtener(char * dataPaquete, uint16_t length){

	t_struct_obtener * estructuraDestino = malloc(sizeof(t_struct_obtener));

	int tamanoTotal = 0, tamanoDato = 0;

	tamanoTotal = tamanoDato;

	for(tamanoDato = 1; (dataPaquete + tamanoTotal)[tamanoDato -1] != '\0';tamanoDato++); 	//incremento tamanoDato, hasta el tamaño del nombre.

	estructuraDestino->path = malloc(tamanoDato);
	memcpy(estructuraDestino->path, dataPaquete + tamanoTotal, tamanoDato); //copio el string a la estructura

	tamanoTotal+= tamanoDato;

	memcpy(&estructuraDestino->confirmacion,dataPaquete+tamanoTotal,tamanoDato=sizeof(uint32_t));

	tamanoTotal+= tamanoDato;

	memcpy(&estructuraDestino->modo_lectura,dataPaquete+tamanoTotal,tamanoDato=sizeof(uint32_t));

	tamanoTotal+= tamanoDato;

	memcpy(&estructuraDestino->offset,dataPaquete+tamanoTotal,tamanoDato=sizeof(uint32_t));

	tamanoTotal+= tamanoDato;

	memcpy(&estructuraDestino->size,dataPaquete+tamanoTotal,tamanoDato=sizeof(uint32_t));

	tamanoTotal+= tamanoDato;

	tamanoDato=estructuraDestino->size;

	estructuraDestino->obtenido= malloc(estructuraDestino->size);

	memcpy(estructuraDestino->obtenido, dataPaquete + tamanoTotal, tamanoDato);

	return estructuraDestino;
}

t_struct_job * deserializeStruct_job(char * dataPaquete, uint16_t length){

	t_struct_job * estructuraDestino = malloc(sizeof(t_struct_job));

		int tamanoTotal = 0, tamanoDato = 0;

		tamanoTotal = tamanoDato;

		for(tamanoDato = 1; (dataPaquete + tamanoTotal)[tamanoDato -1] != '\0';tamanoDato++);
		estructuraDestino->scriptTransformacion = malloc(tamanoDato);
		memcpy(estructuraDestino->scriptTransformacion, dataPaquete + tamanoTotal, tamanoDato);

		tamanoTotal+= tamanoDato;

		for(tamanoDato = 1; (dataPaquete + tamanoTotal)[tamanoDato -1] != '\0';tamanoDato++);
		estructuraDestino->scriptReduccion = malloc(tamanoDato);
		memcpy(estructuraDestino->scriptReduccion, dataPaquete + tamanoTotal, tamanoDato);

		tamanoTotal+= tamanoDato;

		for(tamanoDato = 1; (dataPaquete + tamanoTotal)[tamanoDato -1] != '\0';tamanoDato++);
		estructuraDestino->archivoObjetivo = malloc(tamanoDato);
		memcpy(estructuraDestino->archivoObjetivo, dataPaquete + tamanoTotal, tamanoDato);

		tamanoTotal+= tamanoDato;

		for(tamanoDato = 1; (dataPaquete + tamanoTotal)[tamanoDato -1] != '\0';tamanoDato++);
		estructuraDestino->archivoResultado = malloc(tamanoDato);
		memcpy(estructuraDestino->archivoResultado, dataPaquete + tamanoTotal, tamanoDato);

		return estructuraDestino;
}

