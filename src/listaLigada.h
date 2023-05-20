#ifndef LISTA_LIGADA_H
#define LISTA_LIGADA_H

/**********************************************************************************************
		ARCHIVO DE CABECERA DONDE SE ENCUENTRAN TODAS LAS FUNCIONES RELACIONADAS
		A LA POLITICA DE ADMINISTRACION DE MEMORIA "LISTAS LIGADAS"
**********************************************************************************************/

/************************************ INCLUDES / DEFINES *************************************/

#include "memoria.h"  /* Archivo de Cabecera donde está lo Relacionado a la Memoria */
#define PART_SIZE 4

/********************************** ELEMENTOS DE LA LISTA ************************************/

/* Estructura para los Nodos */
typedef struct Nodo {
    Partition info;     /* Partición de Memoria      */
    struct Nodo* sig;   /* Puntero al Siguiente Nodo */
} Nodo;

/*********************************** FUNCIONES A UTILIZAR ************************************/

// PRINCIPALES:
int allocListas(Process*);
void freeListas(int);
//void adaptListas();
void initListas();
void listas();
// MANEJO DE LISTAS:
void insertNode(Nodo**, Partition);
// EXTRAS:
void printMemoryLista(Nodo*);       /* Imprime el Estado de la Memoria en Pantalla       */
int enoughSpaceList(Nodo*, int);        /* Verifica que haya Suficiente Espacio en Memoria   */
int newSpaceLista(int);             /* Determina la Potencia de 2 más cercana            */
// HILOS:
sem_t mutex;
void *startListas(void *);

/************************************ VARIABLES GLOBALES *************************************/

Nodo *mem;       /* Memoria Para Uso en Listas */

/*********************************** FUNCIONES PRINCIPALES ***********************************/
// Asigna Memoria a un Proceso
int allocListas(Process *p) {
    Nodo *actual = mem;

    while (actual != NULL) {
        if (actual->info.isFree) {
            int size = newSpaceLista(p->size);
            if (enoughSpaceList(actual, size)) {
                int canParts = 0;
                /**********************************************
                    Para saber cuántos espacios de Memoria
                    vamos a necesitar para el Proceso,
                    dividimos el tamaño (redondeado) del
                    Proceso entre 4.
                **********************************************/
                if (size >= 4) { canParts = size / 4; }
                else { canParts = 1; }
                // Asignamos el Proceso:
                for (int j = 0; j < canParts; j++) {
                    actual->info.isFree = FALSE;
                    actual->info.process = p;
                    actual->info.id = j + 1;
                    actual = actual->sig;
                }
                system("clear");
                printMemoryLista(mem);
                printf("Memoria Asignada al Proceso %d (%d KB)\n", p->id, p->size);
                return TRUE;
            }
        }
        actual = actual->sig;
    }
    printf("Error: No se le pudo asignar Memoria la Proceso %d (%d KB).\n", p->id, p->size);
    printf("No hay Suficiente Espacio en Memoria.");
    return FALSE;
}
// Libera la Memoria Utilizada por un Proceso
void freeListas(int processID) {
    Nodo *actual = mem;
    if (actual != NULL) {
        while (actual != NULL) {
            if (actual->info.process != NULL) {
                if (actual->info.process->id == processID) {
                    int canParts = 0;
                    if (actual->info.process->size <= 2) { canParts = 1; }
                    else { canParts = actual->info.process->size / PART_SIZE; }
                    for (int j = 0; j < canParts; j++) {
                        actual->info.process = NULL;
                        actual->info.isFree = TRUE;
                        actual->info.id = 0;
                        actual = actual->sig;
                    }
                    system("clear");
                    printMemoryLista(mem);
                    printf("Memoria Liberada (Proceso %d)...\n", processID);
                    return;
                }
            }
            actual = actual->sig;
        }
        printf("No se pudo Liberar la Memoria del Proceso %d\n", processID);
        return;
    }
}
// Inicializa la Memoria
void initListas() {
    mem = NULL;
    for (int size = PART_SIZE; size <= MEMORY_SIZE; size += PART_SIZE) {
        Partition temp;
        temp.size = PART_SIZE;
        temp.process = NULL;
        temp.isFree = TRUE;
        temp.id = 0;
        insertNode(&mem, temp);
    }
    printf("\n");
    printMemoryLista(mem);
    sysPause();
}

/************************************* MANEJO DE LISTAS **************************************/
// Inserta un Elemento al Final de la Lista
void insertNode(Nodo **head, Partition info) {
    Nodo *newNode = (Nodo*)malloc(sizeof(Nodo));
    newNode->info = info;
    newNode->sig = NULL;

    if (*head == NULL) {
        *head = newNode;
        return;
    } 
    else {
        Nodo *actual = *head;
        while (actual->sig != NULL) {
            actual = actual->sig;
        }
        actual->sig = newNode;
    }
}

/************************************* FUNCIONES EXTRA ***************************************/
// Imprime la Info de cada Elemento de la Lista
void printMemoryLista(Nodo *head) {
    /****************************************************
        Mostramos el estado de cada uno de los bloques
        de Memoria en forma de Matriz:
    *****************************************************/
    Nodo *actual = head;

    // Mostramos el tamaño de la Memoria:
    printf("Tamaño de la Memoria:\t%i\n\n", MEMORY_SIZE);
    for (int i = 0; actual != NULL; i++) {
        printf("[%i] ", !actual->info.isFree);
        if (i > 0) {
            if (((i + 1) % 8) == 0) {
                printf("\n");
            }
        }
        actual = actual->sig;
    }
    printf("\n");
}
// Verifica que Haya Espacio en Memoria
int enoughSpaceList(Nodo *actual, int processSize) {
    /********************************************************
        Esta función verifica que, al sumar el tamaño de
        todos los huecos ubicados después de la actual,
        este sea suficiente para poderselo asignar al
        proceso.
    ********************************************************/
    int contador = 0;
    while (actual != NULL) {
        if (actual->info.isFree) {
            contador++;
            actual = actual->sig;
        } else { break; }
    }
    return (contador * PART_SIZE) >= processSize ? TRUE : FALSE;
}
// Redondea el Tamaño del Proceso
int newSpaceLista(int processSize) {
	/******************************************************
		Esta función se encarga de determinar que tamaño
		de Partición es el más indicado para Asignarle al
		proceso, tomando en cuenta el tamaño del Proceso
		y redondéandolo a la Potencia de 2 más cercana:
	*******************************************************/
	int powers[9] = {1, 2, 4, 8, 16, 32, 64, 128, 256};	// Potencias de 2
	for (int i = 0; i < 9; i++) {
		/**********************************************
			Devolvemos el Tamaño del Proceso en caso
			de que este ya sea una Potencia de 2:
		***********************************************/
		if (processSize == powers[i]) { return processSize; }
		/**********************************************
			En caso contrario devolvemos la Potencia
			de 2 más cercana al Tamaño del Proceso
		***********************************************/
		else if (processSize < powers[i]) { return powers[i]; }
	}
	return -1; // Retornamos -1 en caso de que haya ocurrido un error
}

// Funcion Principal de la Política
void listas() {
    initListas();

    int powers[9] = {2, 3, 5, 7, 28, 64, 128, 14, 9};
    srand(time(NULL));
    for (int i = 0; i < NUM_PROCESS; i++) {
        int size = rand() % 9;
        list[i].size = powers[size];
        list[i].id = i + 1;
        allocListas(&list[i]);
        sysPause();
    }

    for (int i = 0; i < NUM_PROCESS; i++) {
        freeListas(list[i].id);
        sysPause();
    }
}

#endif