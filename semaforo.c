#include "semaforo.h"

Semaforo *createSemaphore(char name) {
    Semaforo *new = malloc(sizeof(Semaforo));
    if (!new) {
        printf("ERRO: falha ao alocar memória para o semáforo");
        return NULL;
    }

    pthread_mutex_init(&new->mutex_lock, NULL);
    new->name = name;
    new->v = 0;
    new->refcount = 1;
    new->waiting_list = NULL;
    new->next = NULL;
    insert_semaphore(new);
    return new;
}

void insert_semaphore(Semaforo *semaforo) {
    if (existing_semaphores.head == NULL) {
        existing_semaphores.head = semaforo;
        return;
    }

    // percorremos a lista de semáforos existente até o final
    Semaforo *aux, *prev = NULL;
    for (aux = existing_semaphores.head; aux != NULL; prev = aux, aux = aux->next) {
        // se um semáforo com esse nome já existe, o apagamos e paramos
        if (semaforo->name == aux->name) {
            aux->refcount++;
            free(semaforo);
            return;
        }
    }

    // inserirmos o semáforo novo no final da lista
    semaforo->next = aux;
    if (prev)
        prev->next = semaforo;
    else
        existing_semaphores.head = semaforo;
}

void removeSemaphore(Semaforo *semaforo) {
    Semaforo *aux = existing_semaphores.head, *prev = NULL;
    for (; aux != NULL && aux != semaforo; prev = aux, aux = aux->next);
    if (aux) {
        if (prev)
            prev->next = aux->next;
        else //aux é a head atual
            existing_semaphores.head = existing_semaphores.head->next;
        free(aux);
    }
}

Semaforo *retrieveSemaphore(char name) {
    Semaforo *aux = existing_semaphores.head;
    while (aux) {
        if (aux->name == name)
            return aux;
        aux = aux->next;
    }
    printf("ERRO: busca de semáforo não pode ser concluida.\n");
    sleep(1);
    return NULL;
}

void sem_queue(Lista_Espera_BCP **list, BCP *proc) {
    Lista_Espera_BCP *new = malloc(sizeof(Lista_Espera_BCP));
    new->next = NULL;
    new->proc = proc;

    if (*list == NULL) {
        *list = new;
        return;
    }
    Lista_Espera_BCP *aux, *prev = NULL;
    for (aux = *list; aux; prev = aux, aux = aux->next);
    new->next = aux;
    if (prev)
        prev->next = new;
    else
        (*list) = new;
}

void showSemaphoreList() {
    sem_wait(&sem_terminal);
    CLEAR_SCREEN
    printf("lista de semáforos existentes:\n");
    Semaforo *aux = existing_semaphores.head;
    if (!aux) {
        printf("Nenhum.\n");
        sleep(3);
        sem_post(&sem_terminal);
        return;
    }
    while (aux) {
        printf("%c | Contador: %d\n", aux->name, aux->refcount);
        aux = aux->next;
    }
    sleep(3);
    sem_post(&sem_terminal);
}
