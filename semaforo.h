#ifndef SIMULACRO_SO_SEMAFORO_H
#define SIMULACRO_SO_SEMAFORO_H

#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include "sintetico.h"

// Declaração de Tipos -------------------------------------------------------------------------------------------------

// declaração avançada de BCP para evitar dependência circular
struct bcp;
typedef struct bcp BCP;

typedef struct semaphore {
    pthread_mutex_t mutex_lock;
    char name; // nome do semáforo
    volatile int v; // valor do semáforo
    int refcount; // conta o número de processos que estão usando o semáforo
    struct sem_li *waiting_list; // topo da lista de espera de processos do semáforo
    struct semaphore *next;
} semaphore_t;

// lista de processos esperando o semáforo
typedef struct sem_li {
    BCP *proc;
    struct sem_li *next;
} sem_list_item_t;

// lista de todos semáforos
typedef struct all_sem_li {
    semaphore_t *head;
} all_sem_list_t;

// Variáveis Globais ---------------------------------------------------------------------------------------------------

all_sem_list_t existing_semaphores;

// Funções -------------------------------------------------------------------------------------------------------------

// Cria um semáforo
void createSemaphore(char name);

// Insere um semáforo na lista global de todos os semáforos existentes
void insert_semaphore(semaphore_t *item);

// Remove um semáforo da lista global de todos os semáforos existentes
void removeSemaphore(semaphore_t *item);

// Retorna um semáforo a partir de seu identificador
semaphore_t *retrieveSemaphore(char name);

// Enfileira processos bloqueados por uma chamada falha à semaphoreP()
void sem_queue(sem_list_item_t **list, BCP *proc);

// Imprime os semáforos da lista global
void showSemaphoreList();

#endif //SIMULACRO_SO_SEMAFORO_H