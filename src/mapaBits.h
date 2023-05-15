#ifndef MAPA_BITS_H
#define MAPA_BITS_H

/*******************************************************************************
    ARCHIVO DE CABECERA DONDE SE ENCUENTRAN TODAS LAS FUNCIONES RELACIONADAS
	A LA POLITICA DE ADMINISTRACION DE MEMORIA "MAPA DE BITS"
*******************************************************************************/

/************************************ INCLUDES / DEFINES *************************************/

#include "memoria.h"        /* Archivo de Caexera para uso de Memoria */
#define PART_SIZE 4         /* Tamaño de cada Particion de Memoria    */
#define BITS 8              /* Definimos cantidad de filas y col      */

/*********************************** FUNCIONES A UTILIZAR ************************************/

// PRINCIPALES:
int allocBits(Process *);  /* Le Asigna Memoria a un Proceso específico         */
void initMapaBits();        /* Inicializa la Partición Inicial de Memoria        */
void freeBits(int);         /* Libera la Memoria Utilizada por un Proceso        */
void mapaDeBits();          /* Función que se Ejecuta en el Main                 */
void adaptBits();           /* Función para Adaptar la Memoria entre Políticas   */
// EXTRAS:
int enoughSpace(int, int);  /* Verifica que haya Suficiente Espacio en Memoria   */
int newSpace_Bits(int);     /* Determina la Potencia de 2 más cercana            */
void printMemoryBits();		/* Imprime el Estado de la Memoria en Pantalla       */
// HILOS:
sem_t mutex;
void *startBits(void *);   /* Función que se va a Encargar de manejar los Hilos */

/*********************************** FUNCIONES PRINCIPALES ***********************************/

int allocBits(Process *p) {
    /******************************************************************
		Esta funcion se encarga de Asignarle Memoria a un Proceso
		pasado por parámetro.

        Buscamos una partición Libre, si existe, verificamos que el
        tamaño total al sumar los huecos entre particiones sea 
        suficiente para podersela asignar al proceso.

        En caso contrario mostramos un mensaje de Error indicando
        que no existe el suficiente espacio para asignar la Memoria.
    *******************************************************************/
    for (int i = 0; i < memory.numPart; i++) {
        // Verificamos que está Libre:
        if (memory.partitions[i].isFree) {
            // Redondeamos el Tamaño del Proceso:
            int size = newSpace_Bits(p->size);
            // Verificamos que haya suficiente espacio:
            if (enoughSpace(i, size)) {
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
                    memory.partitions[i + j].isFree = FALSE;
                    memory.partitions[i + j].process = p;
                    memory.partitions[i + j].id = j + 1;
                }
				system("clear");
                printMemoryBits();
                printf("Memoria Asignada al Proceso %d (%d KB)\n", p->id, p->size);
                return TRUE;
            }
        }
    }
    return FALSE;
}
void initMapaBits() {
    /***********************************************************
        Inicializa la Memoria del Programa, creando BITS*BITS
        cantidad de particiones (en nuestro caso 8x8) 
    ************************************************************/
    memory.numPart = BITS * BITS;  /* Creamos la Matriz de 8x8 */

    for (int i = 0; i < memory.numPart; i++) {
        memory.partitions[i].size = PART_SIZE;	// Tamaño asignado a la Partición
        memory.partitions[i].process = NULL;	// No tiene Proceso Asignado
        memory.partitions[i].isFree = TRUE;	    // No está Creada
        memory.partitions[i].id = -1;	        // No tiene ID
        memory.size += 4;                       // Tamaño de Memoria
    }
}
void freeBits(int processID) {
    if (!memoryEmpty()) {
        int deleted = FALSE;
        for (int i = 0; i < memory.numPart; i++) {
            if (memory.partitions[i].process != NULL) {
                if (memory.partitions[i].process->id == processID) {
                    int canParts = 0;
                    if (memory.partitions[i].size <= 2) { canParts = 1; }
                    else { canParts = memory.partitions[i].size / PART_SIZE; }
                    for (int j = 0; j < canParts; j++) {
                        memory.partitions[i + j].process = NULL;
                        memory.partitions[i + j].isFree = TRUE;
                        memory.partitions[i + j].id = -1;
                    }
                    system("clear");
                    printMemoryBits();
                    printf("Memoria Liberada (Proceso %d)...\n", processID);
                    deleted = TRUE;
            }
            }
        }
        if (!deleted) { printf("No se pudo Liberar la Memoria del Proceso %d\n", processID); }
        return;
    }
}
void adaptBits() {
    /******************************************************
        Adaptamos la Memoria en caso de haber sido
        Manipulada por la Política de Sistemas Socios:
    ******************************************************/
    Memory aux;             /* Variable Auxiliar para Manejar Memoria  */
    aux.numPart = 0;        /* Cantidad de Particiones Existente       */
    aux.size = MEMORY_SIZE; /* Asignamos el Tamaño Total de la Memoria */
    for (int i = 0; i < memory.numPart; i++) {
        int canParts;       /* Cantidad de Particiones a Crear */
        /*************************************************************
            Dividimos el Tamaño de la Memoria entre 4 en caso de
            que este sea mayor o igual a 4, en caso contrario
            solo creamos una sola partición de Memoria:
        *************************************************************/
        if (memory.partitions[i].size <= 2) { canParts = 1; }
        else { canParts = memory.partitions[i].size / PART_SIZE; }
        for (int j = 0; j < canParts; j++) {
            // Asignamos a cada Partición la Información proveniente de Sistemas Socios:
            aux.partitions[aux.numPart].id = memory.partitions[i].id;
            aux.partitions[aux.numPart].isFree = memory.partitions[i].isFree;
            aux.partitions[aux.numPart].process = memory.partitions[i].process;
            aux.partitions[aux.numPart].size = PART_SIZE;
            aux.numPart++;
        }
    }
    // Finalizamos la Adaptación:
    memory = aux;
}

/************************************* FUNCIONES EXTRA ***************************************/

int enoughSpace(int i, int processSize) {
    /********************************************************
        Esta función verifica que, al sumar el tamaño de
        todos los huecos ubicados después de la actual,
        este sea suficiente para poderselo asignar al
        proceso.
    ********************************************************/
    int contador = 0;
    for (i; i < memory.numPart; i++) {
        if (memory.partitions[i].isFree) {
            contador++;
        } else { break; }
    }
    return (contador * PART_SIZE) >= processSize ? TRUE : FALSE;
}
int newSpace_Bits(int processSize) {
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
void printMemoryBits() {
    /****************************************************
        Mostramos el estado de cada uno de los bloques
        de Memoria en forma de Matriz:
    *****************************************************/
    // Mostramos el tamaño de la Memoria:
    printf("Tamaño de la Memoria:\t%i\n\n", memory.size);
    for (int i = 0; i < memory.numPart; i++) {
        printf("[%i] ", !memory.partitions[i].isFree);
        if (i > 0) {
            if (((i + 1) % 8) == 0) {
                printf("\n");
            }
        }
    }
    printf("\n");
}

/********************************* EJECUCION MAPAS DE BITS ***********************************/

void *startBits(void *arg) {
    // Recibimos la ID del Proceso:
	int ID = *(int *)arg;

	// Bloqueamos el Semáforo:
	sem_wait(&mutex);

	// Asignamos Memoria al Proceso:
	srand(time(NULL));
	while (!allocBits(&list[ID])) {
		/*******************************************************
			Si no se le pudo asignar Memoria a un Proceso,
			se muestra un mensaje de Error y se intenta
			liberar la Memoria de algún otro Proceso al
			azar hasta que haya espacio Disponible
		*******************************************************/
		system("clear");
		printMemoryBits();
		// Mostramos Mensaje de Error:
	    printf("No se le pudo asignar la Memoria al Proceso %d (%d KB)...\n", list[ID].id, list[ID].size);
		printf("Espacio Insuficiente...\n\n"); sysPause();
		// Liberamos la Memoria:
		int id = 2 + rand() % NUM_PROCESS - 1;
		freeBits(id);
		sysPause();
	}
    sysPause();

	// Liberamos el Semáforo:
	sem_post(&mutex);

	// Indicamos que ya terminó el Hilo:
	pthread_exit(NULL);
}

void mapaDeBits() {
    // Mostramos la Memoria:
    printMemoryBits();

    // Creamos los Hilos y el Semáforo:
    pthread_t hilos[NUM_PROCESS];
	sem_init(&mutex, 0, 1);

    /*************************************************************
		Esto permite terminar el Programa en Cualquier Momento
		con solo presionar la combinación de Teclas Ctrl + C:
	*************************************************************/

    // Creamos los Procesos:
    int powers[9] = {2, 3, 5, 7, 28, 64, 128, 14, 9};
    srand(time(NULL));
    for (int i = 0; i < NUM_PROCESS; i++) {
        int size = rand() % 9;
        list[i].size = powers[size];
        list[i].id = i + 1;
    }

    // Iniciamos los Hilos:
    for (int i = 0; i < NUM_PROCESS; i++) {
        pthread_create(&hilos[i], NULL, startBits, &i);
        pthread_join(hilos[i], NULL);
    }

	// // Registra el manejador de señal para SIGINT
	// signal(SIGINT, sigint_handler);
	// printf("Presiona Ctrl-C para Terminar el Programa en Cualquier Momento.\n");
	// sysPause();

    // while (1) {
        
    // }

    // Destruimos el Semáforo:
	sem_destroy(&mutex);
}

#endif