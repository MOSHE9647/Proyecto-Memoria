/*
Política de asignación de memoria de listas ligadas con algoritmo del siguiente ajuste.

Cuando se crea un nuevo nodo en la lista ligada, 
se busca el siguiente bloque de memoria disponible 
que sea lo suficientemente grande para almacenar el nodo. 
Si se encuentra un bloque adecuado, se utiliza para crear el 
nodo y se ajusta el puntero del nodo anterior para que apunte al nuevo nodo. 
Si no se encuentra un bloque lo suficientemente grande, se solicita una
 nueva asignación de memoria al sistema operativo y se utiliza para crear el nuevo nodo.

Después de crear un nuevo nodo, se actualiza un puntero 
especial en la lista ligada que apunta al último bloque 
de memoria utilizado. Cuando se necesita crear otro nuevo 
nodo, se comienza la búsqueda de memoria desde el bloque 
apuntado por el puntero especial, en lugar de comenzar 
desde el principio de la lista.

Esta técnica puede ser eficiente para la asignación de 
memoria en listas ligadas, ya que evita la fragmentación 
de la memoria y reduce la cantidad de búsquedas necesarias. 
Sin embargo, puede generar un mayor costo de gestión de la 
memoria debido a la necesidad de mantener actualizado el 
puntero especial. Además, esta técnica puede no ser óptima 
en situaciones en las que los nodos de la lista no tienen 
un tamaño uniforme, lo que puede generar una mayor 
fragmentación de la memoria.
*/



#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h>

pthread_mutex_t mutex;

struct Node {
    char* process_name;
    int flag;
    int number;
    struct Node* next;
};

struct List {
    struct Node* head;
    struct Node* last_used_node;
    pthread_mutex_t mutex;
};

void init_list(struct List* list) {
    list->head = NULL;
    list->last_used_node = NULL;
    pthread_mutex_init(&list->mutex, NULL);
}

struct Node* create_node(char* process_name, int number, int flag) {
    struct Node* node = (struct Node*) malloc(sizeof(struct Node));
    node->flag = flag;
    node->process_name = process_name;
    node->number = number;
    node->next = NULL;
    return node;
}


void insetarNodo(struct List* list, char* nombre, int valor){
    struct Node* nodoAux = NULL;
    if(list->last_used_node != NULL){
        nodoAux = list->last_used_node;
    }else{
        nodoAux = list->head;
    }
    pthread_mutex_lock(&list->mutex);
        while (nodoAux != NULL) {
            if(nodoAux->flag == 0 && nodoAux->number >= valor){
                if(nodoAux->next == NULL){
                    list->last_used_node = list->head;
                }else{
                    list->last_used_node = nodoAux->next;
                }
                nodoAux->process_name = nombre;
                nodoAux->flag = 1;
                int espacio = ceil(valor / 4);
                int restante = nodoAux->number-(espacio*4);
                nodoAux->number = espacio*4;
                if(restante != 0){
                    struct Node* temp = nodoAux->next;
                    struct Node* node = create_node("No Asignado", restante,0);
                    nodoAux->next = node;
                    node->next = temp;
                }
                break;
            }else{
                nodoAux = nodoAux->next;
            }  
        }
        /*
        if(nodoAux == NULL){
            list->last_used_node = list->head;
            struct Node* node = create_node(nombre, valor,0);
            struct Node* nodoA = list->last_used_node->next;
            while (nodoA->next != NULL) {
                nodoA = nodoA->next;
            }
            nodoA->next = node;
        }
        */
    pthread_mutex_unlock(&list->mutex);
}

void limpiarMemoria(struct List* list, int pos){
    pthread_mutex_lock(&list->mutex);
    struct Node* nodoAux = list->head;
    int cont = 0;
        while (nodoAux != NULL) {
            if(cont == pos){
                if(nodoAux->next != NULL && nodoAux->flag == 0){
                    nodoAux->next->process_name = "No Asignado";
                    nodoAux->next->flag = 0;
                }else{
                    nodoAux->process_name = "No Asignado";
                    nodoAux->flag = 0;
                }
                break;
            }else{
                cont ++;
                nodoAux = nodoAux->next;
            }
        }
    pthread_mutex_unlock(&list->mutex);
}

int size(struct List* list){
    pthread_mutex_lock(&list->mutex);
    struct Node* nodoAux = list->head;
    int cont = 0;
        while (nodoAux != NULL) {
           cont ++;
           nodoAux = nodoAux->next;
        }
    return cont;
    pthread_mutex_unlock(&list->mutex);
}

void print_list(struct List* list) {
    pthread_mutex_lock(&list->mutex);

    struct Node* curr = list->head;
    while (curr != NULL) {
        printf("%s %d\n", curr->process_name, curr->number);
        curr = curr->next;
    }

    pthread_mutex_unlock(&list->mutex);
}


void *hilo_agregar(void* lista)
{
    pthread_mutex_lock(&mutex);
    int flag = 0;
    int cont = 3;
    while(flag != 1 && cont < 15){
        printf("\n********************* Insertando Proceso ********************* \n");
        insetarNodo((struct List*)lista, "Programa",(rand() % 13)+4);
        print_list((struct List*)lista);
        sleep(1);
        printf("\n********************* Insertando Proceso ********************* \n");
        insetarNodo((struct List*)lista, "Programa ",(rand() % 13)+4);
        print_list((struct List*)lista);
        sleep(1);
        printf("\n********************* Eliminando Proceso ********************* \n");
        //int pos = size((struct List*)lista);
        limpiarMemoria((struct List*)lista, (rand() % cont)+1);
        print_list((struct List*)lista);
        sleep(1);
        cont ++;
    }
    pthread_mutex_unlock(&mutex);
    return NULL;
}

void *hilo_eliminar(void* lista)
{
    pthread_mutex_lock(&mutex);
    limpiarMemoria((struct List*)lista, "Programa B");
    printf("\n********************* Eliminando Proceso ********************* \n");
    print_list((struct List*)lista);
    sleep(2);
    pthread_mutex_unlock(&mutex);
    return NULL;
}

int main() {

    struct List *list = (struct List *)malloc(sizeof(struct List));
    //init_list(&list);
    //list = (struct List) malloc(sizeof);
    list->head = create_node("No Asignado",64,0);
    list->head->next = create_node("No Asignado",64,0);
    list->head->next->next = create_node("No Asignado",64,0);
    list->head->next->next->next = create_node("No Asignado",64,0);

    printf("\n********************* Lista Principal ********************* \n");
    print_list(list);
    //sleep(2);

    pthread_mutex_init(&mutex, NULL);
    pthread_t thread1;
    pthread_create(&thread1, NULL, hilo_agregar, (void*)list);
    pthread_join(thread1, NULL);
    pthread_mutex_destroy(&mutex);

    return 0;
}