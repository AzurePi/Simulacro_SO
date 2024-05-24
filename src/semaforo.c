#include "include/semaforo.h"

Espera_BCP *novaEsperaBCP() {
    Espera_BCP *new = malloc(sizeof(Espera_BCP));
    if (!new) return NULL;
    new->processo = NULL;
    new->prox = NULL;
    return new;
}

Lista_Espera_BCP *novaListaEsperaBCP() {
    Lista_Espera_BCP *new = malloc(sizeof(Lista_Espera_BCP));
    if (!new) return NULL;
    new->head = novaEsperaBCP();
    if (!new->head) {
        free(new);
        return NULL;
    }
    new->tail = novaEsperaBCP();
    if (!new->tail) {
        free(new->head);
        free(new);
        return NULL;
    }
    return new;
}
// TODO: verificar se o semáforo existe; não faz sentido sempre criar um novo
Semaforo *novoSemaforo(char nome) {
    Semaforo *new = malloc(sizeof(Semaforo));
    if (!new) {
        sem_wait(&sem_terminal);
        printf(ERROR "falha ao alocar memória para o semáforo" CLEAR);
        sem_post(&sem_terminal);
        return NULL;
    }
    pthread_mutex_init(&new->mutex_lock, NULL);
    new->nome = nome;
    new->v = 0;
    new->refcount = 1;
    new->waiting_list = novaListaEsperaBCP();
    if (!new->waiting_list) {
        free(new);
        return NULL;
    }
    new->prox = NULL;
    insereSemaforo(new);
    return new;
}

void freeSemaforo(Semaforo *semaforo) {
    // freeListaEsperaBCP(semaforo->waiting_list);
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
        if (semaforo->nome == aux->nome) {
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

void freeListaSemaforo(Lista_Semaforos *semaforos) {
    if (!semaforos) return;
    Semaforo *temp;
    while (semaforos->head != NULL) {
        temp = semaforos->head;
        semaforos->head = semaforos->head->prox;
        freeSemaforo(temp);
    }
    free(semaforos);
}

Semaforo *retrieveSemaforo(char nome) {
    Semaforo *aux = semaforos_existentes->head;
    while (aux) {
        if (aux->nome == nome)
            return aux;
        aux = aux->prox;
    }
    sem_wait(&sem_terminal);
    printf(ERROR "busca de semáforo não pode ser concluida." CLEAR);
    sleep(2);
    sem_post(&sem_terminal);
    return NULL;
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

void sem_queue(Lista_Espera_BCP *lista, BCP *processo) {
    if (!lista || !processo) return; // se os parâmetros são ponteiros nulos

    Espera_BCP *new = malloc((sizeof(Espera_BCP)));
    if (!new) return;
    //TODO: falta colocar o processo aqui!
    new->processo = processo;
    new->prox = NULL;

    if (lista->head == NULL) {
        lista->head = new;
        lista->tail = new;
    } else {
        lista->tail->prox = new;
        lista->tail = new;
    }
}

