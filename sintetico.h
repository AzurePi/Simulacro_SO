#ifndef SIMULACRO_SO_SINTETICO_H
#define SIMULACRO_SO_SINTETICO_H

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

typedef struct {
    char nome[61]; //nome do programa
    int id_seg; //identificador do segmento
    int prioridade_OG; //prioridade original do programa
    int tamanho_seg; //tamanho do segmento (em kbytes)
    int estado; //0 - pronto; 1 - executando; 2 - parado; 3 - terminado
    char *semaforos; //Lista de semáforos usados pelo programa
} BCP;

BCP *BCP_From_Sintetico(FILE *programa);

#endif //SIMULACRO_SO_SINTETICO_H
