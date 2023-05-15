#ifndef MEMORIA_H
#define MEMORIA_H

/***********************************************************
	ARCHIVO DE CABECERA QUE CONTIENE TODA LA INFORMACIÓN
	RELACIONADA A LOS PROCESOS, MEMORIA Y SUS PARTICIONES
************************************************************/

/*************************************** INCLUDES **************************************/

#include <semaphore.h>   /* Para uso de Semáforos    */
#include <pthread.h>     /* Para uso de los Hilos    */
#include <stdlib.h>      /* Para uso de Rand y Srand */
#include <signal.h>      /* Funcion que lee CTRL+Z   */
#include <unistd.h>      /* Para uso de Sleep        */
#include <stdio.h>       /* Para Entrada / Salida    */

/*************************************** DEFINES ***************************************/

// PARTE LOGICA:
#define MEMORY_SIZE 256  /* Memoria Total a Utilizar            */
#define NUM_PROCESS 5	 /* Cantidad de Procesos a Utilizar     */
// OPCIONES:
#define FALSE 0          /* Definimos FALSE como 0              */
#define TRUE 1           /* Definimos TRUE como 1               */

/************************************* ESTRUCTURAS *************************************/

/* Estructura para los Procesos */
typedef struct {
	int id;     /* ID del Proceso     */
	int size;   /* Tamaño del Proceso */
} Process;
/* Estructura para las Particiones */
typedef struct {
	int id;             /* ID de la Particiión       */
	int size;           /* Tamaño de la Partición    */
	int isFree;         /* Indica si está libre      */
	Process *process;   /* Para almacenar el Proceso */
} Partition;
/* Estructura para la Memoria */
typedef struct {
	Partition partitions[MEMORY_SIZE]; /* Particiones */
	int numPart;			 /* Numero de Particiones */
	int size;				 /* Tamaño de la Memoria  */
} Memory;

/************************************* VARIABLES **************************************/

Process list[NUM_PROCESS];	/* Lista de Procesos	*/
Memory memory;				/* Memoria del Programa */
int needSpace;

/************************************** EXTRAS ****************************************/
/* Función para Pausar el Programa */
void sysPause() {
	/****************************************
		Esta función se encarga de simular
		el system("pause") que se utiliza
		en Windows
	*****************************************/
	printf("Press enter to continue...");
	fflush(stdin);
	int c = getchar();
	getchar();
}
/* Función para cerrar el Programa */
void sigint_handler(int sig) {
	/************************************************
		Esta función se ejecuta cuando el programa
		detecta que se presionó CTRL+C
	*************************************************/
	printf("\nPrograma detenido por el usuario.\n");
	exit(0);
}

/* Verifica si Todas las Particiones están Libres */
int memoryEmpty() {
	/***********************************************
		Esta funcion se encarga de verificar si
		todas las Particiones de la Memoria están
		Libres. De ser así Retorna TRUE
	************************************************/
	int allFree = TRUE;	// Con esto Verificamos
	for (int i = 0; i < memory.numPart; i++) {
		/**********************************************
			Si existe alguna Partición que no esté
			libre, cambiamos el valor de allFree
			por FALSE
		***********************************************/
		if (memory.partitions[i].isFree == FALSE) {	allFree = FALSE; break; }
	}
	/************************************************
		Si todas las Particiones están Libres,
		reiniciamos las Particiones de la Memoria
		y retornamos. En caso contrario, mostramos
		un mensaje de Error por pantalla
	*************************************************/
	if (allFree) { return TRUE; }
	else { return FALSE; }
}

#endif