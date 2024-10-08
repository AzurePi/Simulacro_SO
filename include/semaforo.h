#ifndef SIMULACRO_SO_SEMAFORO_H
#define SIMULACRO_SO_SEMAFORO_H

#include "globals.h"
#include "processo.h"

#include <malloc.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

// Declaração de Tipos -------------------------------------------------------------------------------------------------

// declaração avançada de BCP para evitar dependência circular
typedef struct bcp BCP;

// lista de processos esperando o semáforo
typedef struct espera_bcp {
    BCP *processo;
    struct espera_bcp *prox;
} Espera_BCP;

typedef struct fila_espera_bcp {
    Espera_BCP *head;
    Espera_BCP *tail;
} Fila_Espera_BCP;

typedef struct semaforo {
    pthread_mutex_t mutex_lock;
    char nome; // nome do semáforo
    volatile int v; // valor do semáforo
    int refcount; // conta o número de processos que estão usando o semáforo
    Fila_Espera_BCP *waiting_list; // topo da lista de espera de processos do semáforo
    //struct semaforo *prox;
} Semaforo;

// Um semáforo em uma lista de semáforos
typedef struct no_semaforo {
    Semaforo *semaforo;
    struct no_semaforo *prox;
} No_Semaforo;

// Lista de semáforos
typedef struct lista_semaforos {
    No_Semaforo *head;
} Lista_Semaforos;

// Protótipos de Funções -----------------------------------------------------------------------------------------------

// Cria uma struct de espera de BCP
Espera_BCP *novaEsperaBCP();

// Cria uma lista de espera de BCP
Fila_Espera_BCP *novaListaEsperaBCP();

// Cria um semáforo, inicializado com v = 0
Semaforo *novoSemaforo(char nome);

// Libera a memória alocada para um semáforo
void freeSemaforo(Semaforo *semaforo);

// Cria uma struct para representar um semáforo numa lista
No_Semaforo *novoNoSemaforo(Semaforo *sem);

// Inicializa uma lista de semáforos vazia
Lista_Semaforos *novaListaSemaforos();

// Libera a memoŕia alocada para uma referência de lista a um semáforo
void freeNoSemaforo(No_Semaforo *no_semaforo);

// Insere um semáforo em uma lista de semáforos
void inserirSemaforo(Lista_Semaforos *lista, Semaforo *semaforo);

// Insere um semáforo na lista global de todos os semáforos existentes
void inserirSemaforoGlobal(Semaforo *semaforo);

// Libera a memória alocada para uma lista de semáforos
void freeListaSemaforo(Lista_Semaforos *semaforos);

// Retorna um semáforo a partir de seu identificador
Semaforo *retrieveSemaforo(char nome);

// Remove um semáforo da lista global de todos os semáforos existentes
void removeSemaforoGlobal(No_Semaforo *semaforo);

// Enfileira processos bloqueados por uma chamada falha à semaphoreP()
void sem_queue(Fila_Espera_BCP *lista, BCP *processo);

#endif //SIMULACRO_SO_SEMAFORO_H
