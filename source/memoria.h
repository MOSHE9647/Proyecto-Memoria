#ifndef MEMORIA_H
#define MEMORIA_H

/***********************************************************
	ARCHIVO DE CABECERA QUE CONTIENE TODA LA INFORMACIÓN
	RELACIONADA A LOS PROCESOS, MEMORIA Y SUS PARTICIONES
************************************************************/

/*************************************** INCLUDES **************************************/

#include <pthread.h>     /* Para uso de los Hilos     */
#include <stdbool.h>     /* Para uso de Booleanos     */
#include <termios.h>	 /* Para la función getch()   */
#include <stdlib.h>      /* Para uso de Rand y Srand  */
#include <string.h>      /* Para manejo de strings    */
#include <signal.h>      /* Funcion que lee CTRL+Z    */
#include <unistd.h>      /* Para uso de Sleep         */
#include <stdio.h>       /* Para Entrada / Salida     */
#include <time.h>		 /* Para uso con Rand y Srand */

/*************************************** DEFINES ***************************************/

#define MEMORY_SIZE 256     /* Memoria Total a Utilizar            */
#define NUM_PROCESS 2	    /* Cantidad de Procesos a Utilizar     */
#define PART_SIZE 4         /* Tamaño de cada Partición de Memoria */

#define LISTAS_LIGADAS 2    /* Variable para indicar Listas Listas */
#define PART_FIJAS 4        /* Variable para indicar Part. Fijas   */
#define MAPA_BITS 1         /* Variable para indicar Mapas de Bits */
#define SOCIOS 3            /* Variable para indicar uso de Socios */

/************************************* ESTRUCTURAS *************************************/

/* Estructura para los Procesos */
typedef struct {
	int id;     	/* ID del Proceso          		*/
	int size;   	/* Tamaño del Proceso      		*/
	int numExe;		/* Cantidad de Ejecuciones 		*/
	int numES;		/* Cantidad de Iteraciones E/S	*/
	int execTime;	/* Tiempo en Ejecución		    */
	int ESTime;		/* Tiempo en Espera por E/S 	*/
	int promExec;	/* Promedio Tiempo de Ejecución */
	int promES;		/* Promedio Tiempo Espera E/S 	*/
	int politica;	/* Indica que Política Uso	    */
	int estado;		/* Indica en que lista está     */
	int ajuste;		/* Indica el Ajuste Usado en LL */
} Process;

/* Estructura para las Particiones */
typedef struct {
	int id;             /* ID de la Particiión       */
	int size;           /* Tamaño de la Partición    */
	int isFree;         /* Indica si está libre      */
	Process *process;   /* Para almacenar el Proceso */
} Partition;

/* Estructura para los Nodos */
typedef struct Nodo {
    Partition info;     /* Partición de Memoria      */
    struct Nodo* sig;   /* Puntero al Siguiente Nodo */
} Nodo;

/* Estructura para las Listas */
typedef struct {
    struct Nodo *head;			/* Inicio de la Lista    */
    struct Nodo *lastUsedNode;	/* Último Nodo Utilizado */
} Lista;

/* Estructura para Memoria de Socios */
typedef struct {
	Partition partitions[MEMORY_SIZE]; /* Particiones */
	int numPart;			 /* Numero de Particiones */
	int size;				 /* Tamaño de la Memoria  */
} Memory;

/* Estructura para las Listas de Procesos */
typedef struct Node {
    Process info;       /* Proceso a Almacenar       */
    struct Node *sig;   /* Puntero al Siguiente Nodo */
} Node;

/* Estructura para las Colas */
typedef struct {
    Node *head;             /* Inicio de las Colas         */
    pthread_mutex_t mutex;  /* Mutex para Manejo con Hilos */
} Cola;

/************************************* FUNCIONES **************************************/

void insertMiddle(Nodo*, Partition);  /* Inserta un Elemento en Medio de la Lista    */
void insertNode(Nodo**, Partition);	  /* Inserta un Elemento al Final de la Lista    */
void sysPause();					  /* Realiza una Pausa en la Consola             */
int getch();						  /* Permite leer una tecla sin presionar enter  */

/************************************* VARIABLES **************************************/

bool firstTime = true;		 /* Indica si es la Primer vez que se ejecuta el Programa */
Lista *memory;				 /* Estructura de Memoria Utilizada en Mapa Bits y Listas */
Memory mem;  			 	 /* Estructura de Memoria Utilizada en Socios y Part Fija */

double totTiempoEjec = 0.0;	 /* Tiempo Total de Ejecución de Todos los Procesos 	  */
int hilosEjecutandose = 0;	 /* Cantidad de Procesos en Ejecución 					  */
double totTiempoES = 0.0;    /* Tiempo Total de Espera E/S de Todos los Procesos 	  */

int despExterno = 0;         /* Desperdicio Externo de Memoria	*/
int despInterno = 0;         /* Desperdicio Interno de Memoria	*/

Cola *solicitudes;           /* Variable para la Cola de Solicitudes     */
Cola *ejecucion;             /* Variable para la Cola de Ejecución       */
Cola *espera;                /* Variable para la Cola de Espera E/S      */
Cola *listos;                /* Variable para la Cola de Listos          */
Cola *todos;                 /* Variable para Guardar Todos los Procesos */

int politica = 1;            /* Politica que se está usando actualmente  */
int ajuste = 7;              /* Ajuste en caso de usar Listas Ligadas    */

/********************************* MANEJO DE LISTAS ***********************************/
// Inserta un Elemento al Final de la Lista
void insertNode(Nodo **head, Partition info) {
	// Creamos la Memoria:
	Nodo *newNode = (Nodo*)malloc(sizeof(Nodo));
    newNode->info = info;
    newNode->sig = NULL;

	// Si está vacía, asignamos al primer espacio
    if (*head == NULL) {
        *head = newNode;
        return;
    } 
	// Si no, entonces buscamos el final de la lista
    else {
        Nodo *actual = *head;
        while (actual->sig != NULL) {
            actual = actual->sig;
        }
		// Insertamos el Nodo
        actual->sig = newNode;
    }
}
// Inserta un Elemento en Medio de la Lista
void insertMiddle(Nodo* prevNode, Partition info) {
    // Verificamos que el Nodo Anterior no sea NULL
	if (prevNode == NULL) {
        printf("El nodo anterior no puede ser NULL.\n");
        return;
    }
	// Insertamos el Nodo:
    Nodo* newNode = (Nodo*)malloc(sizeof(Nodo));
    newNode->info = info;
    newNode->sig = prevNode->sig;
    prevNode->sig = newNode;
}
// Inserta un Proceso en la Lista
void insertProcess(Cola *lista, Process info) {
    // Creamos la Memoria
	Node *newNode = (Node*)malloc(sizeof(Node));
    newNode->info = info;
    newNode->sig = NULL;

	// Si está vacía, asignamos al primer espacio
    if (lista->head == NULL) {
        lista->head = newNode;
        pthread_mutex_unlock(&lista->mutex);
        return;
    } 
	// Si no, entonces buscamos el final de la lista
    else {
        Node *actual = lista->head;
        while (actual->sig != NULL) {
            actual = actual->sig;
        }
		// Insertamos el Nodo
        actual->sig = newNode;
    }
    return;
}
// Elimina un Proceso de la Lista
void delProcess (Cola *lista, int processID) {
    Node* actual = lista->head;
    Node* anterior = NULL;

    // Buscar el proceso con el ID dado
    while (actual != NULL && actual->info.id != processID) {
        anterior = actual;
        actual = actual->sig;
    }

    if (actual != NULL) {
        // El proceso fue encontrado, eliminarlo de la lista
        if (anterior != NULL) {
            anterior->sig = actual->sig;
        } else {
            lista->head = actual->sig;
        }
        free(actual);
    }
}

/************************************** EXTRAS ****************************************/

// Redondea el Tamaño del Proceso
int newSpace(int processSize) {
	/******************************************************
		Esta función se encarga de determinar que tamaño
		de Partición es el más indicado para Asignarle al
		proceso, tomando en cuenta el tamaño del Proceso
		y redondéandolo a la Potencia de 2 más cercana:
	*******************************************************/
	// Vector con todas las Potencias de 2 utilizadas
	int powers[9] = {1, 2, 4, 8, 16, 32, 64, 128, 256};
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
/* Función para Pausar el Programa */
void sysPause() {
	/****************************************
		Esta función se encarga de simular
		el system("pause") que se utiliza
		en Windows
	*****************************************/
	printf("Press enter to continue...\n");
	getch();
}
/* Permite leer una tecla sin presionar enter */
int getch() {
    /************************************************
        Esta función se encarga de leer una tecla
        sin la necesidad de presionar Enter, para
        eso realizamos una modificación en las
        variables de Entorno de la Terminal.
    *************************************************/
    struct termios old, newT;
    int ch;

    /* Modificamos la Terminal */
    tcgetattr(STDIN_FILENO, &old);
    newT = old;
    newT.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newT);

    // Leemos la Tecla
    ch = getchar();

    /* Reestablecemos la Terminal */
    tcsetattr(STDIN_FILENO, TCSANOW, &old);

    /* Retornamos la Tecla */
    return ch;
}
// Cuenta los Nodos que hay en Solicitudes
int contarNodos() {
    /**************************************************
        Esta función se encarga de contar los
        Nodos existentes en la Lista de Solicitudes
        para poder mostrarlos en Pantalla
    ***************************************************/
    Node *actual = solicitudes->head;
    int canNodos = 0;
    while (actual != NULL) {
        canNodos ++;
        actual = actual->sig;
    }
    return canNodos;
}
// Muestra la Información de los Procesos en Pantalla
void printProcess() {
    /**********************************************************
        Esta función se encarga de Mostrar por Pantalla
        todos y cada uno de los Procesos que se encuentran
        dentro del Contexto de Ejecución, mostrando en que
        Lista se encuentra cada uno.
    ***********************************************************/
    Node *actual = ejecucion->head;
    printf("                     LISTA DE PROCESOS\n");
    printf("══════════════════════════════════════════════════════════════\n");
    printf(" PID\tTAMAÑO\tN° EXEC\tN° E/S\tPOLITICA\tESTADO\n");
    printf("--------------------------------------------------------------\n");
    
    /***********************************************
        NOTA= Contraer los for de ser necesario

        En esta parte usamos varios Switch para
        comprobar la política y el ajuste usados
        en cada Proceso para poder mostrarlos por
        Pantalla en la Tabla.
    ************************************************/

    // Lista de Ejecución
    for (actual; actual != NULL; actual = actual->sig) {
        char estado[20] = "";
        char politica[20] = "";

        switch (actual->info.estado) {
            case 1:
                strcpy(estado, "Ejecucion");
                break;
            case 2:
                strcpy(estado, "Espera E/S");
                break;
            case 3:
                strcpy(estado, "Listo");
                break;
            case 4:
                strcpy(estado, "Terminado");
                break;
            case 5:
                strcpy(estado, "Solicitud");
        }
        switch (actual->info.politica) {
            case MAPA_BITS:
                strcpy(politica, "Mapa de Bits");
                break;
            case LISTAS_LIGADAS:
                if (actual->info.ajuste == 7) {
                    strcpy(politica, "LL (Primer)");
                }
                else if (actual->info.ajuste == 8) {
                    strcpy(politica, "LL (Segundo)");
                }
                else if (actual->info.ajuste == 9) {
                    strcpy(politica, "LL (Mejor)");
                }
                else if (actual->info.ajuste == 10) {
                    strcpy(politica, "LL (Peor)");
                }
                break;
            case SOCIOS:
                strcpy(politica, "Socios   ");
                break;
            case PART_FIJAS:
                strcpy(politica, "Part. Fijas");
                break;
        }

        printf(" %d\t%d KB\t%d\t%d\t%s\t%s\n", actual->info.id, actual->info.size, actual->info.numExe, actual->info.numES, politica, estado);
    }

    // Lista de Espera
    actual = espera->head;
    for (actual; actual != NULL; actual = actual->sig) {
        char estado[20] = "";
        char politica[20] = "";

        switch (actual->info.estado) {
            case 1:
                strcpy(estado, "Ejecucion");
                break;
            case 2:
                strcpy(estado, "Espera E/S");
                break;
            case 3:
                strcpy(estado, "Listo");
                break;
            case 4:
                strcpy(estado, "Terminado");
                break;
            case 5:
                strcpy(estado, "Solicitud");
        }
        switch (actual->info.politica) {
            case MAPA_BITS:
                strcpy(politica, "Mapa de Bits");
                break;
            case LISTAS_LIGADAS:
                if (actual->info.ajuste == 7) {
                    strcpy(politica, "LL (Primer)");
                }
                else if (actual->info.ajuste == 8) {
                    strcpy(politica, "LL (Segundo)");
                }
                else if (actual->info.ajuste == 9) {
                    strcpy(politica, "LL (Mejor)");
                }
                else if (actual->info.ajuste == 10) {
                    strcpy(politica, "LL (Peor)");
                }
                break;
            case SOCIOS:
                strcpy(politica, "Socios   ");
                break;
            case PART_FIJAS:
                strcpy(politica, "Part. Fijas");
                break;
        }

        printf(" %d\t%d KB\t%d\t%d\t%s\t%s\n", actual->info.id, actual->info.size, actual->info.numExe, actual->info.numES, politica, estado);
    }

    // Lista de Listos
    actual = listos->head;
    for (actual; actual != NULL; actual = actual->sig) {
        char estado[20] = "";
        char politica[20] = "";

        switch (actual->info.estado) {
            case 1:
                strcpy(estado, "Ejecucion");
                break;
            case 2:
                strcpy(estado, "Espera E/S");
                break;
            case 3:
                strcpy(estado, "Listo");
                break;
            case 4:
                strcpy(estado, "Terminado");
                break;
            case 5:
                strcpy(estado, "Solicitud");
        }
        switch (actual->info.politica) {
            case MAPA_BITS:
                strcpy(politica, "Mapa de Bits");
                break;
            case LISTAS_LIGADAS:
                if (actual->info.ajuste == 7) {
                    strcpy(politica, "LL (Primer)");
                }
                else if (actual->info.ajuste == 8) {
                    strcpy(politica, "LL (Segundo)");
                }
                else if (actual->info.ajuste == 9) {
                    strcpy(politica, "LL (Mejor)");
                }
                else if (actual->info.ajuste == 10) {
                    strcpy(politica, "LL (Peor)");
                }
                break;
            case SOCIOS:
                strcpy(politica, "Socios   ");
                break;
            case PART_FIJAS:
                strcpy(politica, "Part. Fijas");
                break;
        }

        printf(" %d\t%d KB\t%d\t%d\t%s\t%s\n", actual->info.id, actual->info.size, actual->info.numExe, actual->info.numES, politica, estado);
    }

    // Lista de Todos los Procesos
    actual = todos->head;
    for (actual; actual != NULL; actual = actual->sig) {
        char estado[20] = "";
        char politica[20] = "";

        switch (actual->info.estado) {
            case 1:
                strcpy(estado, "Ejecucion");
                break;
            case 2:
                strcpy(estado, "Espera E/S");
                break;
            case 3:
                strcpy(estado, "Listo");
                break;
            case 4:
                strcpy(estado, "Terminado");
                break;
            case 5:
                strcpy(estado, "Solicitud");
        }
        switch (actual->info.politica) {
            case MAPA_BITS:
                strcpy(politica, "Mapa de Bits");
                break;
            case LISTAS_LIGADAS:
                if (actual->info.ajuste == 7) {
                    strcpy(politica, "LL (Primer)");
                }
                else if (actual->info.ajuste == 8) {
                    strcpy(politica, "LL (Segundo)");
                }
                else if (actual->info.ajuste == 9) {
                    strcpy(politica, "LL (Mejor)");
                }
                else if (actual->info.ajuste == 10) {
                    strcpy(politica, "LL (Peor)");
                }
                break;
            case SOCIOS:
                strcpy(politica, "Socios   ");
                break;
            case PART_FIJAS:
                strcpy(politica, "Part. Fijas");
                break;
        }

        printf(" %d\t%d KB\t%d\t%d\t%s\t%s\n", actual->info.id, actual->info.size, actual->info.numExe, actual->info.numES, politica, estado);
    }

    printf("══════════════════════════════════════════════════════════════\n");
    printf("Solicitudes a Ejecución: %d\n\n", contarNodos());
	return;
}

#endif