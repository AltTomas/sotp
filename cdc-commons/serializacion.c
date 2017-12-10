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
			case D_STRUCT_JOBT:
				paquete = serializeStruct_jobT((t_struct_jobT *) estructuraOrigen,D_STRUCT_JOBT);
				break;
			case D_STRUCT_JOBR:
				paquete = serializeStruct_jobR((t_struct_jobR *) estructuraOrigen,D_STRUCT_JOBR);
				break;
			case D_STRUCT_NODO_TRANSFORMACION:
				paquete = serializeStruct_Nodo_Transformacion((t_infoNodo_transformacion *) estructuraOrigen, D_STRUCT_NODO_TRANSFORMACION);
				break;
			case D_STRUCT_NODOS_REDUCCION_LOCAL:
				paquete = serializeStruct_Nodos_Reduccion_L((t_infoNodo_reduccionLocal*) estructuraOrigen, D_STRUCT_NODOS_REDUCCION_LOCAL);
				break;
			case D_STRUCT_NODOS_REDUCCION_GLOBAL:
				paquete = serializeStruct_Nodos_Reduccion_G((t_infoNodo_reduccionGlobal*) estructuraOrigen, D_STRUCT_NODOS_REDUCCION_GLOBAL);
				break;
			case D_STRUCT_NODO_ESCLAVO:
				paquete = serializeStruct_NodoEsclavo((t_struct_nodoEsclavo*) estructuraOrigen, D_STRUCT_NODO_ESCLAVO);
				break;
			case D_STRUCT_BLOQUE_FS_YAMA:
				paquete = serializeStruct_bloque_fs_yama((t_struct_bloques *) estructuraOrigen, D_STRUCT_BLOQUE_FS_YAMA);
				break;
			case FS_DATANODE_ALMACENAR_BLOQUE:
				paquete = serializeStruct_fs_datanode_almacenar((t_almacenar_bloque *) estructuraOrigen, FS_DATANODE_ALMACENAR_BLOQUE);
				break;
			case D_STRUCT_INFO_NODO:
				paquete = serializeStruct_datanode((t_struct_datanode*) estructuraOrigen, D_STRUCT_INFO_NODO);
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

t_stream * serializeStruct_bloque_fs_yama(t_struct_bloques* estructuraOrigen, int headerOperacion){

	t_stream * paquete = malloc(sizeof(t_stream));

	paquete->length = sizeof(t_header) + strlen(estructuraOrigen->ipNodoOriginal)+1
									   + strlen(estructuraOrigen->ipNodoCopia)+1;


	char * data = crearDataConHeader(headerOperacion, paquete->length);

	int tamanoTotal = sizeof(t_header), tamanoDato = 0;

	memcpy(data + tamanoTotal, &estructuraOrigen->finalBloque , tamanoDato = sizeof(int));

	tamanoTotal+=tamanoDato;

	memcpy(data + tamanoTotal, &estructuraOrigen->numNodoOriginal , tamanoDato = sizeof(int));

	tamanoTotal+=tamanoDato;

	memcpy(data + tamanoTotal, &estructuraOrigen->numBloqueOriginal , tamanoDato = sizeof(int));

	tamanoTotal+=tamanoDato;

	memcpy(data + tamanoTotal, estructuraOrigen->ipNodoOriginal , tamanoDato = strlen(estructuraOrigen->ipNodoOriginal)+1);

	tamanoTotal+=tamanoDato;

	memcpy(data + tamanoTotal,&estructuraOrigen->puertoNodoOriginal , tamanoDato = sizeof(int));

	tamanoTotal+=tamanoDato;

	memcpy(data + tamanoTotal, &estructuraOrigen->numNodoCopia , tamanoDato = sizeof(int));

	tamanoTotal+=tamanoDato;

	memcpy(data + tamanoTotal, &estructuraOrigen->numBloqueCopia , tamanoDato = sizeof(int));

	tamanoTotal+=tamanoDato;

	memcpy(data + tamanoTotal, estructuraOrigen->ipNodoCopia , tamanoDato = strlen(estructuraOrigen->ipNodoCopia)+1);

	tamanoTotal+=tamanoDato;

	memcpy(data + tamanoTotal, &estructuraOrigen->puertoNodoCopia , tamanoDato = sizeof(int));

	tamanoTotal+=tamanoDato;

	paquete->data = data;

	return paquete;
}

t_stream * serializeStruct_jobT(t_struct_jobT * estructuraOrigen, int headerOperacion){

	t_stream * paquete = malloc(sizeof(t_stream));

	paquete->length = sizeof(t_header) 	+ sizeof(int)*2
										+ strlen(estructuraOrigen->scriptTransformacion) + 1
										+ strlen(estructuraOrigen->pathOrigen) + 1
										+ strlen(estructuraOrigen->pathTemporal)+ 1;

	char * data = crearDataConHeader(headerOperacion, paquete->length);

	int tamanoTotal = sizeof(t_header), tamanoDato = 0;

	memcpy(data + tamanoTotal, estructuraOrigen->scriptTransformacion , tamanoDato = strlen(estructuraOrigen->scriptTransformacion)+1);

	tamanoTotal+=tamanoDato;

	memcpy(data + tamanoTotal, estructuraOrigen->pathOrigen , tamanoDato = strlen(estructuraOrigen->pathOrigen)+1);

	tamanoTotal+=tamanoDato;

	memcpy(data + tamanoTotal, estructuraOrigen->pathTemporal , tamanoDato = strlen(estructuraOrigen->pathTemporal)+1);

	tamanoTotal+=tamanoDato;

	memcpy(data + tamanoTotal, &estructuraOrigen->bloque , tamanoDato = sizeof(int));

	tamanoTotal+=tamanoDato;

	memcpy(data + tamanoTotal, &estructuraOrigen->bytesOcupadosBloque , tamanoDato = sizeof(int));

	tamanoTotal+=tamanoDato;

	paquete->data = data;

	return paquete;
}

int calcularTamanioBloques(t_list* bloqueDN){

	int tamanio = 0, i;

	for(i=0;i<list_size(bloqueDN);i++){

		int tamanioBloque = sizeof(int)*2;

		int tamanioParcial = tamanioBloque;

		tamanio += tamanioParcial;

	}

	return tamanio;
}

t_stream * serializeStruct_datanode(t_struct_datanode* estructuraOrigen, int headerOperacion){

	t_stream * paquete = malloc(sizeof(t_stream));

	int tamaniobloques = calcularTamanioBloques(estructuraOrigen->bloqueDN);

	paquete->length = sizeof(t_header)  	+ strlen(estructuraOrigen->ipDN) + 1
											+ tamaniobloques
											+ sizeof(int) *3
											+ strlen(estructuraOrigen->nomDN) + 1;

	char * data = crearDataConHeader(headerOperacion, paquete->length);

	int tamanoTotal = sizeof(t_header), tamanoDato = 0;

	memcpy(data + tamanoTotal, &estructuraOrigen->ipDN , tamanoDato = strlen(estructuraOrigen->ipDN)+1);

	tamanoTotal+=tamanoDato;

	memcpy(data + tamanoTotal, &estructuraOrigen->puertoDN , tamanoDato = sizeof(int));

	tamanoTotal+=tamanoDato;

	memcpy(data + tamanoTotal, &estructuraOrigen->nomDN , tamanoDato = strlen(estructuraOrigen->nomDN) + 1);

	tamanoTotal+=tamanoDato;

	int contadorBloques = 0;

	int cantidadBloques = estructuraOrigen->bloquesTotales; //chequear

	while(contadorBloques < cantidadBloques){

		t_struct_bloqueDN* bloque = list_get(estructuraOrigen->bloqueDN,contadorBloques);

		memcpy(data + tamanoTotal, &bloque->estado , tamanoDato = sizeof(int));
		tamanoTotal+=tamanoDato;

		memcpy(data + tamanoTotal, &bloque->numBloque , tamanoDato = sizeof(int));
		tamanoTotal+=tamanoDato;
	}

	paquete->data = data;

	return paquete;
}

int calcularTamanioTemporales(t_list* temporales){

	int tamanio = 0, i;

	for(i=0;i<list_size(temporales);i++){

		char* temporal = list_get(temporales,i);

		int tamanioTemporal = strlen(temporal)+1;

		int tamanioParcial = tamanioTemporal;

		tamanio += tamanioParcial;

	}

	return tamanio;
}

t_stream * serializeStruct_jobR(t_struct_jobR * estructuraOrigen, int headerOperacion){

	t_stream * paquete = malloc(sizeof(t_stream));

	int tamanioTemporales = calcularTamanioTemporales(estructuraOrigen->pathTemp);

	paquete->length = sizeof(t_header)	+ sizeof(uint16_t)
										+ strlen(estructuraOrigen->pathTempFinal) + 1
										+ strlen(estructuraOrigen->scriptReduccion) + 1
										+ tamanioTemporales;

	char * data = crearDataConHeader(headerOperacion, paquete->length);

	int tamanoTotal = sizeof(t_header), tamanoDato = 0;

	memcpy(data + tamanoTotal, &estructuraOrigen->cantidadTemporales , tamanoDato = sizeof(uint16_t));

	tamanoTotal+=tamanoDato;

	memcpy(data + tamanoTotal, estructuraOrigen->scriptReduccion , tamanoDato = strlen(estructuraOrigen->scriptReduccion)+1);

	tamanoTotal+=tamanoDato;

	memcpy(data + tamanoTotal, estructuraOrigen->pathTempFinal , tamanoDato = strlen(estructuraOrigen->pathTempFinal)+1);

	tamanoTotal+=tamanoDato;

	int contadorTemporales = 0;
	while(contadorTemporales < estructuraOrigen->cantidadTemporales){

		char* temporal = list_get(estructuraOrigen->pathTemp,contadorTemporales);

		memcpy(data + tamanoTotal, temporal , tamanoDato = strlen(temporal)+1);
		tamanoTotal+=tamanoDato;
	}

	paquete->data = data;

	return paquete;
}

t_stream * serializeStruct_NodoEsclavo(t_struct_nodoEsclavo* estructuraOrigen, int headerOperacion){

	t_stream* paquete = malloc(sizeof(t_stream));

	paquete->length = 	sizeof(t_header) +
						sizeof(uint32_t) +
						strlen(estructuraOrigen->ip) + 1 +
						strlen(estructuraOrigen->nombreTemporal) + 1 +
						sizeof(int);

	char* data = crearDataConHeader(headerOperacion, paquete->length);

	int tamDato = 0;
	int tamTot = sizeof(t_header);

	memcpy(data + tamTot, estructuraOrigen->ip , tamDato = strlen(estructuraOrigen->ip)+1);
	tamTot+=tamDato;

	memcpy(data + tamTot, estructuraOrigen->nombreTemporal , tamDato = strlen(estructuraOrigen->nombreTemporal)+1);
	tamTot+=tamDato;

	memcpy(data + tamTot, &estructuraOrigen->puerto , tamDato = sizeof(int));
	tamTot+=tamDato;

	memcpy(data + tamTot, &estructuraOrigen->idNodo , tamDato = sizeof(uint32_t));
	tamTot+=tamDato;

	paquete->data = data;

	return paquete;
}

t_stream * serializeStruct_Nodo_Transformacion(t_infoNodo_transformacion * estructuraOrigen, int headerOperacion){

	t_stream * paquete = malloc(sizeof(t_stream));

	paquete->length = 	sizeof(t_header) +
						3*sizeof(int) +
						sizeof(uint32_t) +
						strlen(estructuraOrigen->ip)+ 1 +
						strlen(estructuraOrigen->nombreTemporal) + 1;

	char * data = crearDataConHeader(headerOperacion, paquete->length);

	int tamanoTotal = sizeof(t_header), tamanoDato = 0;

	memcpy(data + tamanoTotal, &estructuraOrigen->bytesOcupados , tamanoDato = sizeof(int));
	tamanoTotal += tamanoDato;

	memcpy(data + tamanoTotal, &estructuraOrigen->idNodo , tamanoDato = sizeof(uint32_t));
	tamanoTotal += tamanoDato;

	memcpy(data + tamanoTotal, estructuraOrigen->ip , tamanoDato = strlen(estructuraOrigen->ip)+1);
	tamanoTotal += tamanoDato;

	memcpy(data + tamanoTotal, estructuraOrigen->nombreTemporal , tamanoDato = strlen(estructuraOrigen->nombreTemporal)+1);
	tamanoTotal += tamanoDato;

	memcpy(data + tamanoTotal, &estructuraOrigen->numBloque , tamanoDato = sizeof(int));
	tamanoTotal += tamanoDato;

	memcpy(data + tamanoTotal, &estructuraOrigen->puerto , tamanoDato = sizeof(int));
	tamanoTotal += tamanoDato;

	paquete->data = data;

	return paquete;
}

int calcularTamanioTemporalesParaReduccion(t_list* temporales){

	int tamanio = 0, i;

	for(i = 0; i < list_size(temporales); i++){
		char* temporal = list_get(temporales,i);

		int tamanioTemporal = strlen(temporal)+1;
		tamanio += tamanioTemporal;
	}

	return tamanio;
}

t_stream * serializeStruct_Nodos_Reduccion_L(t_infoNodo_reduccionLocal* estructuraOrigen, int headerOperacion){

	t_stream * paquete = malloc(sizeof(t_stream));

	int tamanioListaTemporales = calcularTamanioTemporalesParaReduccion(estructuraOrigen->pathTemp);

	paquete->length = 	sizeof(t_header) +
						sizeof(int) + 2*sizeof(uint32_t) +
						strlen(estructuraOrigen->ip)+ 1 +
						strlen(estructuraOrigen->pathTempFinal) + 1 +
						tamanioListaTemporales;

	char * data = crearDataConHeader(headerOperacion, paquete->length);

	int tamanoTotal = sizeof(t_header), tamanoDato = 0;

	memcpy(data + tamanoTotal, &estructuraOrigen->cantidadTemporales , tamanoDato = sizeof(uint32_t));
	tamanoTotal += tamanoDato;

	memcpy(data + tamanoTotal, &estructuraOrigen->idNodo , tamanoDato = sizeof(uint32_t));
	tamanoTotal += tamanoDato;

	memcpy(data + tamanoTotal, estructuraOrigen->ip , tamanoDato = strlen(estructuraOrigen->ip)+1);
	tamanoTotal+=tamanoDato;

	memcpy(data + tamanoTotal, estructuraOrigen->pathTempFinal , tamanoDato = strlen(estructuraOrigen->pathTempFinal)+1);
	tamanoTotal+=tamanoDato;

	memcpy(data + tamanoTotal , &estructuraOrigen->puerto, tamanoDato = sizeof(int));
	tamanoTotal += tamanoDato;

	int contadorTemporales = 0;

	while (contadorTemporales < estructuraOrigen->cantidadTemporales) {

		char* temporal = list_get(estructuraOrigen->pathTemp, contadorTemporales);

		memcpy(data + tamanoTotal, temporal , tamanoDato = strlen(temporal)+1);
		tamanoTotal+=tamanoDato;

		contadorTemporales++;
	}

	paquete->data = data;

	return paquete;
}


t_stream * serializeStruct_Nodos_Reduccion_G(t_infoNodo_reduccionGlobal* estructuraOrigen, int headerOperacion){

	t_stream* paquete = malloc(sizeof(t_stream));

	paquete->length = sizeof(t_header) + sizeof(int) + sizeof(uint32_t) + sizeof(bool) +
						strlen(estructuraOrigen->ip) + 1 +
						strlen(estructuraOrigen->pathFinal)+ 1 +
						strlen(estructuraOrigen->pathTemporal) + 1;

	char * data = crearDataConHeader(headerOperacion, paquete->length);

	int tamanoTotal = sizeof(t_header) , tamanoDato = 0;

	memcpy(data + tamanoTotal , &estructuraOrigen->encargado, tamanoDato = sizeof(bool));
	tamanoTotal += tamanoDato;

	memcpy(data + tamanoTotal , estructuraOrigen->ip, tamanoDato = strlen(estructuraOrigen->ip) + 1);
	tamanoTotal += tamanoDato;

	memcpy(data + tamanoTotal , estructuraOrigen->pathFinal, tamanoDato = strlen(estructuraOrigen->pathFinal) + 1);
	tamanoTotal += tamanoDato;

	memcpy(data + tamanoTotal , estructuraOrigen->pathTemporal, tamanoDato = strlen(estructuraOrigen->pathTemporal) + 1);
	tamanoTotal += tamanoDato;

	memcpy(data + tamanoTotal , &estructuraOrigen->puerto, tamanoDato = sizeof(int));
	tamanoTotal += tamanoDato;

	memcpy(data + tamanoTotal , &estructuraOrigen->idNodo, tamanoDato = sizeof(uint32_t));
	tamanoTotal += tamanoDato;

	paquete->data = data;

	return paquete;
}

uint32_t calcularTamanioListaNodos (t_list* nodos){

	int tamanio = 0, i;
	t_infoNodo_reduccionGlobal * nodo = malloc(sizeof(t_infoNodo_reduccionGlobal));

	for(i=0;i<list_size(nodos);i++){

		nodo = list_get(nodos,i);

		int tamanioIP = sizeof(char)*strlen(nodo->ip);
		int tamanioPuerto = sizeof(int);
		int tamanioEncargado = sizeof(bool);
		int tamanioRutaTemporal = sizeof(char)*strlen(nodo->pathTemporal);
		int tamanioRutaFinal = sizeof(char)*strlen(nodo->pathFinal);

		int tamanioParcial = tamanioIP + tamanioPuerto + tamanioEncargado + tamanioRutaTemporal + tamanioRutaFinal;

		tamanio += tamanioParcial;

	}

	return tamanio;
}

t_stream * serializeStruct_NodosEsclavos(t_struct_jobRG* estructuraOrigen, int headerOperacion){

	t_stream* paquete = malloc(sizeof(t_stream));

	uint32_t tamanioListaNodos = calcularTamanioListaNodos(estructuraOrigen->nodos);

	paquete->length = sizeof(t_header) + sizeof(uint32_t) + strlen(estructuraOrigen->scriptReduccion) + tamanioListaNodos + 1;

	char* data = crearDataConHeader(headerOperacion, paquete->length);

	int tamDato = 0;
	int tamTot = sizeof(t_header);
	int contadorNodos = 0;

	memcpy(data + tamTot, &estructuraOrigen->cantidadNodos , tamDato = sizeof(uint32_t));
	tamTot+=tamDato;

	memcpy(data + tamTot, estructuraOrigen->scriptReduccion , tamDato = strlen(estructuraOrigen->scriptReduccion)+1);
	tamTot+=tamDato;

	while (contadorNodos < estructuraOrigen->nodos->elements_count){

		t_struct_nodoEsclavo* nodo = (t_struct_nodoEsclavo*) list_get(estructuraOrigen->nodos, contadorNodos);

		memcpy(data + tamTot, &nodo->ip, tamDato= strlen(nodo->ip) + 1);
		tamTot += tamDato;

		memcpy(data + tamTot, &nodo->puerto, tamDato= sizeof(int));
		tamTot += tamDato;

		memcpy(data + tamTot, &nodo->nombreTemporal, tamDato= strlen(nodo->nombreTemporal) + 1);
		tamTot += tamDato;

		contadorNodos++;
	}

	paquete->data = data;

	return paquete;
}

t_stream * serializeStruct_fs_datanode_pedido_almacenar(t_pedido_almacenar_bloque* estructuraOrigen, int headerOperacion){

	t_stream* paquete = malloc(sizeof(t_stream));

	paquete->length = sizeof(t_header) + sizeof(int)*2 + strlen(estructuraOrigen->contenidoBloque) + 1;

	char* data = crearDataConHeader(headerOperacion, paquete->length);

	int tamDato = 0;
	int tamTot = sizeof(t_header);

	memcpy(data + tamTot, &estructuraOrigen->bloqueArchivo , tamDato = sizeof(int));
	tamTot+=tamDato;

	memcpy(data + tamTot, &estructuraOrigen->bytesOcupados , tamDato = sizeof(int));
	tamTot+=tamDato;

	memcpy(data + tamTot, estructuraOrigen->contenidoBloque , tamDato = strlen(estructuraOrigen->contenidoBloque)+1);
	tamTot+=tamDato;

	paquete->data = data;

	return paquete;
}

t_stream* serializeStruct_fs_datanode_almacenar(t_almacenar_bloque * estructuraOrigen, int headerOperacion){

	t_stream* paquete = malloc(sizeof(t_stream));

	paquete->length = sizeof(t_header) + sizeof(int) + strlen(estructuraOrigen->nombreNodo) + 1;

	char* data = crearDataConHeader(headerOperacion, paquete->length);

	int tamDato = 0;
	int tamTot = sizeof(t_header);

	memcpy(data + tamTot, &estructuraOrigen->bloqueNodo , tamDato = sizeof(int));
	tamTot+=tamDato;

	memcpy(data + tamTot, estructuraOrigen->nombreNodo , tamDato = strlen(estructuraOrigen->nombreNodo)+1);
	tamTot+=tamDato;

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
			case D_STRUCT_JOBT:
				estructuraDestino = deserializeStruct_jobT(dataPaquete, length);
				break;
			case D_STRUCT_JOBR:
				estructuraDestino = deserializeStruct_jobR(dataPaquete, length);
				break;
			case D_STRUCT_NODO_TRANSFORMACION:
				estructuraDestino = deserializeStruct_Nodo_Transformacion(dataPaquete, length);
				break;
			case D_STRUCT_NODOS_REDUCCION_LOCAL:
				estructuraDestino = deserializeStruct_Nodos_Reduccion_L(dataPaquete, length);
				break;
			case D_STRUCT_NODOS_REDUCCION_GLOBAL:
				estructuraDestino = deserializeStruct_Nodos_Reduccion_G(dataPaquete, length);
				break;
			case D_STRUCT_NODO_ESCLAVO:
				estructuraDestino = deserializeStruct_NodoEsclavo(dataPaquete, length);
				break;
			case D_STRUCT_BLOQUE_FS_YAMA:
				estructuraDestino = deserializeStruct_bloque_fs_yama(dataPaquete, length);
				break;
			case FS_DATANODE_ALMACENAR_BLOQUE:
				estructuraDestino = deserializeStruct_fs_datanode_almacenar(dataPaquete, length);
				break;
			case D_STRUCT_INFO_NODO:
				estructuraDestino = deserializeStruct_datanode(dataPaquete, length);
				break;
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

t_struct_bloques * deserializeStruct_bloque_fs_yama(char * dataPaquete, uint16_t length){

	t_struct_bloques * estructuraDestino = malloc(sizeof(t_struct_bloques));

		int tamanoTotal = 0, tamanoDato = 0;

		tamanoTotal = tamanoDato;

		memcpy(&estructuraDestino->finalBloque,dataPaquete+tamanoTotal,tamanoDato=sizeof(int));

		tamanoTotal+= tamanoDato;

		memcpy(&estructuraDestino->numNodoOriginal,dataPaquete+tamanoTotal,tamanoDato=sizeof(int));

		tamanoTotal+= tamanoDato;

		memcpy(&estructuraDestino->numBloqueOriginal,dataPaquete+tamanoTotal,tamanoDato=sizeof(int));

		tamanoTotal+= tamanoDato;

		for(tamanoDato = 1; (dataPaquete + tamanoTotal)[tamanoDato -1] != '\0';tamanoDato++);
		estructuraDestino->ipNodoOriginal = malloc(tamanoDato);
		memcpy(estructuraDestino->ipNodoOriginal, dataPaquete + tamanoTotal, tamanoDato);

		tamanoTotal+= tamanoDato;

		memcpy(&estructuraDestino->puertoNodoOriginal,dataPaquete+tamanoTotal,tamanoDato=sizeof(int));

		tamanoTotal+= tamanoDato;

		memcpy(&estructuraDestino->numNodoCopia,dataPaquete+tamanoTotal,tamanoDato=sizeof(int));

		tamanoTotal+= tamanoDato;

		memcpy(&estructuraDestino->numBloqueCopia,dataPaquete+tamanoTotal,tamanoDato=sizeof(int));

		tamanoTotal+= tamanoDato;

		for(tamanoDato = 1; (dataPaquete + tamanoTotal)[tamanoDato -1] != '\0';tamanoDato++);
		estructuraDestino->ipNodoCopia = malloc(tamanoDato);
		memcpy(estructuraDestino->ipNodoCopia, dataPaquete + tamanoTotal, tamanoDato);

		tamanoTotal+= tamanoDato;

		memcpy(&estructuraDestino->puertoNodoCopia,dataPaquete+tamanoTotal,tamanoDato=sizeof(int));

		return estructuraDestino;
}

t_struct_datanode * deserializeStruct_datanode(char * dataPaquete, uint16_t length){

			t_struct_datanode * estructuraDestino = malloc(sizeof(t_struct_datanode));

			int tamanoTotal = 0, tamanoDato = 0;

			tamanoTotal = tamanoDato;

			for(tamanoDato = 1; (dataPaquete + tamanoTotal)[tamanoDato -1] != '\0';tamanoDato++);
			estructuraDestino->ipDN = malloc(tamanoDato);
			memcpy(estructuraDestino->ipDN, dataPaquete + tamanoTotal, tamanoDato);

			tamanoTotal+= tamanoDato;

			memcpy(&estructuraDestino->puertoDN,dataPaquete+tamanoTotal,tamanoDato=sizeof(int));

			tamanoTotal+= tamanoDato;

			memcpy(&estructuraDestino->nomDN,dataPaquete+tamanoTotal,tamanoDato=sizeof(int));

			tamanoTotal+= tamanoDato;

			int contadorBloques = 0;

			int cantidadBloques = list_size(estructuraDestino->bloqueDN); //chequear

			while(contadorBloques < cantidadBloques){
				char* temporal = list_get(estructuraDestino->bloqueDN,contadorBloques);

				for(tamanoDato = 1; (dataPaquete + tamanoTotal)[tamanoDato -1] != '\0';tamanoDato++);
				temporal = (char*)malloc(tamanoDato);
				memcpy(temporal, dataPaquete + tamanoTotal, tamanoDato);
				tamanoTotal+= tamanoDato;
			}

			return estructuraDestino;
	}




t_struct_jobT * deserializeStruct_jobT(char * dataPaquete, uint16_t length){

	t_struct_jobT * estructuraDestino = malloc(sizeof(t_struct_jobT));

		int tamanoTotal = 0, tamanoDato = 0;

		memcpy(&estructuraDestino->bloque,dataPaquete+tamanoTotal,tamanoDato=sizeof(int));
		tamanoTotal+= tamanoDato;

		memcpy(&estructuraDestino->bytesOcupadosBloque,dataPaquete+tamanoTotal,tamanoDato=sizeof(int));
		tamanoTotal+= tamanoDato;

		for(tamanoDato = 1; (dataPaquete + tamanoTotal)[tamanoDato -1] != '\0';tamanoDato++);
		estructuraDestino->scriptTransformacion = malloc(tamanoDato);
		memcpy(estructuraDestino->scriptTransformacion, dataPaquete + tamanoTotal, tamanoDato);
		tamanoTotal+= tamanoDato;

		for(tamanoDato = 1; (dataPaquete + tamanoTotal)[tamanoDato -1] != '\0';tamanoDato++);
		estructuraDestino->pathOrigen = malloc(tamanoDato);
		memcpy(estructuraDestino->pathOrigen, dataPaquete + tamanoTotal, tamanoDato);
		tamanoTotal+= tamanoDato;

		for(tamanoDato = 1; (dataPaquete + tamanoTotal)[tamanoDato -1] != '\0';tamanoDato++);
		estructuraDestino->pathTemporal = malloc(tamanoDato);
		memcpy(estructuraDestino->pathTemporal, dataPaquete + tamanoTotal, tamanoDato);
		tamanoTotal+= tamanoDato;

		return estructuraDestino;
}

t_struct_jobR * deserializeStruct_jobR(char * dataPaquete, uint16_t length){

	t_struct_jobR * estructuraDestino = malloc(sizeof(t_struct_jobR));

		int tamanoTotal = 0, tamanoDato = 0;

		memcpy(&estructuraDestino->cantidadTemporales,dataPaquete+tamanoTotal,tamanoDato=sizeof(uint16_t));
		tamanoTotal+= tamanoDato;

		for(tamanoDato = 1; (dataPaquete + tamanoTotal)[tamanoDato -1] != '\0';tamanoDato++);
		estructuraDestino->scriptReduccion = malloc(tamanoDato);
		memcpy(estructuraDestino->scriptReduccion, dataPaquete + tamanoTotal, tamanoDato);
		tamanoTotal+= tamanoDato;

		for(tamanoDato = 1; (dataPaquete + tamanoTotal)[tamanoDato -1] != '\0';tamanoDato++);
		estructuraDestino->pathTempFinal = malloc(tamanoDato);
		memcpy(estructuraDestino->pathTempFinal, dataPaquete + tamanoTotal, tamanoDato);
		tamanoTotal+= tamanoDato;

		int contadorTemporales = 0;
		while(contadorTemporales < estructuraDestino->cantidadTemporales){
			char* temporal = list_get(estructuraDestino->pathTemp,contadorTemporales);

			for(tamanoDato = 1; (dataPaquete + tamanoTotal)[tamanoDato -1] != '\0';tamanoDato++);
			temporal = (char*)malloc(tamanoDato);
			memcpy(temporal, dataPaquete + tamanoTotal, tamanoDato);
			tamanoTotal+= tamanoDato;
		}

		return estructuraDestino;
}

t_struct_nodoEsclavo * deserializeStruct_NodoEsclavo(char * dataPaquete, uint16_t length){

	t_struct_nodoEsclavo * estructuraDestino = malloc(length);

		int tamanoTotal = 0, tamanoDato = 0;

		tamanoTotal+= tamanoDato;

		for(tamanoDato = 1; (dataPaquete + tamanoTotal)[tamanoDato -1] != '\0';tamanoDato++);
		estructuraDestino->ip = malloc(tamanoDato);
		memcpy(estructuraDestino->ip, dataPaquete + tamanoTotal, tamanoDato);
		tamanoTotal+= tamanoDato;

		for(tamanoDato = 1; (dataPaquete + tamanoTotal)[tamanoDato -1] != '\0';tamanoDato++);
		estructuraDestino->nombreTemporal = malloc(tamanoDato);
		memcpy(estructuraDestino->nombreTemporal, dataPaquete + tamanoTotal, tamanoDato);
		tamanoTotal+= tamanoDato;

		memcpy(&estructuraDestino->puerto,dataPaquete+tamanoTotal,tamanoDato=sizeof(int));
		tamanoTotal+= tamanoDato;

		memcpy(&estructuraDestino->idNodo,dataPaquete+tamanoTotal,tamanoDato=sizeof(uint32_t));
		tamanoTotal+= tamanoDato;

		return estructuraDestino;
}

t_infoNodo_transformacion* deserializeStruct_Nodo_Transformacion(char * dataPaquete, uint16_t length){

	t_infoNodo_transformacion* estructuraDestino = malloc(sizeof(t_infoNodo_transformacion));

	int tamanoTotal = 0, tamanoDato = 0;

	memcpy(&estructuraDestino->bytesOcupados,dataPaquete+tamanoTotal,tamanoDato=sizeof(int));
	tamanoTotal+= tamanoDato;

	memcpy(&estructuraDestino->idNodo,dataPaquete+tamanoTotal,tamanoDato=sizeof(uint32_t));
	tamanoTotal+= tamanoDato;

	for(tamanoDato = 1; (dataPaquete + tamanoTotal)[tamanoDato -1] != '\0';tamanoDato++); 	//incremento tamanoDato, hasta el tamaño del nombre.
	estructuraDestino->ip = (char*)malloc(tamanoDato);
	memcpy(estructuraDestino->ip, dataPaquete + tamanoTotal, tamanoDato); //copio el string a la estructura
	tamanoTotal+= tamanoDato;

	for(tamanoDato = 1; (dataPaquete + tamanoTotal)[tamanoDato -1] != '\0';tamanoDato++); 	//incremento tamanoDato, hasta el tamaño del nombre.
	estructuraDestino->nombreTemporal = (char*)malloc(tamanoDato);
	memcpy(estructuraDestino->nombreTemporal, dataPaquete + tamanoTotal, tamanoDato); //copio el string a la estructura
	tamanoTotal+= tamanoDato;

	memcpy(&estructuraDestino->numBloque,dataPaquete+tamanoTotal,tamanoDato=sizeof(int));
	tamanoTotal+= tamanoDato;

	memcpy(&estructuraDestino->puerto,dataPaquete+tamanoTotal,tamanoDato=sizeof(int));
	tamanoTotal+= tamanoDato;

	return estructuraDestino;
}

t_infoNodo_reduccionLocal* deserializeStruct_Nodos_Reduccion_L(char * dataPaquete, uint16_t length){

	t_infoNodo_reduccionLocal* estructuraDestino = malloc(length);

	int tamanoTotal = 0, tamanoDato = 0;

	memcpy(&estructuraDestino->cantidadTemporales,dataPaquete+tamanoTotal,tamanoDato=sizeof(uint32_t));
	tamanoTotal+= tamanoDato;

	memcpy(&estructuraDestino->idNodo,dataPaquete+tamanoTotal,tamanoDato=sizeof(uint32_t));
	tamanoTotal+= tamanoDato;

	for(tamanoDato = 1; (dataPaquete + tamanoTotal)[tamanoDato -1] != '\0';tamanoDato++); 	//incremento tamanoDato, hasta el tamaño del nombre.
	estructuraDestino->ip = (char*)malloc(tamanoDato);
	memcpy(estructuraDestino->ip, dataPaquete + tamanoTotal, tamanoDato); //copio el string a la estructura
	tamanoTotal+= tamanoDato;

	for(tamanoDato = 1; (dataPaquete + tamanoTotal)[tamanoDato -1] != '\0';tamanoDato++); 	//incremento tamanoDato, hasta el tamaño del nombre.
	estructuraDestino->pathTempFinal = (char*)malloc(tamanoDato);
	memcpy(estructuraDestino->pathTempFinal, dataPaquete + tamanoTotal, tamanoDato); //copio el string a la estructura
	tamanoTotal+= tamanoDato;

	memcpy(&estructuraDestino->puerto, dataPaquete + tamanoTotal, tamanoDato = sizeof(int));
	tamanoTotal += tamanoDato;

	estructuraDestino->pathTemp = list_create();
	int contadorStrings = 0;

	while(contadorStrings < estructuraDestino->cantidadTemporales){

		for(tamanoDato = 1; (dataPaquete + tamanoTotal)[tamanoDato -1] != '\0';tamanoDato++);
		char* string =(char*)malloc(tamanoDato);
		memcpy(string, dataPaquete + tamanoTotal, tamanoDato);
		tamanoTotal += tamanoDato;

		list_add(estructuraDestino->pathTemp,string);


		contadorStrings++;
	}

	return estructuraDestino;

}

t_infoNodo_reduccionGlobal* deserializeStruct_Nodos_Reduccion_G(char * dataPaquete, uint16_t length){

	t_infoNodo_reduccionGlobal* estructuraDestino = malloc(length);

	int tamanoTotal = 0, tamanoDato = 0;

	memcpy(&estructuraDestino->encargado,dataPaquete+tamanoTotal,tamanoDato=sizeof(bool));
	tamanoTotal+= tamanoDato;

	for(tamanoDato = 1; (dataPaquete + tamanoTotal)[tamanoDato -1] != '\0';tamanoDato++); 	//incremento tamanoDato, hasta el tamaño del nombre.
	estructuraDestino->ip = (char*)malloc(tamanoDato);
	memcpy(estructuraDestino->ip, dataPaquete + tamanoTotal, tamanoDato); //copio el string a la estructura
	tamanoTotal+= tamanoDato;

	for(tamanoDato = 1; (dataPaquete + tamanoTotal)[tamanoDato -1] != '\0';tamanoDato++); 	//incremento tamanoDato, hasta el tamaño del nombre.
	estructuraDestino->pathFinal = (char*)malloc(tamanoDato);
	memcpy(estructuraDestino->pathFinal, dataPaquete + tamanoTotal, tamanoDato); //copio el string a la estructura
	tamanoTotal+= tamanoDato;

	for(tamanoDato = 1; (dataPaquete + tamanoTotal)[tamanoDato -1] != '\0';tamanoDato++); 	//incremento tamanoDato, hasta el tamaño del nombre.
	estructuraDestino->pathTemporal = (char*)malloc(tamanoDato);
	memcpy(estructuraDestino->pathTemporal, dataPaquete + tamanoTotal, tamanoDato); //copio el string a la estructura
	tamanoTotal+= tamanoDato;

	memcpy(&estructuraDestino->puerto,dataPaquete+tamanoTotal,tamanoDato=sizeof(int));
	tamanoTotal+= tamanoDato;

	memcpy(&estructuraDestino->idNodo,dataPaquete+tamanoTotal,tamanoDato=sizeof(uint32_t));
	tamanoTotal+= tamanoDato;

	return estructuraDestino;
}

t_pedido_almacenar_bloque* deserializeStruct_fs_datanode_pedido_almacenar(char * dataPaquete, uint16_t length){

	t_pedido_almacenar_bloque* estructuraDestino = malloc(length);

	int tamanoTotal = 0, tamanoDato = 0;

	memcpy(&estructuraDestino->bloqueArchivo,dataPaquete+tamanoTotal,tamanoDato=sizeof(int));
	tamanoTotal+= tamanoDato;

	memcpy(&estructuraDestino->bytesOcupados,dataPaquete+tamanoTotal,tamanoDato=sizeof(int));
	tamanoTotal+= tamanoDato;

	for(tamanoDato = 1; (dataPaquete + tamanoTotal)[tamanoDato -1] != '\0';tamanoDato++); 	//incremento tamanoDato, hasta el tamaño del nombre.
	estructuraDestino->contenidoBloque = (char*)malloc(tamanoDato);
	memcpy(estructuraDestino->contenidoBloque, dataPaquete + tamanoTotal, tamanoDato); //copio el string a la estructura
	tamanoTotal+= tamanoDato;

	return estructuraDestino;
}


t_almacenar_bloque* deserializeStruct_fs_datanode_almacenar(char * dataPaquete, uint16_t length){

	t_almacenar_bloque* estructuraDestino = malloc(length);

		int tamanoTotal = 0, tamanoDato = 0;

		memcpy(&estructuraDestino->bloqueNodo,dataPaquete+tamanoTotal,tamanoDato=sizeof(int));
		tamanoTotal+= tamanoDato;

		for(tamanoDato = 1; (dataPaquete + tamanoTotal)[tamanoDato -1] != '\0';tamanoDato++);
		estructuraDestino->nombreNodo = (char*)malloc(tamanoDato);
		memcpy(estructuraDestino->nombreNodo, dataPaquete + tamanoTotal, tamanoDato);
		tamanoTotal+= tamanoDato;

		return estructuraDestino;
}
