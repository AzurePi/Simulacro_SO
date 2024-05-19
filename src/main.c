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
    relogio = 0.0;
    pthread_attr_init(&atrib);
    pthread_attr_setscope(&atrib, PTHREAD_SCOPE_SYSTEM);

    // criamos uma thread para a exibição do menu, e outra para o processamento do simulador
    pthread_t t_menu, t_kernel;

    pthread_create(&t_menu, &atrib, menu, NULL);
    pthread_create(&t_kernel, &atrib, roundRobin, NULL);
    pthread_join(t_kernel, NULL);
    pthread_join(t_menu, NULL);


    // destruímos atributos de thread e semáforo inicializados
    pthread_attr_destroy(&atrib);
    sem_destroy(&sem_terminal);

    // liberamos as memórias alocadas
    free(semaforos_existentes);
    return 0;
}

void *roundRobin() {
    while (1) {
        sem_wait(&sem_CPU);

        BCP *executar = lista_processos; // começamos pensando que o processo a executar é o primeiro da lista

        // econtrar o próximo processo pronto com a maior prioridade
        BCP *temp = lista_processos;
        while (temp) {
            if (temp->estado == PRONTO && (executar == NULL || temp->prioridade > executar->prioridade))
                executar = temp;
            temp = temp->prox;
        }

        // se não encontramos um processo para executar, é porque todos os processos terminaram
        if (!executar) {
            sem_post(&sem_CPU);
            break;
        }

        double quantum = QUANTUN / executar->prioridade;

        // processamento dos comandos do estado atual
        Comando *atual = executar->comandos->head;
        while (atual) {
            double tempo;

            switch (atual->opcode) {
                case EXEC:
                    tempo = atual->parametro;
                    break;
                case P:
                case V:
                    tempo = 200;
                    break;
                default:
                    tempo = 0;
                    break;
            }

            // verifica se o quantum seria excedido
            if (tempo > quantum)
                tempo = quantum;

            // verifica se o quantum foi excedido
            if (relogio + tempo > relogio + quantum) {
                sem_post(&sem_CPU);
                break;
            }

            // atualiza o relógio com a execução
            relogio += tempo;

            // descarta o comando atual
            Comando *aux = atual;
            if (atual->opcode == EXEC) {
                atual->parametro -= (int) tempo;
                if (atual->parametro == 0) {
                    atual = atual->prox;
                    free(aux);
                }
            } else {
                atual = atual->prox;
                free(aux);
            }
            sem_post(&sem_CPU);
        }
    }
    return NULL;
}