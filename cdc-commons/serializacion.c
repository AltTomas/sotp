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
			case D_STRUCT_BLOQUE:
				paquete = serializeStruct_bloque((t_info_bloque *) estructuraOrigen, D_STRUCT_BLOQUE);
				break;
			case D_STRUCT_NODOS_ESCLAVOS:
				paquete = serializeStruct_NodosEsclavos((t_struct_jobRG*) estructuraOrigen, D_STRUCT_NODOS_ESCLAVOS);
				break;
			case D_STRUCT_CONFIRMACION_TRANSFORMACION:
				paquete = serializeStruct_ConfirmacionEtapa((t_struct_confirmacion*) estructuraOrigen, D_STRUCT_CONFIRMACION_TRANSFORMACION);
				break;
			case D_STRUCT_CONFIRMACION_REDUCCIONL:
				paquete = serializeStruct_ConfirmacionEtapa((t_struct_confirmacion*) estructuraOrigen, D_STRUCT_CONFIRMACION_REDUCCIONL);
				break;
			case D_STRUCT_CONFIRMACION_REDUCCIONG:
				paquete = serializeStruct_ConfirmacionEtapa((t_struct_confirmacion*) estructuraOrigen, D_STRUCT_CONFIRMACION_REDUCCIONG);
				break;
			case D_STRUCT_CONFIRMACION_ALMACENAMIENTO_FINAL:
				paquete = serializeStruct_ConfirmacionEtapa((t_struct_confirmacion*) estructuraOrigen, D_STRUCT_CONFIRMACION_ALMACENAMIENTO_FINAL);
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

int calcularTamanioUbicacionBloques(t_list* lista){
	puts("Calcular Tamanio Lista Bloques");
	int tamanio = 0;
	int i;

	for(i = 0; i < list_size(lista); i++){
		printf("I for: %d\n", i);
		t_info_nodo* nodo = list_get(lista,i);
		int tamanioIP = strlen(nodo->ip)+1;
		int tamanioPuerto = sizeof(int);
		printf("tamanioIP: %d\n", tamanioIP);
		printf("tamanioPuerto: %d\n", tamanioPuerto);
		tamanio += tamanioPuerto + tamanioIP;
		printf("tamanio: %d\n", tamanio);
	}

	return tamanio;

}

t_stream * serializeStruct_bloque(t_info_bloque* estructuraOrigen, int headerOperacion){

	t_stream * paquete = malloc(sizeof(t_stream));
	int tamanioLista = calcularTamanioUbicacionBloques(estructuraOrigen->ubicacionBloques);

	paquete->length = sizeof(t_header) + 2 * sizeof(int)+ sizeof(uint32_t) + tamanioLista;

	printf("Length paquete: %d\n", paquete->length);

	char * data = crearDataConHeader(headerOperacion, paquete->length); //creo el data

	int tamanoTotal = sizeof(t_header) , tamanoDato = 0;
	puts("agregamos Header");
	printf("Tamanio hasta ahora %d\n", tamanoTotal);

	estructuraOrigen->cantidadNodos = estructuraOrigen->ubicacionBloques->elements_count;

	memcpy(data + tamanoTotal , &estructuraOrigen->cantidadNodos, tamanoDato = sizeof(uint32_t));
	tamanoTotal += tamanoDato;

	puts("agregamos uint32 - CantidadNodos");
	printf("Tamanio hasta ahora %d\n", tamanoTotal);

	memcpy(data + tamanoTotal , &estructuraOrigen->idNodo, tamanoDato = sizeof(int));
	tamanoTotal += tamanoDato;

	puts("agregamos Int - idNodo");
	printf("Tamanio hasta ahora %d\n", tamanoTotal);

	memcpy(data + tamanoTotal , &estructuraOrigen->nroBloque, tamanoDato = sizeof(int));
	tamanoTotal += tamanoDato;

	puts("agregamos Int - nroBloque");
	printf("Tamanio hasta ahora %d\n", tamanoTotal);

	int contadorNodos = 0;

	while (contadorNodos < estructuraOrigen->cantidadNodos) {

		t_info_nodo * nodo = list_get(estructuraOrigen->ubicacionBloques, contadorNodos);

		memcpy(data + tamanoTotal , &nodo->puerto, tamanoDato = sizeof(int));
		tamanoTotal += tamanoDato;

		puts("agregamos Int - Puerto");
		printf("Tamanio hasta ahora %d\n", tamanoTotal);

		memcpy(data + tamanoTotal, nodo->ip , tamanoDato = strlen(nodo->ip)+1);
		tamanoTotal+=tamanoDato;

		puts("agregamos char* - IP");
		printf("Tamanio hasta ahora %d\n", tamanoTotal);

		contadorNodos++;
	}

	paquete->data = data;

	return paquete;
}

t_stream * serializeStruct_jobT(t_struct_jobT * estructuraOrigen, int headerOperacion){

	t_stream * paquete = malloc(sizeof(t_stream));

	paquete->length = sizeof(t_header) 	+ strlen(estructuraOrigen->scriptTransformacion)
										+ strlen(estructuraOrigen->pathOrigen)
										+ strlen(estructuraOrigen->pathTemporal)
										+ 1;

	char * data = crearDataConHeader(headerOperacion, paquete->length);

	int tamanoTotal = sizeof(t_header), tamanoDato = 0;

	memcpy(data + tamanoTotal, estructuraOrigen->scriptTransformacion , tamanoDato = strlen(estructuraOrigen->scriptTransformacion)+1);

	tamanoTotal+=tamanoDato;

	memcpy(data + tamanoTotal, estructuraOrigen->pathOrigen , tamanoDato = strlen(estructuraOrigen->pathOrigen)+1);

	tamanoTotal+=tamanoDato;

	memcpy(data + tamanoTotal, estructuraOrigen->pathTemporal , tamanoDato = strlen(estructuraOrigen->pathTemporal)+1);

	tamanoTotal+=tamanoDato;

	paquete->data = data;

	return paquete;
}

t_stream * serializeStruct_jobR(t_struct_jobR * estructuraOrigen, int headerOperacion){

	t_stream * paquete = malloc(sizeof(t_stream));

	paquete->length = sizeof(t_header) 	+ strlen(estructuraOrigen->scriptReduccion)
										+ strlen(estructuraOrigen->pathTemp)
										+ strlen(estructuraOrigen->pathTempFinal)
										+ 1;

	char * data = crearDataConHeader(headerOperacion, paquete->length);

	int tamanoTotal = sizeof(t_header), tamanoDato = 0;


	memcpy(data + tamanoTotal, estructuraOrigen->scriptReduccion , tamanoDato = strlen(estructuraOrigen->scriptReduccion)+1);

	tamanoTotal+=tamanoDato;

	memcpy(data + tamanoTotal, estructuraOrigen->pathTemp , tamanoDato = strlen(estructuraOrigen->pathTemp)+1);

	tamanoTotal+=tamanoDato;

	memcpy(data + tamanoTotal, estructuraOrigen->pathTempFinal , tamanoDato = strlen(estructuraOrigen->pathTempFinal)+1);

	tamanoTotal+=tamanoDato;

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



t_stream * serializeStruct_ConfirmacionEtapa(t_struct_confirmacion* estructuraOrigen, int headerOperacion){

	t_stream* paquete = malloc(sizeof(t_stream));

	paquete->length = sizeof(t_header) 	+  2 * sizeof(uint32_t);

	char * data = crearDataConHeader(headerOperacion, paquete->length);
	int tamanoTotal = sizeof(t_header), tamanoDato = 0;

	memcpy(data + tamanoTotal, &estructuraOrigen->confirmacion, sizeof(uint32_t));
	tamanoTotal+=tamanoDato;

	memcpy(data + tamanoTotal, &estructuraOrigen->nodo, sizeof(uint32_t));
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
			case D_STRUCT_JOBT:
				estructuraDestino = deserializeStruct_jobT(dataPaquete, length);
				break;
			case D_STRUCT_JOBR:
				estructuraDestino = deserializeStruct_jobR(dataPaquete, length);
				break;
			case D_STRUCT_BLOQUE:
				estructuraDestino = deserializeStruct_bloque(dataPaquete, length);
				break;
			case D_STRUCT_NODOS_ESCLAVOS:
				estructuraDestino = deserializeStruct_NodosEsclavos(dataPaquete, length);
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

t_info_bloque * deserializeStruct_bloque(char * dataPaquete, uint16_t length){

	t_info_bloque * estructuraDestino = malloc(length);

	puts("Empezamos a deserializar");
	printf("Length paquete %d\n", length);

		int tamanoTotal = 0, tamanoDato = 0;

		memcpy(&estructuraDestino->cantidadNodos,dataPaquete+tamanoTotal,tamanoDato=sizeof(uint32_t));
		tamanoTotal+= tamanoDato;

		puts("Sacamos uint32 - Cantidad Nodos");
		printf("tamanoTotal %d\n", tamanoTotal);

		memcpy(&estructuraDestino->idNodo,dataPaquete+tamanoTotal,tamanoDato=sizeof(int));
		tamanoTotal+= tamanoDato;

		puts("Sacamos int - idNodo");
		printf("tamanoTotal %d\n", tamanoTotal);

		memcpy(&estructuraDestino->nroBloque,dataPaquete+tamanoTotal,tamanoDato=sizeof(int));
		tamanoTotal+= tamanoDato;

		puts("Sacamos int - nroBloque");
		printf("tamanoTotal %d\n", tamanoTotal);

		int contadorNodos = 0;
		estructuraDestino->ubicacionBloques = list_create();

		while(contadorNodos < estructuraDestino->cantidadNodos){

			printf("Ciclo while %d\n", contadorNodos);

			t_info_nodo* nodo = malloc(sizeof(t_info_nodo));

			memcpy(&nodo->puerto, dataPaquete + tamanoTotal, tamanoDato = sizeof(int));
			tamanoTotal += tamanoDato;

			puts("Sacamos int - Puerto");
			printf("tamanoTotal %d\n", tamanoTotal);
			printf("Puerto %d\n", nodo->puerto);

			for(tamanoDato = 1; (dataPaquete + tamanoTotal)[tamanoDato -1] != '\0';tamanoDato++);
			printf("String tamano dato: %d\n", tamanoDato);
			nodo->ip = malloc(tamanoDato);
			memcpy(nodo->ip, dataPaquete + tamanoTotal, tamanoDato);

			puts("Sacamos int - IP");
			printf("tamanoTotal %d\n", tamanoTotal + tamanoDato);
			printf("IP %s\n", nodo->ip);

			list_add(estructuraDestino->ubicacionBloques,nodo);

			contadorNodos++;
			}

			return estructuraDestino;
}

t_struct_jobT * deserializeStruct_jobT(char * dataPaquete, uint16_t length){

	t_struct_jobT * estructuraDestino = malloc(sizeof(t_struct_jobT));

		int tamanoTotal = 0, tamanoDato = 0;

		tamanoTotal = tamanoDato;

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

		return estructuraDestino;
}

t_struct_jobR * deserializeStruct_jobR(char * dataPaquete, uint16_t length){

	t_struct_jobR * estructuraDestino = malloc(sizeof(t_struct_jobR));

		int tamanoTotal = 0, tamanoDato = 0;

		tamanoTotal+= tamanoDato;

		for(tamanoDato = 1; (dataPaquete + tamanoTotal)[tamanoDato -1] != '\0';tamanoDato++);
		estructuraDestino->scriptReduccion = malloc(tamanoDato);
		memcpy(estructuraDestino->scriptReduccion, dataPaquete + tamanoTotal, tamanoDato);

		tamanoTotal+= tamanoDato;

		for(tamanoDato = 1; (dataPaquete + tamanoTotal)[tamanoDato -1] != '\0';tamanoDato++);
		estructuraDestino->pathTemp = malloc(tamanoDato);
		memcpy(estructuraDestino->pathTemp, dataPaquete + tamanoTotal, tamanoDato);

		tamanoTotal+= tamanoDato;

		for(tamanoDato = 1; (dataPaquete + tamanoTotal)[tamanoDato -1] != '\0';tamanoDato++);
		estructuraDestino->pathTempFinal = malloc(tamanoDato);
		memcpy(estructuraDestino->pathTempFinal, dataPaquete + tamanoTotal, tamanoDato);

		return estructuraDestino;
}

t_struct_jobRG * deserializeStruct_NodosEsclavos(char * dataPaquete, uint16_t length){

	t_struct_jobRG * estructuraDestino = malloc(sizeof(t_struct_jobRG));

		int tamanoTotal = 0, tamanoDato = 0;

		tamanoTotal+= tamanoDato;

		memcpy(&estructuraDestino->cantidadNodos,dataPaquete+tamanoTotal,tamanoDato=sizeof(uint32_t));
		tamanoTotal+= tamanoDato;

		for(tamanoDato = 1; (dataPaquete + tamanoTotal)[tamanoDato -1] != '\0';tamanoDato++);
		estructuraDestino->scriptReduccion = malloc(tamanoDato);
		memcpy(estructuraDestino->scriptReduccion, dataPaquete + tamanoTotal, tamanoDato);

		tamanoTotal+= tamanoDato;

		int contadorNodos = 0;
		estructuraDestino->nodos = list_create();

		while(contadorNodos < estructuraDestino->cantidadNodos){

			t_struct_nodoEsclavo* nodo = malloc(sizeof(t_struct_nodoEsclavo));

			for(tamanoDato = 1; (dataPaquete + tamanoTotal)[tamanoDato -1] != '\0';tamanoDato++);
			nodo->ip = malloc(tamanoDato);
			memcpy(nodo->ip, dataPaquete + tamanoTotal, tamanoDato);

			for(tamanoDato = 1; (dataPaquete + tamanoTotal)[tamanoDato -1] != '\0';tamanoDato++);
			nodo->nombreTemporal = malloc(tamanoDato);
			memcpy(nodo->nombreTemporal, dataPaquete + tamanoTotal, tamanoDato);


			memcpy(&nodo->puerto, dataPaquete + tamanoTotal, tamanoDato = sizeof(int));
			tamanoTotal += tamanoDato;

			list_add(estructuraDestino->nodos,nodo);

			contadorNodos++;
			}

		return estructuraDestino;
}

t_struct_confirmacion * deserializeStruct_ConfirmacionEtapa(char * dataPaquete, uint16_t length){

	t_struct_confirmacion * estructuraDestino = malloc(sizeof(t_struct_confirmacion));

	int tamanoTotal = 0, tamanoDato = 0;

	memcpy(&estructuraDestino->confirmacion,dataPaquete+tamanoTotal,tamanoDato=sizeof(uint32_t));

	tamanoTotal+= tamanoDato;

	memcpy(&estructuraDestino->nodo,dataPaquete+tamanoTotal,tamanoDato=sizeof(uint32_t));

	tamanoTotal+= tamanoDato;

	return estructuraDestino;
}
