#include "include/disco.h"

void *disco() {
    while (!encerrar) {}
    return NULL;
}

void *newQueue() {
    DiskQueue *new = malloc(sizeof(DiskQueue));
    if (!new) return NULL;

    new->head = NULL;
    new->tail = NULL;

    return new;
}

void enqueue_disk(DiskQueue *queue, DiskArgs *args) {
    DiskNode *new_node = malloc(sizeof(DiskNode));
    new_node->args = args;
    new_node->next = NULL;

    pthread_mutex_lock(&mutex_disk_queue);
    if (!queue->head) { queue->head = queue->tail = new_node; }
    else {
        DiskNode *current = queue->head;
        DiskNode *previous = NULL;

        while (current && ((direction_up && current->args->track < args->track) ||
            (!direction_up && current->args->track > args->track))) {
            previous = current;
            current = current->next;
        }

        if (previous) { previous->next = new_node; }
        else { queue->head = new_node; }
        new_node->next = current;
        if (!current) { queue->tail = new_node; }
    }
    pthread_mutex_unlock(&mutex_disk_queue);

    print_disk_queue(queue);
}

DiskArgs *dequeue_disk(DiskQueue *queue) {
    pthread_mutex_lock(&mutex_disk_queue);
    if (!queue->head) {
        pthread_mutex_unlock(&mutex_disk_queue);
        return NULL;
    }

    DiskNode *selected = NULL;
    DiskNode *current = queue->head;
    DiskNode *previous = NULL;
    DiskNode *prev_selected = NULL;

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
        pthread_mutex_unlock(&mutex_disk_queue);
        return dequeue_disk(queue);
    }

    if (prev_selected) { prev_selected->next = selected->next; }
    else { queue->head = selected->next; }
    if (!selected->next) { queue->tail = prev_selected; }

    DiskArgs *args = selected->args;
    free(selected);
    pthread_mutex_unlock(&mutex_disk_queue);
    return args;
}

void print_disk_queue(DiskQueue *queue) {
    const DiskNode *node = queue->head;
    printf("Disk queue: ");
    while (node) {
        printf("(%d) -> ", node->args->track);
        node = node->next;
    }
    printf("NULL\n");
}
