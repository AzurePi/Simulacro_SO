#ifndef SIMULACRO_SO_INTERFACE_H
#define SIMULACRO_SO_INTERFACE_H

#include "syscalls.h"
#include "semaforo.h"
#include "processo.h"
#include "globals.h"
#include "memoria.h"

#include <ncurses.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

// Definições de Macros ------------------------------------------------------------------------------------------------

// repetição dos macros de memoria.h

#define TAMANHO_PAGINA 8 // 8 unidades de memória (1 um = 1 kbyte)
#define TAMANHO_MEMORIA (1024 * 1024) // 1 GB de memória (1024 * 1024 kbytes = 1024 * 1024 um)
#define NUMERO_PAGINAS (TAMANHO_MEMORIA / TAMANHO_PAGINA)

// Funções -------------------------------------------------------------------------------------------------------------

// Inicializa a interface do ncurses e delimita as janelas
void initializeInterface();

// Desenha as bordas de uma janela
void draw_borders(WINDOW *win);

// Exibição do menu no terminal e acesso às operações do usuário.
void * menu();

// Exibe a infromação do procesos sendo executado nesse instante
void *processandoAgora();

// Exibe uma lista de todos os processos no sistema.
void *informacaoProcessos();

// Exibe uma lista de todos os semáforos no sistema.
void *informacaoSemaforos();

// Calcula e exibir a taxa de ocupação da memória.
void *informacaoMemoria();

// Imprime uma mensagem de erro advinda da criação de um BCP.
BCP *mensagemErroBCP(const char *mensagem, BCP *processo);

#endif //SIMULACRO_SO_INTERFACE_H
