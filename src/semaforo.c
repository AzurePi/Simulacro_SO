#include "include/semaforo.h"

Espera_BCP *novaEsperaBCP() {
    Espera_BCP *new = malloc(sizeof(Espera_BCP));
    if (!new) return NULL;
    new->processo = NULL;
    new->prox = NULL;
    return new;
}

Fila_Espera_BCP *novaListaEsperaBCP() {
    Fila_Espera_BCP *new = malloc(sizeof(Fila_Espera_BCP));
    if (!new) return NULL;
    new->head = novaEsperaBCP();
    if (!new->head) {
        free(new);
        return NULL;
    }
    new->tail = new->head;
    return new;
}

void freeListaEsperaBCP(Fila_Espera_BCP *lista) {
    Espera_BCP *aux = lista->head;

    while (aux != NULL) {
        Espera_BCP *prox = aux->prox;
        free(aux);
        aux = prox;
    }

    // após liberar todos os nós, limpa os ponteiros da lista
    lista->head = NULL;
    lista->tail = NULL;

    free(lista); // libera a própria lista
}

Semaforo *novoSemaforo(char nome) {
    Semaforo *new = retrieveSemaforo(nome); // se o semáforo já existe
    if (new) {
        new->refcount++; // aumentamos a contagem de referências
        return new;
    }

    new = malloc(sizeof(Semaforo)); // criamos um semáforo novo
    if (!new) {
        pthread_mutex_lock(&mutex_IO);
        puts(ERROR "falha ao alocar memória para o semáforo" CLEAR);
        fflush(stdout);
        pthread_mutex_unlock(&mutex_IO);
        return NULL;
    }
    pthread_mutex_init(&new->mutex_lock, NULL);
    new->nome = nome;
    new->v = 1;
    new->refcount = 1;
    new->waiting_list = novaListaEsperaBCP();
    if (!new->waiting_list) {
        free(new);
        return NULL;
    }
    inserirSemaforoGlobal(new);
    return new;
}

void freeSemaforo(Semaforo *semaforo) {
    if (!semaforo) return;
    if (semaforo->waiting_list)
        freeListaEsperaBCP(semaforo->waiting_list);
    pthread_mutex_destroy(&semaforo->mutex_lock);
    free(semaforo);
}

No_Semaforo *novoNoSemaforo(Semaforo *sem) {
    if (!sem) return NULL;
    No_Semaforo *new = malloc(sizeof(No_Semaforo));
    if (!new) {
        pthread_mutex_lock(&mutex_IO);
        puts(ERROR "falha ao alocar memória para o semáforo" CLEAR);
        fflush(stdout);
        pthread_mutex_unlock(&mutex_IO);
        return NULL;
    }
    new->semaforo = sem;
    new->prox = NULL;
    return new;
}

void freeNoSemaforo(No_Semaforo *no_semaforo) {
    if (!no_semaforo) return;
    no_semaforo->semaforo = NULL;
    no_semaforo->prox = NULL;
    free(no_semaforo);
}

Lista_Semaforos *novaListaSemaforos() {
    Lista_Semaforos *new = malloc(sizeof(Lista_Semaforos));
    if (!new) {
        pthread_mutex_lock(&mutex_IO);
        puts(ERROR "falha ao alocar memória para a lista de semáforos" CLEAR);
        sleep(2);
        pthread_mutex_unlock(&mutex_IO);
        return NULL;
    }
    new->head = NULL;
    return new;
}

void inserirSemaforo(Lista_Semaforos *lista, Semaforo *semaforo) {
    if (!lista || !semaforo) return;

    // verificamos se já existe um semáforo com esse nome na lista
    No_Semaforo *aux = lista->head;
    while (aux) {
        if (aux->semaforo->nome == semaforo->nome) {
            pthread_mutex_lock(&aux->semaforo->mutex_lock);
            aux->semaforo->refcount++;
            pthread_mutex_unlock(&aux->semaforo->mutex_lock);
            return;
        }
        aux = aux->prox;
    }

    // se não existe, criamos um nó de semáforo novo
    No_Semaforo *new = novoNoSemaforo(semaforo);
    if (!new) return;

    if (lista->head == NULL)
        lista->head = new; // se a lista está vazia, inserimos no começo
    else {
        aux = lista->head;
        while (aux->prox)
            aux = aux->prox;
        aux->prox = new;
    }
}

void inserirSemaforoGlobal(Semaforo *semaforo) {
    if (!semaforo) return;

    pthread_mutex_lock(&mutex_semaforos_globais);
    inserirSemaforo(semaforos_existentes, semaforo);
    pthread_mutex_unlock(&mutex_semaforos_globais);
}

void freeListaSemaforo(Lista_Semaforos *semaforos) {
    if (!semaforos) return;
    while (semaforos->head != NULL) {
        No_Semaforo *temp = semaforos->head;
        semaforos->head = semaforos->head->prox;
        freeNoSemaforo(temp);
    }
    free(semaforos);
}

Semaforo *retrieveSemaforo(const char nome) {
    pthread_mutex_lock(&mutex_semaforos_globais);
    const No_Semaforo *aux = semaforos_existentes->head;
    while (aux) {
        if (aux->semaforo->nome == nome) {
            pthread_mutex_unlock(&mutex_semaforos_globais);
            return aux->semaforo;
        }
        aux = aux->prox;
    }
    pthread_mutex_unlock(&mutex_semaforos_globais);
    return NULL;
}

void removeSemaforoGlobal(No_Semaforo *semaforo) {
    if (!semaforo) return;

    pthread_mutex_lock(&mutex_semaforos_globais);
    No_Semaforo *aux = semaforos_existentes->head;
    No_Semaforo *prev = NULL;

    while (aux && aux != semaforo) {
        prev = aux;
        aux = aux->prox;
    }

    if (aux) {
        if (prev)
            prev->prox = aux->prox;
        else
            semaforos_existentes->head = aux->prox;
        freeNoSemaforo(aux);
    }
    pthread_mutex_unlock(&mutex_semaforos_globais);
}

void sem_queue(Fila_Espera_BCP *lista, BCP *processo) {
    if (!lista || !processo) return; // se os parâmetros são ponteiros nulos

    Espera_BCP *new = malloc(sizeof(Espera_BCP));
    if (!new) return; // se a alocação de memória falhou

    new->processo = processo;
    new->prox = NULL;

    if (lista->head == NULL) {
        lista->head = new;
        lista->tail = new;
    }
    else {
        lista->tail->prox = new;
        lista->tail = new;
    }
}
