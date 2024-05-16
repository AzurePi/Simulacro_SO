#ifndef SIMULACRO_SO_SEMAFORO_H
#define SIMULACRO_SO_SEMAFORO_H

#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include "sintetico.h"
#include "interface.h"

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
} Semaforo;

// lista de processos esperando o semáforo
typedef struct sem_li {
    BCP *proc;
    struct sem_li *next;
} Lista_Espera_BCP;

// lista de semáforos
typedef struct all_sem_li {
    Semaforo *head;
} Lista_Semaforos;

// Variáveis Globais ---------------------------------------------------------------------------------------------------

Lista_Semaforos existing_semaphores;

// Funções -------------------------------------------------------------------------------------------------------------

// Cria um semáforo
Semaforo * createSemaphore(char name);

// Insere um semáforo na lista global de todos os semáforos existentes
void insert_semaphore(Semaforo *semaforo);

// Remove um semáforo da lista global de todos os semáforos existentes
void removeSemaphore(Semaforo *semaforo);

// Retorna um semáforo a partir de seu identificador
Semaforo *retrieveSemaphore(char name);

// Enfileira processos bloqueados por uma chamada falha à semaphoreP()
void sem_queue(Lista_Espera_BCP **list, BCP *proc);

// Imprime os semáforos da lista global
void showSemaphoreList();

#endif //SIMULACRO_SO_SEMAFORO_H