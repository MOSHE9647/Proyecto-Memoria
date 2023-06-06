#ifndef MAPABITS_H
#define MAPABITS_H

/**********************************************************************************************
		ARCHIVO DE CABECERA DONDE SE ENCUENTRAN TODAS LAS FUNCIONES RELACIONADAS
		A LA POLITICA DE ADMINISTRACION DE MEMORIA "MAPA DE BITS"
**********************************************************************************************/

#include "memoria.h"		/* Archivo de Cabecera donde está lo Relacionado a la Memoria */

/*********************************** FUNCIONES A UTILIZAR ************************************/

bool allocMemoryBits(Process*);		/* Función para asignarle Memoria a un Proceso   	  	 */
bool enoughSpace(Nodo*, int);		/* Revisa que haya Suficiente Espacio para un Proceso 	 */
void freeMemoryBits(int);			/* Función para liberar la Memoria de un Proceso 	  	 */
void printMemoryBits();				/* Imprime el Estado de la Memoria en Pantalla        	 */
void initMemoryBits();				/* Inicializa la Matriz Inicial de la Memoria         	 */
bool defragMemory();				/* Función que Defragmenta la Memoria en Mapa de Bits 	 */
void adaptBits();					/* Adapta la Memoria de Socios a Mapa de Bits         	 */
bool isDefrag();					/* Verifica que la Defragmentación haya sido exitosa     */

/*********************************** FUNCIONES PRINCIPALES ***********************************/

// Inicializa la Memoria para Mapa de Bits
void initMemoryBits() {
	memory = (Lista*)malloc(sizeof(Lista));
	memory->head = NULL;
	for (int size = PART_SIZE; size <= MEMORY_SIZE; size += PART_SIZE) {
		Partition temp;
		temp.size = PART_SIZE;
		temp.process = NULL;
		temp.isFree = true;
		temp.id = 0;
		insertNode(&memory->head, temp);
	}
	printf("\n");
}
// Asigna Memoria a un Proceso
bool allocMemoryBits(Process *p) {
	if (firstTime) { firstTime = false; }
	else { adaptBits(); }
	
	Nodo *actual = memory->head;
	while (actual != NULL) {
		if (actual->info.isFree) {
			int size = newSpace(p->size);
			if (enoughSpace(actual, size)) {
				int canParts = 0;
				/**********************************************
					Para saber cuántos espacios de Memoria
					vamos a necesitar para el Proceso,
					dividimos el tamaño (redondeado) del
					Proceso entre 4.
				**********************************************/
				if (size >= 4) { canParts = size / PART_SIZE; }
				else { canParts = 1; }
				// Asignamos el Proceso:
				for (int j = 0; j < canParts; j++) {
					actual->info.isFree = false;
					actual->info.process = p;
					actual->info.id = j + 1;
					actual = actual->sig;
				}
				system("clear");
				printProcess();
				printMemoryBits();
				printf("Memoria Asignada al Proceso %d (%d KB)\n", p->id, p->size);

				int restante = actual->info.size - p->size;
				despInterno += restante;
				despExterno += MEMORY_SIZE - despInterno;

				return true;
			}
		}
		actual = actual->sig;
	}
	system("clear");
	printProcess();
	printMemoryBits();

	if (!isDefrag()) {
		defragMemory();
		if (allocMemoryBits(p)) {
			return true;
		}
	}

	printf("\nError: No se le pudo asignar Memoria al Proceso %d (%d KB).\n", p->id, p->size);
	printf("No hay Suficiente Espacio en Memoria.\n");
	return false;
}
// Libera la Memoria Utilizada por un Proceso
void freeMemoryBits(int processID) {
	Nodo *actual = memory->head;
	if (actual != NULL) {
		while (actual != NULL) {
			if (actual->info.process != NULL) {
				if (actual->info.process->id == processID) {
					int size = newSpace(actual->info.process->size);
					int canParts = 0;
					if (size >= 4) { canParts = size / PART_SIZE; }
					else { canParts = 1; }
					for (int j = 0; j < canParts; j++) {
						actual->info.process = NULL;
						actual->info.isFree = true;
						actual->info.id = 0;
						actual = actual->sig;
					}
					system("clear");
					printProcess();
					printMemoryBits();
					printf("Memoria Liberada (Proceso %d)...\n", processID);
					return;
				}
			}
			actual = actual->sig;
		}
		system("clear");
		printProcess();
		printMemoryBits();
		printf("No se pudo Liberar la Memoria del Proceso %d\n", processID);
		return;
	}
}
// Adapta la Memoria de Socios a Mapa de Bits
void adaptBits() {
	/******************************************************
		Adaptamos la Memoria en caso de haber sido
		Manipulada por la Política de Sistemas Socios:
	******************************************************/
	initMemoryBits();
	Nodo *actual = memory->head;

	for (int i = 0; i < mem.numPart; i++) {
		int canParts; /* Cantidad de Particiones a Crear */
		/*************************************************************
			Dividimos el Tamaño de la Memoria entre 4 en caso de
			que esta sea mayor a 4, en caso contrario creamos una 
			sola partición de Memoria:
		*************************************************************/
		if (mem.partitions[i].size <= PART_SIZE) { canParts = 1; }
		else { canParts = mem.partitions[i].size / PART_SIZE; }
		for (int j = 0; j < canParts; j++) {
			// Asignamos a cada Partición la Información proveniente de Sistemas Socios:
			actual->info.id = mem.partitions[i].id;
			actual->info.isFree = mem.partitions[i].isFree;
			actual->info.process = mem.partitions[i].process;
			actual = actual->sig;
		}
	}
	return;
}

/************************************* FUNCIONES EXTRA ***************************************/

// Verifica que Haya Espacio en Memoria
bool enoughSpace(Nodo *actual, int processSize) {
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
	return (contador * PART_SIZE) >= processSize ? true : false;
}
// Imprime la Info de cada Elemento de la Lista
void printMemoryBits() {
	/****************************************************
		Mostramos el estado de cada uno de los bloques
		de Memoria en forma de Lista y Matriz:
	*****************************************************/
	Nodo *actual = memory->head;

	printf("\n            ESTADO DE LA MEMORIA: Mapa de Bits\n");
	printf("══════════════════════════════════════════════════════════════\n");
	printf(" #\tPARTICION   \tTAMAÑO\t        ESTADO\t   PROCESO\t\n");
	printf("--------------------------------------------------------------\n");
	for (int i = 0; actual != NULL; i++) {
		if (actual->info.process != NULL) {
			printf(" %d\tPartición %d\t%d KB\t\t%s\t   %d (%d KB)\n", i + 1, actual->info.id, actual->info.size, actual->info.isFree ? "Libre" : "Ocupada", actual->info.process->id, actual->info.process->size);
		} else {
			printf(" %d\tPartición %d\t%d KB\t\t%s\t   %s (%d KB)\n", i + 1, actual->info.id, actual->info.size, actual->info.isFree ? "Libre" : "Ocupada", "N/A", 0);
		}
		actual = actual->sig;
	}
	printf("══════════════════════════════════════════════════════════════\n\n");

	printf("MATRIZ:\n");
	actual = memory->head;
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
	return;
}
// Verifica que la Desfragmentación de Memoria haya sido exitosa
bool isDefrag() {
	Nodo *actual = memory->head;
	bool nodeIsNotFree = false;

    while (actual != NULL) {
        if (!actual->info.isFree) { nodeIsNotFree = true; }
		else if (nodeIsNotFree) { return false; }
        actual = actual->sig;
    }

    return true;
}
// Defragmenta la Memoria en Mapa de Bits
bool defragMemory() {
	Nodo *actual = memory->head;
	Nodo *anterior = NULL;

	while (actual != NULL) {
		if (actual->info.isFree) {
			Nodo *temp = NULL;
			if (actual == memory->head) {
				temp = memory->head;
				memory->head = memory->head->sig;
				insertNode(&memory->head, temp->info);
			} else {
				temp = actual;
				anterior->sig = actual->sig;
				insertNode(&memory->head, temp->info);
			}
		} else { anterior = actual; }
		actual = actual->sig;
	}

	return isDefrag() ? true : false;
}

#endif