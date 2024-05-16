#ifndef SIMULACRO_SO_INTERFACE_H
#define SIMULACRO_SO_INTERFACE_H

#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <locale.h>
#include "eventos.h"
#include "memoria.h"

#ifdef _WIN32
#define CLEAR_SCREEN system("cls");
#else
#define CLEAR_SCREEN system("clear");
#endif

// Variáveis Globais ---------------------------------------------------------------------------------------------------

sem_t sem_terminal;

// Funções -------------------------------------------------------------------------------------------------------------

void *menu();

void *informacaoProcesso();

void *informacaoMemoria();

#endif //SIMULACRO_SO_INTERFACE_H
