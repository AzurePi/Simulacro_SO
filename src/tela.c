#include "include/tela.h"

void *tela() {
    while (!encerrar) {
        NoTela *no = removeFila();
        if (no) {
            usleep(10000000); // delay de 1s para simular a impressão
            sysCall(print_finish, no->processo);
        }
    }
    return NULL;
}


void inicializarScreenQueue() {
    if (screen_queue)
        return;

    screen_queue = malloc(sizeof(FilaTela));
    screen_queue->head = NULL;
    screen_queue->tail = NULL;
}

void freeScreenQueue() {
    if (!screen_queue)
        return;

    while (screen_queue->head) {
        NoTela *aux = screen_queue->head;
        screen_queue->head = aux->prox;
        free(aux);
    }

    free(screen_queue);
}

NoTela *criaNoTela(BCP *processo) {
    NoTela *new = malloc(sizeof(NoTela));
    if (!new)
        return NULL;

    new->processo = processo;
    new->prox = NULL;
    return new;
}

void inserirFila(NoTela *no) {
    if (!no || !screen_queue)
        return;

    pthread_mutex_lock(&mutex_screen_queue);

    no->prox = NULL;
    if (!screen_queue->head) { //fila vazia
        screen_queue->tail = no;
        screen_queue->head = no;
    }
    else {
        screen_queue->tail->prox = no;
        screen_queue->tail = no;
    }

    pthread_mutex_unlock(&mutex_screen_queue);
}

NoTela *removeFila() {
    if (!screen_queue)
        return NULL;

    pthread_mutex_lock(&mutex_screen_queue);

    if (!screen_queue->head) {
        pthread_mutex_unlock(&mutex_screen_queue);
        return NULL;
    }

    NoTela *aux = screen_queue->head;
    if (aux == screen_queue->tail) {
        screen_queue->head = NULL;
        screen_queue->tail = NULL;
    }
    else
        screen_queue->head = aux->prox;

    pthread_mutex_unlock(&mutex_screen_queue);
    return aux;
}
