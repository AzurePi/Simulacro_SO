#ifndef SIMULACRO_SO_GLOBALS_H
#define SIMULACRO_SO_GLOBALS_H

#include "semaforo.h"

#include <semaphore.h>

// declarações avançadas
typedef struct bcp BCP;
typedef struct lista_semaforos Lista_Semaforos;

// variáveis globais
extern volatile BCP *rodando_agora;
extern volatile int num_processos;
extern Lista_Semaforos *semaforos_existentes;
extern sem_t sem_terminal;
extern volatile long double relogio;

#endif //SIMULACRO_SO_GLOBALS_H
