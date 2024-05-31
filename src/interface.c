#include "include/interface.h"

void *interface() {
    char op;

    do {
        pthread_mutex_lock(&mutex_IO); // bloqueia o acesso ao terminal

        CLEAR_SCREEN();
        puts("╔════════════════════════════════════════╗");
        puts("║    " BOLD UNDERLINE "Simulador de Sistema Operacional" NOT_BOLD_NOR_FAINT NOT_UNDERLINE "    ║");
        puts("╟────────────────────────────────────────╢");
        puts("║ [1] Novo processo                      ║");
        puts("║ [2] Ver informações dos processos      ║");
        puts("║ [3] Ver informações da memória         ║");
        puts("║ [0] Encerrar                           ║");
        puts("╚════════════════════════════════════════╝");

        pthread_mutex_lock(&mutex_lista_processos); // bloqueia acesso à lista de processos
        printf("\n\t" UNDERLINE "Processando agora:" CLEAR " ");
        if (executando_agora)
            printf("%s (%d)\n", executando_agora->nome, executando_agora->id_seg);
        else
            puts(ITALIC "nada" NOT_ITALIC);
        pthread_mutex_unlock(&mutex_lista_processos); // libera acesso à lista de processos

        printf("\nSelecione a operação: " INPUT);
        scanf(" %c", &op);
        printf(CLEAR);
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
                printf("Caminho do programa: " INPUT);
                scanf("%200[^\n]s", filename);
                printf(CLEAR);
                limpar_buffer();
                pthread_mutex_unlock(&mutex_IO);

                // passamos uma cópia, para evitar que filename se perca no fim do bloco do case
                char *filename_copy = strdup(filename);
                sysCall(process_create, filename_copy);
            }
                break;
            case '2':
                create_and_detach(informacaoProcessos, NULL);
                break;
            case '3':
                create_and_detach(informacaoMemoria, NULL);
                break;
            default:
                pthread_mutex_lock(&mutex_IO);
                puts(ERROR "Opção inválida!" CLEAR);
                fflush(stdout);
                sleep(2);
                pthread_mutex_unlock(&mutex_IO);
                break;
        }
        sleep(1);
    } while (!encerrar);

    return NULL;
}

void *informacaoProcessos() {
    pthread_mutex_lock(&mutex_lista_processos); // bloqueia acesso à lista de processos
    BCP *atual = head_lista_processos;

    pthread_mutex_lock(&mutex_IO); // bloqueia acesso ao terminal
    if (!atual) {
        puts("\nAinda não há processos no sistema.");
    } else {
        char *estados[] = {"PRONTO", "EXECUTANDO", "BLOQUEADO", "TERMINADO"};

        while (atual) {
            char estado[11];
            strcpy(estado, estados[atual->estado]);

            printf("\n%s (%d), Prioridade: %d   " BOLD "%s" NOT_BOLD_NOR_FAINT,
                   atual->nome, atual->id_seg,
                   atual->prioridade, estado);
            atual = atual->prox;
        }
    }
    pthread_mutex_unlock(&mutex_lista_processos); // libera acesso à lista de processos

    puts("\nLista de Semáforos sendo utilizados: " BLUE);
    pthread_mutex_lock(&mutex_semaforos_globais); // bloqueia acesso á lista de semáforos
    No_Semaforo *aux = semaforos_existentes->head;
    if (!aux)
        printf("\tNenhum" CLEAR "\n");
    else {
        while (aux) {
            printf("\t%c " BOLD "|" NOT_BOLD_NOR_FAINT " Contador: %d processos\n", aux->semaforo->nome,
                   aux->semaforo->refcount);
            aux = aux->prox;
        }
    }
    printf(CLEAR);
    pthread_mutex_unlock(&mutex_semaforos_globais); //libera acesso à lista de semáforos

    press_any_key_to_continue();
    pthread_mutex_unlock(&mutex_IO); // libera acesso ao terminal

    return NULL;
}

void *informacaoMemoria() {
    pthread_mutex_lock(&mutex_RAM); // bloqueia acesso á RAM

    if (!RAM) {
        pthread_mutex_unlock(&mutex_RAM);
        return NULL;
    }

    double taxa_ocupacao = ((double) RAM->n_paginas_ocupadas / NUMERO_PAGINAS) * 100.0;

    pthread_mutex_lock(&mutex_IO); // bloqueia acesso ao terminal
    printf("\nTaxa de ocupação da memória: " BLUE "%.2f%% (%d/%d)" CLEAR "\n", taxa_ocupacao, RAM->n_paginas_ocupadas,
           NUMERO_PAGINAS);
    fflush(stdout);

    pthread_mutex_unlock(&mutex_RAM); // libera acesso à RAM

    press_any_key_to_continue();
    pthread_mutex_unlock(&mutex_IO); // libera acesso ao terminal

    return NULL;
}

BCP *mensagemErroBCP(const char *mensagem, BCP *processo) {
    if (!mensagem || !processo) return NULL;
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
    printf(FAINT "\n\tAperte qualquer tecla para continuar" NOT_BOLD_NOR_FAINT);
    _getwch(); // lê um caractere, sem escrevê-lo no terminal e sem esperar por um enter
    fflush(stdout);
}

#else

void press_any_key_to_continue() {
    struct termios old, new;

    tcgetattr(STDIN_FILENO, &old); // guarda os atributos velhos do terminal
    new = old;
    new.c_lflag &= ~ICANON; // desabilita a espera por um enter
    new.c_iflag &= ~ECHO; // desabilita o eco
    new.c_cc[VMIN] = 1;
    new.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &new); // seta os atributos novos

    printf(FAINT "\n\tAperte qualquer tecla para continuar" NOT_BOLD_NOR_FAINT);

    getchar(); // espera por uma tecla

    tcsetattr(STDIN_FILENO, TCSANOW, &old); // restaura o terminal para seu estado original
}

#endif