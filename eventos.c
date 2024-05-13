#include <stdio.h>
#include <semaphore.h>
#include "eventos.h"
#include "sintetico.h"
#include "main.h"

void processInterrupt() {}

void semaphoreP(int v) {}

void semaphoreV(int v) {}

void DiskRequest() {}

void DiskFinish() {}

void PrintRequest() {}

void PrintFinish() {}

void memLoadReq() {}

void memLoadFinish() {}

void fsRequest() {}

void fsFinish() {}

void * processCreate() {
    char filename[51];
    sem_wait(&sem_terminal);
    printf("Nome do programa: ");
    scanf(" %108[^\n]", filename);
    sem_post(&sem_terminal);

    FILE *programa = fopen(filename, "r");
    if (programa)
        BCP_From_Sintetico(programa);
    else
        printf("ERRO: %s não pode ser aberto\n", filename);
    fclose(programa);
}

void processFinish() {}
