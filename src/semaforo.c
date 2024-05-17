#include "include/semaforo.h"

Semaforo *novoSemaforo(char name) {
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
    insereSemaforo(new);
    return new;
}

Lista_Semaforos *novaListaSemaforos() {
    Lista_Semaforos *new = malloc(sizeof(Lista_Semaforos));
    if (!new) {
        printf("ERRO: falha ao alocar memória para a lista de semáforos");
        return NULL;
    }
    new->head = NULL;
    return new;
}

void insereSemaforo(Semaforo *semaforo) {
    if (semaforos_existentes == NULL) {
        semaforos_existentes = novaListaSemaforos();
    }

    if (semaforos_existentes->head == NULL) {
        semaforos_existentes->head = semaforo;
        return;
    }

    // percorremos a lista de semáforos existente até o final
    Semaforo *aux, *prev = NULL;
    for (aux = semaforos_existentes->head; aux != NULL; prev = aux, aux = aux->next) {
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
        semaforos_existentes->head = semaforo;
}

void removeSemaforo(Semaforo *semaforo) {
    Semaforo *aux = semaforos_existentes->head, *prev = NULL;
    for (; aux != NULL && aux != semaforo; prev = aux, aux = aux->next);
    if (aux) {
        if (prev)
            prev->next = aux->next;
        else //aux é a head atual
            semaforos_existentes->head = semaforos_existentes->head->next;
        free(aux);
    }
}

Semaforo *retrieveSemaphore(char name) {
    Semaforo *aux = semaforos_existentes->head;
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

