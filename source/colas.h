#ifndef COLAS_H
#define COLAS_H

/**********************************************************************************************
		ARCHIVO DE CABECERA DONDE SE ENCUENTRAN TODAS LAS FUNCIONES RELACIONADAS
		AL MANEJO DE LAS COLAS, A LA EJECUCIÓN DE LOS HILOS Y A ESTADÍSTICAS
**********************************************************************************************/

#include "memoria.h"        /* Archivo de Cabecera donde está lo Relacionado a la Memoria */
#define CLOCK_MONOTONIC 1   /* Variable utilizada para contar el Tiempo o Duración        */

/*********************************** FUNCIONES A UTILIZAR ************************************/

void entrarEjecucion(Process*);     /* Introduce el Proceso a la Lista de Ejecución   */
void *ejecutarProceso(void*);       /* Introduce al Proceso al Contexto de Ejecucion  */
void crearColaSolicitudes();        /* Inicializa la Lista de Solicitudes de Procesos */
void allocMemory(Process*);         /* Asigna Memoria según la Política Elegida       */
void pasarListo(Process*);          /* Introduce al Proceso a la Cola de Listos       */
void freeMemory(Process);           /* Libera Memoria según la Política Elegida       */
void esperarES(Process*);           /* Introduce el Proceso a la Lista de Espera E/S  */
int memoriaSolicitada();            /* Devuelve una Cantidad de Memoria Aleatoria     */
void printStadistics();             /* Imprime en Pantalla las Estadísticas           */
void iniciarEjecucion();            /* Inicializa el Contexto de Ejecución            */
void printMemory();                 /* Imprime en Pantalla el Estado de la Memoria    */
void initColas();                   /* Inicializa todas las Listas necesarias         */

/************************************ VARIABLES GLOBALES *************************************/

pthread_mutex_t mutex;  /* Para sincronización de Hilos    */
int crecimiento[20];    /* Para el Crecimiento Aleatorio   */
int finalizados = 0;    /* Cantidad de Procesos Terminados */

/*********************************** FUNCIONES PRINCIPALES ***********************************/

void initColas() {
    /* ***************************************************
        Inicializa las Colas de Solicitudes, Ejecución,
        Espera por E/S, Listos y la Lista General de
        Procesos, además inicializa el Vector de
        Crecimiento Aleatorio
    ******************************************************/
    solicitudes = (Cola*)malloc(sizeof(Cola));  // Asignamos Memoria
    ejecucion = (Cola*)malloc(sizeof(Cola));    // Asignamos Memoria
    espera = (Cola*)malloc(sizeof(Cola));       // Asignamos Memoria
    listos = (Cola*)malloc(sizeof(Cola));       // Asignamos Memoria
    todos = (Cola*)malloc(sizeof(Cola));        // Asignamos Memoria

    solicitudes->head = NULL;   // Inicializamos en NULL
    ejecucion->head = NULL;     // Inicializamos en NULL
    espera->head = NULL;        // Inicializamos en NULL
    listos->head = NULL;        // Inicializamos en NULL
    todos->head = NULL;         // Inicializamos en NULL

    // Inicializamos el Vector de Crecimiento
    for (int i = 0; i < 20; i++) {
        if (i < 15) { crecimiento[i] = 0; }
        else {
            crecimiento[i] = rand() % 50 + 1;
        }
    }
}
void allocMemory(Process *p) {
    /***************************************************
        Esta funcion se encarga de Asignarle Memoria
        a un Proceso según la Política de Memoria
        elegida por el Programa.

        Verificamos si es la Primera Vez que se Ejecuta
        el Programa, de ser así se Inicializan las
        Variables de Memoria, en caso contrario se
        adapta la Memoria según la Política Anterior
    ****************************************************/
    switch (politica) {
        case MAPA_BITS:
            if (firstTime) { initMemoryBits(); firstTime = false; }
            else { adaptBits(); }
            allocMemoryBits(p);
            break;
        case LISTAS_LIGADAS:
            if (firstTime) { initListas(); firstTime = false; }
            else { adaptListas(); }
            allocListas(ajuste, p);
            ajuste++;
            if (ajuste > 10) { ajuste = 7; }
            break;
        case SOCIOS:
            if (firstTime) { initSocios(); firstTime = false; }
            else { adaptSocios(); }
            allocMemorySocios(p);
            break;
        case PART_FIJAS:
            if (firstTime) { initMemoryFijas(); firstTime = false; }
            allocMemoryFijas(p);
            break;
    }
}
void freeMemory(Process p) {
    /***************************************************
        Esta funcion se encarga de Liberarle Memoria
        a un Proceso según la Política de Memoria
        elegida por el Programa.
    ****************************************************/
    switch (p.politica) {
        case MAPA_BITS:
            freeMemoryBits(p.id);
            break;
        case LISTAS_LIGADAS:
            freeListas(p.id);
            break;
        case SOCIOS:
            freeMemorySocios(p.id);
            break;
        case PART_FIJAS:
            freeMemoryFijas(p.id);
            break;
    }
}
void printMemory() {
    /***************************************************
        Esta funcion se encarga de Imprimir en Pantalla
        el Estado de Memoria según la Política de
        Administración Elegida
    ****************************************************/
    switch (politica) {
        case LISTAS_LIGADAS:
            printMemoryListas();
            break;
        case MAPA_BITS:
            printMemoryBits();
            break;
        case PART_FIJAS:
            printMemoryFijas();
            break;
        case SOCIOS:
            printMemorySocios();
            break;
    }
}
void iniciarEjecucion() {
    // Inicializamos las Colas
    initColas();
    crearColaSolicitudes();

    // Creamos un Vector de Hilos
    pthread_t procesos[NUM_PROCESS];

    // Creamos y Asignamos los Procesos
    for (int i = 0; i < NUM_PROCESS; i++) {
        system("clear");    // Limpiamos la Pantalla
        printProcess();     // Imprimimos la Lista de Procesos
        printMemory();      // Imprimimos el Estado de la Memoria
        Node *actual = solicitudes->head;
        if (actual != NULL) {
            // Tomamos un Proceso de la Lista de Solicitudes
            Process proceso = actual->info;
            // Le asignamos la Política
            if (politica == LISTAS_LIGADAS) {
                proceso.politica = politica;
                proceso.ajuste = ajuste;
            }
            else { proceso.politica = politica; }
            // Eliminamos el Proceso de la Lista
            delProcess(solicitudes, proceso.id);
            // Iniciamos y Esperamos a que terminen los Hilos
            pthread_create(&procesos[i], NULL, ejecutarProceso, &proceso);
            pthread_join(procesos[i], NULL);
            // Cambiamos de Política
            politica ++;
            if (politica > 4) { 
                politica = 1;
            }
        }
    }
    return;
}

/************************************** MANEJO DE HILOS **************************************/

void crearColaSolicitudes() {
    /********************************************
        Esta función se encarga de crear los
        Procesos que van para la Lista de
        Solicitudes
    *********************************************/
    srand(time(NULL));
    for (int i = 0; i < NUM_PROCESS; i++) {
        Process temp;                      // Proceso Temporal
        temp.id = i + 1;                   // Asignamos una ID
        temp.size = memoriaSolicitada();   // Solicitamos Memoria
        temp.numExe = rand() % 3 + 1;      // Asignamos entre 1 a 3 veces
        temp.execTime = rand() % 5 + 1;    // Asignamos entre 1 a 5 segundos
        temp.ESTime = rand() % 3 + 1;      // Asignamos entre 1 a 3 segundos
        temp.numES = rand() % 3 + 1;       // Asignamos entre 1 a 3 veces
        temp.estado = 5;                   // Indicamos que está en Solicitudes
        insertProcess(solicitudes, temp);  // Insertamos el Proceso a la Lista
    }
    return;
}
void entrarEjecucion(Process *p) {
    /*******************************************
        Esta función introduce un Proceso a 
        la Cola de Ejecución
    *******************************************/
    // Empezamosa medir el Tiempo que dura
    struct timespec inicio, fin;
    clock_gettime(CLOCK_MONOTONIC, &inicio);

    p->estado = 1;                  // Indicamos que está en Ejecución
    insertProcess(ejecucion, *p);   // Lo Insertamos a la Lista
    system("clear");                // Limpiamos la Pantalla
    printProcess();                 // Imprimimos la Lista de Procesos
    printMemory();                  // Imprimimos el Estado de la Memoria
    sleep(p->execTime);             // Simulamos una acción del Proceso
    p->numExe--;                    // Decrementamos la cantidad de Iteraciones Restantes
    delProcess(ejecucion, p->id);   // Eliminamos el Proceso de la Lista de Ejecución

    // Dejamos de medir el Tiempo
    clock_gettime(CLOCK_MONOTONIC, &fin);
    double tiempo_transcurrido = (fin.tv_sec - inicio.tv_sec) + (fin.tv_nsec - inicio.tv_nsec) / 1e9;
    totTiempoEjec += tiempo_transcurrido;   // Guardamos el Tiempo que tardó
    return;
}
void esperarES(Process *p) {
    /*******************************************
        Esta función introduce un Proceso a 
        la Cola de Espera por E/S
    *******************************************/
    // Empezamosa medir el Tiempo que dura
    struct timespec inicio, fin;
    clock_gettime(CLOCK_MONOTONIC, &inicio);
    
    p->estado = 2;                  // Indicamos que está en Espera
    insertProcess(espera, *p);      // Lo Insertamos a la Lista
    system("clear");                // Limpiamos la Pantalla
    printProcess();                 // Imprimimos la Lista de Procesos
    printMemory();                  // Imprimimos el Estado de la Memoria
    sleep(p->ESTime);               // Simulamos el Tiempo en Espera
    p->numES --;                    // Decrementamos la cantidad de Iteraciones Restantes
    delProcess(espera, p->id);      // Eliminamos el Proceso de la Lista de Espera

    // Dejamos de medir el Tiempo
    clock_gettime(CLOCK_MONOTONIC, &fin);
    double tiempo_transcurrido = (fin.tv_sec - inicio.tv_sec) + (fin.tv_nsec - inicio.tv_nsec) / 1e9;
    totTiempoES += tiempo_transcurrido;    // Guardamos el Tiempo que tardó

    return;
}
void pasarListo(Process *p) {
    /*******************************************
        Esta función introduce un Proceso a 
        la Cola de Listos
    *******************************************/
    system("clear");            // Limpiamos la Pantalla
    printProcess();             // Imprimimos la Lista de Procesos
    printMemory();              // Imprimimos el Estado de la Memoria
    p->estado = 3;              // Indicamos que está en Listos
    insertProcess(listos, *p);  // Lo Insertamos a la Lista
    system("clear");            // Limpiamos la Pantalla
    printProcess();             // Imprimimos la Lista de Procesos
    printMemory();              // Imprimimos el Estado de la Memoria
    sleep(2);                   // Simulamos el Tiempo en la Lista
    delProcess(listos, p->id);  // Eliminamos el Proceso de la Cola de Listos
}
void *ejecutarProceso(void *arg) {
    // Bloqueamos el Mutex
    pthread_mutex_lock(&mutex);

    // Obtenemos el Proceso
    Process proceso = *(Process*) arg;
    hilosEjecutandose++;

    // Le asignamos Memoria
    allocMemory(&proceso);

    // Emulamos el Contexto de Ejecución
    while (proceso.numExe > 0) {
        system("clear");    // Limpiamos la Pantalla
        printProcess();     // Imprimimos la Lista de Procesos
        printMemory();      // Imprimimos el Estado de la Memoria

        // Entramos a Ejecución
        proceso.execTime = rand() % 5 + 1;  // Asignamos el Tiempo en Ejecución
        int i = rand() % 20 + 1;            // Obtenemos un Valor Aleatorio
        proceso.size += crecimiento[i];     // Aumentamos el Tamaño del Proceso
        entrarEjecucion(&proceso);          // Enviamos a Ejecución

        // Verificamos si tiene wue Entrar a Espera
        if (proceso.numES > 0) {
            proceso.ESTime = rand() % 3 + 1; // Asignamos el Tiempo en Espera
            esperarES(&proceso);             // Enviamos a Espera
        }

        // Enviamos a la Cola de Listos
        pasarListo(&proceso);
    }
    printf("\n");

    proceso.estado = 4;             // Indicamos que ya Terminó
    insertProcess(todos, proceso);  // Insertamos en la Lista General
    system("clear");                // Limpiamos la Pantalla
    printProcess();                 // Imprimimos la Lista de Procesos
    printMemory();                  // Imprimimos el Estado de la Memoria
    freeMemory(proceso);            // Liberamos la Memoria
    finalizados++;                  // Incrementamos la Cantidad de Procesos Finalizados
    pthread_mutex_unlock(&mutex);   // Desbloqueamos el Mutex

    // El proceso ha finalizado
    pthread_exit(NULL);
}

/************************************* FUNCIONES EXTRA ***************************************/

void printStadistics() {
    /******************************************************
        Esta funcion se encarga de Imprimir en Pantalla
        el Resultado de las Estadísticas de las 4
        Políticas de Administración de Memoria
    *******************************************************/
    double tiempoEjecucion = totTiempoEjec / NUM_PROCESS;
    double promedioTiempo = totTiempoEjec / NUM_PROCESS;
    double promFinalizados = finalizados / NUM_PROCESS;
    double tiempoEspera = totTiempoES / NUM_PROCESS;
    double desperdicioInterno = despInterno / 4;
    double desperdicioExterno = despExterno / 4;
    system("clear");
    printf("\n               ESTADISTICAS DE USO\n");
	printf("══════════════════════════════════════════════════\n");
	printf(" Promedio Desperdicio Interno:\t\t%.2f KB\n", desperdicioInterno);
	printf(" Promedio Desperdicio Interno:\t\t%.2f KB\n", desperdicioExterno);
	printf(" Cantidad Procesos en Ejecución:\t%d\n", hilosEjecutandose);
	printf(" Promedio Procesos Finalizados:\t\t%.2f\n", promFinalizados);
	printf(" Prom. Tiempo Ejecución Proceso:\t%.2f s\n", tiempoEjecucion);
	printf(" Prom. Tiempo Espera Proceso:\t\t%.2f s\n", tiempoEspera);
	printf("══════════════════════════════════════════════════\n\n");
    return;
}
int memoriaSolicitada() {
    /**********************************************
        Esta funcion se encarga de devolver una 
        tamaño aleatorio al Proceso para que
        este se lo solicite al Administrador
        de Memoria 
    ***********************************************/
    int canMemory = 0;  // Cantidad de Memoria a Devolver
    canMemory = rand() % (MEMORY_SIZE / 2) + 1;
    return canMemory;
}

#endif