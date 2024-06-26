#ifndef DISCO_H
#define DISCO_H

#include <stdio.h>
#include <stdlib.h>

#include "include/globals.h"

// Declarações de Tipos ------------------------------------------------------------------------------------------------

// declarações avançadas para evitar declaração circular
typedef struct bcp BCP;

// Informação de um processo e qual trilha ele está tentando acessar
typedef struct {
    BCP *processo;
    int trilha;
} DiskArgs;

// Nó de requisição se acesso a disco
typedef struct no_disco {
    DiskArgs *args;
    struct no_disco *next;
} NoDisco;

// Cria uma lista (ordenada por trilha) de requisição de acesso a disco
typedef struct disk_queue {
    NoDisco *head;
    NoDisco *tail;
} FilaDisco;

// Protótipos de Funções -----------------------------------------------------------------------------------------------

// Laço que dura toda a execução da simulação para processar acesso ao disco
void *discoElevador();

// Inicializa uma nova fila para requisições ao disco
void inicializarDiskQueue();

// Adiciona um novo pedido de acesso ao disco par E/S, ordenando pela direção do cabeçote de leitura (algoritmo do Elevador)
void enqueue_disk(DiskArgs *args);

// Remove e retorna o pedido da E/S da Fila e seleciona o próximo baseado no algoritmo do Elevador
DiskArgs *dequeue_disk();

#endif //DISCO_H
