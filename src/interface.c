#include "include/interface.h"

void *menu() {
    char op;
    pthread_t t_processo, t_info_proc, t_info_mem;

    do {
        sem_wait(&sem_terminal); // requer acesso ao terminal

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

        sem_post(&sem_terminal); // libera acesso ao terminal

        switch (op) {
            case '0':
                sem_wait(&sem_terminal);
                printf("Encerrando programa.");
                sem_post(&sem_terminal);
                break;
            case '1':
                pthread_create(&t_processo, &atrib, processCreate, NULL);
                pthread_detach(t_processo);
                break;
            case '2':
                pthread_create(&t_info_proc, &atrib, informacaoProcessos, NULL);
                pthread_detach(t_info_proc);
                break;
            case '3':
                pthread_create(&t_info_mem, &atrib, informacaoMemoria, NULL);
                pthread_detach(t_info_mem);
                break;
            default:
                printf(ERROR "Opção inválida!" CLEAR);
                sleep(2);
                break;
        }
    } while (op != '0');
    return NULL;
}

void *informacaoProcessos() {
    sem_wait(&sem_terminal); // bloqueia acesso ao terminal
    sem_wait(&sem_lista_processos); // bloqueia acesso à lista de processos

    BCP *atual = head_lista_processos;

    if (!atual) {
        printf("Ainda não há processos no sistema.");
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
    sem_post(&sem_terminal);

    showSemaphoreList();

    printf("\nAperte qualquer tecla para retornar");
    scanf("%*s");
    limpar_buffer();

    sem_post(&sem_terminal); // libera acesso ao terminal
    return NULL;
}

void *informacaoMemoria() {
    sem_wait(&sem_terminal);

    double taxa_ocupacao = ((double) RAM->n_paginas_ocupadas / NUMERO_PAGINAS) * 100.0;
    printf("Taxa de ocupação da memória: %.2f%% (%d/%d)  \n", taxa_ocupacao, RAM->n_paginas_ocupadas, NUMERO_PAGINAS);

    printf("\nAperte qualquer tecla para retornar");
    scanf("%*s");
    limpar_buffer();

    sem_post(&sem_terminal);
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
    sem_wait(&sem_terminal);
    printf(ERROR "%s" CLEAR, mensagem);
    sleep(2);
    sem_post(&sem_terminal);
    freeBCP(processo);
    return NULL;
}

void limpar_buffer() {
    char c;
    while ((c = (char) getchar()) != '\n' && c != EOF);
}