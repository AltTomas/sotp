#include "funcionesYAMA.h"

int main(void) {
	listaProcesos = list_create();
	logger = log_create("../logYAMA","YAMA", 0, LOG_LEVEL_TRACE);
	init();
	crearConfig();
	conectarConFS();
	escucharConexiones();

	destruirConfig(config);
	return EXIT_SUCCESS;
}
