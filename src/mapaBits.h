#ifndef MAPA_BITS_H
#define MAPA_BITS_H

/*******************************************************************************
    ARCHIVO DE CABECERA DONDE SE ENCUENTRAN TODAS LAS FUNCIONES RELACIONADAS
	A LA POLITICA DE ADMINISTRACION DE MEMORIA "MAPA DE BITS"
*******************************************************************************/

#include "memoria.h"

/*
    NOTA:
    Debemos implementar una función que se encargue de modificar y
    adaptar la particiones de Memoria, de forma que pueda funcionar
    sin importar que Política de Administración haya sido la
    última en ejecutarse.

    EJEMPLO:
    Si la última política en manipular la memoria fue Sistemas Socios,
    el programa debe de ser capaz de agarrar y dividir cada partición
    en bloques de 'PART_SIZE' KB cada una para así poder manipular la
    memoria en forma de Matriz y a como lo pide el Proyecto
*/

void memoryAdapt() {
    
}

void mapaDeBits() {
    printMemoryStatus(MAPA_BITS);
    sysPause();
}

#endif