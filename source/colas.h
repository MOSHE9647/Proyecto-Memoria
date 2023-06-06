#ifndef COLAS_H
#define COLAS_H

#include "partFijas.h"
#include <sys/ioctl.h>

#define CLOCK_MONOTONIC 1

pthread_mutex_t mutex;		    /* Para uso de Hilos        */
int crecimiento[20];
int finalizados = 0;

bool isFull = false;

void initColas() {
    solicitudes = (Cola*)malloc(sizeof(Cola));
    ejecucion = (Cola*)malloc(sizeof(Cola));
    espera = (Cola*)malloc(sizeof(Cola));
    listos = (Cola*)malloc(sizeof(Cola));
    todos = (Cola*)malloc(sizeof(Cola));

    solicitudes->head = NULL;
    ejecucion->head = NULL;
    espera->head = NULL;
    listos->head = NULL;
    todos->head = NULL;

    for (int i = 0; i < 20; i++) {
        if (i < 15) { crecimiento[i] = 0; }
        else {
            crecimiento[i] = rand() % 50 + 1;
        }
    }
}
void allocMemory(Process *p) {
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
            // freeMemoryFijas(p.id);
            break;
    }
}
void printMemory() {
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

int memoriaSolicitada() {
    int canMemory = 0;
    canMemory = rand() % (MEMORY_SIZE / 2) + 1;
    return canMemory;
}
void crearColaSolicitudes() {
    srand(time(NULL));
    for (int i = 0; i < NUM_PROCESS; i++) {
        Process temp;
        temp.id = i + 1;
        temp.size = memoriaSolicitada();
        temp.numExe = rand() % 3 + 1;      /* De 1 a 10 veces    */
        temp.execTime = rand() % 5 + 1;    /* De 1 a 10 segundos */
        temp.ESTime = rand() % 3 + 1;      /* De 1 a 10 segundos */
        temp.numES = rand() % 3 + 1;
        temp.estado = 5;
        insertProcess(solicitudes, temp);
    }
    return;
}

void entrarEjecucion(Process *p) {
    // Paso por la lista de Ejecución
    struct timespec inicio, fin;
    clock_gettime(CLOCK_MONOTONIC, &inicio);

    p->estado = 1;
    insertProcess(ejecucion, *p);
    system("clear");
    printProcess();
    printMemory();
    sleep(p->execTime);
    p->numExe--;
    delProcess(ejecucion, p->id);

    clock_gettime(CLOCK_MONOTONIC, &fin);
    double tiempo_transcurrido = (fin.tv_sec - inicio.tv_sec) + (fin.tv_nsec - inicio.tv_nsec) / 1e9;
    totTiempoEjec += tiempo_transcurrido;

    return;
}
void esperarES(Process *p) {
    struct timespec inicio, fin;
    clock_gettime(CLOCK_MONOTONIC, &inicio);
    
    p->estado = 2;
    insertProcess(espera, *p);
    system("clear");
    printProcess();
    printMemory();
    sleep(p->ESTime);
    p->numES --;
    delProcess(espera, p->id);

    clock_gettime(CLOCK_MONOTONIC, &fin);
    double tiempo_transcurrido = (fin.tv_sec - inicio.tv_sec) + (fin.tv_nsec - inicio.tv_nsec) / 1e9;
    totTiempoES += tiempo_transcurrido;

    return;
}
void pasarListo(Process *p) {
    // Paso por la lista de Listos
    system("clear");
    printProcess();
    printMemory();
    p->estado = 3;
    insertProcess(listos, *p);
    system("clear");
    printProcess();
    printMemory();
    sleep(2);
    delProcess(listos, p->id);
}

void *ejecutarProceso(void *arg) {
    pthread_mutex_lock(&mutex);

    Process proceso = *(Process*) arg;
    hilosEjecutandose++;

    allocMemory(&proceso);

    while (proceso.numExe > 0) {
        system("clear");
        printProcess();
        printMemory();

        proceso.execTime = rand() % 5 + 1;
        int i = rand() % 20 + 1;
        proceso.size += crecimiento[i];
        entrarEjecucion(&proceso);

        if (proceso.numES > 0) {
            proceso.ESTime = rand() % 3 + 1;
            esperarES(&proceso);
        }

        pasarListo(&proceso);
    }

    printf("\n");

    proceso.estado = 4;

    insertProcess(todos, proceso);
    system("clear");
    printProcess();
    printMemory();

    freeMemory(proceso);
    finalizados++;
    
    pthread_mutex_unlock(&mutex);

    // El proceso ha finalizado
    pthread_exit(NULL);
}

void printStadistics() {
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

int iniciarEjecucion() {
    initColas();
    crearColaSolicitudes();

    pthread_t procesos[NUM_PROCESS];

    for (int i = 0; i < NUM_PROCESS; i++) {
        system("clear");
        printProcess();
        printMemory();
        Node *actual = solicitudes->head;
        if (actual != NULL) {
            Process proceso = actual->info;
            if (politica == LISTAS_LIGADAS) {
                proceso.politica = politica;
                proceso.ajuste = ajuste;
            }
            else { proceso.politica = politica; }
            delProcess(solicitudes, proceso.id);
            pthread_create(&procesos[i], NULL, ejecutarProceso, &proceso);
            pthread_join(procesos[i], NULL);
            politica ++;
            if (politica > 4) { 
                politica = 1;
            }
        }
    }

    return 0;
}

#endif