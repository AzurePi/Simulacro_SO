#ifndef SIMULACRO_SO_INTERFACE_H
#define SIMULACRO_SO_INTERFACE_H

#include "eventos.h"
#include "semaforo.h"
#include "sintetico.h"
#include "globals.h"

#include <stdio.h>
#include <malloc.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#ifdef _WIN32
#define CLEAR_SCREEN system("cls");
#else
#define CLEAR_SCREEN printf("\033[H\033[J"); fflush(stdout);
#endif

// Funções -------------------------------------------------------------------------------------------------------------

void *menu();

void *informacaoProcesso();

void *informacaoMemoria();

void showSemaphoreList();

void limpar_buffer();

#endif //SIMULACRO_SO_INTERFACE_H
