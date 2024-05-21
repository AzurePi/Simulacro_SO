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
    sem_wait(&sem_terminal);
    //TODO: informações dos processos na fila do escalonador


    printf("\nProcessando agora: ");
    if (rodando_agora)
        printf("%s (%d)\n", rodando_agora->nome, rodando_agora->id_seg);
    else
        printf("nada\n");

    sem_post(&sem_terminal);
    return NULL;
}

void *informacaoMemoria() {
    sem_wait(&sem_terminal);

    printf("");
    //TODO: mostrar a proporção da memória sendo usada, quanto está livre, etc.

    //TODO: em algum momento aqui, exibir a lista de semáforos

    sem_post(&sem_terminal);
    return NULL;
}

void showSemaphoreList() {
    sem_wait(&sem_terminal);
    printf("Semáforos existentes:\n");
    Semaforo *aux = semaforos_existentes->head;
    if (!aux) {
        printf("\tNenhum.\n");
        sleep(3);
        sem_post(&sem_terminal);
        return;
    }
    while (aux) {
        printf("\t%c | Contador: %d\n", aux->name, aux->refcount);
        aux = aux->prox;
    }
    sleep(3);
    sem_post(&sem_terminal);
}

void limpar_buffer() {
    char c;
    while ((c = (char) getchar()) != '\n' && c != EOF);
}

