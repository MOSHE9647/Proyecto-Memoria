#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define FILAS 8
#define COLUMNAS 8
int x = 0;
int matriz[FILAS][COLUMNAS];

// Función para mostrar la matriz en pantalla
void mostrarMatriz() {
    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLUMNAS; j++) {
            printf("%d ", matriz[i][j]);
        }
        printf("\n");
    }
}

// Estructura para hilos individuales
typedef struct
{
    pthread_t thread_id;
    int activo;
    int cantidadMemoria;
    int posI;
} Hilo;

typedef struct segmento
{
    int id;
    char tipo;                  // H o P
    int inicio;                 // dirección de inicio
    int longitud;               // longitud en bytes
    struct segmento *siguiente; // apuntador al siguiente segmento
} segmento;

// Función para crear un nuevo segmento
segmento *crear_segmento(int id, char tipo, int inicio, int longitud)
{
    segmento *nuevo = malloc(sizeof(segmento)); // reservar espacio en memoria
    nuevo->id = id;
    nuevo->tipo = tipo;
    nuevo->inicio = inicio;
    nuevo->longitud = longitud;
    nuevo->siguiente = NULL;
    return nuevo;
}

void inicializarMatriz(){
    for(int i = 0; i<FILAS; i++){
        for(int j = 0; j<COLUMNAS; j++){
            matriz[i][j]= 0;
        }
    }
}

void actualizarMatriz(int accion, int inicio, int cantidad) {
    printf("\n se actualizo la matriz \ninicio:  %d, cantidad: %d\n",inicio,cantidad);

    if(x = 0){
        for(int i = 0; i<1; i++){
            for(int j = 0; j<cantidad; j++){
                matriz[i][j] = accion;
            }
        }
        x=1;
    }else{
        int fila = inicio / COLUMNAS;
        int columna = inicio % COLUMNAS;
        // Verificar que la posición inicial y la cantidad sean válidas
        if (fila >= FILAS || columna >= COLUMNAS ||
            fila * COLUMNAS + columna + cantidad > FILAS * COLUMNAS) {
            printf("Error: posición o cantidad inválidas\n");
            return;
        }

        // Llenar la matriz con el valor correspondiente
        for (int i = 0; i < cantidad; i++) {
            matriz[fila][columna] = accion;
            columna++;
            if (columna >= COLUMNAS) {
                fila++;
                columna = 0;
            }
        }
        mostrarMatriz();
    }

}

void asignar_memoria(segmento **lista, int pid, int tamano)
{
    segmento *actual = *lista;     // apuntador al primer segmento
    segmento *mejor_ajuste = NULL; // puntero al hueco con el mejor ajuste
    int diferencia = INT_MAX;      // diferencia entre el tamaño del hueco y el tamaño del proceso

    while (actual != NULL)
    { // mientras no se llegue al final de la lista
        if (actual->tipo == 'H' && actual->longitud >= tamano)
        { // si se encuentra un hueco suficiente
            if (actual->longitud == tamano)
            {                       // si el hueco tiene el mismo tamaño que el proceso
                actual->tipo = 'P'; // cambiar el tipo a proceso
                actual->id = pid;
            }
            else if (actual->longitud - tamano < diferencia)
            {                                           // si el hueco es más grande que el proceso y es el mejor ajuste hasta el momento
                mejor_ajuste = actual;                  // actualizar el puntero al hueco con el mejor ajuste
                diferencia = actual->longitud - tamano; // actualizar la diferencia entre el tamaño del hueco y el tamaño del proceso
            }
        }
        actual = actual->siguiente; // avanzar al siguiente segmento
    }

    if (mejor_ajuste != NULL)
    { // si se encontró un hueco con el mejor ajuste
        if (mejor_ajuste->longitud == tamano)
        {                             // si el hueco tiene el mismo tamaño que el proceso
            mejor_ajuste->tipo = 'P'; // cambiar el tipo a proceso
            mejor_ajuste->id = pid;
        }
        else
        {                                                                             // si el hueco es más grande que el proceso
            segmento *nuevo = crear_segmento(pid, 'P', mejor_ajuste->inicio, tamano); // crear un nuevo segmento para el proceso
            nuevo->siguiente = mejor_ajuste->siguiente;                               // enlazar el nuevo segmento con el siguiente del hueco
            mejor_ajuste->siguiente = nuevo;                                          // enlazar el hueco con el nuevo segmento
            mejor_ajuste->inicio += tamano;                                           // actualizar el inicio del hueco
            mejor_ajuste->longitud -= tamano;
            actualizarMatriz(1,nuevo->inicio, nuevo->longitud);// actualizar la longitud del hueco
        }
        printf("Se asigno %d bytes al proceso %d\n", tamano, pid); // imprimir mensaje de éxito

    }
    else
    {                                                                       // si no se encontró un hueco con el mejor ajuste
        printf("No se pudo asignar %d bytes al proceso %d\n", tamano, pid); // imprimir mensaje de error
    }
}


// Función para liberar la memoria de un proceso
void liberar_memoria(segmento **lista, int pid)
{
    segmento *actual = *lista; // apuntador al primer segmento
    segmento *anterior = NULL; // apuntador al segmento anterior
    while (actual != NULL)
    { // mientras no se llegue al final de la lista
        if (actual->tipo == 'P' && actual->id == pid)
        {                       // si se encuentra un proceso
            actual->tipo = 'H'; // cambiar el tipo a hueco
            actual->id = -1;
            printf("Se libero %d bytes del proceso %d\n", actual->longitud, pid); // imprimir mensaje de éxito
            if (anterior != NULL && anterior->tipo == 'H')
            {                                            // si el segmento anterior es un hueco
                anterior->longitud += actual->longitud;  // fusionar los huecos
                anterior->siguiente = actual->siguiente; // enlazar el segmento anterior con el siguiente del actual
                free(actual);                            // liberar el espacio del segmento actual
                actual = anterior;                       // actualizar el apuntador al segmento actual
            }
            if (actual->siguiente != NULL && actual->siguiente->tipo == 'H')
            {                                                    // si el segmento siguiente es un hueco
                actual->longitud += actual->siguiente->longitud; // fusionar los huecos
                segmento *temp = actual->siguiente;              // guardar el apuntador al segmento siguiente
                actual->siguiente = temp->siguiente;             // enlazar el segmento actual con el siguiente del siguiente
                free(temp);                                      // liberar el espacio del segmento siguiente
            }
            actualizarMatriz(0,actual->inicio,actual->longitud);
            return; // terminar la función
        }
        anterior = actual;          // actualizar el apuntador al segmento anterior
        actual = actual->siguiente; // avanzar al siguiente segmento
    }
    printf("No se encontró el proceso %d\n", pid); // imprimir mensaje de error

}

// Función para imprimir la lista de segmentos
void imprimir_lista(segmento *lista)
{
    segmento *actual = lista;           // apuntador al primer segmento
    printf("Lista de segmentos:\n");    // Imprimir un encabezado
    printf("Tipo\tInicio\tLongitud\n"); // Imprimir los nombres de las columnas
    while (actual != NULL)
    {                                                                                           // mientras no se llegue al final de la lista
        printf("%c(%d)\t%d\t%d\n", actual->tipo, actual->id, actual->inicio, actual->longitud); // imprimir los datos del segmento
        actual = actual->siguiente;                                                             // avanzar al siguiente segmento
    }
}

int buscar_nodo(segmento *lista, int id)
{
    segmento *actual = lista; // apuntador al primer segmento
    while (actual != NULL)
    { // mientras no se llegue al final de la lista
        if (actual->tipo == 'P' && actual->id == id)
        {             // si se encuentra un proceso con el id buscado
            return 1; // devolver 1
        }
        actual = actual->siguiente; // avanzar al siguiente segmento
    }
    return 0; // devolver 0 si no se encontró el nodo
}

int asignarMemoriaRandom()
{
    int resultado = 0;
    srand(time(NULL));
    resultado = 1 + rand() % 4;
    return resultado;
}

int calcular_memoria_usada(segmento *lista) {
    int memoria_usada = 0;
    while (lista != NULL) {
        if(lista->tipo == 'P'){
            memoria_usada += lista->longitud;
        }
        lista = lista->siguiente;
    }
    return memoria_usada;
}


pthread_mutex_t mutex;

int posI = 0;
int segmentos = 0;
segmento *lista;
int encontro = 0;
int memoriaActual = 0;

// Función para ejecutar los hilos individuales
void *funcion_hilo(void *arg)
{
    Hilo *hilo = (Hilo *)arg;

    // Ejecución continua hasta que se le indique lo contrario
    while (hilo->activo)
    {

        pthread_mutex_lock(&mutex);
        //printf("Hilo %d en ejecucion\n", hilo->thread_id);

        // encontro = buscar_nodo(lista,hilo->thread_id);
       // printf("\nENCONTRO: %d\n", buscar_nodo(lista, hilo->thread_id));
        if (buscar_nodo(lista, hilo->thread_id) == 0)
        {
           // printf("entra");
            asignar_memoria(&lista, hilo->thread_id, hilo->cantidadMemoria);
            imprimir_lista(lista);
        }

        pthread_mutex_unlock(&mutex);

        sleep(1);
    }

    printf("Hilo %d finalizado\n", hilo->thread_id);
    pthread_exit(NULL);
}

// Función para crear un nuevo hilo y agregarlo al arreglo de hilos
void agregar_hilo(Hilo **hilos, int *num_hilos)
{
    Hilo *nuevo_hilo = (Hilo *)malloc(sizeof(Hilo));
    nuevo_hilo->activo = 1;
    nuevo_hilo->cantidadMemoria = asignarMemoriaRandom();
    nuevo_hilo->posI = 0;
    pthread_create(&(nuevo_hilo->thread_id), NULL, funcion_hilo, (void *)nuevo_hilo);
    hilos[*num_hilos] = nuevo_hilo;
    (*num_hilos)++;
    printf("Se ha creado un nuevo hilo %d memoria: %d\n", nuevo_hilo->thread_id, nuevo_hilo->cantidadMemoria);
}

// Función para finalizar un número aleatorio de hilos
void finalizar_hilos(Hilo **hilos, int *num_hilos)
{
    
    int num_hilos_finalizar = rand() % (*num_hilos - 1) + 1; //numero random entre 1 a la cantidad de hilos -1
    int hilo_finalizado = 0;

    printf("Finalizando %d hilos...\n", num_hilos_finalizar);

    // Iteramos hasta que hayamos finalizado el número deseado de hilos
    while (hilo_finalizado < num_hilos_finalizar)
    {
        int indice_hilo = rand() % (*num_hilos);
        Hilo *hilo = hilos[indice_hilo];

        // Si el hilo no ha sido finalizado todavía, lo finalizamos
        if (hilo->activo)
        {
            hilo->activo = 0;

            liberar_memoria(&lista, hilo->thread_id);

            pthread_join(hilo->thread_id, NULL);
            hilo_finalizado++;
            printf("Se ha finalizado un hilo %d\n", hilo->thread_id);
            imprimir_lista(lista);
        }
    }

    // Eliminamos los hilos finalizados del arreglo de hilos
    int i, j;
    for (i = 0, j = 0; i < *num_hilos; i++)
    {
        Hilo *hilo = hilos[i];
        if (hilo->activo)
        {
            hilos[j++] = hilos[i];
        }
        else
        {
            free(hilo);
        }
    }

    *num_hilos = j;
}

// Función principal
int main()
{
    int memoria = 64;
    int memoriaUsada =0;
    lista = crear_segmento(-1, 'H', 0, memoria);
    inicializarMatriz();
    mostrarMatriz();
    
    srand(time(NULL));
    Hilo *hilos[10];
    int num_hilos = 0;
    int activo = 1;

    // Creamos el hilo central
    pthread_t hilo_central;
    pthread_create(&hilo_central, NULL, NULL, NULL);

    // Iteramos hasta que se finalice el programa
    while (activo)
    {
        
        // Agregamos un nuevo hilo con cierta probabilidad
        if (memoriaUsada < (memoria * 70) / 100)
        {
            if (rand() % 5 == 0)
            {
                printf("\nPorcentaje de memoria usada: %.2f%%\n",((float)memoriaUsada / (float)memoria) * 100.0);
                agregar_hilo(hilos, &num_hilos);
            }
            
        }
        

        // Finalizamos algunos hilos con cierta probabilidad
        if ( num_hilos > 0 && memoriaUsada > (memoria * 70) / 100)
        {
            finalizar_hilos(hilos, &num_hilos);
            printf("\nPorcentaje Usado: %.2f%%\n",((float)memoriaUsada / (float)memoria) * 100.0);
        }
            memoriaUsada = calcular_memoria_usada(lista);

        // Esperamos un tiempo aleatorio antes de volver a iterar
        usleep((rand() % 1000) * 1000);
    }
    // Finalizamos todos los hilos restantes
    int i;
    for (i = 0; i < num_hilos; i++)
    {
        Hilo *hilo = hilos[i];
        hilo->activo = 0;
        pthread_join(hilo->thread_id, NULL);
        free(hilo);
    }

    // Finalizamos el hilo central
    pthread_join(hilo_central, NULL);

    return 0;
}
