#include "include/globals.h"

// Variáveis globais ---------------------------------------------------------------------------------------------------

volatile bool encerrar;
unsigned long int relogio;

BCP *head_lista_processos;
volatile BCP *executando_agora;
Lista_Semaforos *semaforos_existentes;
pthread_mutex_t mutex_lista_processos;
pthread_mutex_t mutex_semaforos_globais;

Memoria *RAM;
pthread_mutex_t mutex_RAM;

pthread_mutex_t mutex_IO;

FilaDisco *disk_queue;
bool disk_busy;
bool direction_up;
volatile int current_track;
pthread_mutex_t mutex_disk_queue;

FilaTela *screen_queue;
pthread_mutex_t mutex_screen_queue;

// Implementação de funções --------------------------------------------------------------------------------------------

void initializeGlobals() {
    encerrar = false;
    relogio = 0;

    head_lista_processos = NULL;
    executando_agora = NULL;
    semaforos_existentes = novaListaSemaforos();
    pthread_mutex_init(&mutex_lista_processos, NULL);
    pthread_mutex_init(&mutex_semaforos_globais, NULL);

    inicializarRAM();
    pthread_mutex_init(&mutex_RAM, NULL);

    pthread_mutex_init(&mutex_IO, NULL);

    inicializarDiskQueue();
    disk_busy = false;
    direction_up = true;
    current_track = 0;
    pthread_mutex_init(&mutex_disk_queue, NULL);

    inicializarScreenQueue();
    pthread_mutex_init(&mutex_screen_queue, NULL);
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
}
