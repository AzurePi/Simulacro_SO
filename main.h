#ifndef SIMULACRO_SO_MAIN_H
#define SIMULACRO_SO_MAIN_H

#include <locale.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include "memoria.h"
#include "eventos.h"

#ifdef _WIN32
#define CLEAR_SCREEN system("cls")
#else
#define CLEAR_SCREEN system("clear")
#endif

void *menu();

void *informacaoProcesso();

void *informacaoMemoria();

void *kernel();

#endif //SIMULACRO_SO_MAIN_H
