#ifndef SIMULACRO_SO_SYSCALLS_H
#define SIMULACRO_SO_SYSCALLS_H

#include "memoria.h"
#include "semaforo.h"
#include "processo.h"
#include "globals.h"
#include "disco.h"

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
    FINAL_EXECUCAO, PROCESS_CREATE, INICIO_E_S, TERMINO_E_S
} INTERRUPCAO;

// struct usada para passagem de argumentos para processInterrupt()
typedef struct {
    INTERRUPCAO tipo_interrupcao;
    BCP *processo;
} InterruptArgs;

//struct usada para os nós processos de E/S do disco serem printados na tela
typedef struct no_es {
    BCP *processo;
    struct no_es *prox;
} noES;

//struct que será a fila de processos E/S a serem printados
typedef struct fila_es {
    noES *head;
    noES *tail;
} filaES;

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

// Chamada de operação de E/S para disco
void *DiskRequest(void *args);

// Sinalização de final de E/S para disco
void *DiskFinish(void *args);

// Chamada de operação de E/S oara impressão
void *PrintRequest(void *args);

// Sinalização de final de E/S para impressão
void *PrintFinish(void *args);

//Inicializa fila E/S
filaES *newScreenQueue();

//Cria o nó com os processos
noES *criaNo(BCP *processo);

//Insere o nó criado na fila
void inserirFila(noES *noAdd);

//Declara a fila inicialmente vazia
void iniciaFila();

#endif //SIMULACRO_SO_SYSCALLS_H
