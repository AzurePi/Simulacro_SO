#include "include/globals.h"

volatile BCP *rodando_agora;
volatile int num_processos;
Lista_Semaforos *semaforos_existentes;
sem_t sem_terminal;
volatile long double relogio;