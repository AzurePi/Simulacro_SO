#include "include/main.h"

int main() {
    setlocale(LC_ALL, "PORTUGUESE");

    // inicialização das variáveis globais
    lista_processos = NULL;
    rodando_agora = NULL;
    num_processos = 0;
    semaforos_existentes = novaListaSemaforos();
    sem_init(&sem_terminal, false, 1);
    sem_init(&sem_CPU, false, 1);
    sem_init(&sem_lista_processos, false, 1);
    relogio = 0.0;
    pthread_attr_init(&atrib);
    pthread_attr_setscope(&atrib, PTHREAD_SCOPE_SYSTEM);

    // criamos uma thread para a exibição do menu
    pthread_t t_menu;

    pthread_create(&t_menu, &atrib, menu, NULL);
    pthread_join(t_menu, NULL);

    processCreate();
    roundRobin();

    // destruímos atributos de thread e semáforo inicializados
    pthread_attr_destroy(&atrib);
    sem_destroy(&sem_terminal);
    sem_destroy(&sem_CPU);
    sem_destroy(&sem_lista_processos);

    // liberamos as memórias alocadas
    freeListaBCP(lista_processos);
    freeBCP(rodando_agora);
    freeListaSemaforo(semaforos_existentes);

    return 0;
}

