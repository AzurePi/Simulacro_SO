#ifndef SIMULACRO_SO_INTERFACE_H
#define SIMULACRO_SO_INTERFACE_H

#include "syscalls.h"
#include "semaforo.h"
#include "processo.h"
#include "globals.h"
#include "memoria.h"

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

// Definições de Macros ------------------------------------------------------------------------------------------------

// definição dependente do sistema operacional
#ifdef _WIN32

#define CLEAR_SCREEN() system("cls")

#include <conio.h>

#else

#define CLEAR_SCREEN() printf("\033[H\033[J")

#include <termios.h>

#endif

// definições de códigos ANSI para customização do terminal

#define CLEAR "\033[0m\33[48;5;242m"
#define BOLD "\033[1m"
#define FAINT "\033[2m"
#define ITALIC "\033[3m"
#define UNDERLINE "\033[4m"
#define NOT_BOLD_NOR_FAINT "\033[22m"
#define NOT_ITALIC "\033[23m"
#define NOT_UNDERLINE "\033[24m"

#define RED "\033[38;5;52m"
#define BLUE "\033[38;5;20m"
#define BRIGHT_YELLOW "\033[93m"

#define ERROR ITALIC RED "\a\tERRO: "
#define INPUT ITALIC BRIGHT_YELLOW

// repetição dos macros de memoria.h

#define TAMANHO_PAGINA 8 // 8 unidades de memória (1 um = 1 kbyte)
#define TAMANHO_MEMORIA (1024 * 1024) // 1 GB de memória (1024 * 1024 kbytes = 1024 * 1024 um)
#define NUMERO_PAGINAS (TAMANHO_MEMORIA / TAMANHO_PAGINA)

// Funções -------------------------------------------------------------------------------------------------------------

// Exibição do interface no terminal e acesso às operações do usuário.
void *interface();

// Exibe uma lista de todos os processos no sistema.
void informacaoProcessos();

// Calcula e exibir a taxa de ocupação da memória.
void informacaoMemoria();

// Imprime uma mensagem de erro advinda da criação de um BCP.
BCP *mensagemErroBCP(const char *mensagem, BCP *processo);

// Limpa o buffer de entrada do terminal.
void limpar_buffer();

// Espera o input de qualquer tecla para continuar.
void press_any_key_to_continue();

#endif //SIMULACRO_SO_INTERFACE_H
