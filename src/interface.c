#include "include/interface.h"

void *interface() {
    char op;

    pthread_attr_t atrib;
    pthread_t t_info_proc, t_info_mem;

    pthread_attr_init(&atrib);
    pthread_attr_setscope(&atrib, PTHREAD_SCOPE_PROCESS);

    do {
        pthread_mutex_lock(&mutex_IO); // bloqueia o acesso ao terminal

        CLEAR_SCREEN
        printf("╔════════════════════════════════════════╗\n");
        printf("║    " BOLD UNDERLINE "Simulador de Sistema Operacional" NOT_BOLD_NOR_FAINT NOT_UNDERLINE"    ║\n");
        printf("╟────────────────────────────────────────╢\n");
        printf("║ [1] Novo processo                      ║\n");
        printf("║ [2] Ver informações dos processos      ║\n");
        printf("║ [3] Ver informações da memória         ║\n");
        printf("║ [0] Encerrar                           ║\n");
        printf("╚════════════════════════════════════════╝\n");

        printf("\n\t" UNDERLINE "Processando agora:" NOT_UNDERLINE " ");
        if (executando_agora)
            printf("%s (%d)\n", executando_agora->nome, executando_agora->id_seg);
        else
            printf("nada\n");

        printf("\nSelecione a operação: ");
        scanf("%c", &op);
        limpar_buffer();

        switch (op) {
            case '0':
                printf("Encerrando programa.\n");
                pthread_mutex_unlock(&mutex_IO);
                break;
            case '1': {
                char filename[201];

                printf("Caminho do programa: ");
                scanf("%200[^\n]s", filename);
                limpar_buffer();

                // passamos uma cópia, para evitar que filename se perca no fim do bloco do case
                char *filename_copy = strdup(filename);

                sysCall(process_create, filename_copy);
            }
                sleep(1);
                break;
            case '2': {
                pthread_mutex_unlock(&mutex_IO); //TODO: verificar se essa informações tá fazendo sentido
                pthread_create(&t_info_proc, &atrib, informacaoProcessos, NULL);
                pthread_detach(t_info_proc);
            }
                sleep(1);
                break;
            case '3': {
                pthread_mutex_unlock(&mutex_IO);
                pthread_create(&t_info_mem, &atrib, informacaoMemoria, NULL);
                pthread_detach(t_info_mem);
            }
                sleep(1);
                break;
            default:
                printf(ERROR "Opção inválida!" CLEAR);
                sleep(2);
                pthread_mutex_unlock(&mutex_IO);
                break;
        }
    } while (op != '0');

    pthread_attr_destroy(&atrib);

    return NULL;
}

void *informacaoProcessos() {
    pthread_mutex_lock(&mutex_lista_processos); // bloqueia acesso à lista de processos

    BCP *atual = head_lista_processos;

    pthread_mutex_lock(&mutex_IO); // bloqueia acesso ao terminal
    if (!atual) {
        printf("Ainda não há processos no sistema.\n");
    } else {
        char *estados[] = {"PRONTO", "EXECUTANDO", "BLOQUEADO", "TERMINADO"};

        while (atual != NULL) {
            char estado[11];
            strcpy(estado, estados[atual->estado]);

            printf("%s (%d), Prioridade: %d    " BOLD "%s" NOT_BOLD_NOR_FAINT "\n", atual->nome, atual->id_seg,
                   atual->prioridade,
                   estado);
            atual = atual->prox;
        }
    }
    pthread_mutex_unlock(&mutex_lista_processos); // libera acesso à lista de processos

    printf("Lista de Semáforos sendo utilizados: ");
    Semaforo *aux = semaforos_existentes->head;
    if (!aux)
        printf("\tNenhum.\n");
    else {
        while (aux) {
            printf("\t%c | Contador: %d\n", aux->nome, aux->refcount);
            aux = aux->prox;
        }
    }

    printf(FAINT "\n\tAperte qualquer tecla para retornar" NOT_BOLD_NOR_FAINT); //TODO: isso não funciona
    getchar();
    limpar_buffer();

    pthread_mutex_unlock(&mutex_IO); // libera acesso ao terminal
    return NULL;
}

void *informacaoMemoria() {
    pthread_mutex_lock(&mutex_IO);

    double taxa_ocupacao = ((double) RAM->n_paginas_ocupadas / NUMERO_PAGINAS) * 100.0;
    printf("\nTaxa de ocupação da memória: %.2f%% (%d/%d)\n", taxa_ocupacao, RAM->n_paginas_ocupadas, NUMERO_PAGINAS);

    printf(FAINT "\n\tAperte qualquer tecla para retornar" NOT_BOLD_NOR_FAINT); //TODO: ajeitar isso
    getchar();
    limpar_buffer();

    pthread_mutex_unlock(&mutex_IO);
    return NULL;
}

BCP *mensagemErroBCP(const char *mensagem, BCP *processo) {
    pthread_mutex_lock(&mutex_IO);
    printf(ERROR "%s" CLEAR "\n", mensagem);
    sleep(2);
    pthread_mutex_unlock(&mutex_IO);
    freeBCP(processo);
    return NULL;
}

void limpar_buffer() {
    while (getchar() != '\n');
}