#ifndef MEMORIA_H
#define MEMORIA_H

/***********************************************************
    ARCHIVO DE CABECERA QUE CONTIENE TODA LA INFORMACIÓN
    RELACIONADA A LOS PROCESOS, MEMORIA Y SUS PARTICIONES
************************************************************/

#include <semaphore.h>   /* Para uso de Semáforos    */
#include <pthread.h>     /* Para uso de los Hilos    */
#include <stdlib.h>      /* Para uso de Rand y Srand */
#include <signal.h>      /* Funcion que lee CTRL+Z   */
#include <unistd.h>      /* Para uso de Sleep        */
#include <stdio.h>       /* Para Entrada / Salida    */

#define MEMORY_SIZE 256  /* Memoria Total a Utilizar        */
#define NUM_PROCESS 5	 /* Cantidad de Procesos a Utilizar */
#define FALSE 0          /* Definimos FALSE como 0          */
#define TRUE 1           /* Definimos TRUE como 1           */

// ESTRUCTURAS:
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
    Partition partitions[MEMORY_SIZE];
    int numPart;
    int size;
} Memory;

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

#endif