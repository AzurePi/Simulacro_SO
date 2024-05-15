#include <stdio.h>
#include "eventos.h"

void sem_queue(struct sem_li **pLi, BCP *pBcp);

void processInterrupt() {}

void semaphoreP(semaphore_t *semaph, BCP *proc) {
    pthread_mutex_lock(&semaph->mutex_lock);
    if (semaph->v < 0) {
        sem_queue(&semaph->waiting_list, proc);
        proc_sleep(proc);
    }
    semaph->v--;
    pthread_mutex_unlock(&semaph->mutex_lock);
}

void semaphoreV(semaphore_t *semaph) {
    pthread_mutex_lock(&semaph->mutex_lock);
    semaph->v++;
    if (semaph->v <= 0) {
        if (semaph->waiting_list) {
            BCP *proc = semaph->waiting_list->proc;
            semaph->waiting_list = semaph->waiting_list->next;
            proc_wakeup(proc);
        }
    }
    pthread_mutex_unlock(&semaph->mutex_lock);
}

void DiskRequest() {}

void DiskFinish() {}

void PrintRequest() {}

void PrintFinish() {}

void memLoadReq() {}

void memLoadFinish() {}

void fsRequest() {}

void fsFinish() {}

void *processCreate() {
    processInterrupt();

    char filename[51];
    sem_wait(&sem_terminal);
    printf("Nome do programa: ");
    scanf(" %108[^\n]", filename);
    sem_post(&sem_terminal);

    FILE *programa = fopen(filename, "r");
    if (programa) {
        BCP *processo = BCP_From_Sintetico(programa);
        if (!processo)
            printf("ERRO: não foi possível criar o processo");
    } else
        printf("ERRO: %s não pode ser aberto\n", filename);
    fclose(programa);

    //TODO: coloca no escalonamento
}

void processFinish() {}
