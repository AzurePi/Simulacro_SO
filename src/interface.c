#include "include/interface.h"

void draw_borders(WINDOW *win) {
    box(win, 0, 0);
    wrefresh(win);
}

void initializeInterface() {
    initscr();
    start_color();
    cbreak();

    short height, width;
    getmaxyx(stdscr, height, width);
    const short middle = width / 2;

    const short menu_lines = 7;
    const short error_lines = 5;
    const short mem_lines = 3;
    const short end_lines = height - menu_lines - error_lines - mem_lines;

    // Definindo dimensões e posições das janelas ----------------------------------------------------------------------

    // Menu na parte superior
    WINDOW *win_super_menu = newwin(menu_lines, width, 0, 0);

    // Processo em Execução ao lado da Memória (lado esquerdo)
    WINDOW *win_super_exec_proc = newwin(mem_lines, middle, menu_lines, 0);

    // Estado da Memória abaixo do Menu (lado direito)
    WINDOW *win_super_mem_state = newwin(mem_lines, middle, menu_lines, middle);

    // Log de Erros abaixo do Processo em execução
    WINDOW *win_super_error_log = newwin(error_lines, width, menu_lines + mem_lines, 0);

    // Lista de Processos abaixo do Log de Erros (lado esquerdo)
    WINDOW *win_super_processos = newwin(end_lines, middle, menu_lines + mem_lines + error_lines, 0);

    // Lista de Semáforos abaixo do Log de Erros (lado direito)
    WINDOW *win_super_semaforos = newwin(end_lines, middle, menu_lines + mem_lines + error_lines, middle);


    // Definindo a parte interna das janelas, que é onde escreveremos de fato
    win_menu = derwin(win_super_menu, menu_lines - 2, width - 2, 1, 1);
    win_exec_proc = derwin(win_super_exec_proc, mem_lines - 2, middle - 2, 1, 1);
    win_mem_state = derwin(win_super_mem_state, mem_lines - 2, middle - 2, 1, middle + 1);
    win_error_log = derwin(win_super_error_log, error_lines - 2, width - 2, 1, 1);
    win_processos = derwin(win_super_processos, end_lines - 2, middle - 2, 1, 1);
    win_semaforos = derwin(win_super_semaforos, end_lines - 2, middle - 2, 1, middle + 1);

    // Habilitar scrolling em algumas janelas
    scrollok(win_error_log, TRUE);
    scrollok(win_processos, TRUE);
    scrollok(win_semaforos, TRUE);

    // Desenha as bordas das janelas
    draw_borders(win_super_menu);
    draw_borders(win_super_exec_proc);
    draw_borders(win_super_mem_state);
    draw_borders(win_super_error_log);
    draw_borders(win_super_processos);
    draw_borders(win_super_semaforos);

    // Atualiza as janelas com as bordas desenhadas
    wrefresh(win_super_menu);
    wrefresh(win_super_exec_proc);
    wrefresh(win_super_mem_state);
    wrefresh(win_super_error_log);
    wrefresh(win_super_processos);
    wrefresh(win_super_semaforos);
}

void *menu() {
    while (!encerrar) {
        wclear(win_menu); // limpa a janela
        flushinp();

        mvwprintw(win_menu, 0, 1, " [1] Novo processo");
        mvwprintw(win_menu, 1, 1, " [0] Encerrar");

        mvwprintw(win_menu, 3, 1, "Selecione a operação: ");
        wrefresh(win_menu); // Atualiza a tela com o conteúdo impresso

        const char op = wgetch(win_menu); // Captura a entrada do usuário
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
    }

    return NULL;
}

void *processandoAgora() {
    while (!encerrar) {
        if (!refresh_atual) {
            usleep(500);
            continue;
        }
        refresh_atual = false;

        wclear(win_exec_proc);
        mvwprintw(win_exec_proc, 0, 0, "Processando agora: ");

        pthread_mutex_lock(&mutex_lista_processos); // bloqueia acesso à lista de processos
        if (executando_agora) {
            char nome[61];
            strcpy(nome, executando_agora->nome);
            mvwprintw(win_exec_proc, 0, 20, "%s (%d)", nome, executando_agora->id_seg);
        } else
            mvwprintw(win_exec_proc, 0, 20, "nada");
        pthread_mutex_unlock(&mutex_lista_processos); // libera acesso à lista de processos

        wrefresh(win_exec_proc);
    }
    return NULL;
}

void *informacaoProcessos() {
    while (!encerrar) {
        if (!refresh_proc) {
            usleep(500);
            continue;
        }
        refresh_proc = false;

        wclear(win_processos);

        pthread_mutex_lock(&mutex_lista_processos); // bloqueia acesso à lista de processos
        BCP *aux = head_lista_processos;
        if (!aux)
            mvwprintw(win_processos, 0, 1, "Ainda não há processos no sistema.");
        else {
            char *estados[] = {"PRONTO", "EXECUTANDO", "BLOQUEADO", "TERMINADO"};
            int i = 0;

            while (aux) {
                // monta uma linha com as informações do processo
                char linha[128];
                snprintf(linha, sizeof(linha),
                         "%s (%d), Prioridade: %d   %s",
                         aux->nome, aux->id_seg, aux->prioridade, estados[aux->estado]);

                mvwprintw(win_processos, i++, 1, "%s", linha);

                aux = aux->prox;
            }
        }
        pthread_mutex_unlock(&mutex_lista_processos); // libera acesso à lista de processos

        wrefresh(win_processos);
    }
    return NULL;
}

void *informacaoSemaforos() {
    while (!encerrar) {
        if (!refresh_sem) {
            usleep(500);
            continue;
        }
        refresh_sem = false;

        wclear(win_semaforos);
        mvwprintw(win_semaforos, 0, 1, "Lista de Semáforos sendo utilizados: ");

        pthread_mutex_lock(&mutex_semaforos_globais); // bloqueia acesso à lista de semáforos
        No_Semaforo *aux = semaforos_existentes->head;
        if (!aux)
            mvwprintw(win_semaforos, 1, 6, "Nenhum");
        else {
            int i = 1;

            while (aux) {
                // monta a linha com as informações do semáforo
                char linha[128];
                snprintf(linha, sizeof(linha),
                         "%c | Contador: %d processos",
                         aux->semaforo->nome, aux->semaforo->refcount);

                mvwprintw(win_semaforos, i++, 6, "%s", linha);

                aux = aux->prox;
            }
        }
        pthread_mutex_unlock(&mutex_semaforos_globais); // libera acesso à lista de semáforos

        wrefresh(win_semaforos);
    }
    return NULL;
}

void *informacaoMemoria() {
    while (!encerrar) {
        if (!refresh_mem) {
            usleep(500);
            continue;
        }
        refresh_mem = false;

        wclear(win_mem_state);

        pthread_mutex_lock(&mutex_RAM); // bloqueia acesso à RAM

        const int paginas_ocupadas = RAM->n_paginas_ocupadas;
        const double taxa_ocupacao = (double) paginas_ocupadas / NUMERO_PAGINAS * 100.0;

        mvwprintw(win_mem_state, 0, 1, "Taxa de ocupacao da memoria: %.2f%% (%d/%d)", taxa_ocupacao,
                  paginas_ocupadas, NUMERO_PAGINAS);

        pthread_mutex_unlock(&mutex_RAM); // libera acesso à RAM

        wrefresh(win_mem_state);
    }
    return NULL;
}

BCP *mensagemErroBCP(const char *mensagem, BCP *processo) {
    if (!mensagem || !processo) return NULL;
    static int i = 0;

    wclear(win_error_log);
    mvwprintw(win_error_log, i, 0, "%s", mensagem);
    wrefresh(win_error_log);
    i++;
    freeBCP(processo);
    return NULL;
}
