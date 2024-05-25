#include "include/main.h"

int main() {
    setlocale(LC_ALL, "PORTUGUESE");

    // inicialização das variáveis globais
    head_lista_processos = NULL;
    rodando_agora = NULL;
    semaforos_existentes = novaListaSemaforos();
    inicializarRAM();
    pthread_mutex_init(&mutex_terminal, NULL);
    pthread_mutex_init(&mutex_CPU, NULL);
    pthread_mutex_init(&mutex_RAM, NULL);
    pthread_mutex_init(&mutex_lista_processos, NULL);
    relogio = 0.0;
    pthread_attr_init(&atrib);
    pthread_attr_setscope(&atrib, PTHREAD_SCOPE_SYSTEM);

    // criamos uma thread para a exibição do menu
    pthread_t t_menu;

    pthread_create(&t_menu, &atrib, menu, NULL);
    pthread_join(t_menu, NULL);

    // mostramos ao usuário quanto tempo a execução dos processos demorou na simulação
    printf("Tempo total de execução do simulador: %Lf", relogio);

    // destruímos atributos de thread e semáforo inicializados
    pthread_attr_destroy(&atrib);
    pthread_mutex_destroy(&mutex_terminal);
    pthread_mutex_destroy(&mutex_CPU);
    pthread_mutex_destroy(&mutex_RAM);
    pthread_mutex_destroy(&mutex_lista_processos);

    // liberamos as memórias alocadas
    freeRAM();
    freeListaSemaforo(semaforos_existentes);
    freeBCP(rodando_agora);
    freeListaBCP(head_lista_processos);

    return 0;
}