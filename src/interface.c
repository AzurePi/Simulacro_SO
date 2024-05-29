#include "include/interface.h"

void *interface() {
    char op;

    pthread_attr_t atrib;
    pthread_t t_info;

    pthread_attr_init(&atrib);
    pthread_attr_setscope(&atrib, PTHREAD_SCOPE_SYSTEM);

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

        pthread_mutex_lock(&mutex_lista_processos); // bloqueia acesso à lista de processos
        printf("\n\t" UNDERLINE "Processando agora:" NOT_UNDERLINE " ");
        if (executando_agora)
            printf("%s (%d)\n", executando_agora->nome, executando_agora->id_seg);
        else
            printf("nada\n");
        pthread_mutex_unlock(&mutex_lista_processos); // libera acesso à lista de processos

        printf("\nSelecione a operação: ");
        scanf(" %c", &op);
        limpar_buffer();
        pthread_mutex_unlock(&mutex_IO);

        switch (op) {
            case '0':
                pthread_mutex_lock(&mutex_IO);
                printf("Encerrando programa.\n");
                pthread_mutex_unlock(&mutex_IO);
                encerrar = true;
                break;
            case '1': {
                char filename[201];

                pthread_mutex_lock(&mutex_IO);
                printf("Caminho do programa: ");
                scanf("%200[^\n]s", filename);
                limpar_buffer();
                pthread_mutex_unlock(&mutex_IO);

                // passamos uma cópia, para evitar que filename se perca no fim do bloco do case
                char *filename_copy = strdup(filename);
                sysCall(process_create, filename_copy);
            }
                break;
            case '2': {
                pthread_create(&t_info, &atrib, informacaoProcessos, NULL);
                pthread_detach(t_info);
                sleep(1);
            }
                break;
            case '3': {
                pthread_create(&t_info, &atrib, informacaoMemoria, NULL);
                pthread_detach(t_info);
                sleep(1);
            }
                break;
            default:
                pthread_mutex_lock(&mutex_IO);
                printf(ERROR "Opção inválida!" CLEAR);
                fflush(stdout);
                sleep(2);
                pthread_mutex_unlock(&mutex_IO);
                break;
        }
    } while (!encerrar);

    pthread_attr_destroy(&atrib);

    return NULL;
}

void *informacaoProcessos() {
    pthread_mutex_lock(&mutex_lista_processos); // bloqueia acesso à lista de processos

    BCP *atual = head_lista_processos;

    pthread_mutex_lock(&mutex_IO); // bloqueia acesso ao terminal
    if (!atual) {
        printf("\nAinda não há processos no sistema.\n"); // TODO: tá sempre caindo aqui; provavelmente é problema com a inserção na lista global e quando ela acontece
        fflush(stdout);
    } else {
        char *estados[] = {"PRONTO", "EXECUTANDO", "BLOQUEADO", "TERMINADO"};

        while (atual != NULL) {
            char estado[11];
            strcpy(estado, estados[atual->estado]);

            printf("%s (%d), Prioridade: %d    " BOLD "%s" NOT_BOLD_NOR_FAINT "\n", atual->nome, atual->id_seg,
                   atual->prioridade, estado);
            atual = atual->prox;
        }
    }
    pthread_mutex_unlock(&mutex_lista_processos); // libera acesso à lista de processos

    printf("\nLista de Semáforos sendo utilizados: \n");
    pthread_mutex_lock(&mutex_lista_semaforos); // bloqueia acesso á lista de semáforos
    Semaforo_Global *aux = semaforos_existentes->head;
    if (!aux)
        printf("\tNenhum\n");
    else {
        while (aux) {
            printf("\t%c | Contador: %d processos\n", aux->semaforo->nome, aux->semaforo->refcount);
            aux = aux->prox;
        }
    }
    pthread_mutex_unlock(&mutex_lista_semaforos);

    //press_any_key_to_continue();
    printf(FAINT "\n\tAperte qualquer tecla para continuar" NOT_BOLD_NOR_FAINT);
    fflush(stdout);
    getchar();
    limpar_buffer();

    pthread_mutex_unlock(&mutex_IO); // libera acesso ao terminal
    return NULL;
}

void *informacaoMemoria() {
    pthread_mutex_lock(&mutex_IO); // bloqueia acesso ao terminal
    pthread_mutex_lock(&mutex_RAM); // bloqueia acesso á RAM

    double taxa_ocupacao = ((double) RAM->n_paginas_ocupadas / NUMERO_PAGINAS) * 100.0;
    printf("\nTaxa de ocupação da memória: %.2f%% (%d/%d)\n", taxa_ocupacao, RAM->n_paginas_ocupadas, NUMERO_PAGINAS);
    fflush(stdout);

    pthread_mutex_unlock(&mutex_RAM); // libera acesso à RAM

    //press_any_key_to_continue();
    printf(FAINT "\n\tAperte qualquer tecla para continuar" NOT_BOLD_NOR_FAINT);
    fflush(stdout);
    getchar();
    limpar_buffer();

    pthread_mutex_unlock(&mutex_IO); // libera acesso ao terminal
    return NULL;
}

BCP *mensagemErroBCP(const char *mensagem, BCP *processo) {
    pthread_mutex_lock(&mutex_IO);
    printf(ERROR "%s" CLEAR "\n", mensagem);
    fflush(stdout);
    sleep(2);
    pthread_mutex_unlock(&mutex_IO);
    freeBCP(processo);
    return NULL;
}

void limpar_buffer() {
    while (getchar() != '\n');
}

// função dependente do sistema operacional
#ifdef _WIN32

void press_any_key_to_continue() {
    printf(FAINT "\n\tAperte qualquer tecla para continuar\n" NOT_BOLD_NOR_FAINT);
    _getch(); // Espera até que qualquer tecla seja pressionada
    limpar_buffer();
}

#else

void press_any_key_to_continue() {
    struct termios oldt, newt;

    printf(FAINT "\n\tAperte qualquer tecla para continuar\n" NOT_BOLD_NOR_FAINT);

    // Desativa o modo canônico e o eco
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    // Espera por uma tecla
    getchar();
    limpar_buffer();

    // Restaura o terminal para seu estado original
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

#endif