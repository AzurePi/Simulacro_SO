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
            pthread_join(t, (void **) res);
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
    const InterruptArgs *intArgs = (InterruptArgs*)args;
    const INTERRUPCAO tipo_interrupcao = intArgs->tipo_interrupcao;
    BCP *proc = intArgs->processo;

    switch (tipo_interrupcao) {
    case FINAL_EXECUCAO: {
        // interrupção pelo final da execução de um processo

        if (proc->comandos == NULL || proc->comandos->head == NULL) // se todos os comandos já foram executados
            sysCall(process_finish, proc); // finaliza o processo
        else {
            // se ainda há comandos para executar
            pthread_mutex_lock(&mutex_lista_processos);
            proc->estado = PRONTO; // atualiza o estado do processo atual para PRONTO
            pthread_mutex_unlock(&mutex_lista_processos);
        }
        pthread_mutex_lock(&mutex_lista_processos);
        executando_agora = NULL; // dizemos que nenhum processo está sendo executado agora
        pthread_mutex_unlock(&mutex_lista_processos);

        break;
    }
    case PROCESS_CREATE: {
        // interrupção pela criação de um novo processo
        inserirBCP(proc); // adiciona o novo processo na lista global de processos
        break;
    }
    case TERMINO_E_S: {
        // interrupção pelo término de uma operação de E/S
        pthread_mutex_lock(&mutex_lista_processos);
        proc->estado = PRONTO; // atualiza o estado do processo para PRONTO
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
    const SemaphorePArgs *sem_args = (SemaphorePArgs*)args_semaforo;
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

//Chamada para inicio E/S
void *DiskRequest(void *args) { 
    current_track = args->track;
    printf("Starting disk I/O on track %d\n", args->track);
    
    DiskArgs* diskArgs = (DiskArgs*)args;

    pthread_mutex_lock(&mutex_disk_queue);
    enqueue_disk(&disk_queue, diskArgs);
    if (!disk_busy) {
        disk_busy = true;
        DiskArgs* next_request = dequeue_disk(&disk_queue);
        start_disk_io(next_request);
        free(next_request);
    }
    pthread_mutex_unlock(&mutex_disk_queue);

    return NULL;
}

//Finalização da E/S
void *DiskFinish(void *args) { 
    printf("Finishing disk I/O on track %d\n", current_track);
    pthread_mutex_lock(&mutex_disk_queue);
    if (disk_queue.head) {
        DiskArgs* next_request = dequeue_disk(&disk_queue);
        start_disk_io(next_request);
        free(next_request);
    } else {
        disk_busy = false;
    }
    pthread_mutex_unlock(&mutex_disk_queue);

    return NULL;
}

void print_disk_queue(DiskQueue* queue){
    DiskNode* node = queue->head;
    printf("Disk queue: ");
    while (node) {
        printf("(%d) -> ", node->args->track);
        node = node->next;
    }
    printf("NULL\n");
}

//Adiciona na fila de E/S pelo algoritmo Elevator
void enqueue_disk(DiskQueue* queue, DiskArgs* args){
    DiskNode* new_node = (DiskNode*)malloc(sizeof(DiskNode));
    new_node->args = args;
    new_node->next = NULL;

    if (!queue->head) {
        queue->head = queue->tail = new_node;
    } else {
        DiskNode* current = queue->head;
        DiskNode* previous = NULL;

        while (current && ((direction_up && current->args->track < args->track) || 
                           (!direction_up && current->args->track > args->track))) {
            previous = current;
            current = current->next;
        }

        if (previous) {
            previous->next = new_node;
        } else {
            queue->head = new_node;
        }
        new_node->next = current;
        if (!current) {
            queue->tail = new_node;
        }
    }

    print_disk_queue(queue);
}

//Remove a E/S que vai ser executada e escolhe a próxima
DiskArgs* dequeue_disk(DiskQueue* queue){
    if (!queue->head) return NULL;
    
        DiskNode* selected = NULL;
        DiskNode* current = queue->head;
        DiskNode* previous = NULL;
        DiskNode* prev_selected = NULL;
    
        while (current) {
            if ((direction_up && current->args->track >= current_track) || 
                (!direction_up && current->args->track <= current_track)) {
                if (!selected || 
                    (direction_up && current->args->track < selected->args->track) ||
                    (!direction_up && current->args->track > selected->args->track)) {
                    selected = current;
                    prev_selected = previous;
                }
            }
            previous = current;
            current = current->next;
        }
    
        if (!selected) {
            direction_up = !direction_up;
            return dequeue_disk(queue);
        }
    
        if (prev_selected) {
            prev_selected->next = selected->next;
        } else {
            queue->head = selected->next;
        }
        if (!selected->next) {
            queue->tail = prev_selected;
        }
    
        DiskArgs* args = selected->args;
        free(selected);
        return args;
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

void *fsRequest(void *args) { return NULL; }

void *fsFinish(void *args) { return NULL; }

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
