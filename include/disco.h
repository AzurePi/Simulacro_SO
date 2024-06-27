#ifndef DISCO_H
#define DISCO_H

#include <stdio.h>
#include <stdlib.h>

#include "include/globals.h"

// Declarações de Tipos ------------------------------------------------------------------------------------------------

// declarações avançadas para evitar declaração circular
typedef struct bcp BCP;

// Informação de um processo tnetando acessar o disco
typedef struct {
    BCP *processo;
    int trilha; //trilha tentando ser acessada
    int t; //tempo decorrido no acesso ao disco
} DiskArgs;

// Nó da fila de requisição de acesso a disco
typedef struct no_disco {
    DiskArgs *args;
    struct no_disco *next;
} NoDisco;

// Lista de requisição de acesso a disco
typedef struct disk_queue {
    NoDisco *head;
    NoDisco *tail;
} FilaDisco;

// Protótipos de Funções -----------------------------------------------------------------------------------------------

// Permanece em execução até o encerramento do programa para processar acesso ao disco
void *discoElevador();

// Inicializa uma nova fila global para requisições ao disco
void inicializarDiskQueue();

// Libera a memória alocada para a fila global de requisições ao disco
void freeDiskQueue();

// Adiciona um novo pedido de acesso ao disco par E/S, ordenando pela trilha em relação á direção do cabeçote de leitura (algoritmo do elevador)
void enqueue_disk(DiskArgs *args);

// Remove e retorna a próxima requisição de acesso ao disco
DiskArgs *dequeue_disk(int t);

#endif //DISCO_H
