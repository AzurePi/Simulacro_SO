#include "include/syscalls.h"

bool sysCall(short op, void *args) {
    pthread_attr_t atrib;
    pthread_t t_operacao;

    pthread_attr_init(&atrib);
    pthread_attr_setscope(&atrib, PTHREAD_SCOPE_SYSTEM);

    bool result = true; // presumimos sucesso

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
    InterruptArgs *intArgs = (InterruptArgs *) args;
    INTERRUPCAO tipo_interrupcao = intArgs->tipo_interrupcao;
    BCP *proc = intArgs->processo;

    switch (tipo_interrupcao) {
        case FINAL_EXECUCAO: { // interrupção pelo final do quantum time
            pthread_mutex_lock(&mutex_lista_processos);

            if (proc->comandos->head == NULL) // se todos os comandos já foram executados
                sysCall(process_finish, proc); // finaliza o processo
            else
                proc->estado = PRONTO; // atualiza o estado do processo atual para PRONTO

            executando_agora = NULL;

            pthread_mutex_unlock(&mutex_lista_processos);
            break;
        }
        case PROCESS_CREATE: { // interrupção pela criação de um novo processo
            inserirBCP(proc); // adiciona o novo processo na lista global de processos
            break;
        }
        case TERMINO_E_S: { // interrupção pelo término de uma operação de E/S
            pthread_mutex_lock(&mutex_lista_processos);
            proc->estado = PRONTO; // atualiza o estado do processo para PRONTO
            pthread_mutex_unlock(&mutex_lista_processos);
            break;
        }
        default:
            // Tipo de interrupção desconhecida
            printf(ERROR "Tipo de interrupção desconhecida\n" CLEAR);
            break;
    }
    return NULL;
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
    if (semaph->v >= 0 && semaph->waiting_list->head != NULL) {
        Espera_BCP *acordar = semaph->waiting_list->head;
        BCP *proc = acordar->processo;
        semaph->waiting_list->head = acordar->prox;

        if (semaph->waiting_list->head == NULL)
            semaph->waiting_list->tail = NULL;

        free(acordar);
        process_wakeup(proc);
    }
    pthread_mutex_unlock(&semaph->mutex_lock);
    return NULL;
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
    char *arquivo = (char *) filename;
    FILE *programa = fopen(arquivo, "r");

    if (programa) {
        BCP *processo = lerProgramaSintetico(programa);
        fclose(programa);
        if (processo) {
            // Chama sysCall para process_interrupt, especificando o tipo de interrupção PROCESS_CREATE
            InterruptArgs *args = malloc(sizeof(InterruptArgs));
            args->tipo_interrupcao = PROCESS_CREATE;
            args->processo = processo;

            sysCall(process_interrupt, args);
        } else {
            pthread_mutex_lock(&mutex_IO);
            printf(ERROR "não foi possível criar o processo do programa %s" CLEAR, arquivo);
            fflush(stdout);
            sleep(2);
            pthread_mutex_unlock(&mutex_IO);
        }
    } else {
        pthread_mutex_lock(&mutex_IO);
        printf(ERROR "arquivo %s do programa sintético não pôde ser aberto" CLEAR, arquivo);
        fflush(stdout);
        sleep(2);
        pthread_mutex_unlock(&mutex_IO);
    }
    free(arquivo);

    return NULL;
}

void *processFinish(void *args) {
    pthread_mutex_lock(&mutex_lista_processos);
    BCP *process = (BCP *) args;
    process->estado = TERMINADO;
    if (process == executando_agora)
        executando_agora = NULL;

    // percorre a lista de semáforos associada ao processo
    Semaforo *sem = process->head_semaforos;
    while (sem != NULL) {
        sem->refcount--;
        sem = sem->prox;
    }
    pthread_mutex_unlock(&mutex_lista_processos);
    return NULL;
}