#include "source/listasLigadas.h"
#include "source/partFijas.h"
#include "source/mapaBits.h"
#include "source/socios.h"
#include "source/colas.h"

int main () {
	pthread_mutex_init(&mutex, NULL);

	initMemoryFijas();
    initMemoryBits();
    initSocios();

	int option = 0;
	system("clear");
	printf("Proyecto Administración de Memoria\n");
	printf("**************************************\n");
	printf("1. Mapas de Bits\n");
	printf("2. Listas Ligadas\n");
	printf("3. Sistemas Socios\n");
	printf("4. Tamaños Variables\n");
	printf("5. Salir del Programa\n");
	printf("**************************************\n");
	printf("Digite una Opcion: "); scanf("%d", &option);
	system("clear");

	switch (option) {
		case 1: {
			politica = MAPA_BITS;
			break;
		}
		case 2: {
			politica = LISTAS_LIGADAS;
			break;
		}
		case 3: {
			politica = SOCIOS;
			break;
		}
		case 4: {
			politica = PART_FIJAS;
			break;
		}
		case 5: {
			printf("\nSaliendo del Programa...\n");
			break;
		}
		default: {
			printf("\nOpcion Invalida!!\n");
			printf("Digite una Opcion Valida...\n\n");
			break;
		}
	}

    iniciarEjecucion();
	printStadistics();

	pthread_mutex_destroy(&mutex);
	return 1;
}