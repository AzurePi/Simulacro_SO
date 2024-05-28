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

    pthread_join(t_menu, NULL); // quando o interface é encerrado

    pthread_mutex_lock(&mutex_CPU); // bloqueamos a CPU (para garantir que o processamento se encerre)

    // mostramos ao usuário quanto tempo a execução dos processos demorou na simulação
    printf("Tempo total de execução do simulador: %Lf", relogio);

    // destruímos atributos de thread e semáforo inicializados

    pthread_attr_destroy(&atrib);
    pthread_mutex_destroy(&mutex_IO);
    pthread_mutex_destroy(&mutex_CPU);
    pthread_mutex_destroy(&mutex_RAM);
    pthread_mutex_destroy(&mutex_lista_processos);

    // liberamos as memórias alocadas
    freeRAM();
    freeListaSemaforo(semaforos_existentes);
    freeBCP(executando_agora);
    freeListaBCP(head_lista_processos);

    return 0;
}

