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

int lerSintetico(FILE *file, BCP *p);
void escreverSintetico(BCP p, FILE *file);

#endif //SIMULACRO_SO_SINTETICO_H
