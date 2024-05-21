#include "include/semaforo.h"

Semaforo *novoSemaforo(char name) {
    Semaforo *new = malloc(sizeof(Semaforo));
    if (!new) {
        sem_wait(&sem_terminal);
        printf(ERROR "falha ao alocar memória para o semáforo" CLEAR);
        sem_post(&sem_terminal);
        return NULL;
    }
    pthread_mutex_init(&new->mutex_lock, NULL);
    new->name = name;
    new->v = 0;
    new->refcount = 1;
    new->waiting_list = NULL;
    new->prox = NULL;
    insereSemaforo(new);
    return new;
}

void freeSemaforo(Semaforo *semaforo) {
    freeListaBCP(semaforo->waiting_list);
    pthread_mutex_destroy(&semaforo->mutex_lock);
    free(semaforo);
}

Lista_Semaforos *novaListaSemaforos() {
    Lista_Semaforos *new = malloc(sizeof(Lista_Semaforos));
    if (!new) {
        sem_wait(&sem_terminal);
        printf(ERROR "falha ao alocar memória para a lista de semáforos" CLEAR);
        sleep(2);
        sem_post(&sem_terminal);
        return NULL;
    }
    new->head = NULL;
    return new;
}

void freeListaSemaforo(Lista_Semaforos *semaforos) {
    Semaforo *temp;
    while (semaforos->head != NULL) {
        temp = semaforos->head;
        semaforos->head = semaforos->head->prox;
        freeSemaforo(temp);
    }
    free(semaforos);
}

void insereSemaforo(Semaforo *semaforo) {
    if (semaforos_existentes == NULL)
        semaforos_existentes = novaListaSemaforos();

    if (semaforos_existentes->head == NULL) {
        semaforos_existentes->head = semaforo;
        return;
    }

    // percorremos a lista de semáforos existente até o final
    Semaforo *aux, *prev = NULL;
    for (aux = semaforos_existentes->head; aux != NULL; prev = aux, aux = aux->prox) {
        // se um semáforo com esse nome já existe, o apagamos e paramos
        if (semaforo->name == aux->name) {
            aux->refcount++;
            free(semaforo);
            return;
        }
    }

    // inserirmos o semáforo novo no final da lista
    semaforo->prox = aux;
    if (prev)
        prev->prox = semaforo;
    else
        semaforos_existentes->head = semaforo;
}

void removeSemaforo(Semaforo *semaforo) {
    Semaforo *aux = semaforos_existentes->head, *prev = NULL;
    for (; aux != NULL && aux != semaforo; prev = aux, aux = aux->prox);
    if (aux) {
        if (prev)
            prev->prox = aux->prox;
        else //aux é a head atual
            semaforos_existentes->head = semaforos_existentes->head->prox;
        free(aux);
    }
}

Semaforo *retrieveSemaphore(char name) {
    Semaforo *aux = semaforos_existentes->head;
    while (aux) {
        if (aux->name == name)
            return aux;
        aux = aux->prox;
    }
    sem_wait(&sem_terminal);
    printf(ERROR "busca de semáforo não pode ser concluida." CLEAR);
    sleep(2);
    sem_post(&sem_terminal);
    return NULL;
}

void sem_queue(Lista_Espera_BCP **list, BCP *proc) {
    Lista_Espera_BCP *new = malloc(sizeof(Lista_Espera_BCP));
    new->prox = NULL;
    new->proc = proc;

    if (*list == NULL) {
        *list = new;
        return;
    }
    Lista_Espera_BCP *aux, *prev = NULL;
    for (aux = *list; aux; prev = aux, aux = aux->prox);
    new->prox = aux;
    if (prev)
        prev->prox = new;
    else
        (*list) = new;
}

