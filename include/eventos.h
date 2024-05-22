#ifndef SIMULACRO_SO_EVENTOS_H
#define SIMULACRO_SO_EVENTOS_H

#include "semaforo.h"
#include "sintetico.h"
#include "globals.h"
#include "interface.h"

#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>

// declaração avançada para evitar dependência circular
typedef struct semaforo Semaforo;
typedef struct bcp BCP;

// Funções -------------------------------------------------------------------------------------------------------------

// interrupção gerada pela execução da função processCreate e pelo término de operações de E/S
void processInterrupt();

// tratamento de bloqueio de processo
void semaphoreP(Semaforo *semaph, BCP *proc);

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
void processFinish(BCP *bcp);

#endif //SIMULACRO_SO_EVENTOS_H