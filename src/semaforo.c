#include "include/semaforo.h"

void freeSemaforoGlobal(Semaforo_Global *semaforoG);

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
    Espera_BCP *prox;

    while (aux != NULL) {
        prox = aux->prox;
        free(aux);
        aux = prox;
    }

    // após liberar todos os nós, limpa os ponteiros da lista
    lista->head = NULL;
    lista->tail = NULL;

    // libera a própria lista
    free(lista);
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
        printf(ERROR "falha ao alocar memória para o semáforo" CLEAR);
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
    new->prox = NULL;
    inserirSemaforoGlobal(new);
    return new;
}

void freeSemaforo(Semaforo *semaforo) {
    if (semaforo->refcount > 0) return;
    if (semaforo->waiting_list)
        freeListaEsperaBCP(semaforo->waiting_list);
    pthread_mutex_destroy(&semaforo->mutex_lock);
    free(semaforo);
}

Semaforo_Global *novoSemaforoGlobal(Semaforo *sem) {
    Semaforo_Global *new = malloc(sizeof(Semaforo_Global));
    if (!new) {
        pthread_mutex_lock(&mutex_IO);
        printf(ERROR "falha ao alocar memória para o semáforo" CLEAR);
        fflush(stdout);
        pthread_mutex_unlock(&mutex_IO);
        return NULL;
    }
    new->semaforo = sem;
    new->prox = NULL;
    return new;
}

void freeSemaforoGlobal(Semaforo_Global *semaforoG) {
    freeSemaforo(semaforoG->semaforo);
    free(semaforoG);
}

Lista_Semaforos_Global *novaListaSemaforos() {
    Lista_Semaforos_Global *new = malloc(sizeof(Lista_Semaforos_Global));
    if (!new) {
        pthread_mutex_lock(&mutex_IO);
        printf(ERROR "falha ao alocar memória para a lista de semáforos" CLEAR);
        sleep(2);
        pthread_mutex_unlock(&mutex_IO);
        return NULL;
    }
    new->head = NULL;
    return new;
}

void inserirSemaforoGlobal(Semaforo *semaforo) {
    Semaforo_Global *new  = novoSemaforoGlobal(semaforo);

    pthread_mutex_lock(&mutex_lista_semaforos);
    if (semaforos_existentes == NULL)
        semaforos_existentes = novaListaSemaforos();

    if (semaforos_existentes->head == NULL) {
        new->prox = NULL;
        semaforos_existentes->head = new;
        pthread_mutex_unlock(&mutex_lista_semaforos);
        return;
    }

    // percorremos a lista de semáforos existente até o final
    Semaforo_Global *aux = semaforos_existentes->head;
    while (aux->prox) {
        // Se um semáforo com esse nome já existe, liberamos a memória do semáforo global sendo inserido
        if (new->semaforo == aux->semaforo) {
            freeSemaforoGlobal(new);
            pthread_mutex_unlock(&mutex_lista_semaforos);
            return;
        }
        aux = aux->prox;
    }

    // Inserimos o semáforo no final da lista
    aux->prox = new;
    new->prox = NULL;
    pthread_mutex_unlock(&mutex_lista_semaforos);
}

void freeListaSemaforo(Lista_Semaforos_Global *semaforos) {
    if (!semaforos) return;
    Semaforo_Global *temp;
    while (semaforos->head != NULL) {
        temp = semaforos->head;
        semaforos->head = semaforos->head->prox;
        freeSemaforoGlobal(temp);
    }
    free(semaforos);
}

Semaforo *retrieveSemaforo(char nome) {
    pthread_mutex_lock(&mutex_lista_semaforos);
    Semaforo_Global *aux = semaforos_existentes->head;
    while (aux) {
        if (aux->semaforo->nome == nome) {
            pthread_mutex_unlock(&mutex_lista_semaforos);
            return aux->semaforo;
        }
        aux = aux->prox;
    }
    pthread_mutex_unlock(&mutex_lista_semaforos);
    return NULL;
}

void removeSemaforo(Semaforo *semaforo) {
    pthread_mutex_lock(&mutex_lista_semaforos);
    Semaforo_Global *aux = semaforos_existentes->head, *prev = NULL;
    for (; aux != NULL && aux->semaforo != semaforo; prev = aux, aux = aux->prox);
    if (aux) {
        if (prev)
            prev->prox = aux->prox;
        else //aux é a head atual
            semaforos_existentes->head = semaforos_existentes->head->prox;
        free(aux);
    }
    pthread_mutex_unlock(&mutex_lista_semaforos);
}

void sem_queue(Fila_Espera_BCP *lista, BCP *processo) {
    if (!lista || !processo) return; // se os parâmetros são ponteiros nulos

    Espera_BCP *new = malloc((sizeof(Espera_BCP))); //TODO: revisar se isso faz sentido
    if (!new) return;

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
