#ifndef SIMULACRO_SO_SEMAFORO_H
#define SIMULACRO_SO_SEMAFORO_H

#include "globals.h"
#include "sintetico.h"
#include "interface.h"

#include <malloc.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>

// Declaração de Tipos -------------------------------------------------------------------------------------------------

// declaração avançada de BCP para evitar dependência circular
typedef struct bcp BCP;

// lista de processos esperando o semáforo
typedef struct lista_espera_bcp {
    BCP *proc;
    struct lista_espera_bcp *prox;
} Lista_Espera_BCP;

typedef struct semaforo {
    pthread_mutex_t mutex_lock;
    char name; // nome do semáforo
    volatile int v; // valor do semáforo
    int refcount; // conta o número de processos que estão usando o semáforo
    BCP *waiting_list; // topo da lista de espera de processos do semáforo
    struct semaforo *prox;
} Semaforo;

// lista de semáforos
typedef struct lista_semaforos {
    Semaforo *head;
} Lista_Semaforos;

// Funções -------------------------------------------------------------------------------------------------------------

// Cria um semáforo
Semaforo *novoSemaforo(char name);

// Libera a memória alocada para um semáforo
void freeSemaforo(Semaforo *semaforo);

// Inicializa uma lista de semáforos vazia
Lista_Semaforos *novaListaSemaforos();

// Libera a memória alocada para uma lista de semáforos
void freeListaSemaforo(Lista_Semaforos *semaforos);

// Insere um semáforo na lista global de todos os semáforos existentes
void insereSemaforo(Semaforo *semaforo);

// Remove um semáforo da lista global de todos os semáforos existentes
void removeSemaforo(Semaforo *semaforo);

// Retorna um semáforo a partir de seu identificador
Semaforo *retrieveSemaphore(char name);

// Enfileira processos bloqueados por uma chamada falha à semaphoreP()
void sem_queue(Lista_Espera_BCP **list, BCP *proc);

#endif //SIMULACRO_SO_SEMAFORO_H