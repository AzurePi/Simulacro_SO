#include "main.h"
#include "interface.h"

volatile int num_processos = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int main() {
    setlocale(LC_ALL, "PORTUGUESE");

    pthread_attr_t atrib;
    pthread_attr_init(&atrib);
    pthread_attr_setscope(&atrib, PTHREAD_SCOPE_SYSTEM);

    pthread_t t_menu, t_kernel; //criamos uma thread para a exibição do menu, e outra para o processamento do simulador

    pthread_create(&t_menu, &atrib, menu, NULL);
    pthread_create(&t_kernel, &atrib, kernel, NULL);
    pthread_join(t_kernel, NULL);
    pthread_join(t_menu, NULL);

    return 0;
}

void roundRobin() {
    int i, j;

    while (1) {
        pthread_mutex_lock(&mutex);
        int executando = -1;

        //encontra o próximo processo pronto com a maior prioridade
        for (i = 0; i < num_processos; i++) {
        }
        break;
    }
}

void *kernel() {

}