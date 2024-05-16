#ifndef SIMULACRO_SO_EVENTOS_H
#define SIMULACRO_SO_EVENTOS_H
#include <pthread.h>

#include "sintetico.h"

int relogio = 0;

//struct semaforo
typedef struct semaphore {
    pthread_mutex_t mutex_lock;
    volatile int v; //valor do semaforo
    char name;
    int refcount; //conta o numero de processos que esta usando o semaforo
    struct sem_li* waiting_list; // topo da lista de espera de processos do semaforo
    struct semaphor* ne;
} semaphore_t;

typedef struct sem_li //lista de processos esperando o semaforo
{
    BCP* proc;
    struct sem_li* next;
} sem_list_item_t;

typedef struct all_sem_li //lista de todos semaforos
{
    semaphore_t* head;
} all_sem_list_t;


// interrupção gerada pela execução da função processCreate e pelo término de operações de E/S
void processInterrupt();

// tratamento de bloqueio de processo
void semaphoreP(int v); //TODO: trocar parâmetro para o tipo do semáforo

// tratamento de desbloqueio de processo
void semaphoreV(int v); //TODO: trocar parâmetro para o tipo do semáforo

// chamada de operação de E/S para disco
void DiskRequest();

// sinalização de final de E/S para disco
void DiskFinish();

// chamada de operação de E/S para impressão
void PrintRequest();

//  sinalização de final de E/S para impressão
void PrintFinish();

// chamada de operação de carregamento na memória
void memLoadReq();

// sinalização de final de carregamento
void memLoadFinish();

//  chamada para operação no sistema de arquivos
void fsRequest();

// sinalização de final de operação no sistema de arquivos
void fsFinish();

// chamada para iniciar a criação de um processo no BCP
void *processCreate();

// chamada para terminar a existência de um processo no BCP
void processFinish();

#endif //SIMULACRO_SO_EVENTOS_H
