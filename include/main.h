#ifndef SIMULACRO_SO_MAIN_H
#define SIMULACRO_SO_MAIN_H

#include "semaforo.h"
#include "interface.h"
#include "globals.h"

#include <malloc.h>
#include <stdbool.h>
#include <locale.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>

void *kernel();

#endif //SIMULACRO_SO_MAIN_H