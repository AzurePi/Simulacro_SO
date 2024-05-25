#include "include/eventos.h"

void processInterrupt() {
    // Bloqueia o acesso à lista de processos e à CPU
    pthread_mutex_lock(&mutex_lista_processos);
    pthread_mutex_lock(&mutex_CPU);

    // se há um processo rodando atualmente
    if (rodando_agora) {
        // atualiza o estado do processo para PRONTO
        rodando_agora->estado = PRONTO;

        // insere o processo de volta na lista de processos
        inserirBCP(rodando_agora);

        // libera a referência ao processo rodando agora
        rodando_agora = NULL;
    }

    // Libera os semáforos
    pthread_mutex_unlock(&mutex_CPU);
    pthread_mutex_unlock(&mutex_lista_processos);
}

bool semaphoreP(Semaforo *semaph, BCP *proc) {
    pthread_mutex_lock(&semaph->mutex_lock);
    semaph->v--;
    if (semaph->v <= 0) {
        sem_queue(semaph->waiting_list, proc);
        return false; // avisa que o processo precisa ser bloqueado
    }
    pthread_mutex_unlock(&semaph->mutex_lock);
    return true;
}

void semaphoreV(Semaforo *semaph) {
    pthread_mutex_lock(&semaph->mutex_lock);
    semaph->v++;
    if (semaph->v <= 0 && semaph->waiting_list->head != NULL) {
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

void memLoadReq(BCP *processo) {
    carregarPaginasNecessarias(processo);

    //TODO: ver se tem mais coisa pra fazer
}

void memLoadFinish() {
//TODO: fazer isso
}

void fsRequest() {}

void fsFinish() {}

void *processCreate() {
    processInterrupt();

    char filename[201];

    pthread_mutex_lock(&mutex_terminal);
    printf("Caminho do programa: ");
    scanf("%200[^\n]s", filename);
    limpar_buffer();
    pthread_mutex_unlock(&mutex_terminal);

    FILE *programa = fopen(filename, "r");
    if (programa) {
        BCP *processo = lerProgramaSintetico(programa);
        if (!processo) {
            pthread_mutex_lock(&mutex_terminal);
            printf(ERROR "não foi possível criar o processo com esse programa" CLEAR);
            sleep(2);
            pthread_mutex_unlock(&mutex_terminal);
        }
        inserirBCP(processo); // insere o BCP na lista de processos do sistema
    } else {
        pthread_mutex_lock(&mutex_terminal);
        printf(ERROR "arquivo do programa sintético não pôde ser aberto" CLEAR);
        sleep(2);
        pthread_mutex_unlock(&mutex_terminal);
    }
    fclose(programa);

    roundRobin();

    pthread_exit(NULL);
}

void processFinish(BCP *bcp) {
    bcp->estado = TERMINADO;
    rodando_agora = NULL;
}