#ifndef SIMULACRO_SO_GLOBALS_H
#define SIMULACRO_SO_GLOBALS_H

#include "processo.h"
#include "semaforo.h"
#include "memoria.h"
#include "disco.h"

#include <pthread.h>
#include <stdbool.h>

// declarações avançadas para evitar declaração circular
typedef struct bcp BCP;
typedef struct lista_semaforos Lista_Semaforos;
typedef struct memoria Memoria;
typedef struct disk_queue DiskQueue;
typedef struct filaES fila;

// Variáveis Globais ---------------------------------------------------------------------------------------------------

extern volatile bool encerrar; // variável de controle da execução do simulador
extern BCP *head_lista_processos; // cabeça da lista de todos os processos do simulador
extern volatile BCP *executando_agora; // ponteiro para o processo atualmente em execução
extern Lista_Semaforos *semaforos_existentes; // cabeça da lista global de todos os semáforos do simulador
extern Memoria *RAM; // simulação da memória
extern DiskQueue *disk_queue; //fila de entrada e saída do disco
extern pthread_mutex_t mutex_IO; // mutex para acesso ao terminal
extern pthread_mutex_t mutex_RAM; // mutex para acesso à memória simulada
extern pthread_mutex_t mutex_lista_processos; // mutex para acesso à lista global de processos
extern pthread_mutex_t mutex_semaforos_globais; // mutex para acesso à lista global de semáforos
extern pthread_mutex_t mutex_disk_queue; // mutex para a fila de acesso ao disco
extern bool disk_busy; // se algum processo está acessando o disco ou ele está livre
extern volatile int current_track; // trilha atual do braço do disco
extern bool direction_up; // direção inicial do braço do disco
extern unsigned long int relogio; // contador de tempo de execução simulada
extern InterruptArgs interrompe;

// Protótipos de Funções -----------------------------------------------------------------------------------------------

// Inicialização das variáveis globais.
void initializeGlobals();

// Finalização e liberação das variáveis globais.
void finalizeGlobals();

#endif //SIMULACRO_SO_GLOBALS_H
