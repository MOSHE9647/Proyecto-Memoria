#include "src/socios.h"

Memory mainMemory;

int main () {
	int option = 0;
	do {
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

		switch (option) {
			case 1: {
				//mapaDeBits();

				break;
			}
			case 2: {
				int option = 0;
				do {
					system("clear");
					printf("Politica: Listas Ligadas\n");
					printf("**************************************\n");
					printf("1. Siguiente Ajuste\n");
					printf("2. Primer Ajuste\n");
					printf("3. Mejor Ajuste\n");
					printf("4. Peor Ajuste\n");
					printf("5. Volver\n");
					printf("**************************************\n");
					printf("Digite una Opcion: "); scanf("%d", &option);

					switch (option) {
						case 1:
							//sigAjuste();
							break;
						case 2:
							//primAjuste();
							break;
						case 3:
							//mejorAjuste();
							break;
						case 4:
							//peorAjuste();
							break;
						case 5:
							break;
						default:
							printf("\nOpcion Invalida!!\n");
							printf("Digite una Opcion Valida...\n\n");
							sysPause();
							break;
					}
				} while (option != 5);
				break;
			}
			case 3: {
				system("clear");
				socios(&mainMemory);
				break;
			}
			case 4: {
				//multiProgram();
				break;
			}
			case 5: {
				printf("\nSaliendo del Programa...\n");
				break;
			}
			default: {
				printf("\nOpcion Invalida!!\n");
				printf("Digite una Opcion Valida...\n\n");
				sysPause();
				break;
			}
		}
	} while (option != 5);
	return 1;
}