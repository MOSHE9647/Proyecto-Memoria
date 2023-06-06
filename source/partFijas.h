#ifndef PARTFIJAS_H
#define PARTFIJAS_H

/**********************************************************************************************
		ARCHIVO DE CABECERA DONDE SE ENCUENTRAN TODAS LAS FUNCIONES RELACIONADAS
		A LA POLITICA DE ADMINISTRACION DE MEMORIA "PARTICIONES FIJAS DE VARIOS TAMAÑOS"
**********************************************************************************************/

#include "memoria.h"	/* Archivo de Cabecera donde se encuentra la Memoria */

/*********************************** FUNCIONES A UTILIZAR ************************************/

bool allocMemoryFijas(Process*);	/* Le Asigna Memoria a un Proceso específico         */
void freeMemoryFijas(int);			/* Liberar la Memoria Usada por un Proceso           */
void printMemoryFijas();			/* Imprime el Estado de la Memoria en Pantalla       */
void initMemoryFijas();				/* Inicializa la Partición Inicial de Memoria        */

/*********************************** FUNCIONES PRINCIPALES ***********************************/

// Asigna Memoria a un Proceso
bool allocMemoryFijas(Process *p) {
	/******************************************************************
		Esta funcion se encarga de Asignarle Memoria a un Proceso
		pasado por parámetro.
		
		Lo que se hace aquí es recorrer la Memoria verificando 
		si el tamaño del Proceso dado es menor al Tamaño de la
		Partición de Memoria, de ser así se le asigna esa Partición
		al Proceso.
	*******************************************************************/
	for (int i = 0; i < mem.numPart; i++) {
		if (mem.partitions[i].isFree) {
			if (p->size <= mem.partitions[i].size) {
				mem.partitions[i].isFree = false;	// Indicamos que está ocupada
				mem.partitions[i].process = p;		// Asignamos el Proceso
				system("clear");					// Limpiamos Pantalla
				printProcess();						// Imprimimos Lista de Procesos
				printMemoryFijas();					// Imprimimos el Estado de la Memoria
				/* Calculamos el Desperdicio Externo e Interno */
				int restante = mem.partitions[i].size - p->size;
				despInterno += restante;
				despExterno += MEMORY_SIZE - despInterno;
				return true; // Retornamos
			}
		}
	}
	// No se pudo asignar la Memoria
	return false;
}
// Libera la Memoria de un Proceso
void freeMemoryFijas(int processID) {
	/****************************************************************************
		Esta funcion se encarga de Liberar la Memoria que posee asignada
		un Proceso.

		Simplemente buscamos entre las Particiones el ID de Proceso que
		coincida con el ID pasado por parámetro, de encontrarlo se libera
		la Memoria ocupada por el mismo.
	******************************************************************************/
	for (int i = 0; i < mem.numPart; i++) {
		if (!mem.partitions[i].isFree) {
			if (mem.partitions[i].process->id == processID) {
				mem.partitions[i].process = NULL;   // No tiene Proceso Asignado
				mem.partitions[i].isFree = true;	// Está Libre
				return;								// Retornamos
			}
		}
	}
	system("clear");    // Limpiamos Pantalla de la Consola
	initMemoryFijas();  // Reiniciamos las Particiones de la Memoria
	printProcess();		// Mostramos la Lista de Procesos
}
// Inicializa las Particiones de Memoria
void initMemoryFijas() {
    /************************************************************
		Inicializa la Memoria de la Política creando un vector
		inicial con tamaños quemados, los cuales sumados dan
        el Tamaño Total de la Memoria, para luego iniciar y
        asignarle ese tamaño a cada una de las Particiones
	*************************************************************/
    // Vector de Tamaños Quemados
    int sizes[7] = {4, 16, 4, 64, 32, 128, 8};
    // Asignamos el Número de Particiones
    mem.numPart = 7;
	// Inicializamos las Particiones de la Memoria
	for (int i = 0; i < mem.numPart; i++) {
		mem.partitions[i].size = sizes[i];  // Obtiene un tamaño del Vector
		mem.partitions[i].process = NULL;   // No tiene Proceso Asignado
		mem.partitions[i].isFree = true;	// Está Libre
		mem.partitions[i].id = i + 1;	    // ID de Partición
	}
    return;
}
// Imprime en Pantalla el estado actual de la Memoria
void printMemoryFijas() {
    /****************************************************
		Esta función se encarga de imprimir el Estado
		de la Memoria por Pantalla, mostrando la ID,
		el Tamaño, el Estado y el ID del Proceso asignado
		a esa Partición.
	*****************************************************/
	printf("\n         ESTADO DE LA MEMORIA: Particiones Fijas\n");
	printf("══════════════════════════════════════════════════════════════\n");
	printf(" #\tPARTICION\tTAMAÑO\t        ESTADO\t   PROCESO\t\n");
	printf("--------------------------------------------------------------\n");
	for (int i = 0; i < mem.numPart; i++) {
		/*************************************************
			Si la Partición posee un Proceso, mostramos
			por pantalla el ID y el Tamaño del mismo
		*************************************************/
		if (mem.partitions[i].process != NULL) { 
			printf(" %d\tPartición %d\t%d KB\t\t%s\t   %d (%d KB)\n", i + 1, mem.partitions[i].id, mem.partitions[i].size, mem.partitions[i].isFree ? "Libre" : "Ocupada", mem.partitions[i].process->id, mem.partitions[i].process->size);
		}
		else {
			/*******************************************
				En caso contrario, solo mostramos las
				siglas N/A por pantalla
			********************************************/
			printf(" %d\tPartición %d\t%d KB\t\t%s\t   %s\n", i + 1, mem.partitions[i].id, mem.partitions[i].size, mem.partitions[i].isFree ? "Libre" : "Ocupada", "N/A");
		}
	}
	printf("══════════════════════════════════════════════════════════════\n\n");
}

#endif