#ifndef SIMULACRO_SO_SEMAFORO_H
#define SIMULACRO_SO_SEMAFORO_H

#include "globals.h"
#include "processo.h"
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
typedef struct espera_bcp {
    BCP *processo;
    struct espera_bcp *prox;
} Espera_BCP;

typedef struct lista_espera_bcp {
    Espera_BCP *head;
    Espera_BCP *tail;
} Lista_Espera_BCP;

typedef struct semaforo {
    pthread_mutex_t mutex_lock;
    char nome; // nome do semáforo
    volatile int v; // valor do semáforo
    int refcount; // conta o número de processos que estão usando o semáforo
    Lista_Espera_BCP *waiting_list; // topo da lista de espera de processos do semáforo
    struct semaforo *prox;
} Semaforo;

// lista de semáforos
typedef struct lista_semaforos {
    Semaforo *head;
} Lista_Semaforos;

// Funções -------------------------------------------------------------------------------------------------------------

// Cria uma struct de espera de BCP
Espera_BCP *novaEsperaBCP();

// Cria uma lista de espera de BCP
Lista_Espera_BCP *novaListaEsperaBCP();

// Cria um semáforo, inicializado com v = 0
Semaforo *novoSemaforo(char nome);

// Libera a memória alocada para um semáforo
void freeSemaforo(Semaforo *semaforo);

// Inicializa uma lista de semáforos vazia
Lista_Semaforos *novaListaSemaforos();

// Insere um semáforo na lista global de todos os semáforos existentes
void inserirSemaforoGlobal(Semaforo *semaforo);

// Libera a memória alocada para uma lista de semáforos
void freeListaSemaforo(Lista_Semaforos *semaforos);

// Retorna um semáforo a partir de seu identificador
Semaforo *retrieveSemaforo(char nome);

// Remove um semáforo da lista global de todos os semáforos existentes
void removeSemaforo(Semaforo *semaforo);

// Enfileira processos bloqueados por uma chamada falha à semaphoreP()
void sem_queue(Lista_Espera_BCP *lista, BCP *processo);

#endif //SIMULACRO_SO_SEMAFORO_H