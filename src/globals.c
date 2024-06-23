#include "include/globals.h"

// Variáveis globais ---------------------------------------------------------------------------------------------------

BCP *head_lista_processos;
volatile BCP *executando_agora;
Lista_Semaforos *semaforos_existentes;
Memoria *RAM;
volatile bool encerrar;
pthread_mutex_t mutex_IO;
pthread_mutex_t mutex_RAM;
pthread_mutex_t mutex_lista_processos;
pthread_mutex_t mutex_semaforos_globais;
unsigned long int relogio;

pthread_mutex_t mutex_disk_queue;
DiskQueue *disk_queue;
bool disk_busy;
int current_track;
bool direction_up;

// Implementação de funções --------------------------------------------------------------------------------------------

void initializeGlobals() {
    head_lista_processos = NULL;
    executando_agora = NULL;
    semaforos_existentes = novaListaSemaforos();
    inicializarRAM();
    encerrar = false;

    pthread_mutex_init(&mutex_disk_queue, NULL);
    disk_queue = newQueue();
    disk_busy = false;
    current_track = 0;
    direction_up = true;

    pthread_mutex_init(&mutex_IO, NULL);
    pthread_mutex_init(&mutex_RAM, NULL);
    pthread_mutex_init(&mutex_lista_processos, NULL);
    pthread_mutex_init(&mutex_semaforos_globais, NULL);
    relogio = 0;
}

void finalizeGlobals() {
    pthread_mutex_destroy(&mutex_semaforos_globais);
    pthread_mutex_destroy(&mutex_lista_processos);
    pthread_mutex_destroy(&mutex_RAM);
    pthread_mutex_destroy(&mutex_IO);
    freeRAM();
    freeListaSemaforo(semaforos_existentes);
    freeListaBCP(head_lista_processos);
}
