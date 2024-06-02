#include "include/main.h"

/*
 * Projeto de simulador de Sistema Operacional
 *
 * Heloísa Silveira Bula
 * Lucas Xavier de Britto Pereira
 * Pedro benedicto de Melo Cardana
 */

int main() {
    setlocale(LC_ALL, "PORTUGUESE");

    initializeGlobals();
    initializeInterface();

    create_and_detach(CPU, NULL); // uma thread para execução da CPU em segundo plano

    // threads para controle das múltiplas janelas da interface
    create_and_detach(menu, NULL); // exibição do menu com input do usuário
    create_and_detach(processandoAgora, NULL); // exibição do processo atualmente em execução
    create_and_detach(informacaoProcessos, NULL); // exibição da lista de processos
    create_and_detach(informacaoSemaforos, NULL); // exibição da lista de semáforos
    create_and_detach(informacaoMemoria, NULL); // exibição do status da memória

    while (!encerrar); // esperamos enquanto o programa não é encerrado.

    endwin();

    // mostramos ao usuário quanto tempo a execução dos processos demorou na simulação e a lista final de processos
    printw("Tempo total de execução do simulador: %ld ut\n", relogio);

    finalizeGlobals();

    return 0;
}

