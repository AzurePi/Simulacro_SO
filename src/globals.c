#include "include/globals.h"

BCP *head_lista_processos;
BCP *rodando_agora;
Lista_Semaforos *semaforos_existentes;
Memoria *RAM;
pthread_mutex_t mutex_terminal;
pthread_mutex_t mutex_CPU;
pthread_mutex_t mutex_RAM;
pthread_mutex_t mutex_lista_processos;
volatile long double relogio;
pthread_attr_t atrib;
