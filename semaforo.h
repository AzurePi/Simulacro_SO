#ifndef SIMULACRO_SO_SEMAFORO_H
#define SIMULACRO_SO_SEMAFORO_H

#include <pthread.h>
#include "sintetico.h"

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

#endif //SIMULACRO_SO_SEMAFORO_H