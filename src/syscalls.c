#include "include/syscalls.h"

bool sysCall(int op, void *args) {
    pthread_attr_t atrib;
    pthread_t t_operacao;

    pthread_attr_init(&atrib);
    pthread_attr_setscope(&atrib, PTHREAD_SCOPE_SYSTEM);

    bool result = true; // presumimos sucesso, a menos que haja uma falha específica

    switch (op) {
        case process_interrupt:
            pthread_create(&t_operacao, &atrib, processInterrupt, args);
            pthread_detach(t_operacao);
            break;
        case semaphore_P: {
            bool *res = (bool *) malloc(sizeof(bool));
            pthread_create(&t_operacao, &atrib, semaphoreP, args);
            pthread_join(t_operacao, (void **) res);
            result = *res;
            free(res);
            break;
        }
        case semaphore_V:
            pthread_create(&t_operacao, &atrib, semaphoreV, args);
            pthread_detach(t_operacao);
            break;
        case disk_request:
            pthread_create(&t_operacao, &atrib, DiskRequest, args);
            pthread_detach(t_operacao);
            break;
        case disk_finish:
            pthread_create(&t_operacao, &atrib, DiskFinish, args);
            pthread_detach(t_operacao);
            break;
        case print_request:
            pthread_create(&t_operacao, &atrib, PrintRequest, args);
            pthread_detach(t_operacao);
            break;
        case print_finish:
            pthread_create(&t_operacao, &atrib, PrintFinish, args);
            pthread_detach(t_operacao);
            break;
        case mem_load_req:
            pthread_create(&t_operacao, &atrib, memLoadReq, args);
            pthread_detach(t_operacao);
            break;
        case mem_load_finish:
            pthread_create(&t_operacao, &atrib, memLoadFinish, args);
            pthread_detach(t_operacao);
            break;
        case fs_request:
            pthread_create(&t_operacao, &atrib, fsRequest, args);
            pthread_detach(t_operacao);
            break;
        case fs_finish:
            pthread_create(&t_operacao, &atrib, fsFinish, args);
            pthread_detach(t_operacao);
            break;
        case process_create:
            pthread_create(&t_operacao, &atrib, processCreate, args);
            pthread_detach(t_operacao);
            break;
        case process_finish:
            pthread_create(&t_operacao, &atrib, processFinish, args);
            pthread_detach(t_operacao);
            break;
        default:
            result = false;
            break;
    }

    pthread_attr_destroy(&atrib);
    return result;
}

void *processInterrupt(void *args) {
    pthread_mutex_lock(&mutex_lista_processos); // bloqueia o acesso à lista de processos

    // se há um processo rodando atualmente
    if (executando_agora) {
        executando_agora->estado = PRONTO; // atualiza o estado do processo para PRONTO

        inserirBCP(executando_agora); // insere o processo de volta na lista de processos

        executando_agora = NULL; // libera a referência ao processo rodando agora
    }

    pthread_mutex_unlock(&mutex_lista_processos); // libera o acesso à lista de processos
}

void *semaphoreP(void *args) {
    SemaphorePArgs *sem_args = (SemaphorePArgs *) args;
    Semaforo *semaforo = sem_args->semaforo;
    BCP *proc = sem_args->proc;

    pthread_mutex_lock(&semaforo->mutex_lock);
    semaforo->v--;
    if (semaforo->v < 0) {
        process_sleep(proc); // o processo é bloqueado
        sem_queue(semaforo->waiting_list, proc); // o processo é posto na lista de espera desse semáforo
        pthread_mutex_unlock(&semaforo->mutex_lock);

        bool *result = (bool *) malloc(sizeof(bool));
        *result = false; // avisa que o processo foi bloqueado
        return result;
    }
    pthread_mutex_unlock(&semaforo->mutex_lock);
    bool *result = (bool *) malloc(sizeof(bool));
    *result = true; // avisa que o processo pode prosseguir
    return result;
}

void *semaphoreV(void *semaforo) {
    Semaforo *semaph = (Semaforo *) semaforo;

    pthread_mutex_lock(&semaph->mutex_lock);
    semaph->v++;
    if (semaph->v >= 0 && semaph->waiting_list->head != NULL) { //TODO: verificar esse v < 0
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

void *DiskRequest(void *args) { return NULL; }

void *DiskFinish(void *args) { return NULL; }

void *PrintRequest(void *args) { return NULL; }

void *PrintFinish(void *args) { return NULL; }

void *memLoadReq(void *args) {
    BCP *processo = (BCP *) args;
    carregarPaginasNecessarias(processo);
    return NULL;
}

void *memLoadFinish(void *args) {
    BCP *processo = (BCP *) args;
    descarregarPaginas(processo);
    return NULL;
}

void *fsRequest(void *args) { return NULL; }

void *fsFinish(void *args) { return NULL; }

void *processCreate(void *filename) {
    sysCall(process_interrupt, NULL);

    char *arquivo = (char *) filename;
    FILE *programa = fopen(arquivo, "r");

    if (programa) {
        BCP *processo = lerProgramaSintetico(programa);
        if (!processo) {
            pthread_mutex_lock(&mutex_IO);
            printf(ERROR "não foi possível criar o processo o programa %s" CLEAR, arquivo);
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
    free(arquivo);

    return NULL;
}

void *processFinish(void *args) {
    BCP *process = (BCP *) args;
    process->estado = TERMINADO;
    executando_agora = NULL;
}