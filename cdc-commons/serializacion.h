#ifndef SERIALIZACION_H_
#define SERIALIZACION_H_

	#include "estructuras.h"

	t_header crearHeader(uint8_t tipoEstructura, uint16_t lengthDatos);
	char * crearDataConHeader(uint8_t tipoEstructura, int length);

	t_stream * serialize(int tipoEstructura, void * estructuraOrigen);

	t_stream * serializeStruct_numero(t_struct_numero * estructuraOrigen, int headerOperacion);
	t_stream * serializeStruct_char(t_struct_char * estructuraOrigen, int headerOperacion);
	t_stream * serializeStruct_string(t_struct_string * estructuraOrigen, int headerOperacion);
	t_stream * serializeStruct_jobT(t_struct_jobT * estructuraOrigen, int headerOperacion);
	t_stream * serializeStruct_jobR(t_struct_jobR * estructuraOrigen, int headerOperacion);
<<<<<<< HEAD
=======
	t_stream * serializeStruct_bloque(t_info_bloque* estructuraOrigen, int headerOperacion);
>>>>>>> bac1c3cf5de083d5e114fc76b1064775a1a3dbb6
	t_header desempaquetarHeader(char * header);
	void * deserialize(uint8_t tipoEstructura, char * dataPaquete, uint16_t length);

	t_struct_numero * deserializeStruct_numero(char * dataPaquete, uint16_t length);
	t_struct_char * deserializeStruct_char(char * dataPaquete, uint16_t length);
	t_struct_string * deserializeStruct_string(char * dataPaquete, uint16_t length);
	t_struct_jobT * deserializeStruct_jobT(char* dataPaquete, uint16_t length);
	t_struct_jobR * deserializeStruct_jobR(char* dataPaquete, uint16_t length);
<<<<<<< HEAD
=======
	t_info_bloque * deserializeStruct_bloque(char * dataPaquete, uint16_t length);
>>>>>>> bac1c3cf5de083d5e114fc76b1064775a1a3dbb6

#endif /* SERIALIZACION_H_ */
