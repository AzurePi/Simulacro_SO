#ifndef SIMULACRO_SO_GLOBALS_H
#define SIMULACRO_SO_GLOBALS_H

#include "semaforo.h"

#include <semaphore.h>
#include <pthread.h>

#define QUANTUM 5000.0

// declarações avançadas para evitar declaração circular
typedef struct bcp BCP;
typedef struct lista_semaforos Lista_Semaforos;

// variáveis globais
extern BCP *lista_processos;
extern BCP *rodando_agora;
extern volatile int num_processos;
extern Lista_Semaforos *semaforos_existentes;
extern sem_t sem_terminal;
extern sem_t sem_CPU;
extern sem_t sem_lista_processos;
extern volatile long double relogio;
extern pthread_attr_t atrib;

#endif //SIMULACRO_SO_GLOBALS_H
