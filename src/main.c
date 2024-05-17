#include "include/main.h"

int main() {
    setlocale(LC_ALL, "PORTUGUESE");

    // inicialização das variáveis globais
    rodando_agora = NULL;
    num_processos = 0;
    semaforos_existentes = novaListaSemaforos();
    sem_init(&sem_terminal, false, 1);
    relogio = 0.0;

    //criamos uma thread para a exibição do menu, e outra para o processamento do simulador
    pthread_attr_t atrib;
    pthread_attr_init(&atrib);
    pthread_attr_setscope(&atrib, PTHREAD_SCOPE_SYSTEM);

    pthread_t t_menu, t_kernel;

    pthread_create(&t_menu, &atrib, menu, NULL);
    pthread_create(&t_kernel, &atrib, kernel, NULL);
    pthread_join(t_kernel, NULL);
    pthread_join(t_menu, NULL);

    // liberamos as memórias alocadas
    free(semaforos_existentes);
    return 0;
}


void *kernel() {

}