#ifndef SIMULACRO_SO_SINTETICO_H
#define SIMULACRO_SO_SINTETICO_H

#include "lista.h"

typedef struct {
    char nome[50]; //nome do programa
    int id_seg; //identificador do segmento
    int prioridade_OG; //prioridade original do programa
    int tamanho_seg; //tamanho do segmento (em kbytes)
    Lista *semaforos; //lista de semáforos usados pelo programa
} BCP;

int lerBCP(FILE *file, BCP *p);

#endif //SIMULACRO_SO_SINTETICO_H
