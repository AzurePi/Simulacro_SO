#ifndef SIMULACRO_SO_GLOBALS_H
#define SIMULACRO_SO_GLOBALS_H

#include "semaforo.h"

#include <semaphore.h>
#include <pthread.h>

// declarações avançadas para evitar declaração circular
typedef struct bcp BCP;
typedef struct lista_semaforos Lista_Semaforos;
typedef struct memoria Memoria;

// variáveis globais
extern BCP *head_lista_processos;
extern BCP *rodando_agora;
extern Lista_Semaforos *semaforos_existentes;
extern Memoria *RAM;
extern pthread_mutex_t mutex_terminal;
extern pthread_mutex_t mutex_CPU;
extern pthread_mutex_t mutex_RAM;
extern pthread_mutex_t mutex_lista_processos;
extern volatile long double relogio;
extern pthread_attr_t atrib;

#endif //SIMULACRO_SO_GLOBALS_H