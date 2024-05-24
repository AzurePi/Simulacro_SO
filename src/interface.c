#include "include/interface.h"

void *menu() {
    char op;
    pthread_t t_processo, t_info_proc, t_info_mem;

    do {
        //sem_wait(&sem_terminal); // requer acesso ao terminal
        pthread_mutex_lock(&mutex_terminal);

        CLEAR_SCREEN
        printf("╔════════════════════════════════════════╗\n");
        printf("║    " BOLD UNDERLINE "Simulador de Sistema Operacional" NOT_BOLD NOT_UNDERLINE"    ║\n");
        printf("╟────────────────────────────────────────╢\n");
        printf("║ [1] Novo processo                      ║\n");
        printf("║ [2] Ver informações dos processos      ║\n");
        printf("║ [3] Ver informações da memória         ║\n");
        printf("║ [0] Encerrar                           ║\n");
        printf("╚════════════════════════════════════════╝\n");

        printf("\n\t" UNDERLINE "Processando agora:" NOT_UNDERLINE " ");
        if (rodando_agora)
            printf("%s (%d)\n", rodando_agora->nome, rodando_agora->id_seg);
        else
            printf("nada\n");

        printf("\nSelecione a operação: ");
        scanf("%c", &op);
        limpar_buffer();

        switch (op) {
            case '0':
                printf("Encerrando programa.\n");
                pthread_mutex_unlock(&mutex_terminal);
                // sem_post(&sem_terminal);
                break;
            case '1':
                pthread_mutex_unlock(&mutex_terminal);
                pthread_create(&t_processo, &atrib, processCreate, NULL);
                pthread_detach(t_processo);
                sleep(1);
                break;
            case '2':
                pthread_mutex_unlock(&mutex_terminal);
                pthread_create(&t_info_proc, &atrib, informacaoProcessos, NULL);
                pthread_detach(t_info_proc);
                sleep(1);
                break;
            case '3':
                pthread_mutex_unlock(&mutex_terminal);
                pthread_create(&t_info_mem, &atrib, informacaoMemoria, NULL);
                pthread_detach(t_info_mem);
                sleep(1);
                break;
            default:
                printf(ERROR "Opção inválida!" CLEAR);
                sleep(2);
                pthread_mutex_unlock(&mutex_terminal);
                break;
        }
    } while (op != '0');
    return NULL;
}

void *informacaoProcessos() {
    pthread_mutex_lock(&mutex_terminal); // bloqueia acesso ao terminal
    sem_wait(&sem_lista_processos); // bloqueia acesso à lista de processos

    BCP *atual = head_lista_processos;

    if (!atual) {
        printf("Ainda não há processos no sistema.\n");
    } else {
        while (atual != NULL) {
            char estado[11];
            switch (atual->estado) {
                case PRONTO:
                    strcpy(estado, "PRONTO");
                    break;
                case EXECUTANDO:
                    strcpy(estado, "EXECUTANDO");
                    break;
                case BLOQUEADO:
                    strcpy(estado, "BLOQUEADO");
                    break;
                case TERMINADO:
                    strcpy(estado, "TERMINADO");
                    break;
            }

            printf("%s (%d), Prioridade: %d    " BOLD "%s" NOT_BOLD "\n", atual->nome, atual->id_seg, atual->prioridade,
                   estado);
            atual = atual->prox;
        }
    }
    sem_post(&sem_lista_processos); // libera acesso à lista de processos

    showSemaphoreList();

    printf("\n\tAperte qualquer tecla para retornar");
    getchar();
    limpar_buffer();

    pthread_mutex_unlock(&mutex_terminal); // libera acesso ao terminal
    return NULL;
}

void *informacaoMemoria() {
    pthread_mutex_lock(&mutex_terminal);

    double taxa_ocupacao = ((double) RAM->n_paginas_ocupadas / NUMERO_PAGINAS) * 100.0;
    printf("\nTaxa de ocupação da memória: %.2f%% (%d/%d)\n", taxa_ocupacao, RAM->n_paginas_ocupadas, NUMERO_PAGINAS);

    printf("\n\tAperte qualquer tecla para retornar"); //TODO: ajeitar isso
    getchar();
    limpar_buffer();

    pthread_mutex_unlock(&mutex_terminal);
    return NULL;
}

void showSemaphoreList() {
    sem_wait(&sem_terminal);
    printf("Lista de Semáforos sendo utilizados: ");
    Semaforo *aux = semaforos_existentes->head;
    if (!aux) {
        printf("\tNenhum.\n");
        sem_post(&sem_terminal);
        return;
    }
    while (aux) {
        printf("\t%c | Contador: %d\n", aux->nome, aux->refcount);
        aux = aux->prox;
    }
    sem_post(&sem_terminal);
}

BCP *mensagemErroBCP(const char *mensagem, BCP *processo) {
    pthread_mutex_lock(&mutex_terminal);
    printf(ERROR "%s" CLEAR "\n", mensagem);
    sleep(2);
    pthread_mutex_unlock(&mutex_terminal);
    freeBCP(processo);
    return NULL;
}

void limpar_buffer() {
    while (getchar() != '\n');
}