#include "include/main.h"

int main() {
    setlocale(LC_ALL, "PORTUGUESE");

    initializeGlobals();

    create_and_detach(roundRobin, NULL); // criamos uma thread para execução da CPU em segundo plano

    // criamos uma thread para a exibição do interface -----------------------------------------------------------------
    pthread_attr_t atrib;
    pthread_attr_init(&atrib);
    pthread_attr_setscope(&atrib, PTHREAD_SCOPE_SYSTEM);

    pthread_t t_menu;

    pthread_create(&t_menu, &atrib, interface, NULL);
    pthread_attr_destroy(&atrib);

    pthread_join(t_menu, NULL); // quando o interface é encerrado -----------------------------------------------

    // mostramos ao usuário quanto tempo a execução dos processos demorou na simulação e a lista final de processos
    printf("\nTempo total de execução do simulador: %Lf\n", relogio);

    finalizeGlobals();

    return 0;
}

