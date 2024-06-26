#include "include/disco.h"

void *discoElevador() {
    while (!encerrar) {
        DiskArgs *args = dequeue_disk();
        pthread_mutex_lock(&mutex_disk_queue);
        if (args) {
            usleep(10000000); // delay de 1s para simular o acesso
            sysCall(disk_finish, args);
        }
        pthread_mutex_unlock(&mutex_disk_queue);
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

void enqueue_disk(DiskArgs *args) {
    NoDisco *new_node = malloc(sizeof(NoDisco));
    new_node->args = args;
    new_node->next = NULL;

    pthread_mutex_lock(&mutex_disk_queue);
    if (!disk_queue->head) {
        disk_queue->head = new_node;
        disk_queue->tail = new_node;
        new_node->next = NULL;

        pthread_mutex_unlock(&mutex_disk_queue);
        return;
    }

    NoDisco *current = disk_queue->head;
    NoDisco *previous = NULL;

    while (current && ((direction_up && current->args->trilha < args->trilha) ||
        (!direction_up && current->args->trilha > args->trilha))) {
        previous = current;
        current = current->next;
    }

    if (previous) { previous->next = new_node; }
    else { disk_queue->head = new_node; }
    new_node->next = current;
    if (!current) { disk_queue->tail = new_node; }

    pthread_mutex_unlock(&mutex_disk_queue);

    //print_disk_queue();
}

DiskArgs *dequeue_disk() {
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
    }

    if (!selected) { // se não encontramos nenhum nó
        direction_up = !direction_up; // invertemos a direção da busca
        pthread_mutex_unlock(&mutex_disk_queue);
        return dequeue_disk(); // procuramos recursivamente
    }

    // removemos o nó selecionado da lista
    if (prev_selected) { prev_selected->next = selected->next; }
    else { disk_queue->head = selected->next; }

    if (!selected->next) { disk_queue->tail = prev_selected; }

    DiskArgs *args = selected->args;
    free(selected);
    pthread_mutex_unlock(&mutex_disk_queue);
    return args;
}

/*
void print_disk_queue() {
    pthread_mutex_lock(&mutex_disk_queue);
    const NoDisco *node = disk_queue->head;
    printf("Disk queue: ");
    while (node) {
        printf("(%d) -> ", node->args->trilha);
        node = node->next;
    }
    printf("NULL\n");
    pthread_mutex_unlock(&mutex_disk_queue);
}
*/