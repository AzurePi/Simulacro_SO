#include "main.h"


volatile int num_processos = 0;
BCP *rodando_agora = NULL;
volatile long double relogio = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

sem_t sem;


int main() {
    setlocale(LC_ALL, "PORTUGUESE");

    pthread_attr_t atrib;
    pthread_attr_init(&atrib);
    pthread_attr_setscope(&atrib, PTHREAD_SCOPE_SYSTEM);

    pthread_t t_menu, t_kernel; //criamos uma thread para a exibição do menu, e outra para o processamento do simulador

    pthread_create(&t_menu, &atrib, menu, NULL);
    pthread_create(&t_kernel, NULL, &kernel, NULL);
    pthread_join(t_kernel, NULL);
    pthread_join(t_menu, NULL);

    return 0;
}

void *menu() {
    char op;
    char filename[101];

    do {
        sem_wait(&sem);

        CLEAR_SCREEN;
        printf("╔══════════════════════════════════════╗\n");
        printf("║   Simulador de Sistema Operacional   ║\n");
        printf("╟──────────────────────────────────────╢\n");
        printf("║ [1] Novo processo                    ║\n");
        printf("║ [2] Ver informações de um processo   ║\n");
        printf("║ [3] Ver informações da memória       ║\n");
        printf("║ [0] Encerrar                         ║\n");
        printf("╚══════════════════════════════════════╝\n");

        printf("\n Processando agora: ");
        if (rodando_agora) printf("%s (%d)\n", rodando_agora->nome, rodando_agora->id_seg);
        else printf("nada\n");

        sem_post(&sem);

        printf("Selecione a operação: ");
        scanf(" %c", &op);

        switch (op) {
            case '0':
                printf("Encerrando programa.");
                sleep(3);
                break;
            case '1':
                sem_wait(&sem);
                printf("Nome do programa: ");
                scanf(" %108[^\n]", filename);
                sem_post(&sem);
                processCreate(filename);
                break;
            case '2':
                informacaoProcesso();
                break;
            case '3':
                informacaoMemoria();
                break;
            default:
                printf("Opção inválida!\n");
                sleep(3);
                break;
        }
    } while (op != '0');
}

void informacaoProcesso() {

}

void informacaoMemoria() {

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