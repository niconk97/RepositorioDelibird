#include "gameboy.h"

int main(int argc, char *argv[]){
	t_config* config;
	char* proceso;
	uint32_t cola_destino;


	inicializar_datos(config);
	/*argc = 4;
	argv[1]="SUSCRIPTOR";
	argv[2] = "GET_POKEMON";
	argv[3] = "10";*/

	if(argc >= 2) {
		proceso = argv[1];

	} else {
		log_info(logger_gameboy, "Faltan argumentos\n");
		return -1;
	}

	if(argc >= 3) {
		cola_destino = obtener_cola_mensaje(argv[2]);
		if(cola_destino == 0) {
			log_info(logger_gameboy, "Ingrese una cola valida.");
			return -1;
		}
	} else {
		log_info(logger_gameboy, "Faltan argumentos\n");
		return -1;
	}

	if((strcmp(proceso,"SUSCRIPTOR") == 0) && (argc == 4)) {
		// TODO SUSCRIPCION
	/*	printf("ip %s puerto %s\n", ip_gameboy, puerto_gameboy);
		crear_hilo_escucha(ip_gameboy, puerto_gameboy);
		realizar_suscripcion(cola_destino, atoi(argv[3]));
		sleep(30000);


	*/
		op_code cola = obtener_cola_mensaje(argv[2]);
		int tiempo = atoi(argv[3]);

		suscribirse_a_cola_gameboy(cola,tiempo);

	} else if ((strcmp(proceso,"SUSCRIPTOR") == 0) && (argc != 4)) {
		manejar_error_mensaje();
		return -1;
	} else {
		int a = clasificar_mensaje(argc, argv, cola_destino, proceso);
		if(a == -1) {
			return a;
		}
	}

	//log_destroy(logger_gameboy);
	//config_destroy(config);
	return 0;
}

int clasificar_mensaje(int argc, char* argv[], uint32_t cola_destino, char* proceso) {

	int conexion;
	char* mensaje;

	switch(cola_destino) {
		case NEW_POKEMON: {
			if((strcmp(proceso,"BROKER") == 0) && (argc == 7)){
				conexion = crear_conexion(ip_broker, puerto_broker);
				send_message_new_pokemon(argv[3], atoi(argv[4]),  atoi(argv[5]), atoi(argv[6]), 0, 0, conexion);
			} else if((strcmp(proceso,"GAMECARD") == 0) && (argc == 8)){
				conexion = crear_conexion(ip_gamecard, puerto_gamecard);
				send_message_new_pokemon(argv[3], atoi(argv[4]), atoi(argv[5]), atoi(argv[6]), atoi(argv[7]), 0, conexion);
			} else {
				manejar_error_mensaje();
				return -1;
			}
			break;
		}
		case GET_POKEMON: {
			if((strcmp(proceso,"BROKER") == 0) && (argc == 4)){
				conexion = crear_conexion(ip_broker, puerto_broker);
				send_message_get_pokemon(argv[3],0, 0, conexion);
			} else if((strcmp(proceso,"GAMECARD") == 0) && (argc == 5)){
				conexion = crear_conexion(ip_gamecard, puerto_gamecard);
				send_message_get_pokemon(argv[3], atoi(argv[4]), 0, conexion);
			} else {
				manejar_error_mensaje();
				return -1;
			}
			break;
		}
		case CATCH_POKEMON: {
			if((strcmp(proceso,"BROKER") == 0) && (argc == 6)){
				conexion = crear_conexion(ip_broker, puerto_broker);
				send_message_catch_pokemon(argv[3], atoi(argv[4]), atoi(argv[5]), 0, 0, conexion);
			} else if((strcmp(proceso,"GAMECARD") == 0) && (argc == 7)){
				conexion = crear_conexion(ip_gamecard, puerto_gamecard);
				send_message_catch_pokemon(argv[3], atoi(argv[4]), atoi(argv[5]), atoi(argv[6]), 0, conexion);
			} else {
				manejar_error_mensaje();
				return -1;
			}
			break;
		}
		case APPEARED_POKEMON: {
			if((strcmp(proceso,"BROKER") == 0) && (argc == 7)){
				conexion = crear_conexion(ip_broker, puerto_broker);
				send_message_appeared_pokemon(argv[3], atoi(argv[4]), atoi(argv[5]), 0, atoi(argv[6]), conexion);
			} else if((strcmp(proceso,"TEAM") == 0) && (argc == 6)){
				conexion = crear_conexion(ip_team, puerto_team);
				send_message_appeared_pokemon(argv[3], atoi(argv[4]), atoi(argv[5]), 0, 0, conexion);
			} else {
				manejar_error_mensaje();
				return -1;
			}
			break;
		}
		case CAUGHT_POKEMON: {
			if((strcmp(proceso,"BROKER") == 0) && (argc == 5)){
				conexion = crear_conexion(ip_broker, puerto_broker);
				int id_corr = atoi(argv[3]);
				send_message_caught_pokemon(argv[4], 0, id_corr, conexion);
			} else {
				manejar_error_mensaje();
				return -1;
			}
			break;
		}
	}

	mensaje = client_recibir_mensaje(conexion);
	log_info(logger_gameboy, mensaje);
	liberar_conexion(conexion);
	return 0;
}

uint32_t obtener_cola_mensaje(char* cola_string) {
	if(strcmp(cola_string,"NEW_POKEMON") == 0) {
		return NEW_POKEMON;
	} else if(strcmp(cola_string,"GET_POKEMON") == 0) {
		return GET_POKEMON;
	} else if(strcmp(cola_string,"APPEARED_POKEMON") == 0) {
		return APPEARED_POKEMON;
	} else if(strcmp(cola_string,"LOCALIZED_POKEMON") == 0) {
		return LOCALIZED_POKEMON;
	} else if(strcmp(cola_string,"CATCH_POKEMON") == 0) {
		return CATCH_POKEMON;
	} else if(strcmp(cola_string,"CAUGHT_POKEMON") == 0) {
		return CAUGHT_POKEMON;
	} else if(strcmp(cola_string,"SUSCRIBE")==0){
		return SUSCRIBE;
	}else{
		return 0;
	}
}

void manejar_error_mensaje() {
	log_info(logger_gameboy, "Mensaje invalido");
}

void inicializar_datos(t_config* config) {
	logger_gameboy = log_create("/home/utnso/tp-2020-1c-Elite-Four/gameboy/gameboy.log", "GAMEBOY", 1, LOG_LEVEL_INFO);
	config = config_create("/home/utnso/tp-2020-1c-Elite-Four/gameboy/gameboy.config");
	ip_broker = config_get_string_value(config, "IP_BROKER");
	puerto_broker = config_get_string_value(config, "PUERTO_BROKER");
	ip_team = config_get_string_value(config, "IP_TEAM");
	puerto_team = config_get_string_value(config, "PUERTO_TEAM");
	ip_gamecard = config_get_string_value(config, "IP_GAMECARD");
	puerto_gamecard = config_get_string_value(config, "PUERTO_GAMECARD");
	ip_gameboy = config_get_string_value(config, "IP_GAMEBOY");
	puerto_gameboy = config_get_string_value(config, "PUERTO_GAMEBOY");
	id_proceso = config_get_string_value(config,"ID");

	ACK = "ACK";
}

/*void realizar_suscripcion(uint32_t cola_destino, int tiempo_suscripto) {
	char* ip_puerto_gameboy = malloc(strlen(ip_gameboy) + strlen(puerto_gameboy) + strlen(":") + 2);
	strcpy(ip_puerto_gameboy, ip_gameboy);
	strcat(ip_puerto_gameboy, ":");
	strcat(ip_puerto_gameboy, puerto_gameboy);
	int conexion;
	char* mensaje;

	conexion = crear_conexion(ip_broker, puerto_broker);
	enviar_mensaje_suscribir(cola_destino, ip_puerto_gameboy, conexion);

	mensaje = client_recibir_mensaje(conexion);

	log_info(logger_gameboy, mensaje);

	liberar_conexion(conexion);
	free(ip_puerto_gameboy);
}*/

void aplica_funcion_escucha(int * socket) {
	printf("recibe mensaje del broker\n");
	op_code cod_op;
	puntero_mensaje mensajeRecibido;
	uint32_t size;

	recv(*socket, &cod_op, sizeof(op_code), MSG_WAITALL);
	printf("socket %d\n", *socket);
	printf("cod op %d\n", cod_op);
	devolver_mensaje(ACK, strlen(ACK) + 1, *socket);
	printf("PASE ACK %s\n", ACK);
	switch(cod_op) {
		case NEW_POKEMON: {
			mensajeRecibido = recibir_new_pokemon(*socket, &size);
			printf("mensaje new pokemon recibido con id %d\n", mensajeRecibido->id);
			break;
		}
		case APPEARED_POKEMON: {
			mensajeRecibido = recibir_appeared_pokemon(*socket, &size);
			printf("mensaje appeared pokemon recibido con id %d\n", mensajeRecibido->id);
			break;
		}
		case GET_POKEMON: {
			mensajeRecibido = recibir_get_pokemon(*socket, &size);
			printf("mensaje get pokemon recibido con id %d\n", mensajeRecibido->id);
			break;
		}
		case LOCALIZED_POKEMON: {
			mensajeRecibido = recibir_localized_pokemon(*socket, &size);
			printf("mensaje localized pokemon recibido con id %d\n", mensajeRecibido->id);
			break;
		}
		case CATCH_POKEMON: {
			mensajeRecibido = recibir_catch_pokemon(*socket, &size);
			printf("mensaje catch pokemon recibido con id %d\n", mensajeRecibido->id);
			break;
		}
		case CAUGHT_POKEMON: {
			mensajeRecibido = recibir_caught_pokemon(*socket, &size);
			printf("mensaje caught pokemon recibido con id %d\n", mensajeRecibido->id);
			break;
		}
		case MESSAGE: {
			mensajeRecibido = client_recibir_mensaje(*socket);
			printf("mensaje recibido\n");
			break;
		}
		default: {

		}
	}
}


op_code vectorDeColas[1];
//--------------------------------------------------------------------------------------------------------------------//

bool suscribirse_a_cola_gameboy(op_code cola_elegida ,int tiempo){

	vectorDeColas[0]=cola_elegida;
	bool conexionOK=false;
	int conexion, i=0, tiempoRestante = 0;
	char* mensaje;
	pthread_t hiloEscucha, hiloSleep;

	op_code cola;
	printf("hola1\n");

	conexion = crear_conexion(ip_broker,puerto_broker);
	printf("hola\n");
	if(conexion != -1){
		conexionOK = true;
		pthread_create(&hiloEscucha,NULL,(void*)hilo_escucha, conexion);
		pthread_detach(&hiloEscucha);
		//while(vectorDeColas[i]!=NULL){

			cola = vectorDeColas[i];
			printf("Conexion %d\n", conexion);
			enviar_mensaje_suscribir_con_id(cola, id_proceso, conexion, tiempo);
			mensaje = client_recibir_mensaje(conexion);
			log_info(logger_gameboy,"MENSAJE RECIBIDO; Tipo: MENSAJE. Contenido: %s\n", mensaje);
			free(mensaje);
			i++;

		//}

		pthread_create(&hiloSleep,NULL,(void*)sleep(tiempo), NULL);
		pthread_join(&hiloSleep,NULL);


		//log_destroy(logger_gameboy);
		return conexionOK;

	}else{

	log_info(logger_gameboy,"NO SE PUDO CONECTAR AL BROKER");
	//log_destroy(logger_gameboy);
	return conexionOK;

	}
}


//--------------------------------------------------------------------------------------------------------------------//

