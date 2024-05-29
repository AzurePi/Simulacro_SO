#include "include/main.h"

int main() {
    setlocale(LC_ALL, "PORTUGUESE");

    initializeGlobals();

    pthread_attr_t atrib;
    pthread_attr_init(&atrib);
    pthread_attr_setscope(&atrib, PTHREAD_SCOPE_SYSTEM);

    // criamos uma thread para a exibição do interface
    pthread_t t_menu, t_CPU;

    pthread_create(&t_menu, &atrib, interface, NULL);
    pthread_create(&t_CPU, &atrib, roundRobin, NULL);
    pthread_detach(t_CPU);

    pthread_attr_destroy(&atrib);

    pthread_join(t_menu, NULL); // quando o interface é encerrado

    // mostramos ao usuário quanto tempo a execução dos processos demorou na simulação
    printf("\nTempo total de execução do simulador: %Lf", relogio);

    finalizeGlobals();

    return 0;
}

