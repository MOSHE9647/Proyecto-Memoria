#ifndef LISTA_LIGADA_H
#define LISTA_LIGADA_H

/**********************************************************************************************
		ARCHIVO DE CABECERA DONDE SE ENCUENTRAN TODAS LAS FUNCIONES RELACIONADAS
		A LA POLITICA DE ADMINISTRACION DE MEMORIA "LISTAS LIGADAS"
**********************************************************************************************/

#include "memoria.h"  /* Archivo de Cabecera donde está lo Relacionado a la Memoria */

/*********************************** FUNCIONES A UTILIZAR ************************************/

bool allocListas(int, Process*);    /* Le Asigna Memoria a un Proceso específico         */
Nodo *newNodeInfo (Nodo*, char);    /* Busca y devuelve el Puntero al valor de un Nodo   */
bool segundoAjuste(Process*);       /* Algoritmo de Asignación del Segundo Ajuste        */
bool primerAjuste(Process*);        /* Algoritmo de Asignación del Primer Ajuste         */
bool mejorAjuste(Process*);         /* Algoritmo de Asignación del Mejor Ajuste          */
bool peorAjuste(Process*);          /* Algoritmo de Asignación del Peor Ajuste           */
int printMemoryListas();            /* Imprime el Estado de la Memoria en Pantalla       */
void freeListas(int);               /* Libera la Memoria Utilizada por un Proceso        */
void adaptListas();                 /* Adapta la Memoria de Mapa de Bits para uso Listas */
void initListas();                  /* Inicializa la Partición Inicial de Memoria        */

/*********************************** FUNCIONES PRINCIPALES ***********************************/
// Inicializa la Memoria para Listas Ligadas
void initListas() {
    /************************************************
        Esta función se encarga de Inicializar la
        Memoria para su uso con Listas Ligadas
    ************************************************/
    memory = (Lista*)malloc(sizeof(Lista)); /* Se le asigna su espacio en Memoria */
	memory->head = NULL;                    /* Se inicializa la Lista en NULL     */

	Partition temp;             /* Particion Temporal para Iniciar la Lista   */
    temp.size = MEMORY_SIZE;    /* Se le asigna el Tamaño Total de la Memoria */
    temp.process = NULL;        /* Se iniciaiza sin ningún Proceso asignado   */
    temp.isFree = true;         /* Se indica que la Partición está Libre      */
    temp.id = 0;                /* Se le asina un ID genérico                 */
    
    insertNode(&memory->head, temp);   /* Se inserta la Partición en la Lista */
}
// Adapta la Memoria para su uso en Listas Ligadas
void adaptListas() {
    /****************************************************
        Esta función se encarga de adaptar la Memoria
        manejada por Mapa de Bits para su uso con
        Listas Ligadas.
    *****************************************************/
    Lista *temp = (Lista*)malloc(sizeof(Lista)); /* Memoria Temporal  */
    temp->head = NULL;           /* Inicia en NULL    */
    temp->lastUsedNode = NULL;   /* Inicia en NULL    */

    Nodo *actual = memory->head; /* Se toma el Inicio */
    Nodo *auxNode = NULL;        /* Nodo auxiliar     */

    while (actual != NULL) {
        /* Se buscan los Huecos y los Procesos */
        if (actual->info.isFree) { auxNode = newNodeInfo(actual, 'H'); }
        else { auxNode = newNodeInfo(actual, 'P'); }

        /* Una vez Encontrados, se le asignan a Memoria */
        if (auxNode != NULL) {
            insertNode(&temp->head, auxNode->info);
            actual = auxNode->sig;
        } else { actual = actual->sig; }
    }
    /* Se hace la Adaptación */
    memory = temp;
    return;
}
// Algoritmo del Primer Ajuste
bool primerAjuste(Process *p) {
    /**************************************************
        Algoritmo del Primer Ajuste:
        Se encarga de asignarle al Proceso el primer
        Bloque de Memoria que encuentre y esté Libre
    ***************************************************/

    Nodo *actual = memory->head;

    while (actual != NULL) {
        if (actual->info.isFree) {
            if (actual->info.size >= p->size) {
                actual->info.process = p;
                actual->info.isFree = false;
                actual->info.id = 0;
                /****************************************************
                    En caso de que el tamaño del Bloque asignado
                    sea mayor al tamaño del Proceso, dividimos el
                    Bloque y el sobrante lo insertamos a la Lista
                    como un Bloque Nuevo a la par del Actual
                *****************************************************/
                if (actual->info.size > p->size) {
                    Partition temp;                             // Particion Temporal
                    temp.id = 0;                                // Asignamos ID
                    temp.process = NULL;                        // Indicamos que no posee un Proceso
                    temp.isFree = true;                         // Indicamos que está Libre
                    temp.size = actual->info.size - p->size;    // Asignamos el Tamaño Sobrante
                    insertMiddle(actual, temp);                 // Insertamos a la par del Actual
                    actual->info.size = p->size;                // Asignamos el Tamaño del Proceso al Bloque Actual

                    /* Calculamos el Desperdicio Interno y Externo */
                    int restante = actual->info.size - p->size;
                    despInterno += restante;
                    despExterno += MEMORY_SIZE - despInterno;
                }
                return true;
            }
        }
        actual = actual->sig;
    }

    return false;
}
// Algoritmo del Segundo Ajuste
bool segundoAjuste(Process *p) {
    /**************************************************
        Algoritmo del Segundo Ajuste:
        Se encarga de asignarle al Proceso el segundo
        Bloque de Memoria que encuentre y esté Libre
    ***************************************************/
    Nodo *actual = NULL;
    /* Si existe, iniciamos del último Bloque Utilizado */
    if (memory->lastUsedNode != NULL) { actual = memory->lastUsedNode; }
    /* Sino, iniciamos desde el Inicio */
    else { actual = memory->head; }

    while (actual != NULL) {
        if (actual->info.isFree) {
            if (actual->info.size > p->size) {
                actual->info.process = p;       // Asignamos el Proceso
                actual->info.isFree = false;    // Indicamos que está Ocupada
                actual->info.id = 0;            // Asignamos un ID
                /******************************************************
                    Si el Proceso es más grande que el Bloque
                    realizamos lo mismo que en el Algoritmo Anterior.
                *******************************************************/
                if (actual->info.size > p->size) {
                    Partition temp;         
                    temp.id = 0;
                    temp.process = NULL;
                    temp.isFree = true;
                    temp.size = actual->info.size - p->size;
                    insertMiddle(actual, temp);
                    actual->info.size = p->size;
                }
                // Marcamos el Bloque como el Último Usado
                memory->lastUsedNode = actual;

                system("clear");        // Limpiamos la Pantalla de la Consola
				printProcess();         // Imprimimos la Lista de Procesos
				printMemoryListas();    // Imprimimos el Estado de la Memoria
				printf("Memoria Asignada al Proceso %d (%d KB)\n", p->id, p->size);

                // Calculamos el Desperdicio Interno y Externo
                int restante = actual->info.size - p->size;
                despInterno += restante;
                despExterno += MEMORY_SIZE - despInterno;

                return true;
            }
        }
        actual = actual->sig;
    }

    return false;
}
// Algoritmo del Mejor Ajuste
bool mejorAjuste(Process *p) {
    /**************************************************
        Algoritmo del Mejor Ajuste:
        Se encarga de asignarle al Proceso el Bloque
        de Memoria que mejor se ajuste al Tamaño del
        Proceso.
    ***************************************************/
    Nodo *actual = memory->head;
    Nodo *mejor = NULL;

    // Buscamos el Bloque que mejor se Ajuste
    while (actual != NULL) {
        if (actual->info.isFree) {
            if (actual->info.size > p->size) {
                if (mejor == NULL) { mejor = actual; }
                else {
                    if (actual->info.size < mejor->info.size) {
                        mejor = actual;
                    }
                }
            }
        }
        actual = actual->sig;
    }
    /*******************************************************
        En caso de haber encontrado un Bloque de Memoria,
        le asignamos el Bloque de Memoria al Proceso
    ********************************************************/
    if (mejor != NULL) {
        mejor->info.id = 0;             // Asignamos un ID
        mejor->info.isFree = false;     // Indicamos que está Ocupado
        mejor->info.process = p;        // Le Asignamos el Proceso
        if (mejor->info.size > p->size) {
            Partition temp;             // Nueva Partición
            temp.id = 0;                // Asignamos un ID
            temp.process = NULL;        // Indicamos que no posee un Proceso
            temp.isFree = true;         // Indicamos que está Libre
            temp.size = mejor->info.size - p->size; // Asignamos el Tamaño Sobrante
            insertMiddle(mejor, temp);  // La insertamos después del Actual
            mejor->info.size = p->size; // Le asignamos al Actual el Tamaño del Proceso
        }

        system("clear");        // Limpiamos la Pantalla de la Consola
        printProcess();         // Imprimimos la Lista de Procesos
        printMemoryListas();    // Imprimimos el Estado de la Memoria
        printf("Memoria Asignada al Proceso %d (%d KB)\n", p->id, p->size);

        // Calculamos el Desperdicio Interno y Externo
        int restante = actual->info.size - p->size;
        despInterno += restante;
        despExterno += MEMORY_SIZE - despInterno;

        return true;
    } else { return false; }
}
// Algoritmo del Peor Ajuste
bool peorAjuste(Process *p) {
    /**************************************************
        Algoritmo del Peor Ajuste:
        Se encarga de asignarle al Proceso el Bloque
        de Memoria que peor se ajuste al Tamaño del
        Proceso.
    ***************************************************/
    Nodo *actual = memory->head;
    Nodo *peor = NULL;

    // Buscamos el Bloque que Peor se Ajuste
    while (actual != NULL) {
        if (actual->info.isFree) {
            if (actual->info.size > p->size) {
                if (peor == NULL) { peor = actual; }
                else {
                    if (actual->info.size > peor->info.size) {
                        peor = actual;
                    }
                }
            }
        }
        actual = actual->sig;
    }
    /*******************************************************
        En caso de haber encontrado un Bloque de Memoria,
        le asignamos el Bloque de Memoria al Proceso
    ********************************************************/
    if (peor!= NULL) {
        peor->info.id = 0;             // Asignamos un ID
        peor->info.isFree = false;     // Indicamos que está Ocupado
        peor->info.process = p;        // Le Asignamos el Proceso
        if (peor->info.size > p->size) {
            Partition temp;             // Nueva Partición
            temp.id = 0;                // Asignamos un ID
            temp.process = NULL;        // Indicamos que no posee un Proceso
            temp.isFree = true;         // Indicamos que está Libre
            temp.size = peor->info.size - p->size; // Asignamos el Tamaño Sobrante
            insertMiddle(peor, temp);   // La insertamos después del Actual
            peor->info.size = p->size;  // Le asignamos al Actual el Tamaño del Proceso
        }
        system("clear");        // Limpiamos la Pantalla de la Consola
        printProcess();         // Imprimimos la Lista de Procesos
        printMemoryListas();    // Imprimimos el Estado de la Memoria
        printf("Memoria Asignada al Proceso %d (%d KB)\n", p->id, p->size);

        // Calculamos el Desperdicio Interno y Externo
        int restante = actual->info.size - p->size;
        despInterno += restante;
        despExterno += MEMORY_SIZE - despInterno;

        return true;
    } else { return false; }
}
// Asigna Memoria a un Proceso
bool allocListas(int ajuste, Process *p) {
    /***************************************************
        Esta función se encarga de Asignar la Memoria
        según el Ajuste que se haya escogido
    ****************************************************/
    if (ajuste == 1) { primerAjuste(p);  return true; }
    if (ajuste == 2) { segundoAjuste(p); return true; }
    if (ajuste == 3) { mejorAjuste(p);   return true; }
    if (ajuste == 4) { peorAjuste(p);    return true; }
    return false;
}
// Libera la Memoria Utilizada por un Proceso
void freeListas(int processID) {
    /****************************************************************************
		Esta funcion se encarga de Liberar la Memoria que posee asignado
		un Proceso.

		Esta función primero verifica que exista el ID pasado por parámetro,
		una vez encontrado el Proceso procede a dividir el tamaño del mismo
		entre PART_SIZE para saber cuántos Bloques hay que liberar
	******************************************************************************/
    Nodo *actual = memory->head;
    if (actual != NULL) {
        while (actual != NULL) {
            if (actual->info.process != NULL) {
                if (actual->info.process->id == processID) {
                    /**********************************************
						Para saber cuántos espacios de Memoria
						vamos a necesitar para Liberar el Proceso,
						dividimos el tamaño (redondeado) del
						Proceso entre 4.
					**********************************************/
                    int canParts = 0;
                    if (actual->info.process->size <= 2) { canParts = 1; }
                    else { canParts = actual->info.process->size / PART_SIZE; }
                    for (int j = 0; j < canParts; j++) {
                        if (actual != NULL) {
                            actual->info.process = NULL;	// Liberamos el Proceso
                            actual->info.isFree = true;		// Indicamos que está Libre
                            actual->info.id = 0;			// Asignamos el ID en 0
                            actual = actual->sig;			// Continuamos con el Siguiente Bloque
                        }
                    }
                    system("clear");	    // Limpiamos Pantalla
                    printProcess();		    // Imprimimos la Lista de Procesos
                    printMemoryListas();    // Imprimimos el Estado de la Memoria
                    printf("Memoria Liberada (Proceso %d)...\n", processID);
                    return;
                }
            }
            actual = actual->sig;
        }
        printProcess();         // Imprimimos la Lista de Procesos
        printMemoryListas();    // Imprimimos el Estado de la Memoria
        printf("No se pudo Liberar la Memoria del Proceso %d\n", processID);
        return;
    }
}

/************************************* FUNCIONES EXTRA ***************************************/
// Busca y devuelve el Puntero a un Nodo
Nodo *newNodeInfo (Nodo* actual, char flag) {
    /*********************************************************
        Esta función se encarga de buscar cuantos Nodos
        contienen una misma información (sea un Hueco o
        un Proceso), devolviendo un puntero al último 
        Nodo para así poder crear una nueva lista.

        Si la partición está libre, este se va a encargar
        de determinar el tamaño resultante de sumar el
        tamaño de todos los huecos que sean consecutivos
        a este.

        Si la partición contiene un proceso, se va a tomar
        como valor el tamaño del proceso y, de igual forma,
        se va a devolver un puntero que indica cuál fue la
        última partición que contuvo ese Proceso.
    **********************************************************/
    Nodo *aux = NULL;   /* Nodo que contendra el Puntero   */
    int size = 0;       /* Variable para sumar los tamaños */

    if (flag == 'H') {
        /***************************************************
            Si es un hueco, se buscan cuantos huecos hay
            después del primero hasta encontrar, ya sea
            un proceso o el final de la lista.
        ****************************************************/
        while (actual != NULL) {
            if (actual->info.isFree) {
                size += PART_SIZE;
                aux = actual;
            }
            actual = actual->sig;
        }
    }

    if (flag == 'P') {
        /************************************************
            Si es un Proceso, se toma como referencia
            el ID del Proceso actual, para así contar
            únicamente las particiones que contengan
            el mismo Proceso.
        *************************************************/
        int processID = actual->info.process->id;
        while (actual != NULL) {
            if (actual->info.process != NULL) {
                if (actual->info.process->id == processID) {
                    size = actual->info.process->size;
                    aux = actual;
                } else { break; }
            }
            actual = actual->sig;
        }
    }

    /*************************************************************
        Por último se verifica que se haya Encontrado el Nodo
        requerido y, de ser así, se le asigna el tamaño sumado
        al Nodo para devolverlo e insertarlo en la lista.
    **************************************************************/
    if (aux != NULL) { aux->info.size = size; }
    return aux;
}
// Muestra el Estado actual de la Memoria en Pantalla
int printMemoryListas() {
	/****************************************************
		Mostramos el estado de cada uno de los bloques
		de Memoria en forma de Lista:
	*****************************************************/
	Nodo *actual = memory->head;

	printf("\n           ESTADO DE LA MEMORIA: Listas Ligadas\n");
	printf("═══════════════════════════════════════════════════════════════════════\n");
	printf(" #\tPARTICION\tTAMAÑO\t        ESTADO\t   PROCESO\tAJUSTE\n");
	printf("-----------------------------------------------------------------------\n");
	for (int i = 0; actual != NULL; i++) {
		/*************************************************
			Si la Partición posee un Proceso, mostramos
			por pantalla el ID y el Tamaño del mismo
		*************************************************/
		if (actual->info.process != NULL) {
            char politica[20] = "";
            if (actual->info.process->ajuste == 7) {
                    strcpy(politica, "Primer");
                }
                else if (actual->info.process->ajuste == 8) {
                    strcpy(politica, "Segundo");
                }
                else if (actual->info.process->ajuste == 9) {
                    strcpy(politica, "Mejor");
                }
                else if (actual->info.process->ajuste == 10) {
                    strcpy(politica, "Peor");
                }

			printf(" %d\tPartición %d\t%d KB\t\t%s\t   %d (%d KB)\t%s\n", i + 1, actual->info.id, actual->info.size, actual->info.isFree ? "Libre" : "Ocupada", actual->info.process->id, actual->info.process->size, politica);
		}
		else {
			/*******************************************
				En caso contrario, solo mostramos las
				siglas N/A por pantalla
			********************************************/
			printf(" %d\tPartición %d\t%d KB\t\t%s\t   %s\n", i + 1, actual->info.id, actual->info.size, actual->info.isFree ? "Libre" : "Ocupada", "N/A");
		}
		actual = actual->sig;
	}
	printf("═══════════════════════════════════════════════════════════════════════\n\n");
}

#endif