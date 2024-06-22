#ifndef SIMULACRO_SO_SYSCALLS_H
#define SIMULACRO_SO_SYSCALLS_H

#include "memoria.h"
#include "semaforo.h"
#include "processo.h"
#include "globals.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Definição de Macros -------------------------------------------------------------------------------------------------

#define process_interrupt 1
#define semaphore_P 10
#define semaphore_V 11
#define disk_request 4
#define disk_finish 5
#define print_request 14
#define print_finish 15
#define mem_load_req 6
#define mem_load_finish 7
#define fs_request 8
#define fs_finish 9
#define process_create 2
#define process_finish 3

// Declarações de Tipos ------------------------------------------------------------------------------------------------

// declaração avançada para evitar dependência circular
typedef struct semaforo Semaforo;
typedef struct bcp BCP;

// struct usada para passagem de argumentos para a syscall semaphoreP()
typedef struct {
    Semaforo *semaforo;
    BCP *proc;
} SemaphorePArgs;

typedef enum {
    FINAL_EXECUCAO, PROCESS_CREATE, TERMINO_E_S
} INTERRUPCAO;

// struct usada para passagem de argumentos para processInterrupt()
typedef struct {
    INTERRUPCAO tipo_interrupcao;
    BCP *processo;
} InterruptArgs;

//struct usada para para saber que trilha/parte do disco cada processo está utilizando
typedef struct {
    BCP* processo;
    int track;
} DiskArgs;

//Usado para criar uma lista de informações sobre E/S
typedef struct DiskNode {
    DiskArgs* args;
    struct DiskNode* next;
} DiskNode;

//Cria um alista de acessos a disco por E/S
typedef struct {
    DiskNode* head;
    DiskNode* tail;
} DiskQueue;

pthread_mutex_t mutex_disk_queue = PTHREAD_MUTEX_INITIALIZER;

DiskQueue disk_queue = {NULL, NULL}; //Declara a fila de E/S inicialmente como vazia

bool disk_busy = false; //Variável que indica se algum processo está acessando o disco ou ele está livre

int current_track = 0; // trilha atual do braço do disco

bool direction_up = true; // direção inicial do braço do disco

// Protótipos de Funções -----------------------------------------------------------------------------------------------

// função auxiliar para criar e despachar uma thread com uma função
void create_and_detach(void *(*func)(void *), void *args);

// Gerenciamento de chamadas de sistema
bool sysCall(short func, void *args);

// Gerenciamento de interrupções de um processo
void *processInterrupt(void *args);

// Tratamento de bloqueio de processo
void *semaphoreP(void *args_semaforo);

// Tratamento de desbloqueio de processo
void *semaphoreV(void *args_semaforo);

// Chamada de operação de carregamento na memória
void *memLoadReq(void *args);

// Sinalização de final de carregamento
void *memLoadFinish(void *args);

// Chamada para operação no sistema de arquivos
void *fsRequest(void *args);

// Sinalização de final de operação no sistema de arquivos
void *fsFinish(void *args);

// chamada para iniciar a criação de um processo no BCP
void *processCreate(void *filename);

// chamada para terminar a existência de um processo no BCP
void *processFinish(void *args_BCP);

//Adiciona um novo pedido de acesso ao disco par E/S, ordenando pela direção do cabeçote de leitura (algoritmo do Elevador)
void enqueue_disk(DiskQueue* queue, DiskArgs* args);

//Remove e retorna o pedido da E/S da Fila e seleciona o próximo baseado no algoritmo do Elevador
DiskArgs* dequeue_disk(DiskQueue* queue);

// Chamada de operação de E/S para disco
void *DiskRequest(void *args);

// Sinalização de final de E/S para disco
void *DiskFinish(void *args);

//Printa a fila de E/S a ser executada
void print_disk_queue(DiskQueue* queue);

#endif //SIMULACRO_SO_SYSCALLS_H
