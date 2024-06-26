#include "include/syscalls.h"

void create_and_detach(void *(*func)(void *), void *args) {
    pthread_attr_t atrib;
    pthread_attr_init(&atrib);
    pthread_attr_setscope(&atrib, PTHREAD_SCOPE_SYSTEM);

    pthread_t thread;

    pthread_create(&thread, &atrib, func, args);
    pthread_detach(thread);
}

bool sysCall(const short func, void *args) {
    bool result = true; // usado para indicar o status da operação (geralmente, será true)

    switch (func) {
    case process_interrupt:
        create_and_detach(processInterrupt, args);
        break;
    case semaphore_P: {
        bool *res = malloc(sizeof(bool));

        pthread_attr_t a;
        pthread_attr_init(&a);
        pthread_attr_setscope(&a, PTHREAD_SCOPE_SYSTEM);

        pthread_t t;

        pthread_create(&t, &a, semaphoreP, args);
        pthread_join(t, (void **)res);
        result = *res; // indicamos se o processo precisa ou não ser bloqueado pelo semáforo
        free(res);
        pthread_attr_destroy(&a);
        break;
    }
    case semaphore_V:
        create_and_detach(semaphoreV, args);
        break;
    case disk_request:
        create_and_detach(DiskRequest, args);
        break;
    case disk_finish:
        create_and_detach(DiskFinish, args);
        break;
    case print_request:
        create_and_detach(PrintRequest, args);
        break;
    case print_finish:
        create_and_detach(PrintFinish, args);
        break;
    case mem_load_req:
        create_and_detach(memLoadReq, args);
        break;
    case mem_load_finish:
        create_and_detach(memLoadFinish, args);
        break;
    case fs_request:
        create_and_detach(fsRequest, args);
        break;
    case fs_finish:
        create_and_detach(fsFinish, args);
        break;
    case process_create:
        create_and_detach(processCreate, args);
        break;
    case process_finish:
        create_and_detach(processFinish, args);
        break;
    default:
        result = false;
        break;
    }
    return result;
}

void *processInterrupt(void *args) {
    const InterruptArgs *intArgs = (InterruptArgs *)args;
    const INTERRUPCAO tipo_interrupcao = intArgs->tipo_interrupcao;
    BCP *proc = intArgs->processo;

    switch (tipo_interrupcao) {
    case FINAL_EXECUCAO: { // interrupção pelo final da execução de um processo

        if (proc->comandos == NULL || proc->comandos->head == NULL) // se todos os comandos já foram executados
            sysCall(process_finish, proc); // finaliza o processo
        else { // se ainda há comandos para executar
            pthread_mutex_lock(&mutex_lista_processos);
            proc->estado = PRONTO; // atualiza o estado do processo atual para PRONTO
            pthread_mutex_unlock(&mutex_lista_processos);
        }
        pthread_mutex_lock(&mutex_lista_processos);
        executando_agora = NULL; // dizemos que nenhum processo está sendo executado agora
        pthread_mutex_unlock(&mutex_lista_processos);

        break;
    }
    case PROCESS_CREATE: { // interrupção pela criação de um novo processo
        inserirBCP(proc); // adiciona o novo processo na lista global de processos
        break;
    }
    case INICIO_E_S: {
        pthread_mutex_lock(&mutex_lista_processos);
        if (proc->estado != TERMINADO)
            proc->estado = BLOQUEADO; // bloqueia o processo
        pthread_mutex_unlock(&mutex_lista_processos);
        break;
    }
    case TERMINO_E_S: { // interrupção pelo término de uma operação de E/S
        pthread_mutex_lock(&mutex_lista_processos);
        if (proc->estado != TERMINADO)
            proc->estado = PRONTO; // libera o processo para ser executado
        pthread_mutex_unlock(&mutex_lista_processos);
        break;
    }
    default:
        puts(ERROR "Tipo de interrupção desconhecida\n" CLEAR);
        break;
    }
    return NULL;
}

void *semaphoreP(void *args_semaforo) {
    const SemaphorePArgs *sem_args = (SemaphorePArgs *)args_semaforo;
    Semaforo *semaforo = sem_args->semaforo;
    BCP *proc = sem_args->proc;

    pthread_mutex_lock(&semaforo->mutex_lock);
    semaforo->v--;
    if (semaforo->v < 0) {
        process_sleep(proc); // o processo é bloqueado
        sem_queue(semaforo->waiting_list, proc); // o processo é posto na lista de espera desse semáforo
        pthread_mutex_unlock(&semaforo->mutex_lock);

        bool *result = malloc(sizeof(bool));
        *result = false; // avisa que o processo foi bloqueado
        return result;
    }
    pthread_mutex_unlock(&semaforo->mutex_lock);
    bool *result = malloc(sizeof(bool));
    *result = true; // avisa que o processo pode prosseguir
    return result;
}

void *semaphoreV(void *args_semaforo) {
    Semaforo *semaph = args_semaforo;

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

void *DiskRequest(void *args) {
    DiskArgs *disk_args = args;
    current_track = disk_args->track;

    pthread_mutex_lock(&mutex_IO);
    //printf("Starting disk I/O on track %d\n", current_track);
    pthread_mutex_unlock(&mutex_IO);

    enqueue_disk(disk_args);

    return NULL;
}

void *DiskFinish(void *args) {
    pthread_mutex_lock(&mutex_IO);
    //printf("Finishing disk I/O on track %d\n", current_track);
    pthread_mutex_unlock(&mutex_IO);

    sysCall(fs_finish, args);

    return NULL;
}

noES *criaNo(BCP *processo) {
    noES *auxNo = malloc(sizeof(noES));
    auxNo->processo = processo;
    auxNo->prox = NULL;
    return auxNo;
}

void iniciaFila() {
    screen_queue = malloc(sizeof(filaES));
    screen_queue.head = NULL;
    screen_queue.tail = NULL;
}

void inserirFila(noES *noAdd) {
    if (!noAdd) return;

    if (!screen_queue->head) { //Não tem nada na fila
        screen_queue->tail = noAdd;
        screen_queue->head = noAdd;
        screen_queue->head->prox = NULL;
        screen_queue->tail->prox = NULL;
    }
    else if (screen_queue->tail == screen_queue->head) { // Só tem 1 item
        screen_queue->head->prox = screen_queue->tail;
        screen_queue->head = noAdd;
    }
    else { //Mais de 1 - Tail só vai mudar quando retirar ex: 1 - 2 - 3 - 4
        screen_queue->head->prox = screen_queue->head; //Recebe o que tava na cabeça. Ex: 5 -1 - 2 -3 -4
        screen_queue->head = noAdd;
    }
}

void removeFila() {
    filaES *aux;
    filaES *anterior;
    if (!screen_queue) return;
    
    aux = screen_queue.head;
    while(aux.prox != NULL){
        anterior = aux;
        aux = aux.prox;
    }
    //encontrou o tail
    anterior.prox = NULL;
    screen_queue.tail = anterior;
    free(aux);
}

void *PrintRequest(void *args) {
    // interrupção por início de E/S
    BCP *processo = args;
    InterruptArgs *interrupcao = malloc(sizeof(InterruptArgs));
    interrupcao->tipo_interrupcao = INICIO_E_S;
    interrupcao->processo = processo;

    sysCall(process_interrupt, interrupcao);

    noES *no = criaNo(processo);
    inserirFila(no);

    return NULL;
}

void *PrintFinish(void *args) {
    // interrupção por fim de E/S
    BCP *processo = args;
    InterruptArgs *interrupcao = malloc(sizeof(InterruptArgs));
    interrupcao->tipo_interrupcao = TERMINO_E_S;
    interrupcao->processo = processo;

    sysCall(process_interrupt, interrupcao);

    removeFila();
    
}

void *memLoadReq(void *args) {
    BCP *processo = args;
    carregarPaginasNecessarias(processo);
    return NULL;
}

void *memLoadFinish(void *args) {
    BCP *processo = args;
    descarregarPaginas(processo);
    return NULL;
}

void *fsRequest(void *args) {
    DiskArgs *disk_args = args;

    // interrupção por início de E/S
    InterruptArgs *interrupt_args = malloc(sizeof(InterruptArgs));
    interrupt_args->tipo_interrupcao = INICIO_E_S;
    interrupt_args->processo = disk_args->processo;

    sysCall(process_interrupt, interrupt_args);

    // requer a operação de disco
    sysCall(disk_request, disk_args);

    return NULL;
}

void *fsFinish(void *args) {
    DiskArgs *disk_args = args;

    // interrupção por término de E/S
    InterruptArgs *interrupt_args = malloc(sizeof(InterruptArgs));
    interrupt_args->tipo_interrupcao = TERMINO_E_S;
    interrupt_args->processo = disk_args->processo;

    sysCall(process_interrupt, interrupt_args);
    free(args);
    return NULL;
}

void *processCreate(void *filename) {
    char *arquivo = filename;
    FILE *programa = fopen(arquivo, "r");

    if (programa) {
        BCP *processo = lerProgramaSintetico(programa);
        fclose(programa);
        if (processo) {
            InterruptArgs *args = malloc(sizeof(InterruptArgs));
            args->tipo_interrupcao = PROCESS_CREATE;
            args->processo = processo;

            sysCall(process_interrupt, args);
        }
        else {
            pthread_mutex_lock(&mutex_IO);
            printf(ERROR "não foi possível criar o processo do programa %s" CLEAR, arquivo);
            fflush(stdout);
            sleep(2);
            pthread_mutex_unlock(&mutex_IO);
        }
    }
    else {
        pthread_mutex_lock(&mutex_IO);
        printf(ERROR "arquivo %s do programa sintético não pôde ser aberto" CLEAR, arquivo);
        fflush(stdout);
        sleep(2);
        pthread_mutex_unlock(&mutex_IO);
    }
    free(arquivo);

    return NULL;
}

void *processFinish(void *args_BCP) {
    BCP *process = args_BCP;

    pthread_mutex_lock(&mutex_lista_processos);
    process->estado = TERMINADO;
    pthread_mutex_unlock(&mutex_lista_processos);

    sysCall(mem_load_finish, process); // descarrega o processo da memória

    // percorre a lista de semáforos associada ao processo
    pthread_mutex_lock(&mutex_semaforos_globais); // bloqueia acesso à lista de semáforos
    const No_Semaforo *no_sem = process->semaforos->head;
    while (no_sem) {
        const No_Semaforo *next = no_sem->prox;

        pthread_mutex_lock(&no_sem->semaforo->mutex_lock); // bloqueia acesso a esse semáforo
        no_sem->semaforo->refcount--; // decrementa a quantidade de referências
        pthread_mutex_unlock(&no_sem->semaforo->mutex_lock); // desbloqueia o acesso a esse semáforo

        /* TODO: algo aqui não funciona direito para deletar os semáforos
        // se não há mais processos associados ao semáforo
        if (no_sem->semaforo->refcount == 0) {
            pthread_mutex_unlock(&no_sem->semaforo->mutex_lock); // desbloqueia o acesso a esse semáforo
            pthread_mutex_unlock(
                    &mutex_semaforos_globais); // desbloqueia o acesso à lista de semáforos (para evitar deadlock)
            removeSemaforoGlobal(no_sem); // remove o nó do semáforo da lista global
            pthread_mutex_lock(
                    &mutex_semaforos_globais); // re-bloqueia o acesso à lista de semáforos para continuar o loop
        }
        pthread_mutex_unlock(&no_sem->semaforo->mutex_lock); // desbloqueia o acesso a esse semáforo
        */

        no_sem = next;
    }
    pthread_mutex_unlock(&mutex_semaforos_globais); // desbloqueia o acesso à lista de semáforos

    return NULL;
}
