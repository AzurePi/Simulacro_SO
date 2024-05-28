#include "include/syscalls.h"

void processInterrupt() {
    pthread_mutex_lock(&mutex_lista_processos); // bloqueia o acesso à lista de processos

    // se há um processo rodando atualmente
    if (executando_agora) {
        executando_agora->estado = PRONTO; // atualiza o estado do processo para PRONTO

        inserirBCP(executando_agora); // insere o processo de volta na lista de processos

        executando_agora = NULL; // libera a referência ao processo rodando agora
    }

    pthread_mutex_unlock(&mutex_lista_processos); // libera o acesso à lista de processos
}

bool semaphoreP(Semaforo *semaph, BCP *proc) {
    pthread_mutex_lock(&semaph->mutex_lock);
    semaph->v--;
    if (semaph->v <= 0) {
        process_sleep(proc); // o processo é bloqueado
        sem_queue(semaph->waiting_list, proc); // o processo é posto na lista de espera desse semáforo
        return false; // avisa que o processo foi bloqueado
    }
    pthread_mutex_unlock(&semaph->mutex_lock);
    return true; // avisa que o processo pode prosseguir

}

void semaphoreV(Semaforo *semaph) {
    pthread_mutex_lock(&semaph->mutex_lock);
    semaph->v++;
    if (semaph->v <= 0 && semaph->waiting_list->head != NULL) { //TODO: verificar esse v < 0
        Espera_BCP *acordar = semaph->waiting_list->head;
        BCP *proc = acordar->processo;
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

void memLoadReq(BCP *processo) { carregarPaginasNecessarias(processo); }

void memLoadFinish(BCP *processo) { descarregarPaginas(processo); }

void fsRequest() {}

void fsFinish() {}

void *processCreate(void *arg) {
    processInterrupt();

    char *filename = (char *) arg;
    FILE *programa = fopen(filename, "r");

    if (programa) {
        BCP *processo = lerProgramaSintetico(programa);
        if (!processo) {
            pthread_mutex_lock(&mutex_IO);
            printf(ERROR "não foi possível criar o processo o programa %s" CLEAR, filename);
            sleep(2);
            pthread_mutex_unlock(&mutex_IO);
        }
        inserirBCP(processo); // insere o BCP na lista de processos do sistema
    } else {
        pthread_mutex_lock(&mutex_IO);
        printf(ERROR "arquivo do programa sintético não pôde ser aberto" CLEAR);
        sleep(2);
        pthread_mutex_unlock(&mutex_IO);
    }
    fclose(programa);
    free(filename);

    return NULL;
}

void processFinish(BCP *bcp) {
    bcp->estado = TERMINADO;
    executando_agora = NULL;
}