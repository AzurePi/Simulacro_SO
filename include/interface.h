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

// Definições de Macros ------------------------------------------------------------------------------------------------
#ifdef _WIN32
#define CLEAR_SCREEN system("cls");
#else
#define CLEAR_SCREEN printf("\033[H\033[J"); fflush(stdout);
#endif

#define ERROR "\033[3;31m" "ERRO: "
#define CLEAR "\033[0m"
#define BOLD "\033[1m"
#define NOT_BOLD "\033[22m"
#define UNDERLINE "\033[4m"
#define NOT_UNDERLINE "\033[24m"


// Funções -------------------------------------------------------------------------------------------------------------

// Exibição do menu no terminal e acesso às operações do usuário
void *menu();

void *informacaoProcessos();

void *informacaoMemoria();

// Exibe uma lista de todos os semáforos presentes
void showSemaphoreList();

// Imprime uma mensagem de erro advinda da criação de um BCP
BCP *mensagemErroBCP(const char *mensagem, BCP *processo);

// Limpa o buffer de entrada do terminal
void limpar_buffer();

#endif //SIMULACRO_SO_INTERFACE_H
