#include "include/globals.h"

BCP *lista_processos;
BCP *rodando_agora;
volatile int num_processos;
Lista_Semaforos *semaforos_existentes;
sem_t sem_terminal;
sem_t sem_CPU;
sem_t sem_lista_processos;
volatile long double relogio;
pthread_attr_t atrib;