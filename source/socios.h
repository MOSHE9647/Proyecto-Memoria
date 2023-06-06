#ifndef SOCIOS_H
#define SOCIOS_H

/**********************************************************************************************
		ARCHIVO DE CABECERA DONDE SE ENCUENTRAN TODAS LAS FUNCIONES RELACIONADAS
		A LA POLITICA DE ADMINISTRACION DE MEMORIA "SISTEMAS SOCIOS"
**********************************************************************************************/

#include "memoria.h"   /* Archivo de Cabecera donde está lo Relacionado a la Memoria */

/*********************************** FUNCIONES A UTILIZAR ************************************/

bool allocMemorySocios(Process*);	/* Le Asigna Memoria a un Proceso específico         */
void freeMemorySocios(int);			/* Liberar la Memoria Usada por un Proceso           */
void printMemorySocios();			/* Imprime el Estado de la Memoria en Pantalla       */
void delPartition(int);				/* Elimina una Partición de la Memoria               */
bool memoryEmpty();					/* Verifica si la Memoria Está Vacía				 */
void adaptSocios();					/* Adapta la Memoria de Listas Ligadas a Socios      */
void initSocios();					/* Inicializa la Partición Inicial de Memoria        */

/************************************ VARIABLES GLOBALES *************************************/

int partitionID = 2;	/* Se utiliza para asignarle un ID a las Particiones           */
int needSpace = 3;		/* Se utiliza para indicar si un Proceso necesita Espacio      */

/*********************************** FUNCIONES PRINCIPALES ***********************************/

bool allocMemorySocios(Process *p) {
	/******************************************************************
		Esta funcion se encarga de Asignarle Memoria a un Proceso
		pasado por parámetro.
		
		Lo que se hace aquí es recorrer la Memoria verificando 
		si el tamaño del Proceso dado es igual al Tamaño de la
		Partición de Memoria, de ser así se le asigna esa Partición
		al Proceso. En caso contrario se divide la partición hasta
		lograr conseguir el tamaño adecuado para el Proceso.

		En caso de no encontrar una parición libre para el Proceso
		se devuelve un mensaje de Error.
	*******************************************************************/
	for (int i = 0; i < mem.numPart; i++) {
		// Comprobamos que la Partición esté Libre:
		if (mem.partitions[i].isFree) {
			int newSize = newSpace(p->size); // Redondeamos el Tamaño del Proceso
			if (mem.partitions[i].size == newSize) {
				/******************************************
					Si el Tamaño del Proceso es Igual al
					Tamaño de la Partición, asignamos el
					Proceso a la Misma y retornamos
				*******************************************/
				// Esto es puramente Estético:
				system("clear");
				printProcess();
				printMemorySocios();
				printf("Asignandole Memoria al Proceso %d (%d KB).\n", p->id, p->size);
				sleep(1);
				system("clear");
				printProcess();
				printMemorySocios();
				printf("Asignandole Memoria al Proceso %d (%d KB)..\n", p->id, p->size);
				sleep(1);
				system("clear");
				printProcess();
				printMemorySocios();
				printf("Asignandole Memoria al Proceso %d (%d KB)...\n", p->id, p->size);
				sleep(1);
				system("clear");
				// Aquí asignamos los Procesos:
				mem.partitions[i].isFree = false; // Indicamos que la Partición está Ocupada
				mem.partitions[i].process = p;	  // Le asignamos el Proceso a la Partición
				printProcess();
				printMemorySocios();			  // Imprimimos el Estado de la Memoria
				partitionID++;					  // Incrementamos el ID
				printf("Proceso %d (%d KB) asignado a la Partición %d (%d KB)\n", p->id, p->size, mem.partitions[i].id, mem.partitions[i].size);
				
				int restante = mem.partitions[i].size - p->size;
				despInterno += restante;
				despExterno += MEMORY_SIZE - despInterno;

				return true; // Retornamos
			}
			else if(mem.partitions[i].size > p->size) {
				/*****************************************************
					Si el Tamaño de la Partición es más Grande que
					la del Proceso, dividimos la Partición en 2 y
					reiniciamos el contador 'i' para volver a
					verificar si ya cabe el Proceso en Memoria.
				******************************************************/
				// Puramente Estético:
				system("clear");
				printProcess();
				printMemorySocios();
				printf("Asignandole Memoria al Proceso %d (%d KB).\n", p->id, p->size);
				printf("Dividiendo la Partición %d (%d KB).\n", mem.partitions[i].id, mem.partitions[i].size);
				sleep(1);
				system("clear");
				printProcess();
				printMemorySocios();
				// Aquí Dividimos:
				Partition aux;							   // Partición Auxiliar
				aux.size = mem.partitions[i].size / 2;  // Le asignamos la mitad del Tamaño de la Particion Actual
				aux.process = NULL;						   // Le decimos que no tiene Proceso asignado
				aux.isFree = true;						   // Le indicamos que está libre
				aux.id = partitionID;					   // Le indicamos un ID
				if (firstTime) {
					/***********************************************************
						Si es la primera vez que se corre el programa
						se va a correr la partición principal 2 posiciones
						a la derecha, se le va a asignar 2 veces la particion
						auxiliar a la Memoria y se va a eliminar la primera
						partición que había (256kb o MEMORY_SIZE)
					************************************************************/
					for (int k = 0; k < 2; k++) {
						// Corremos 2 posiciones:
						for (int j = mem.numPart; j > i; j--) {
							mem.partitions[j] = mem.partitions[j - 1];
						}
					}
					mem.partitions[i] = aux; 					// Asignamos la Partición
					mem.partitions[i + 1] = aux;				// Volvemos a Asignar
					mem.partitions[i + 2].size = MEMORY_SIZE;	// Acomodamos la Partición 1
					mem.numPart++;						// Eliminamos la Partición 1
					firstTime = false;								// Indicamos que ya no es la Primera Vez
				} else {
					/*********************************************************
						Si no es la Primera vez que se corre el Programa
						se van a correr las Particiones solo 1 posición
						a la derecha, se va a asignar la partición auxiliar
						a la posición 'i' y se le va a cambiar el tamaño a
						la Partición en la Posición 'i + 1'
					**********************************************************/
					for (int j = mem.numPart; j > i; j--) {
						// Corremos las Particiones:
						mem.partitions[j] = mem.partitions[j - 1];
					}
					// Asignamos la particion Auxiliar:
					mem.partitions[i] = aux;	 // Asignamos Aux
					mem.partitions[i + 1] = aux; // Cambiamos el Tamaño de 'i + 1'
					mem.numPart++;		 // Incrementamos el Número de Particiones
				}
				i = -1;	// Reiniciamos el Contador del for
			}
		}
	}
	// Si no había espacio para asignar el Proceso, se muestra un mensaje de Error:
	return false;
}
void freeMemorySocios(int processID) {
	/****************************************************************************
		Esta funcion se encarga de Liberar la Memoria que posee asignado
		un Proceso.

		Lo que hacemos es verificar si las Particiones a la Derecha de la Actual
		tienen el mismo ID y si estas están o no libres. De estar libres, se
		libera la memoria de la Partición que posee al Proceso, se elimina la
		Partición libre y se le cambia el tamaño a la Particion multiplicando
		el tamaño actual por 2.
	******************************************************************************/
	// Siempre y cuando la Memoria no esté Vacía:
	if (!memoryEmpty()) {
		int deleted = false; // Para indicar si una Partición se Liberó
		for (int i = 0; i < mem.numPart; i++) {
			// Verificamos Si la Partición posee un Proceso:
			if (mem.partitions[i].process != NULL) {
				// Verificamos que el ID sea igual al del Proceso:
				if (mem.partitions[i].process->id == processID) {
					// Puramente Estético:
					system("clear");
					printMemorySocios();
					printf("Liberando la Memoria del Proceso %d (%d KB).\n", mem.partitions[i].process->id, mem.partitions[i].process->size);
					sleep(1);
					system("clear");
					printMemorySocios();
					printf("Liberando la Memoria del Proceso %d (%d KB)..\n", mem.partitions[i].process->id, mem.partitions[i].process->size);
					sleep(1);
					system("clear");
					printMemorySocios();
					printf("Liberando la Memoria del Proceso %d (%d KB)..\n", mem.partitions[i].process->id, mem.partitions[i].process->size);
					sleep(1);
					// Aquí Liberamos la Memoria:
					/************************************************************
						Si la siguiente Partición está Libre, y el tamaño
						de esta es igual al de la Partición Actual, eliminamos
						la siguiente y le duplicamos el Tamaño a la Primera
					*************************************************************/
					if (mem.partitions[i + 1].isFree) {
						if (mem.partitions[i].id == mem.partitions[i + 1].id) {
							if (mem.partitions[i].size == mem.partitions[i + 1].size) {
								mem.partitions[i].size *= 2;  // Duplicamos su Tamaño
								delPartition(i + 1);             // Eliminamos la Particion Sobrante
								deleted = true;					 // Indicamos que se Liberó
							}
						}
					}
					mem.partitions[i].process = NULL;  // Eliminamos el Proceso
					mem.partitions[i].isFree = true;   // Liberamos la Memoria
					deleted = true;						  // Indicamos que se Liberó
					// Estético:
					system("clear");
					printMemorySocios();
					printf("Memoria Liberada...\n");
					break;
				}
			}
			/******************************************************
				Con el siguiente ciclo, verificamos si existen
				Particiones Liberadas del mismo tamaño que no
				se llegaron a unir. De ser así se intentan unir:
			*******************************************************/
			for (int j = 0; j < mem.numPart; j++) {
				if (mem.partitions[j].isFree && mem.partitions[j + 1].isFree) {
					if (mem.partitions[j].size == mem.partitions[j + 1].size) {
						mem.partitions[j].size *= 2; // Duplicamos su Tamaño
						delPartition(j + 1); 			 // Eliminamos la Particion Sobrante
					}
				}
			}
		}
		if (memoryEmpty()) {
			/*********************************************
				Si la memoria está vacía, se reinician
				todas las variables de la Memoria
			**********************************************/
			system("clear");     // Limpiamos Pantalla de la Consola
			initSocios();  // Reiniciamos las Particiones de la Memoria
			printProcess();
			printMemorySocios(); // Imprimimos el Estado de la Memoria
			printf("Toda la Memoria ha sido Liberada...\n");
		}
		/*****************************************************
			Si 'deleted' es igual a 'false' significa que no
			se pudo Liberar la Memoria, por lo tanto se
			manda un mensaje de Error.
		******************************************************/
		if (!deleted) { printf("No se pudo Liberar la Memoria del Proceso %d\n", processID); }
		return; // Retornamos
	}
}
void initSocios() {
    /**********************************************************
		Inicializa la Memoria del Programa, creando primero
		una parición Inicial de 256kb (MEMORY_SIZE) y luego
		inicializando el resto de la Memoria en NULL
	***********************************************************/
	mem.numPart = 1;	// Iniciamos con 1 Partición

	// Iniciamos la Partición 1:
	mem.partitions[0].size = MEMORY_SIZE; // Le asignamos 256 KB de Memoria
	mem.partitions[0].process = NULL;	 // Lo iniciamos en NULL
	mem.partitions[0].isFree = true;		 // Indicamos que está Libre
	mem.partitions[0].id = 1;			 // Le decimos que es la Partición 1

	// Iniciamos el Resto de la Memoria en NULL
	for (int i = 1; i < MEMORY_SIZE; i++) {
		mem.partitions[i].process = NULL;	// No tiene Proceso Asignado
		mem.partitions[i].isFree = -1;		// No está Creada
		mem.partitions[i].size = -1;		    // No tiene Tamaño
		mem.partitions[i].id = -1;			// No tiene ID
	}
}
void adaptSocios() {
    /*****************************************************************
        Esta función se encarga de adaptar la Memoria modificada
        por la Política de Listas Ligadas para que esta pueda
        ser utilizada por Sistemas Socios realizando una copia
        de la misma.
    *****************************************************************/
    initSocios();                   /* Inicializamos la Memoria de Socios  */
	Nodo *actual = memory->head;    /* Toma el Primer Elemento de la Lista */
    while (actual != NULL) {        /* Recorremos la Lista */
        if (actual->info.process != NULL) {
            /*********************************************************
                Si existe un proceso, lo asignamos a la memoria de
                socios, como si fuera otro proceso más que entra a 
                memoria.
            **********************************************************/
            if (!allocMemorySocios(actual->info.process)) {
                /******************************************************
                    Si por alguna razón ocurre un Error al momento
                    de hacer la copia de un Proceso a Memoria,
                    procedemos a mostrar un mensaje de Error y a 
                    salir del Programa Inmediatamente
                ******************************************************/
                printf("Error: No se pudo completar la copia de Memoria.\n");
                sysPause();
                exit(1);
            }
        }
        actual = actual->sig; /* Pasamos al Siguiente Nodo de la Lista */
    }
    return;                 /* Retornamos y continuamos        */
}

/************************************* FUNCIONES EXTRA ***************************************/

void printMemorySocios() {
	/****************************************************
		Esta función se encarga de imprimir el Estado
		de la Memoria por Pantalla, mostrando la ID,
		el Tamaño, el Estado y el ID del Proceso asignado
		a esa Partición.
	*****************************************************/
	printf("\n              ESTADO DE LA MEMORIA: Socios\n");
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
void delPartition(int pos) {
	/*************************************************
		Esta función se encarga de eliminar una
		Partición de la Memoria según su Posición,
		corriendo todas las particiones siguientes
		una posición a la Izquierda
	**************************************************/
	for (int i = pos; i < mem.numPart; i++) {
		mem.partitions[i] = mem.partitions[i + 1];
	}
	mem.numPart--; // Disminuimos la Cantidad de Particiones
}
bool memoryEmpty() {
	/***********************************************
		Esta funcion se encarga de verificar si
		todas las Particiones de la Memoria están
		Libres. De ser así Retorna true
	************************************************/
	int allFree = true;	// Con esto Verificamos
	for (int i = 0; i < mem.numPart; i++) {
		/**********************************************
			Si existe alguna Partición que no esté
			libre, cambiamos el valor de allFree
			por false
		***********************************************/
		if (mem.partitions[i].isFree == false) {	allFree = false; break; }
	}
	/************************************************
		Si todas las Particiones están Libres,
		reiniciamos las Particiones de la Memoria
		y retornamos. En caso contrario, mostramos
		un mensaje de Error por pantalla
	*************************************************/
	if (allFree) { return true; }
	else { return false; }
}

#endif