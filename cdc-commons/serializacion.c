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
t_stream * serializeStruct_bloque(t_info_bloque* estructuraOrigen, int headerOperacion){

	t_stream * paquete = malloc(sizeof(t_stream));

	paquete->length = sizeof(t_header) + sizeof(estructuraOrigen->nroBloque) +
					  sizeof(estructuraOrigen->ubicacionBloques) +  1;

	char * data = crearDataConHeader(headerOperacion, paquete->length); //creo el data

	int tamanoTotal = sizeof(t_header) , tamanoDato = 0;

	memcpy(data + tamanoTotal, estructuraOrigen->nroBloque , tamanoDato = sizeof(estructuraOrigen->nroBloque) + 1);

	tamanoTotal+=tamanoDato;

	memcpy(data + tamanoTotal, estructuraOrigen->ubicacionBloques , tamanoDato = sizeof(estructuraOrigen->ubicacionBloques) + 1);

	tamanoTotal+=tamanoDato;

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

	t_info_bloque * estructuraDestino = malloc(sizeof(t_info_bloque));

		int tamanoTotal = 0, tamanoDato = 0;

		tamanoTotal = tamanoDato;
		/* Deserialize Bloque*/
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
