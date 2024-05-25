#ifndef SIMULACRO_SO_EVENTOS_H
#define SIMULACRO_SO_EVENTOS_H

#include "memoria.h"
#include "semaforo.h"
#include "processo.h"
#include "globals.h"
#include "interface.h"
#include "cpu.h"

#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>
#include <stdlib.h>

// declaração avançada para evitar dependência circular
typedef struct semaforo Semaforo;
typedef struct bcp BCP;

// Funções -------------------------------------------------------------------------------------------------------------

// interrupção gerada pelo final do quantum-time de um processo
void processInterrupt();

// tratamento de bloqueio de processo
bool semaphoreP(Semaforo *semaph, BCP *proc);

// tratamento de desbloqueio de processo
void semaphoreV(Semaforo *semaph);

// chamada de operação de E/S para disco
void DiskRequest();

// sinalização de final de E/S para disco
void DiskFinish();

// chamada de operação de E/S para impressão
void PrintRequest();

//  sinalização de final de E/S para impressão
void PrintFinish();

// chamada de operação de carregamento na memória
void memLoadReq(BCP *processo);

// sinalização de final de carregamento
void memLoadFinish();

//  chamada para operação no sistema de arquivos
void fsRequest();

// sinalização de final de operação no sistema de arquivos
void fsFinish();

// chamada para iniciar a criação de um processo no BCP
void *processCreate();

// chamada para terminar a existência de um processo no BCP
void processFinish(BCP *bcp);

#endif //SIMULACRO_SO_EVENTOS_H