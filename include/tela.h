#ifndef TELA_H
#define TELA_H

#include <stdlib.h>

#include "globals.h"
#include "processo.h"

// Declarações de Tipos ------------------------------------------------------------------------------------------------

// declarações avançadas para evitar declaração circular
typedef struct bcp BCP;

//Nó de um processo na fila de escrita na tela
typedef struct no_tela {
    BCP *processo;
    struct no_tela *prox;
} NoTela;

// Fila de saída da tela
typedef struct fila_tela {
    NoTela *head;
    NoTela *tail;
} FilaTela;

// Protótipos de Funções -----------------------------------------------------------------------------------------------

// Inicializa uma fila global vazia de requisição dde acesso à tela
void inicializarScreenQueue();

// Libera a memória alocada para a fila global de requisição de acesso à tela
void freeScreenQueue();

// Cria o nó de requisição à tela para um processo processos
NoTela *criaNoTela(BCP *processo);

// Insere um nó de requisição á tela na fila global
void inserirFila(NoTela *no);

// Remove um nó de requisição à tela da fila global
NoTela *removeFila();

#endif //TELA_H
