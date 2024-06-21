#ifndef SIMULACRO_SO_CPU_H
#define SIMULACRO_SO_CPU_H

#include "globals.h"
#include "interface.h"
#include "semaforo.h"

#include <pthread.h>
#include <unistd.h>
#include <locale.h>
#include <stdbool.h>
#include <malloc.h>

#define QUANTUM 5000

// Protótipos de Funções -----------------------------------------------------------------------------------------------

// Permanece em execução até o encerramento do programa, procurando o próximo processo a executar
void *CPU();

// Processa os comandos de um dado processo com um limite de tempo proporcional a sua prioridade.
void processarComandos(BCP *processo);

#endif //SIMULACRO_SO_CPU_H
