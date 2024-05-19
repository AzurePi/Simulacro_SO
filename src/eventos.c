#include "include/eventos.h"
#include "include/interface.h"

void processInterrupt() {

}

void semaphoreP(Semaforo *semaph, BCP *proc) {
    pthread_mutex_lock(&semaph->mutex_lock);
    if (semaph->v < 0) {
        //sem_queue(&semaph->waiting_list, proc);
        process_sleep(proc);
    }
    semaph->v--;
    pthread_mutex_unlock(&semaph->mutex_lock);
}

void semaphoreV(Semaforo *semaph) {
    pthread_mutex_lock(&semaph->mutex_lock);
    semaph->v++;
    if (semaph->v <= 0 && semaph->waiting_list) {
        BCP *proc = semaph->waiting_list->prox;
        semaph->waiting_list = semaph->waiting_list->prox;
        process_wakeup(proc);
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

    char filename[201];
    sem_wait(&sem_terminal);
    printf("Caminho do programa: ");
    scanf("%200[^\n]", filename);
    limpar_buffer();
    sem_post(&sem_terminal);

    FILE *programa = fopen(filename, "r");
    if (programa) {
        BCP *processo = lerProgramaSintetico(programa);
        if (!processo) {
            sem_wait(&sem_terminal);
            printf("ERRO: não foi possível criar o processo com esse programa");
            sleep(2);
            sem_post(&sem_terminal);
        }

        //TODO: coloca no escalonamento

    } else {
        sem_wait(&sem_terminal);
        printf("ERRO: arquivo do programa sintético não pôde ser aberto");
        sleep(2);
        sem_post(&sem_terminal);
    }
    fclose(programa);
}

void processFinish() {

}
