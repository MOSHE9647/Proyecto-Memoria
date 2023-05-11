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

#define MEMORY_SIZE 256  /* Memoria Total a Utilizar            */
#define NUM_PROCESS 5	 /* Cantidad de Procesos a Utilizar     */
#define PART_SIZE 4      /* Tamaño de cada Particion de Memoria */
#define MAPA_BITS 1      /* Indica que se usan Mapas de Bits    */
#define VARIABLE 2       /* Indica el uso de Memoria Variable   */
#define LIGADAS 3        /* Indica el uso de Listas Ligadas     */
#define SOCIOS 4         /* Indica el uso de Sistemas Socios    */
#define FALSE 0          /* Definimos FALSE como 0              */
#define TRUE 1           /* Definimos TRUE como 1               */
#define BITS 8           /* Definimos cantidad de filas y col   */

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
    Partition partitions[MEMORY_SIZE];
    int numPart;
    int size;
} Memory;

/************************************* VARIABLES **************************************/
Process list[NUM_PROCESS];
Memory memory;

// FUNCIONES A UTILIZAR:
void printMemoryStatus(int);		/* Imprime el Estado de la Memoria en Pantalla    */
void sigint_handler(int);           /* Detiene el programa al presionar CTRL+C        */
void initMemory(int);				/* Inicializa la Partición Inicial de Memoria     */
void sysPause();                    /* Detiene la Ejecución del Programa              */

/******************************** FUNCIONES GLOBALES **********************************/
void initMemory(int policy) {
    /**************************************************************
        Esta función se encarga de inicializar la memoria
        del programa en función de la política de Administración
        que haya sido escogida por el usuario
    ***************************************************************/
    switch (policy) {
        case MAPA_BITS: {
            /***********************************************************
                Inicializa la Memoria del Programa, creando BITS*BITS
                cantidad de particiones (en nuestro caso 8x8) 
            ************************************************************/
            memory.numPart = BITS * BITS;  /* Creamos la Matriz de 8x8 */

            for (int i = 0; i < memory.numPart; i++) {
                memory.partitions[i].size = PART_SIZE;	// Tamaño asignado a la Partición
                memory.partitions[i].process = NULL;	// No tiene Proceso Asignado
                memory.partitions[i].isFree = TRUE;	    // No está Creada
                memory.partitions[i].id = i + 1;	    // No tiene ID
                memory.size += 4;                       // Tamaño de Memoria
            }
            break;
        }
        case SOCIOS: {
            /********************************************************
                Inicializa la Memoria del Programa, creando primero
                una parición Inicial de 256kb (MEMORY_SIZE) y luego
                inicializando el resto de la Memoria en NULL
            *********************************************************/
            memory.numPart = 1;	// Iniciamos con 1 Partición

            // Iniciamos la Partición 1:
            memory.partitions[0].size = MEMORY_SIZE; // Le asignamos 256 KB de Memoria
            memory.partitions[0].process = NULL;	 // Lo iniciamos en NULL
            memory.partitions[0].isFree = TRUE;		 // Indicamos que está Libre
            memory.partitions[0].id = 1;			 // Le decimos que es la Partición 1

            // Iniciamos el Resto de la Memoria en NULL
            for (int i = 1; i < MEMORY_SIZE; i++) {
                memory.partitions[i].process = NULL;	// No tiene Proceso Asignado
                memory.partitions[i].isFree = -1;		// No está Creada
                memory.partitions[i].size = -1;		    // No tiene Tamaño
                memory.partitions[i].id = -1;			// No tiene ID
            }
            break;
        }
    }
}
void printMemoryStatus(int policy) {
    /********************************************************
        Esta funcion se encarga de imprimir por pantalla
        el estado en el que se encuentra la memoria en
        función de la Política que haya seleccionado
        el usuario para mostrar.
    *********************************************************/
    switch (policy) {
        case MAPA_BITS: {
            // Mostramos el tamaño de la Memoria:
            printf("Tamaño de la Memoria:\t%i\n\n", memory.size);
            /****************************************************
                Mostramos el estado de cada uno de los bloques
                de Memoria en forma de Matriz:
            *****************************************************/
            for (int i = 1; i <= memory.numPart; i++) {
                printf("[%i] ", !memory.partitions[i].isFree);
                if ((i % 8) == 0) {
                    printf("\n");
                }
            }
            printf("\n");
            break;
        }
        case SOCIOS: {
            /****************************************************
                Esta función se encarga de imprimir el Estado
                de la Memoria por Pantalla, mostrando la ID,
                el Tamaño, el Estado y el ID del Proceso asignado
                a esa Partición.
            *****************************************************/
            printf("\n                   ESTADO DE LA MEMORIA\n");
            printf("══════════════════════════════════════════════════════════════\n");
            printf(" #\tPARTICION\tTAMAÑO\t        ESTADO\t   PROCESO\t\n");
            printf("--------------------------------------------------------------\n");
            for (int i = 0; i < memory.numPart; i++) {
                /*************************************************
                    Si la Partición posee un Proceso, mostramos
                    por pantalla el ID y el Tamaño del mismo
                *************************************************/
                if (memory.partitions[i].process != NULL) { 
                    printf(" %d\tPartición %d\t%d KB\t\t%s\t   %d (%d KB)\n", i + 1, memory.partitions[i].id, memory.partitions[i].size, memory.partitions[i].isFree ? "Libre" : "Ocupada", memory.partitions[i].process->id, memory.partitions[i].process->size);
                }
                else {
                    /*******************************************
                        En caso contrario, solo mostramos las
                        siglas N/A por pantalla
                    ********************************************/
                    printf(" %d\tPartición %d\t%d KB\t\t%s\t   %s\n", i + 1, memory.partitions[i].id, memory.partitions[i].size, memory.partitions[i].isFree ? "Libre" : "Ocupada", "N/A");
                }
            }
            printf("══════════════════════════════════════════════════════════════\n\n");
            break;
        }
    }
}

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

#endif