// INCLUDE / DEFINE:
#include <semaphore.h>   /* Para uso de Semáforos     */
#include <pthread.h>     /* Para uso de los Hilos     */
#include <stdlib.h>      /* Para uso de Rand y Srand  */
#include <time.h>		 /* Para uso con Srand y Rand */
#include <signal.h>      /* Funcion que lee CTRL+Z    */
#include <unistd.h>      /* Para uso de Sleep         */
#include <stdio.h>       /* Para Entrada / Salida     */

#define MEMORY_SIZE 256  /* Memoria Total a Utilizar        */
#define NUM_PROCESS 5	 /* Cantidad de Procesos a Utilizar */
#define FALSE 0          /* Definimos FALSE como 0          */
#define TRUE 1           /* Definimos TRUE como 1           */

// ESTRUCTURAS:
/* Estructura para los Procesos */
typedef struct {
    int id;         /* ID del Proceso     */
    int size;       /* Tamaño del Proceso */
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
    Partition partitions[MEMORY_SIZE];  /* Vector de Particiones           */
    int numPartitions;                  /* Cantidad de Particiones Creadas */
    int size;                           /* Tamaño de la Memoria            */
} Memory;

// FUNCIONES A UTILIZAR:
void freeMemory(int processID);		/* Liberar la Memoria Usada por un Proceso        */
int newSpace(int processSize);		/* Determina la Potencia de 2 más cercana         */
void allocMemory(Process *p);		/* Le Asigna Memoria a un Proceso específico      */
void sigint_handler(int sig);       /* Se ejecuta al presionar las teclas CTRL+C      */
void delPartition(int pos);			/* Elimina una Partición de la Memoria            */
void socios(Memory *main);			/* Función que se Ejecuta en el Main              */
void printMemoryStatus();			/* Imprime el Estado de la Memoria en Pantalla    */
void initMemory();					/* Inicializa la Partición Inicial de Memoria     */
int memoryEmpty();					/* Verifica si Todas las Particiones están Libres */
void sysPause();                    /* Se encarga de Simular una Pausa en el Programa */

// HILOS:
sem_t mutex;				  /* Administrador de los Hilos en Sistemas Socios     */
void *startSocios(void *arg); /* Función que se va a Encargar de manejar los Hilos */

// VARIABLES GLOBALES:
Process list[NUM_PROCESS];	  /* Vector que contiene a cada uno de los Procesos del Programa */
int firstTime = TRUE;		  /* Indica si es la Primera vez que se Ejecuta el Programa      */
int partitionID = 2;		  /* Se utiliza para asignarle un ID a las Particiones           */
int needSpace = 3;			  /* Se utiliza para indicar si un Proceso necesita Espacio      */
Memory memory;				  /* Variable que Representa a la Memoria dentro del Programa    */

// FUNCIONES:
void initMemory() {
	/********************************************************
		Inicializa la Memoria del Programa, creando primero
		una parición Inicial de 256kb (MEMORY_SIZE) y luego
		inicializando el resto de la Memoria en NULL
	*********************************************************/
	memory.numPartitions = 1;	// Iniciamos con 1 Partición

	// Iniciamos la Partición 1:
	memory.partitions[0].size = MEMORY_SIZE;	// Le asignamos 256 KB de Memoria
	memory.partitions[0].isFree = TRUE;			// Indicamos que está Libre
	memory.partitions[0].process = NULL;		// Lo iniciamos en NULL
	memory.partitions[0].id = 1;				// Le decimos que es la Partición 1

	// Iniciamos el Resto de la Memoria en NULL
	for (int i = 1; i < MEMORY_SIZE; i++) {
		memory.partitions[i].process = NULL;	// No tiene Proceso Asignado
		memory.partitions[i].isFree = -1;		// No está Creada
		memory.partitions[i].size = -1;			// No tiene Tamaño
		memory.partitions[i].id = -1;			// No tiene ID
	}
}
int newSpace(int processSize) {
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
void delPartition(int pos) {
	/*************************************************
		Esta función se encarga de eliminar una
		Partición de la Memoria según su Posición,
		corriendo todas las particiones siguientes
		una posición a la Izquierda
	**************************************************/
	for (int i = pos; i < memory.numPartitions; i++) {
		memory.partitions[i] = memory.partitions[i + 1];
	}
	memory.numPartitions--; // Disminuimos la Cantidad de Particiones
}
void printMemoryStatus() {
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
	for (int i = 0; i < memory.numPartitions; i++) {
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
}
void allocMemory(Process *p) {
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

	for (int i = 0; i < memory.numPartitions; i++) {
		// Comprobamos que la Partición esté Libre:
		if (memory.partitions[i].isFree) {
			int newSize = newSpace(p->size); // Redondeamos el Tamaño del Proceso
			if (memory.partitions[i].size == newSize) {
				/******************************************
					Si el Tamaño del Proceso es Igual al
					Tamaño de la Partición, asignamos el
					Proceso a la Misma y retornamos
				*******************************************/
				// Esto es puramente Estético:
				system("clear");
				printMemoryStatus();
				printf("Asignandole Memoria al Proceso %d (%d KB).\n", p->id, p->size);
				sleep(1);
				system("clear");
				printMemoryStatus();
				printf("Asignandole Memoria al Proceso %d (%d KB)..\n", p->id, p->size);
				sleep(1);
				system("clear");
				printMemoryStatus();
				printf("Asignandole Memoria al Proceso %d (%d KB)...\n", p->id, p->size);
				sleep(1);
				system("clear");
				// Aquí asignamos los Procesos:
				memory.partitions[i].isFree = FALSE;  // Indicamos que la Partición está Ocupada
				memory.partitions[i].process = p;	  // Le asignamos el Proceso a la Partición
				printMemoryStatus();				  // Imprimimos el Estado de la Memoria
				partitionID++;						  // Incrementamos el ID
				printf("Proceso %d (%d KB) asignado a la Partición %d (%d KB)\n", p->id, p->size, memory.partitions[i].id, memory.partitions[i].size);
				return; // Retornamos
			}
			else if(memory.partitions[i].size > p->size) {
				/*****************************************************
					Si el Tamaño de la Partición es más Grande que
					la del Proceso, dividimos la Partición en 2 y
					reiniciamos el contador 'i' para volver a
					verificar si ya cabe el Proceso en Memoria.
				******************************************************/
				// Puramente Estético:
				system("clear");
				printMemoryStatus();
				printf("Dividiendo la Partición %d (%d KB).\n", memory.partitions[i].id, memory.partitions[i].size);
				sleep(1);
				system("clear");
				printMemoryStatus();
				// Aquí Dividimos:
				Partition aux;							   // Partición Auxiliar
				aux.size = memory.partitions[i].size / 2; // Le asignamos la mitad del Tamaño de la Particion Actual
				aux.process = NULL;						   // Le decimos que no tiene Proceso asignado
				aux.isFree = TRUE;						   // Le indicamos que está libre
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
						for (int j = memory.numPartitions; j > i; j--) {
							memory.partitions[j] = memory.partitions[j - 1];
						}
					}
					memory.partitions[i] = aux; 					// Asignamos la Partición
					memory.partitions[i + 1] = aux;				// Volvemos a Asignar
					memory.partitions[i + 2].size = MEMORY_SIZE;	// Acomodamos la Partición 1
					memory.numPartitions++;						// Eliminamos la Partición 1
					firstTime = FALSE;								// Indicamos que ya no es la Primera Vez
				} else {
					/*********************************************************
						Si no es la Primera vez que se corre el Programa
						se van a correr las Particiones solo 1 posición
						a la derecha, se va a asignar la partición auxiliar
						a la posición 'i' y se le va a cambiar el tamaño a
						la Partición en la Posición 'i + 1'
					**********************************************************/
					for (int j = memory.numPartitions; j > i; j--) {
						// Corremos las Particiones:
						memory.partitions[j] = memory.partitions[j - 1];
					}
					// Asignamos la particion Auxiliar:
					memory.partitions[i] = aux;	 // Asignamos Aux
					memory.partitions[i + 1] = aux; // Cambiamos el Tamaño de 'i + 1'
					memory.numPartitions++;		 // Incrementamos el Número de Particiones
				}
				i = -1;	// Reiniciamos el Contador del for
			}
		}
	}
	// Si no había espacio para asignar el Proceso, se muestra un mensaje de Error:
    printf("No se le pudo asignar la Memoria al Proceso %d (%d KB) por Falta de Espacio...\n", p->id, p->size);
	needSpace = TRUE; // Indicamos que se Necesita Espacio
	return;
}
int memoryEmpty() {
	/***********************************************
		Esta funcion se encarga de verificar si
		todas las Particiones de la Memoria están
		Libres. De ser así Retorna TRUE
	************************************************/
	int allFree = TRUE;	// Con esto Verificamos
	for (int i = 0; i < memory.numPartitions; i++) {
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
void freeMemory(int processID) {
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
		int deleted = FALSE; // Para indicar si una Partición se Liberó
		for (int i = 0; i < memory.numPartitions; i++) {
			// Verificamos Si la Partición posee un Proceso:
			if (memory.partitions[i].process != NULL) {
				// Verificamos que el ID sea igual al del Proceso:
				if (memory.partitions[i].process->id == processID) {
					// Puramente Estético:
					system("clear");
					printMemoryStatus();
					printf("Liberando la Memoria del Proceso %d (%d KB).\n", memory.partitions[i].process->id, memory.partitions[i].process->size);
					sleep(1);
					system("clear");
					printMemoryStatus();
					printf("Liberando la Memoria del Proceso %d (%d KB)..\n", memory.partitions[i].process->id, memory.partitions[i].process->size);
					sleep(1);
					system("clear");
					printMemoryStatus();
					printf("Liberando la Memoria del Proceso %d (%d KB)..\n", memory.partitions[i].process->id, memory.partitions[i].process->size);
					sleep(1);
					// Aquí Liberamos la Memoria:
					/************************************************************
						Si la siguiente Partición está Libre, y el tamaño
						de esta es igual al de la Partición Actual, eliminamos
						la siguiente y le duplicamos el Tamaño a la Primera
					*************************************************************/
					if (memory.partitions[i + 1].isFree) {
						if (memory.partitions[i].id == memory.partitions[i + 1].id) {
							if (memory.partitions[i].size == memory.partitions[i + 1].size) {
								memory.partitions[i].size *= 2; // Duplicamos su Tamaño
								delPartition(i + 1);             // Eliminamos la Particion Sobrante
								deleted = TRUE;					 // Indicamos que se Liberó
							}
						}
					}
					memory.partitions[i].process = NULL; // Eliminamos el Proceso
					memory.partitions[i].isFree = TRUE;  // Liberamos la Memoria
					deleted = TRUE;						  // Indicamos que se Liberó
					// Estético:
					system("clear");
					printMemoryStatus();
					printf("Memoria Liberada...\n");
					break;
				}
			}
			/******************************************************
				Con el siguiente ciclo, verificamos si existen
				Particiones Liberadas del mismo tamaño que no
				se llegaron a unir. De ser así se intentan unir:
			*******************************************************/
			for (int j = 0; j < memory.numPartitions; j++) {
				if (memory.partitions[j].isFree && memory.partitions[j + 1].isFree) {
					if (memory.partitions[j].size == memory.partitions[j + 1].size) {
						memory.partitions[j].size *= 2; // Duplicamos su Tamaño
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
			initMemory();        // Reiniciamos las Particiones de la Memoria
			printMemoryStatus(); // Imprimimos el Estado de la Memoria
			printf("Toda la Memoria ha sido Liberada...\n");
		}
		/*****************************************************
			Si 'deleted' es igual a 'FALSE' significa que no
			se pudo Liberar la Memoria, por lo tanto se
			manda un mensaje de Error.
		******************************************************/
		if (!deleted) { printf("No se pudo Liberar la Memoria del Proceso %d\n", processID); }
		return; // Retornamos
	}
}
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
void sigint_handler(int sig) {
    /************************************************
        Esta función se ejecuta cuando el programa
        detecta que se presionó CTRL+C
    *************************************************/
    printf("\nPrograma detenido por el usuario.\n");
    exit(0);
}
void *startSocios(void *arg) {
	/***********************************************
		Esta función es la que se encarga de la
		Funcionalidad de cada uno de los Hilos.

		Empezamos asigandole Memoria a un Proceso,
		y aumentamos la Variable 'needSpace' que
		utilizamos para simular que un Proceso ya
		Termino y/o que necesita Espacio.

		Cuando 'needSpace' sea divisible entre 2
		se va a Liberar la Memoria de un Proceso.
	************************************************/
	// Recibimos la ID del Proceso:
	int ID = *(int *)arg;

	// Bloqueamos el Semáforo:
	sem_wait(&mutex);

	// Asignamos Memoria al Proceso:
	allocMemory(&list[ID]);
	sysPause();

	// Liberamos la Memoria del Proceso:
	if (needSpace % 2 == 0 || needSpace == TRUE) {
		freeMemory(ID + 1);
		sysPause();
	}
	// Limpiamos la Memoria si se llegó al Último Proceso:
	if (ID == NUM_PROCESS - 1) {
		for (int i = 0; i < NUM_PROCESS; i++) {
			freeMemory(i + 1);
			sysPause();
		}
	}
	// Incrementamos needSpace:
	needSpace++;

	// Liberamos el Semáforo:
	sem_post(&mutex);

	// Indicamos que ya terminó el Hilo:
	pthread_exit(NULL);
}
int main() {
    // Limpiamos Pantalla:
    system("clear");

    // Creamos e Inicializamos la Memoria:
	initMemory();
	printMemoryStatus();

	// Creamos los Hilos y el Semáforo:
	pthread_t hilos[NUM_PROCESS];
	sem_init(&mutex, 0, 1);

	/*************************************************************
		Esto permite terminar el Programa en Cualquier Momento
		con solo presionar la combinación de Teclas Ctrl + C:
	*************************************************************/

	// Registra el manejador de señal para SIGINT
	signal(SIGINT, sigint_handler);
	printf("Presiona Ctrl-C para Terminar el Programa en Cualquier Momento.\n");
	sysPause();

	while(1) {
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
			pthread_create(&hilos[i], NULL, startSocios, &i);
			pthread_join(hilos[i], NULL);
		}
	}

	// Destruimos el Semáforo:
	sem_destroy(&mutex);
    return 0;
}