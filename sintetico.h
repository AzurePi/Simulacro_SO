#ifndef SIMULACRO_SO_SINTETICO_H
#define SIMULACRO_SO_SINTETICO_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

#define MAX 100

typedef struct comando {
    int opcode;
    int parametro;
    struct comando *prox;
} Comando;

typedef struct {
    Comando *head;
    int nElementos;
} Lista_Comandos;

typedef struct {
    char nome[61]; //nome do programa
    int id_seg; //identificador do segmento
    int prioridade_OG; //prioridade original do programa
    int tamanho_seg; //tamanho do segmento (em kbytes)
    int estado; //0 - pronto; 1 - executando; 2 - parado; 3 - terminado
    char *semaforos; //Lista de semáforos usados pelo programa
    Lista_Comandos *comandos;
} BCP;

BCP *BCP_From_Sintetico(FILE *programa);

//Retorna true (1) se vazia, false (0) caso contrário.
bool Vazia(Lista_Comandos *l);

//Retorna true (1) se cheia, false (0) caso contrário.
bool Cheia(Lista_Comandos *l);

//Inicializa uma Lista_Comandos vazia.
void Definir(Lista_Comandos *l);

/*
Insere um novo comando no final de uma Lista_Comandos.
Devolve true se sucesso, false caso contrário.
*/
bool Inserir(Comando *comando, Lista_Comandos *lista)


#endif //SIMULACRO_SO_SINTETICO_H
