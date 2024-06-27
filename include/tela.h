#ifndef TELA_H
#define TELA_H

#include <stdlib.h>

#include "globals.h"
#include "processo.h"

// Declarações de Tipos ------------------------------------------------------------------------------------------------

// declarações avançadas para evitar declaração circular
typedef struct bcp BCP;

// Informação de um processo e por quanto tempo ele irá imprimir na tela
typedef struct tela_args {
    BCP *processo;
    int t;
} TelaArgs;

// Nó de um processo na fila de requisição de escrita na tela
typedef struct no_tela {
    TelaArgs *proc;
    struct no_tela *prox;
} NoTela;

// Fila de requisições de impressão na tela
typedef struct fila_tela {
    NoTela *head;
    NoTela *tail;
} FilaTela;

// Protótipos de Funções -----------------------------------------------------------------------------------------------

// Permanece em execução até o fim da simulação para processar impressões na tela
void *tela();

// Inicializa uma fila global vazia de requisição de acesso à tela
void inicializarScreenQueue();

// Libera a memória alocada para a fila global de requisição de acesso à tela
void freeScreenQueue();

// Cria um nó de requisição à tela para um processo
NoTela *criaNoTela(TelaArgs *proc);

// Insere um nó de requisição de acesso à tela na fila global
void inserirFila(NoTela *no);

// Remove e retorna um nó de requisição de acesso à tela da fila global
NoTela *removeFila();

#endif //TELA_H
