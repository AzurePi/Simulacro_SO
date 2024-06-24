#ifndef DISCO_H
#define DISCO_H

#include <stdio.h>
#include <stdlib.h>

#include "include/globals.h"

// Declarações de Tipos ------------------------------------------------------------------------------------------------

// declarações avançadas para evitar declaração circular
typedef struct bcp BCP;

// usada para para saber que trilha/parte do disco cada processo está utilizando
typedef struct {
    BCP *processo;
    int track;
} DiskArgs;

// Usado para criar uma lista de informações sobre E/S
typedef struct DiskNode {
    DiskArgs *args;
    struct DiskNode *next;
} DiskNode;

// Cria um alista de acessos a disco por E/S
typedef struct disk_queue {
    DiskNode *head;
    DiskNode *tail;
} DiskQueue;

// Protótipos de Funções -----------------------------------------------------------------------------------------------

//
void *discoElevador();

//
void *newDiskQueue();

// Adiciona um novo pedido de acesso ao disco par E/S, ordenando pela direção do cabeçote de leitura (algoritmo do Elevador)
void enqueue_disk(DiskArgs *args);

// Remove e retorna o pedido da E/S da Fila e seleciona o próximo baseado no algoritmo do Elevador
DiskArgs *dequeue_disk();

// Printa a fila de E/S a ser executada
void print_disk_queue();

#endif //DISCO_H
