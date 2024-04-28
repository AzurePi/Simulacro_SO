#ifndef SIMULACRO_SO_LISTA_H
#define SIMULACRO_SO_LISTA_H

#include "sintetico.h"

typedef struct Lista Lista;

Lista *criarLista();

void inserirLista(Lista *lista, BCP *programa);

#endif //SIMULACRO_SO_LISTA_H
