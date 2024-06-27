#include "include/disco.h"

void *discoElevador() {
    while (!encerrar) {
        DiskArgs *args = dequeue_disk(0);
        if (args) {
            relogio += args->t; // passamos o tempo calculado para busca da trilha no disco
            sysCall(disk_finish, args);
            pthread_mutex_unlock(&mutex_lista_processos); // desbloqueia o acesso à lista de processos
        }
        sleep(3); // delay de 3s
    }
    return NULL;
}

void inicializarDiskQueue() {
    if (disk_queue)
        return;

    disk_queue = malloc(sizeof(FilaDisco));
    if (!disk_queue) { return; }

    disk_queue->head = NULL;
    disk_queue->tail = NULL;
}

void freeDiskQueue() {
    if (!disk_queue)
        return;

    pthread_mutex_lock(&mutex_disk_queue);

    NoDisco *no = disk_queue->head;
    while (no) {
        NoDisco *temp = no;
        no = no->next;
        free(temp);
    }

    pthread_mutex_unlock(&mutex_disk_queue);

    free(disk_queue);
}

void enqueue_disk(DiskArgs *args) {
    NoDisco *new_node = malloc(sizeof(NoDisco));
    if (!new_node)
        return;

    new_node->args = args;
    new_node->next = NULL;

    pthread_mutex_lock(&mutex_disk_queue);
    if (!disk_queue->head) {
        disk_queue->head = new_node;
        disk_queue->tail = new_node;
    }
    else {
        NoDisco *current = disk_queue->head;
        NoDisco *previous = NULL;

        while (current && ((direction_up && current->args->trilha < args->trilha) ||
            (!direction_up && current->args->trilha > args->trilha))) {
            previous = current;
            current = current->next;
        }

        if (previous)
            previous->next = new_node;
        else
            disk_queue->head = new_node;
        new_node->next = current;
        if (!current)
            disk_queue->tail = new_node;
    }
    pthread_mutex_unlock(&mutex_disk_queue);
}

DiskArgs *dequeue_disk(int t) {
    if (!disk_queue)
        return NULL;

    pthread_mutex_lock(&mutex_disk_queue);
    if (!disk_queue->head) {
        pthread_mutex_unlock(&mutex_disk_queue);
        return NULL;
    }

    NoDisco *selected = NULL;
    NoDisco *aux = disk_queue->head;
    NoDisco *prev = NULL;
    NoDisco *prev_selected = NULL;

    // busca o nó mais próximo da trilha atual na direção do elevador
    while (aux) {
        if ((direction_up && aux->args->trilha >= current_track) ||
            (!direction_up && aux->args->trilha <= current_track)) {
            if (!selected ||
                (direction_up && aux->args->trilha < selected->args->trilha) ||
                (!direction_up && aux->args->trilha > selected->args->trilha)) {
                selected = aux;
                prev_selected = prev;
            }
        }
        prev = aux;
        aux = aux->next;
        t += 200; //assumimos um tempo para cada passo da busca
    }

    if (!selected) { // se não encontramos nenhum nó
        direction_up = !direction_up; // invertemos a direção da busca
        return dequeue_disk(t); // procuramos recursivamente
    }

    // removemos o nó selecionado da lista
    if (prev_selected)
        prev_selected->next = selected->next;
    else
        disk_queue->head = selected->next;

    if (!selected->next)
        disk_queue->tail = prev_selected;

    DiskArgs *args = selected->args;
    args->t = t;
    free(selected);

    pthread_mutex_unlock(&mutex_disk_queue);
    return args;
}
