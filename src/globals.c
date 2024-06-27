#include "include/globals.h"

// Variáveis globais ---------------------------------------------------------------------------------------------------

volatile bool encerrar;
unsigned long int relogio;

BCP *head_lista_processos;
volatile BCP *executando_agora;
Lista_Semaforos *semaforos_existentes;
pthread_mutex_t mutex_lista_processos = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_semaforos_globais = PTHREAD_MUTEX_INITIALIZER;

Memoria *RAM;
pthread_mutex_t mutex_RAM = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t mutex_IO = PTHREAD_MUTEX_INITIALIZER;

FilaDisco *disk_queue;
bool disk_busy;
bool direction_up;
volatile int current_track;
pthread_mutex_t mutex_disk_queue = PTHREAD_MUTEX_INITIALIZER;

FilaTela *screen_queue;
pthread_mutex_t mutex_screen_queue = PTHREAD_MUTEX_INITIALIZER;

// Implementação de funções --------------------------------------------------------------------------------------------

void initializeGlobals() {
    encerrar = false;
    relogio = 0;

    head_lista_processos = NULL;
    executando_agora = NULL;
    semaforos_existentes = novaListaSemaforos();

    inicializarRAM();

    inicializarDiskQueue();
    disk_busy = false;
    direction_up = true;
    current_track = 0;

    inicializarScreenQueue();
}

void finalizeGlobals() {
    pthread_mutex_destroy(&mutex_screen_queue);
    pthread_mutex_destroy(&mutex_disk_queue);
    pthread_mutex_destroy(&mutex_IO);
    pthread_mutex_destroy(&mutex_RAM);
    pthread_mutex_destroy(&mutex_semaforos_globais);
    pthread_mutex_destroy(&mutex_lista_processos);

    freeRAM();
    freeListaSemaforo(semaforos_existentes);
    freeListaBCP(head_lista_processos);
    freeScreenQueue();
    freeDiskQueue();
}
