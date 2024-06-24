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

    create_and_detach(CPU, NULL); // criamos uma thread para execução da CPU em segundo plano
    create_and_detach(discoElevador, NULL); // criamos uma thread para execução do disco em segundo plano

    // criamos uma thread para a exibição do interface -----------------------------------------------------------------
    pthread_attr_t atrib;
    pthread_attr_init(&atrib);
    pthread_attr_setscope(&atrib, PTHREAD_SCOPE_SYSTEM);

    pthread_t t_menu;

    pthread_create(&t_menu, &atrib, interface, NULL);
    pthread_attr_destroy(&atrib);

    pthread_join(t_menu, NULL); // quando o interface é encerrado -----------------------------------------------

    // mostramos ao usuário quanto tempo a execução dos processos demorou na simulação e a lista final de processos
    printf("\nTempo total de execução do simulador: " BOLD ITALIC "%ld" CLEAR " ut\n", relogio);
    limpar_buffer();

    finalizeGlobals();

    return 0;
}
