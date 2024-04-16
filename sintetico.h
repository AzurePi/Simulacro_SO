#ifndef SIMULACRO_SO_SINTETICO_H
#define SIMULACRO_SO_SINTETICO_H

typedef struct {
    char nome[50];
    int mem_space;
    int cpu_time;
    int estado; // (0: executando; 1: pronto; 2: bloqueado)
} Processo;

int lerSintetico(FILE *file, Processo *p);
void escreverSintetico(Processo p, FILE *file);

#endif //SIMULACRO_SO_SINTETICO_H
