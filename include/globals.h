#ifndef SIMULACRO_SO_GLOBALS_H
#define SIMULACRO_SO_GLOBALS_H

#include "processo.h"
#include "semaforo.h"
#include "memoria.h"

#include <pthread.h>

// declarações avançadas para evitar declaração circular
typedef struct bcp BCP;
typedef struct lista_semaforos Lista_Semaforos;
typedef struct memoria Memoria;

// Variáveis globais ---------------------------------------------------------------------------------------------------
extern BCP *head_lista_processos;
extern BCP *executando_agora;
extern Lista_Semaforos *semaforos_existentes;
extern Memoria *RAM;
extern pthread_mutex_t mutex_IO;
extern pthread_mutex_t mutex_CPU;
extern pthread_mutex_t mutex_RAM;
extern pthread_mutex_t mutex_lista_processos;
extern volatile long double relogio;

// Protótipos de Funções -----------------------------------------------------------------------------------------------

// Inicialização das variáveis globais
void initializeGlobals();

#endif //SIMULACRO_SO_GLOBALS_H