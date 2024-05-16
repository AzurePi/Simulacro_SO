#include "interface.h"

void *menu() {
    char op;

    pthread_attr_t atrib;
    pthread_attr_init(&atrib);
    pthread_attr_setscope(&atrib, PTHREAD_SCOPE_PROCESS);

    pthread_t t_novo, t_info_proc, t_info_mem;

    do {
        sem_wait(&sem_terminal);

        CLEAR_SCREEN
        printf("╔══════════════════════════════════════╗\n");
        printf("║   Simulador de Sistema Operacional   ║\n");
        printf("╟──────────────────────────────────────╢\n");
        printf("║ [1] Novo processo                    ║\n");
        printf("║ [2] Ver informações de um processo   ║\n");
        printf("║ [3] Ver informações da memória       ║\n");
        printf("║ [0] Encerrar                         ║\n");
        printf("╚══════════════════════════════════════╝\n");

        printf("\n Processando agora: ");
        if (rodando_agora)
            printf("%s (%d)\n", rodando_agora->nome, rodando_agora->id_seg);
        else
            printf("nada\n");

        sem_post(&sem_terminal);

        printf("Selecione a operação: ");
        scanf(" %c", &op);

        switch (op) {
            case '0':
                printf("Encerrando programa.");
                sleep(3);
                break;
            case '1':
                pthread_create(&t_novo, &atrib, processCreate, NULL);
                pthread_join(t_novo, NULL);
                break;
            case '2':
                pthread_create(&t_info_proc, &atrib, informacaoProcesso, NULL);
                pthread_join(t_info_proc, NULL);
                break;
            case '3':
                pthread_create(&t_info_mem, &atrib, informacaoMemoria, NULL);
                pthread_join(t_info_mem, NULL);
                break;
            default:
                printf("Opção inválida!\n");
                sleep(3);
                break;
        }
    } while (op != '0');
}

void *informacaoProcesso() {

}

void *informacaoMemoria() {

}