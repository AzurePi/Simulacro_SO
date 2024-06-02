#include "include/interface.h"

void draw_borders(WINDOW *win) {
    int x, y, i;
    getmaxyx(win, y, x);

    // Desenha bordas horizontais
    for (i = 0; i < x; i++) {
        mvwprintw(win, 0, i, "═");
        mvwprintw(win, y - 1, i, "═");
    }

    // Desenha bordas verticais
    for (i = 0; i < y; i++) {
        mvwprintw(win, i, 0, "║");
        mvwprintw(win, i, x - 1, "║");
    }

    // Desenha os cantos
    mvwprintw(win, 0, 0, "╔");
    mvwprintw(win, 0, x - 1, "╗");
    mvwprintw(win, y - 1, 0, "╚");
    mvwprintw(win, y - 1, x - 1, "╝");
}

void initializeInterface() {
    initscr();
    start_color();
    cbreak();

    int height, width;
    getmaxyx(stdscr, height, width);

    // Definindo dimensões e posições das janelas
    win_menu = newwin(3, width, 0, 0);                   // Menu na parte superior
    win_exec_proc = newwin(3, width / 2, 0, width / 2);  // Processo em Execução ao lado do Menu
    win_mem_state = newwin(3, width, 3, 0);              // Estado da Memória abaixo do Menu
    win_processos = newwin(height - 9, width / 2, 6, 0); // Lista de Processos abaixo do Estado da Memória (lado esquerdo)
    win_semaforos = newwin(height - 9, width / 2, 6, width / 2); // Lista de Semáforos abaixo do Estado da Memória (lado direito)
    win_error_log = newwin(3, width, height - 3, 0);     // Log de Erros na parte inferior

    // Habilitar scrolling em algumas janelas
    scrollok(win_processos, TRUE);
    scrollok(win_semaforos, TRUE);
    scrollok(win_error_log, TRUE);

    // Desenha as bordas das janelas
    draw_borders(win_menu);
    draw_borders(win_exec_proc);
    draw_borders(win_mem_state);
    draw_borders(win_processos);
    draw_borders(win_semaforos);
    draw_borders(win_error_log);

    // Atualiza as janelas com as bordas desenhadas
    wrefresh(win_menu);
    wrefresh(win_exec_proc);
    wrefresh(win_mem_state);
    wrefresh(win_processos);
    wrefresh(win_semaforos);
    wrefresh(win_error_log);
}

void *menu() {
    char op;

    do {
        short i = 1; // começamos a imprimir na segunda linha, pois a primeira é a borda

        pthread_mutex_lock(&mutex_IO); // bloqueia o acesso ao terminal

        wclear(win_menu); // limpa a janela
        draw_borders(win_menu); // redesenha as bordas

        mvwprintw(win_menu, i++, 1, " [1] Novo processo");
        mvwprintw(win_menu, i++, 1, " [0] Encerrar");
        i++; // pula uma linha
        mvwprintw(win_menu, i, 7, "Processando agora: ");

        pthread_mutex_lock(&mutex_lista_processos); // bloqueia acesso à lista de processos
        if (executando_agora)
            mvwprintw(win_menu, i++, 26, "%s (%d)", executando_agora->nome, executando_agora->id_seg);
        else
            mvwprintw(win_menu, i++, 26, "nada");
        pthread_mutex_unlock(&mutex_lista_processos); // libera acesso à lista de processos

        i++; //pula uma linha

        mvwprintw(win_menu, i++, 1, "Selecione a operação: ");
        wrefresh(win_menu); // Atualiza a tela com o conteúdo impresso

        op = wgetch(win_menu); // Captura a entrada do usuário
        flushinp();

        pthread_mutex_unlock(&mutex_IO); // desbloqueia acesso ao terminal

        switch (op) {
            case '0':
                pthread_mutex_lock(&mutex_IO);
                wprintw(win_menu, "Encerrando programa...");
                wrefresh(win_menu);
                pthread_mutex_unlock(&mutex_IO);
                encerrar = true;
                break;
            case '1': {
                char filename[201];

                pthread_mutex_lock(&mutex_IO);
                wprintw(win_menu, "Caminho do programa: " INPUT);
                wrefresh(win_menu);
                wgetnstr(win_menu, filename, 200);
                flushinp();
                pthread_mutex_unlock(&mutex_IO);

                // passamos uma cópia, para evitar que filename se perca no fim do bloco do case
                char *filename_copy = strdup(filename);
                sysCall(process_create, filename_copy);
            }
                break;
            default:
                pthread_mutex_lock(&mutex_IO);
                mvwprintw(win_menu, i, 1, "Opção inválida!"); //TODO: formatação de um erro
                wrefresh(win_menu);
                napms(1000);
                pthread_mutex_unlock(&mutex_IO);
                break;
        }
        napms(1000);
        flushinp();
    } while (!encerrar);

    return NULL;
}

void *informacaoProcessos() {
    while (!encerrar) {
        if (!refresh_proc)
            continue;

        pthread_mutex_lock(&mutex_lista_processos); // bloqueia acesso à lista de processos
        BCP *aux = head_lista_processos;
        if (!aux) {
            mvwprintw(win_processos, 1, 1, "Ainda não há processos no sistema.");
            pthread_mutex_unlock(&mutex_lista_processos); // libera acesso à lista de processos
        } else {
            char *estados[] = {"PRONTO", "EXECUTANDO", "BLOQUEADO", "TERMINADO"};

            while (aux) { // monta uma linha com as informações do processo
                char linha[128];
                snprintf(linha, sizeof(linha),
                         "%s (%d), Prioridade: %d   " BOLD "%s" NOT_BOLD_NOR_FAINT "\n",
                         aux->nome, aux->id_seg, aux->prioridade, estados[aux->estado]);

                wprintw(win_processos, "%s", linha);

                aux = aux->prox;
            }
            pthread_mutex_unlock(&mutex_lista_processos); // libera acesso à lista de processos

            // atualiza a janela win_processos para mostrar as mudanças
            wrefresh(win_processos);
        }
    }
    return NULL;
}

void *informacaoSemaforos() {
    while (!encerrar) {
        wprintw(win_semaforos, "Lista de Semáforos sendo utilizados: ");
        pthread_mutex_lock(&mutex_semaforos_globais); // bloqueia acesso à lista de semáforos
        No_Semaforo *aux = semaforos_existentes->head;
        if (!aux) {
            wprintw(win_semaforos, "\n\tNenhum");
            pthread_mutex_unlock(&mutex_semaforos_globais); // libera acesso à lista de semáforos
        } else {
            size_t buffer_size = 128;
            char *buffer = malloc(buffer_size);
            buffer[0] = '\0';

            while (aux) { // monta a linha com as informações do semáforo
                char linha[128];
                snprintf(linha, sizeof(linha),
                         "\n\t%c " BOLD "|" NOT_BOLD_NOR_FAINT " Contador: %d processos",
                         aux->semaforo->nome, aux->semaforo->refcount);

                // adiciona a linha ao buffer
                size_t linha_len = strlen(linha);
                size_t new_size = strlen(buffer) + linha_len + 1;
                if (new_size > buffer_size) {
                    buffer_size = new_size;
                    buffer = realloc(buffer, buffer_size);
                }
                strcat(buffer, linha);
                aux = aux->prox;
            }
            wprintw(win_semaforos, "%s\n", buffer);
            free(buffer);

            pthread_mutex_unlock(&mutex_semaforos_globais); // libera acesso à lista de semáforos
        }
    }
    return NULL;
}

void *informacaoMemoria() {
    while (!encerrar) {
        if (!refresh_mem)
            continue;

        pthread_mutex_lock(&mutex_RAM); // bloqueia acesso à RAM

        int paginas_ocupadas = RAM->n_paginas_ocupadas;
        double taxa_ocupacao = ((double) paginas_ocupadas / NUMERO_PAGINAS) * 100.0;

        werase(win_mem_state); // Limpa a janela
        draw_borders(win_mem_state);
        mvwprintw(win_mem_state, 1, 1, "Taxa de ocupacao da memoria: %.2f%% (%d/%d)", taxa_ocupacao,
                  paginas_ocupadas,
                  NUMERO_PAGINAS);
        wrefresh(win_mem_state);

        pthread_mutex_unlock(&mutex_RAM); // libera acesso à RAM
    }
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