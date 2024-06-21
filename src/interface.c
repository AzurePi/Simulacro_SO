#include "include/interface.h"

void* interface() {
    const char* menu =
        "╔════════════════════════════════════════╗\n"
        "║    " BOLD UNDERLINE "Simulador de Sistema Operacional" NOT_BOLD_NOR_FAINT NOT_UNDERLINE "    ║\n"
        "╟────────────────────────────────────────╢\n"
        "║ [1] Novo processo                      ║\n"
        "║ [2] Ver informações dos processos      ║\n"
        "║ [3] Ver informações da memória         ║\n"
        "║ [0] Encerrar                           ║\n"
        "╚════════════════════════════════════════╝";
    char op;

    do {
        pthread_mutex_lock(&mutex_IO); // bloqueia o acesso ao terminal

        CLEAR_SCREEN();
        puts(menu);

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
        pthread_mutex_unlock(&mutex_IO); // desbloqueia acesso ao terminal

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
            char* filename_copy = strdup(filename);
            sysCall(process_create, filename_copy);
        }
        break;
        case '2':
            pthread_mutex_lock(&mutex_IO);
            informacaoProcessos();
            pthread_mutex_unlock(&mutex_IO);
            break;
        case '3':
            pthread_mutex_lock(&mutex_IO);
            informacaoMemoria();
            pthread_mutex_unlock(&mutex_IO);
            break;
        default:
            pthread_mutex_lock(&mutex_IO);
            puts(ERROR "Opção inválida!" CLEAR);
            fflush(stdout);
            sleep(1);
            pthread_mutex_unlock(&mutex_IO);
            break;
        }
        sleep(1);
    }
    while (!encerrar);

    return NULL;
}

void informacaoProcessos() {
    if (!head_lista_processos) { puts("\nAinda não há processos no sistema."); }
    else {
        char* estados[] = {"PRONTO", "EXECUTANDO", "BLOQUEADO", "TERMINADO"};
        size_t buffer_size = 128;
        char* buffer = malloc(buffer_size);
        buffer[0] = '\0';

        pthread_mutex_lock(&mutex_lista_processos); // bloqueia acesso à lista de processos
        BCP* atual = head_lista_processos;
        while (atual) {
            char linha[128];
            snprintf(linha, sizeof(linha),
                     "\n%s (%d), Prioridade: %d   " BOLD "%s" NOT_BOLD_NOR_FAINT,
                     atual->nome, atual->id_seg, atual->prioridade, estados[atual->estado]);

            const size_t linha_len = strlen(linha);
            const size_t new_size = strlen(buffer) + linha_len + 1;

            if (new_size > buffer_size) {
                buffer_size = new_size;
                buffer = realloc(buffer, buffer_size);
            }
            strcat(buffer, linha);
            atual = atual->prox;
        }
        pthread_mutex_unlock(&mutex_lista_processos); // libera acesso à lista de processos
        puts(buffer);
        free(buffer);
    }

    printf("\nLista de Semáforos sendo utilizados: " BLUE);
    pthread_mutex_lock(&mutex_semaforos_globais); // bloqueia acesso à lista de semáforos
    No_Semaforo* aux = semaforos_existentes->head;
    if (!aux)
        printf("\n\tNenhum" CLEAR "\n");
    else {
        size_t buffer_size = 128;
        char* buffer = malloc(buffer_size);
        buffer[0] = '\0';

        while (aux) {
            char linha[128];
            snprintf(linha, sizeof(linha),
                     "\n\t%c " BOLD "|" NOT_BOLD_NOR_FAINT " Contador: %d processos",
                     aux->semaforo->nome, aux->semaforo->refcount);

            const size_t linha_len = strlen(linha);
            const size_t new_size = strlen(buffer) + linha_len + 1;

            if (new_size > buffer_size) {
                buffer_size = new_size;
                buffer = realloc(buffer, buffer_size);
            }
            strcat(buffer, linha);
            aux = aux->prox;
        }
        printf("%s" CLEAR "\n", buffer);
        free(buffer);
    }
    pthread_mutex_unlock(&mutex_semaforos_globais); //libera acesso à lista de semáforos

    press_any_key_to_continue();
}

void informacaoMemoria() {
    pthread_mutex_lock(&mutex_RAM); // bloqueia acesso á RAM

    if (!RAM) {
        pthread_mutex_unlock(&mutex_RAM);
        return;
    }

    const int paginas_ocupadas = RAM->n_paginas_ocupadas;
    const double taxa_ocupacao = (double)paginas_ocupadas / NUMERO_PAGINAS * 100.0;

    printf("\nTaxa de ocupação da memória: " BLUE "%.2f%% (%d/%d)" CLEAR "\n",
           taxa_ocupacao, paginas_ocupadas, NUMERO_PAGINAS);
    fflush(stdout);

    pthread_mutex_unlock(&mutex_RAM); // libera acesso à RAM

    press_any_key_to_continue();
}

BCP* mensagemErroBCP(const char* mensagem, BCP* processo) {
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
    fflush(stdout);
    _getwch(); // lê um caractere, sem escrevê-lo no terminal e sem esperar por um enter
}

#else

void press_any_key_to_continue() {
    struct termios old, new;

    tcgetattr(STDIN_FILENO, &old); // guarda os atributos velhos do terminal
    new = old;
    new.c_lflag &= ~(ICANON | ECHO); // desabilita a espera por um enter
    new.c_cc[VMIN] = 1;
    new.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &new); // seta os atributos novos

    printf(FAINT "\n\tAperte qualquer tecla para continuar" NOT_BOLD_NOR_FAINT);
    fflush(stdout);
    getchar(); // espera por uma tecla

    tcsetattr(STDIN_FILENO, TCSANOW, &old); // restaura o terminal para seu estado original
}

#endif
