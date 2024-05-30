#include "include/interface.h"

void *interface() {
    char op;

    do {
        pthread_mutex_lock(&mutex_IO); // bloqueia o acesso ao terminal

        CLEAR_SCREEN
        puts("╔════════════════════════════════════════╗");
        puts("║    " BOLD UNDERLINE "Simulador de Sistema Operacional" NOT_BOLD_NOR_FAINT NOT_UNDERLINE "    ║");
        puts("╟────────────────────────────────────────╢");
        puts("║ [1] Novo processo                      ║");
        puts("║ [2] Ver informações dos processos      ║");
        puts("║ [3] Ver informações da memória         ║");
        puts("║ [0] Encerrar                           ║");
        puts("╚════════════════════════════════════════╝");

        pthread_mutex_lock(&mutex_lista_processos); // bloqueia acesso à lista de processos
        printf("\n\t" UNDERLINE "Processando agora:" NOT_UNDERLINE " ");
        if (executando_agora)
            printf("%s (%d)\n", executando_agora->nome, executando_agora->id_seg);
        else
            puts("nada");
        pthread_mutex_unlock(&mutex_lista_processos); // libera acesso à lista de processos

        printf("\nSelecione a operação: ");
        scanf(" %c", &op);
        limpar_buffer();
        pthread_mutex_unlock(&mutex_IO);

        switch (op) {
            case '0':
                pthread_mutex_lock(&mutex_IO);
                puts("Encerrando programa...");
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
                create_and_detach(informacaoProcessos, NULL);
                sleep(1);
            }
                break;
            case '3': {
                create_and_detach(informacaoMemoria, NULL);
                sleep(1);
            }
                break;
            default:
                pthread_mutex_lock(&mutex_IO);
                puts(ERROR "Opção inválida!" CLEAR);
                fflush(stdout);
                sleep(2);
                pthread_mutex_unlock(&mutex_IO);
                break;
        }
    } while (!encerrar);

    return NULL;
}

void *informacaoProcessos() {
    pthread_mutex_lock(&mutex_lista_processos); // bloqueia acesso à lista de processos
    BCP *atual = head_lista_processos;

    pthread_mutex_lock(&mutex_IO); // bloqueia acesso ao terminal
    if (!atual) {
        puts("\nAinda não há processos no sistema.\n");
    } else {
        char *estados[] = {"PRONTO", "EXECUTANDO", "BLOQUEADO", "TERMINADO"};

        while (atual) {
            char estado[11];
            strcpy(estado, estados[atual->estado]);

            printf("%s (%d), Prioridade: %d    " BOLD "%s" NOT_BOLD_NOR_FAINT "\n", atual->nome, atual->id_seg,
                   atual->prioridade, estado);
            atual = atual->prox;
        }
    }
    pthread_mutex_unlock(&mutex_lista_processos); // libera acesso à lista de processos

    puts("Lista de Semáforos sendo utilizados: ");
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
    pthread_mutex_unlock(&mutex_lista_semaforos); //libera acesso à lista de semáforos

    press_any_key_to_continue();
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

    press_any_key_to_continue();
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
    char c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// função dependente do sistema operacional
#ifdef _WIN32

void press_any_key_to_continue() {
    puts(FAINT "\n\tAperte qualquer tecla para continuar\n" NOT_BOLD_NOR_FAINT);
    _getwch();
    fflush(stdout);
}

#else

void press_any_key_to_continue() {
    struct termios old, new;

    tcgetattr(STDIN_FILENO, &old);
    new = old;
    new.c_lflag &= ~ICANON;
    new.c_cc[VMIN] = 1;
    new.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &new);

    puts(FAINT "\n\tAperte qualquer tecla para continuar\n" NOT_BOLD_NOR_FAINT);

    // Espera por uma tecla
    getchar();
    fflush(stdout);
    limpar_buffer();

    // Restaura o terminal para seu estado original
    tcsetattr(STDIN_FILENO, TCSANOW, &old);
}

#endif