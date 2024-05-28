#include "include/globals.h"

BCP *head_lista_processos;
BCP *executando_agora;
Lista_Semaforos *semaforos_existentes;
Memoria *RAM;
pthread_mutex_t mutex_IO;
pthread_mutex_t mutex_CPU;
pthread_mutex_t mutex_RAM;
pthread_mutex_t mutex_lista_processos;
volatile long double relogio;

void initializeGlobals() {
    head_lista_processos = NULL;
    executando_agora = NULL;
    semaforos_existentes = novaListaSemaforos();
    inicializarRAM();
    pthread_mutex_init(&mutex_IO, NULL);
    pthread_mutex_init(&mutex_CPU, NULL);
    pthread_mutex_init(&mutex_RAM, NULL);
    pthread_mutex_init(&mutex_lista_processos, NULL);
    relogio = 0.0;
}