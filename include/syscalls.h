#ifndef SIMULACRO_SO_SYSCALLS_H
#define SIMULACRO_SO_SYSCALLS_H

#include "memoria.h"
#include "semaforo.h"
#include "processo.h"
#include "globals.h"
#include "interface.h"
#include "cpu.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

// Definição de Macros -------------------------------------------------------------------------------------------------

#define process_interrupt (1)
#define semaphore_P (10)
#define semaphore_V (11)
#define disk_request (4)
#define disk_finish (5)
#define print_request (14)
#define print_finish (15)
#define mem_load_req (6)
#define mem_load_finish (7)
#define fs_request (8)
#define fs_finish (9)
#define process_create (2)
#define process_finish (3)

// Declarações de tipos ------------------------------------------------------------------------------------------------

// declaração avançada para evitar dependência circular
typedef struct semaforo Semaforo;
typedef struct bcp BCP;

// struct usada para passagem de argumentos para a syscall semaphoreP
typedef struct {
    Semaforo *semaforo;
    BCP *proc;
} SemaphorePArgs;

// Funções -------------------------------------------------------------------------------------------------------------

// Gerenciamento de chamadas de sistema
bool sysCall(int op, void *args);

// interrupção gerada pelo final do quantum-time de um processo
void *processInterrupt(void *args);

// tratamento de bloqueio de processo
void *semaphoreP(void *semaph);

// tratamento de desbloqueio de processo
void *semaphoreV(void *semaforo);

// chamada de operação de E/S para disco
void *DiskRequest(void *args);

// sinalização de final de E/S para disco
void *DiskFinish(void *args);

// chamada de operação de E/S para impressão
void *PrintRequest(void *args);

//  sinalização de final de E/S para impressão
void *PrintFinish(void *args);

// chamada de operação de carregamento na memória
void *memLoadReq(void *args);

// sinalização de final de carregamento
void *memLoadFinish(void *args);

//  chamada para operação no sistema de arquivos
void *fsRequest(void *args);

// sinalização de final de operação no sistema de arquivos
void *fsFinish(void *args);

// chamada para iniciar a criação de um processo no BCP
void *processCreate(void *filename);

// chamada para terminar a existência de um processo no BCP
void *processFinish(void *args);

#endif //SIMULACRO_SO_SYSCALLS_H