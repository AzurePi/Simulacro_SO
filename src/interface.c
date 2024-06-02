#include "include/interface.h"

void draw_borders(WINDOW *win) {
    int x, y, i;
    getmaxyx(win, y, x);

    // Desenha bordas horizontais
    for (i = 0; i <= x; i++) {
        mvwprintw(win, 0, i, "-");
        mvwprintw(win, y - 1, i, "-");
    }

    // Desenha bordas verticais
    for (i = 0; i <= y; i++) {
        mvwprintw(win, i, 0, "|");
        mvwprintw(win, i, x - 1, "|");
    }

    // Desenha os cantos
    mvwprintw(win, 0, 0, "+");
    mvwprintw(win, 0, x - 1, "+");
    mvwprintw(win, y - 1, 0, "+");
    mvwprintw(win, y - 1, x - 1, "+");
}

void initializeInterface() {
    initscr();
    start_color();
    cbreak();

    int height, width;
    getmaxyx(stdscr, height, width);
    int middle = width / 2;

    short menu_lines = 7;
    short mem_lines = 3;
    short bottom_lines = 5;
    short mid_lines = height - menu_lines - mem_lines - 5;
    short mid_start_y = menu_lines + mem_lines;
    short bottom_start_y = menu_lines + mem_lines + mid_lines;

    // Definindo dimensões e posições das janelas
    WINDOW *win_super_menu = newwin(menu_lines, middle, 0, 0); // Menu na parte superior
    WINDOW *win_super_exec_proc = newwin(menu_lines, middle + 1, 0,
                                         middle); // Processo em Execução ao lado do Menu
    WINDOW *win_super_mem_state = newwin(mem_lines, width, menu_lines, 0); // Estado da Memória abaixo do Menu
    WINDOW *win_super_processos = newwin(mid_lines, middle, mid_start_y,
                                         0); // Lista de Processos abaixo do Estado da Memória (lado esquerdo)
    WINDOW *win_super_semaforos = newwin(mid_lines, middle + 1, mid_start_y,
                                         middle); // Lista de Semáforos abaixo do Estado da Memória (lado direito)
    WINDOW *win_super_error_log = newwin(bottom_lines, width, bottom_start_y, 0); // Log de Erros na parte inferior

    // Definindo a parte interna das janelas, que é onde escreveremos de fato
    win_menu = derwin(win_super_menu, menu_lines - 2, middle - 2, 1, 1);
    win_exec_proc = derwin(win_super_exec_proc, menu_lines - 2, middle + 1 - 2, 1, middle + 1);
    win_mem_state = derwin(win_super_mem_state, mem_lines - 2, width - 2, menu_lines + 2, 1);
    win_processos = derwin(win_super_processos, mid_lines - 2, middle - 2, mid_start_y + 1, 1);
    win_semaforos = derwin(win_super_semaforos, mid_lines - 2, middle + 1 - 2, mid_start_y + 1, middle + 1);
    win_error_log = derwin(win_super_error_log, bottom_lines - 2, width - 2, bottom_start_y + 1, 1);

    /*
    // Habilitar scrolling em algumas janelas
    scrollok(win_processos, TRUE);
    scrollok(win_semaforos, TRUE);
    scrollok(win_error_log, TRUE);
     */

    // Desenha as bordas das janelas
    draw_borders(win_super_menu);
    draw_borders(win_super_exec_proc);
    draw_borders(win_super_mem_state);
    draw_borders(win_super_processos);
    draw_borders(win_super_semaforos);
    draw_borders(win_super_error_log);

    // Atualiza as janelas com as bordas desenhadas
    wrefresh(win_super_menu);
    wrefresh(win_super_exec_proc);
    wrefresh(win_super_mem_state);
    wrefresh(win_super_processos);
    wrefresh(win_super_semaforos);
    wrefresh(win_super_error_log);
}

void *processandoAgora() {
    while (!encerrar) {
        if (!refresh_atual)
            continue;

        mvwprintw(win_exec_proc, 1, 7, "Processando agora: ");

        pthread_mutex_lock(&mutex_lista_processos); // bloqueia acesso à lista de processos
        if (executando_agora)
            mvwprintw(win_exec_proc, 1, 26, "%s (%d)", executando_agora->nome, executando_agora->id_seg);
        else
            mvwprintw(win_exec_proc, 1, 26, "nada");
        pthread_mutex_unlock(&mutex_lista_processos); // libera acesso à lista de processos}
    }
    return NULL;
}

void *menu() {
    char op;

    while (!encerrar) {
        wclear(win_menu); // limpa a janela

        mvwprintw(win_menu, 0, 1, " [1] Novo processo");
        mvwprintw(win_menu, 1, 1, " [0] Encerrar");

        mvwprintw(win_menu, 3, 1, "Selecione a operação: ");
        wrefresh(win_menu); // Atualiza a tela com o conteúdo impresso

        op = wgetch(win_menu); // Captura a entrada do usuário
        flushinp();

        switch (op) {
            case '0':
                mvwprintw(win_menu, 4, 1, "Encerrando programa...");
                wrefresh(win_menu);
                encerrar = true;
                break;
            case '1': {
                char filename[201];

                mvwprintw(win_menu, 4, 1, "Caminho do programa: ");
                wrefresh(win_menu);
                wgetnstr(win_menu, filename, 200);
                flushinp();

                // passamos uma cópia, para evitar que filename se perca no fim do bloco do case
                char *filename_copy = strdup(filename);
                sysCall(process_create, filename_copy);
            }
                break;
            default:
                mvwprintw(win_menu, 4, 1, "Opção inválida!"); //TODO: formatação de um erro
                wrefresh(win_menu);
                napms(1000);
                break;
        }
        napms(1000);
        flushinp();
    }

    return NULL;
}

void *informacaoProcessos() {
    while (!encerrar) {
        if (!refresh_proc)
            continue;

        pthread_mutex_lock(&mutex_lista_processos); // bloqueia acesso à lista de processos
        BCP *aux = head_lista_processos;
        if (!aux) {
            mvwprintw(win_processos, 0, 1, "Ainda não há processos no sistema.");
            wrefresh(win_processos);
            pthread_mutex_unlock(&mutex_lista_processos); // libera acesso à lista de processos
        } else {
            char *estados[] = {"PRONTO", "EXECUTANDO", "BLOQUEADO", "TERMINADO"};

            int i = 0;

            while (aux) { // monta uma linha com as informações do processo
                char linha[128];
                snprintf(linha, sizeof(linha),
                         "%s (%d), Prioridade: %d   %s",
                         aux->nome, aux->id_seg, aux->prioridade, estados[aux->estado]);

                mvwprintw(win_processos, i++, 1, "%s", linha);

                aux = aux->prox;
            }
            pthread_mutex_unlock(&mutex_lista_processos); // libera acesso à lista de processos

            wrefresh(win_processos); // atualiza a janela win_processos para mostrar as mudanças
            refresh_proc = false;
        }
    }
    return NULL;
}

void *informacaoSemaforos() {
    while (!encerrar) {
        if (!refresh_sem)
            continue;

        mvwprintw(win_semaforos, 0, 0, "Lista de Semáforos sendo utilizados: ");
        pthread_mutex_lock(&mutex_semaforos_globais); // bloqueia acesso à lista de semáforos
        No_Semaforo *aux = semaforos_existentes->head;
        if (!aux) {
            mvwprintw(win_semaforos, 1, 5, "Nenhum");
            wrefresh(win_semaforos);
            pthread_mutex_unlock(&mutex_semaforos_globais); // libera acesso à lista de semáforos
        } else {

            int i = 1;

            while (aux) { // monta a linha com as informações do semáforo
                char linha[128];
                snprintf(linha, sizeof(linha),
                         "%c | Contador: %d processos",
                         aux->semaforo->nome, aux->semaforo->refcount);

                mvwprintw(win_semaforos, i++, 5, "%s", linha);

                aux = aux->prox;
            }
            pthread_mutex_unlock(&mutex_semaforos_globais); // libera acesso à lista de semáforos

            wrefresh(win_semaforos);
            refresh_sem = false;
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

        mvwprintw(win_mem_state, 0, 0, "Taxa de ocupacao da memoria: %.2f%% (%d/%d)", taxa_ocupacao,
                  paginas_ocupadas, NUMERO_PAGINAS);
        wrefresh(win_mem_state);

        pthread_mutex_unlock(&mutex_RAM); // libera acesso à RAM
        refresh_mem = false;
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