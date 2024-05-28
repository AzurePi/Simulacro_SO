#ifndef SIMULACRO_SO_CPU_H
#define SIMULACRO_SO_CPU_H

#include "globals.h"
#include "interface.h"
#include "semaforo.h"

#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <locale.h>
#include <stdbool.h>
#include <malloc.h>

#define QUANTUM 5000.0

_Noreturn void *roundRobin();

void processarComandos(BCP *processo);

#endif //SIMULACRO_SO_CPU_H