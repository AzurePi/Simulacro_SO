#include "semaforo.h"

void insert_semaphore(semaphore_t *item) {
    if (existing_semaphores.head == NULL) {
        existing_semaphores.head = item;
        return;
    }
    semaphore_t *aux, *prev = NULL;
    for (aux = existing_semaphores.head; aux; prev = aux, aux = aux->next) {
        //semaphore by this name already exists
        if (item->name == aux->name) {
            aux->refcount++;
            free(item);
            return;
        }
    }
    item->next = aux;
    if (prev)
        prev->next = item;
    else
        existing_semaphores.head = item;
}

void createSemaphore(char name) {
    semaphore_t *new = malloc(sizeof(semaphore_t));
    new->next = NULL;
    new->name = name;
    new->refcount = 1;
    semaphore_init(new, 0);
    insert_semaphore(new);
}

void removeSemaphore(semaphore_t *item) {
    semaphore_t *aux = existing_semaphores.head, *prev = NULL;
    for (; aux && aux != item; prev = aux, aux = aux->next);
    if (aux) {
        if (prev)
            prev->next = aux->next;
        else //aux is the current head
            existing_semaphores.head = existing_semaphores.head->next;
        free(aux);
    }
}

semaphore_t *retrieveSemaphore(char name) {
    semaphore_t *aux = existing_semaphores.head;
    while (aux) {
        if (aux->name == name)
            return aux;
        aux = aux->next;
    }
    printf("algo deu errado.\n");
    sleep(1);
    return NULL;
}

void sem_queue(sem_list_item_t **list, BCP *proc) {
    sem_list_item_t *new = malloc(sizeof(sem_list_item_t));
    new->next = NULL;
    new->proc = proc;

    if (*list == NULL) {
        *list = new;
        return;
    }
    sem_list_item_t *aux, *prev = NULL;
    for (aux = *list; aux; prev = aux, aux = aux->next);
    new->next = aux;
    if (prev)
        prev->next = new;
    else
        (*list) = new;
}

void showSemaphoreList() {
    sem_wait(&sem);
    CLEAR_SCREEN
    printf("lista de semáforos existentes:\n");
    semaphore_t *aux = existing_semaphores.head;
    if (!aux) {
        printf("Nenhum.\n");
        sleep(3);
        sem_post(&sem);
        return;
    }
    while (aux) {
        printf("%c | Contador: %d\n", aux->name, aux->refcount);
        aux = aux->next;
    }
    sleep(3);
    sem_post(&sem);
}
