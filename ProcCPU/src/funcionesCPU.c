/*
 * funcionesCPU.c
 *
 *  Created on: 23/9/2015
 *      Author: utnso
 */

#include "funcionesCPU.h"
#include <commons/error.h>
#include "estructuras.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>


void crearMockitoPlanif(protocolo_planificador_cpu* package){

	package->tipoProceso = 'P';
	package->tipoOperacion = 'l';
	package->estado = EJECUTANDO;
	package->pid = 1;
	package->counterProgram = 2;
	package->quantum = 0;
	package->mensaje = malloc(strlen("programa.txt")+1);
	strcpy(package->mensaje,"programa.txt");
	package->tamanioMensaje = strlen(package->mensaje)+1;

}

void* serializarPaqueteMemoria(protocolo_cpu_memoria* paquete, int* tamanio) {

	size_t messageLength = strlen(paquete->mensaje);

	void* paqueteSerializado = malloc(sizeof(protocolo_cpu_memoria) + messageLength);
	int offset = 0;
	int size_to_send;

	size_to_send = sizeof(paquete->tipoProceso);
	memcpy(paqueteSerializado + offset, &(paquete->tipoProceso), size_to_send);
	offset += size_to_send;

	size_to_send = sizeof(paquete->tipoOperacion);
	memcpy(paqueteSerializado + offset, &(paquete->tipoOperacion),
			size_to_send);
	offset += size_to_send;

	size_to_send = sizeof(paquete->pid);
	memcpy(paqueteSerializado + offset, &(paquete->pid), size_to_send);
	offset += size_to_send;

	size_to_send = sizeof(paquete->nroPagina);
	memcpy(paqueteSerializado + offset, &(paquete->nroPagina), size_to_send);
	offset += size_to_send;

	size_to_send = sizeof(paquete->tamanioMensaje);
	memcpy(paqueteSerializado + offset, &messageLength, size_to_send);
	offset += size_to_send;

	size_to_send = messageLength;
	memcpy(paqueteSerializado + offset, paquete->mensaje, size_to_send);
	offset += size_to_send;

	/*size_to_send = sizeof(paquete->tamanioMensaje);
	memcpy(paqueteSerializado + offset, &(paquete->tamanioMensaje),
			size_to_send);
	offset += size_to_send;

	size_to_send = paquete->tamanioMensaje;
	memcpy(paqueteSerializado + offset, &(paquete->mensaje), size_to_send);
	offset += size_to_send;*/

	*tamanio = offset;
	return paqueteSerializado;

}

//TODO poner el socket global
int deserializarPlanificador(protocolo_planificador_cpu *package) { //TODO deserializar mensaje de planificador
	int status;
	void* buffer = malloc(sizeof(protocolo_planificador_cpu)-4);
	int offset = 0;

	status = recv(socketPlanificador, buffer,sizeof(protocolo_planificador_cpu)-4, 0);

	if(!status) return 0;

	memcpy(&(package->tipoProceso), buffer, sizeof(package->tipoProceso));
	offset += sizeof(package->tipoProceso);
	printf("%c\n", package->tipoProceso);
	memcpy(&(package->tipoOperacion), buffer+ offset, sizeof(package->tipoOperacion));
	offset += sizeof(package->tipoOperacion);
	printf("%c\n", package->tipoOperacion);
	memcpy(&(package->estado), buffer + offset, sizeof(package->estado));
	offset += sizeof(package->estado);
	printf("%d\n", package->estado);
	memcpy(&(package->pid), buffer + offset, sizeof(package->pid));
	offset += sizeof(package->pid);
	printf("%d\n", package->pid);
	memcpy(&(package->counterProgram), buffer + offset,sizeof(package->counterProgram));
	offset += sizeof(package->counterProgram);
	printf("%d\n", package->counterProgram);
	memcpy(&(package->quantum), buffer + offset, sizeof(package->quantum));
	offset += sizeof(package->quantum);
	printf("%d\n", package->quantum);
	memcpy(&(package->tamanioMensaje), buffer + offset,sizeof(package->tamanioMensaje));
	offset += sizeof(package->tamanioMensaje);
	printf("%d\n", package->tamanioMensaje);
	package->mensaje = (char*)malloc((package->tamanioMensaje) );

	status = recv(socketPlanificador, package->mensaje, package->tamanioMensaje,0);

	package->mensaje[package->tamanioMensaje -1]= '\0';
    printf("%s\n", package->mensaje);
	free(buffer);

	return status;

}

int deserializarMemoria(protocolo_memoria_cpu* package){
	int status;
		void* buffer = malloc(sizeof(protocolo_memoria_cpu)-4);  //TODO: RESERVAR MEMORIA
		int offset = 0;

		status = recv(socketMemoria, buffer,
				sizeof(protocolo_memoria_cpu) -4, 0);
		if(!status) return 0;

		memcpy(&(package->tipoProceso), buffer, sizeof(package->tipoProceso));
		offset += sizeof(package->tipoProceso);

		memcpy(&(package->codOperacion), buffer + offset, sizeof(package->codOperacion));
		offset += sizeof(package->codOperacion);

		memcpy(&(package->codAux), buffer + offset, sizeof(package->codAux));
		offset += sizeof(package->codAux);

		memcpy(&(package->pid), buffer + offset, sizeof(package->pid));
		offset += sizeof(package->pid);

		memcpy(&(package->numeroPagina), buffer + offset, sizeof(package->numeroPagina));
		offset += sizeof(package->numeroPagina);

		memcpy(&(package->tamanioMensaje), buffer + offset,sizeof(package->tamanioMensaje));
		offset += sizeof(package->tamanioMensaje);

		package->mensaje = (char*)malloc((package->tamanioMensaje) +1);

		status = recv(socketMemoria, package->mensaje, package->tamanioMensaje,0);

		package->mensaje[package->tamanioMensaje]= '\0';
		/*
		memcpy(&(package->tamanioMensaje), buffer + offset,
				sizeof(package->tamanioMensaje));
		offset += sizeof(package->tamanioMensaje);

		if (!status) return 0;
		package->mensaje = malloc((package->tamanioMensaje) +1);
			status = recv(socketMemoria, buffer, package->tamanioMensaje,0);
			memcpy(&(package->mensaje), buffer + offset, package->tamanioMensaje);
			package->mensaje[package->tamanioMensaje+1]= '\0';*/

		free(buffer);

		return status;
}
void enviarAMemoria(tParametroHilo* message) {
	int tamanio;
	char* empaquetado = serializarPaqueteMemoria(message->mensajeAMemoria, &tamanio);
	send(socketMemoria, empaquetado, tamanio,
			0);
	free(empaquetado); //free(1)
}

//MODIFICAR ARMAR PAQUETE PARAMETROS
void armarPaquete(protocolo_cpu_memoria* paquete, char tipoProceso,char codOperacion, int pid, int nroPagina, char* mensaje) {
	paquete->tipoProceso = tipoProceso;
	paquete->tipoOperacion = codOperacion;
	paquete->pid = pid;
	paquete->nroPagina = nroPagina;
	paquete->tamanioMensaje = strlen(mensaje) + 1;
	paquete->mensaje = malloc(paquete->tamanioMensaje);
	strcpy(paquete->mensaje, mensaje);

	//TODO Hacerlo mas genérico con un booleano y cargue la estructura (sin mandar todos los parametros)
}


void armarPaquetePlanificador(protocolo_planificador_cpu* paquete, char tipoProceso,char codOperacion, int pid, testado estado, int counterProgram ,int quantum, int tamanioMensaje,char* mensaje) {
	paquete->tipoProceso = tipoProceso;
	paquete->tipoOperacion = codOperacion;
	paquete->pid = pid;
	paquete->estado = estado;
	paquete->counterProgram = counterProgram;
	paquete->quantum = quantum;
	paquete->tamanioMensaje = strlen(mensaje) + 1;
	paquete->mensaje = malloc(paquete->tamanioMensaje);
	strcpy(paquete->mensaje, mensaje);

	//TODO Hacerlo mas genérico con un booleano y cargue la estructura (sin mandar todos los parametros)
}

void interpretarInstruccion(char* instruccion, tParametroHilo* mensajeParaArmar) {

		char** linea = string_split(instruccion, ";");
		char** lineaFiltrada = string_split(linea[0]," ");

		if (string_starts_with(instruccion, "iniciar")) {
			int numero = atoi(lineaFiltrada[1]);
			armarPaquete(mensajeParaArmar->mensajeAMemoria, 'c', 'i',mensajeParaArmar->mensajeAPlanificador->pid,numero , "\0");
		}
		if (string_starts_with(instruccion, "leer")) {
			int numero = atoi(lineaFiltrada[1]);
			armarPaquete(mensajeParaArmar->mensajeAMemoria, 'c', 'l',
					mensajeParaArmar->mensajeAPlanificador->pid, numero, "\0");
		}
		//if(string_starts_with(message->lineaDeProceso,"escribir")) {  } //TODO cheackpoint 3 supongo
		//if(string_starts_with(message->lineaDeProceso,"entrada-salida")) { }

		if (string_starts_with(instruccion, "finalizar;")) {
				armarPaquete(mensajeParaArmar->mensajeAMemoria, 'c', 'f',
						mensajeParaArmar->mensajeAPlanificador->pid, 0, "\0");
		}
		free(linea);
		free(lineaFiltrada);
}


char* leerInstruccion(int* instructionPointer,char* lineaLeida, FILE* archivo, int tam) {	//ruta+instruction pointer => leo la linea del ip y la devuelvo

	int cont = 1;
	if (*instructionPointer == 1) {
		fgets(lineaLeida, tam, archivo);
		(*instructionPointer) = (*instructionPointer) + 1;
		cont++;
	}

	while (!feof(archivo) && cont != (*instructionPointer)) {
		fgets(lineaLeida, tam, archivo);
		cont++;
		(*instructionPointer) = (*instructionPointer) + 1;
	}
	if (!string_starts_with(lineaLeida, "finalizar;")) lineaLeida[strlen(lineaLeida)-1] = '\0';

	return lineaLeida;

}


void cargarParametrosHilo(protocolo_planificador_cpu* mensajeDePlanif, tParametroHilo* parametros) {

    parametros->mensajeAPlanificador = malloc(sizeof(protocolo_planificador_cpu));
	parametros->mensajeAPlanificador->tipoProceso = mensajeDePlanif->tipoProceso;
	parametros->mensajeAPlanificador->tipoOperacion = mensajeDePlanif->tipoOperacion;
	parametros->mensajeAPlanificador->estado = mensajeDePlanif->estado;
	parametros->mensajeAPlanificador->pid = mensajeDePlanif->pid;
	parametros->mensajeAPlanificador->counterProgram = mensajeDePlanif->counterProgram;
	parametros->mensajeAPlanificador->quantum = mensajeDePlanif->quantum;
	parametros->mensajeAPlanificador->mensaje = malloc(strlen(mensajeDePlanif->mensaje)+1);
	strcpy(parametros->mensajeAPlanificador->mensaje, mensajeDePlanif->mensaje);
	parametros->mensajeAPlanificador->tamanioMensaje = strlen(parametros->mensajeAPlanificador->mensaje)+1;
	parametros->mensajeAPlanificador->mensaje[mensajeDePlanif->tamanioMensaje-1] = '\0';

}

void logueoRecepcionDePlanif(protocolo_planificador_cpu* contextoDeEjecucion) {
	char* logueoContexto = (char*)malloc(50);
	char* estado;
	if (contextoDeEjecucion->estado == LISTO) {
		estado = malloc(7);
		strcpy(estado, " LISTO");
	}
	if (contextoDeEjecucion->estado == IO) {
		estado = malloc(7);
		strcpy(estado, " IO");
	}
	if (contextoDeEjecucion->estado == EJECUTANDO) {
		estado = malloc(12);
		strcpy(estado, " EJECUTANDO");
	}
	if (contextoDeEjecucion->estado == FINALIZADO) {
		estado = malloc(12);
		strcpy(estado, " FINALIZADO");
	}

	strcpy(logueoContexto, "Contexto de ejecucion recibido: PID: ");
	string_append(&logueoContexto, string_itoa(contextoDeEjecucion->pid));
	string_append(&logueoContexto, "\nInstruccion: ");
	string_append(&logueoContexto, string_itoa(contextoDeEjecucion->counterProgram));
	string_append(&logueoContexto, " \nQuantum: ");
	string_append(&logueoContexto, string_itoa(contextoDeEjecucion->quantum));
	string_append(&logueoContexto, " \nEstado: ");
	string_append(&logueoContexto, estado);
	string_append(&logueoContexto, " \nRuta: ");
	string_append(&logueoContexto, contextoDeEjecucion->mensaje);

	log_info(logCpu, logueoContexto);
	free(estado);
    free(logueoContexto);

}

void loguearEstadoMemoria(protocolo_memoria_cpu* respuestaMemoria, char*instruccionLeida){

	char* logueoMemoria = malloc(sizeof(char) * 10);

	strcpy(logueoMemoria, "mProc: ");
	string_append(&logueoMemoria, string_itoa(respuestaMemoria->pid));
	string_append(&logueoMemoria, " - ");

	if (respuestaMemoria->codOperacion == 'i') {
		string_append(&logueoMemoria, "Iniciado \n");
	}
	if (respuestaMemoria->codOperacion == 'l') {
		string_append(&logueoMemoria, "Pagina: ");
		string_append(&logueoMemoria, string_itoa(respuestaMemoria->numeroPagina));
		string_append(&logueoMemoria, " leida: ");
		string_append(&logueoMemoria, respuestaMemoria->mensaje);
	}

	if ((respuestaMemoria->codOperacion == 'f') && (respuestaMemoria->codAux == 'a')) {
		string_append(&logueoMemoria, "Fallo \n");
	}

	if ((respuestaMemoria->codOperacion == 'f') && (respuestaMemoria->codAux == 'i')) {
		string_append(&logueoMemoria, "Finalizado");
			}
	log_info(logCpu, logueoMemoria);
	free(logueoMemoria);

}


