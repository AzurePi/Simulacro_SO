#include "include/eventos.h"

void processInterrupt() {
    // TODO: fazer algo aqui
}

void semaphoreP(Semaforo *semaph, BCP *proc) {
    pthread_mutex_lock(&semaph->mutex_lock);
    semaph->v--;
    if (semaph->v <= 0) {
        sem_queue(semaph->waiting_list, proc);
        process_sleep(proc);
    }
    pthread_mutex_unlock(&semaph->mutex_lock);
}

void semaphoreV(Semaforo *semaph) {
    pthread_mutex_lock(&semaph->mutex_lock);
    semaph->v++;
    if (semaph->v <= 0 && semaph->waiting_list->head != NULL) {
        Espera_BCP *acordar = semaph->waiting_list->head;
        BCP *proc = acordar->bcp;
        semaph->waiting_list->head = acordar->prox;

        if (semaph->waiting_list->head == NULL)
            semaph->waiting_list->tail = NULL;

        free(acordar);
        process_wakeup(proc);
    }
    pthread_mutex_unlock(&semaph->mutex_lock);
}

void DiskRequest() {}

void DiskFinish() {}

void PrintRequest() {}

void PrintFinish() {}

void memLoadReq() {
//TODO; fazer isso
}

void memLoadFinish() {
//TODO: fazer isso
}

void fsRequest() {}

void fsFinish() {}

void *processCreate() {
    processInterrupt();

    char filename[201];
    sem_wait(&sem_terminal);
    printf("Caminho do programa: ");
    scanf("%200[^\n]s", filename);
    limpar_buffer();
    sem_post(&sem_terminal);

    FILE *programa = fopen(filename, "r");
    if (programa) {
        BCP *processo = lerProgramaSintetico(programa);
        if (!processo) {
            sem_wait(&sem_terminal);
            printf(ERROR "não foi possível criar o processo com esse programa" CLEAR);
            sleep(2);
            sem_post(&sem_terminal);
        }
        inserirBCP(processo); // insere o BCP na lista do escalonador
        // TODO: fazer os lances de memória
    } else {
        sem_wait(&sem_terminal);
        printf(ERROR "arquivo do programa sintético não pôde ser aberto" CLEAR);
        sleep(2);
        sem_post(&sem_terminal);
    }
    fclose(programa);
}

void processFinish(BCP *bcp) {
    bcp->estado = TERMINADO;
    //TODO; mais coisa aqui
}