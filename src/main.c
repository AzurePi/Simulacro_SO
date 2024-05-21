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

    // criamos uma thread para a exibição do menu, e outra para o processamento do simulador
    pthread_t t_menu, t_kernel;

    pthread_create(&t_menu, &atrib, menu, NULL);
    pthread_create(&t_kernel, &atrib, roundRobin, NULL);
    pthread_join(t_kernel, NULL);
    pthread_join(t_menu, NULL);

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

void *roundRobin() {
    while (1) {
        sem_wait(&sem_CPU); //dizemos que a CPU está sendo utilizada

        BCP *executar = NULL;

        sem_wait(&sem_lista_processos); // bloqueia o acesso a lista de processos

        // econtrar o próximo processo pronto com a maior prioridade
        BCP *temp = lista_processos;
        while (temp) {
            if (temp->estado == PRONTO && (executar == NULL || temp->prioridade > executar->prioridade))
                executar = temp;
            temp = temp->prox;
        }

        // se não encontramos um processo para executar, é porque todos os processos terminaram
        if (!executar) {
            sem_post(&sem_lista_processos);
            sem_post(&sem_CPU);
            break;
        }

        executar->estado = EXECUTANDO;
        rodando_agora = executar;
        sem_post(&sem_lista_processos);

        // determinamos o quantum desse processo, proporcional a sua prioridade
        long double quantum = QUANTUM / executar->prioridade;
        long double tempo_executado = 0; // tempo que o processo está sendo executado (não pode exceder o quantum time)

        // processamento dos comandos do processo sendo executado
        Comando *atual = executar->comandos->head;
        while (atual) {
            long double t; // tempo que esse comando leva para ser executado

            // dependendo da operação, o t é contado de uma forma diferente
            switch (atual->opcode) {
                case EXEC:
                    t = atual->parametro;
                    break;
                case P:
                case V:
                    t = 200;
                    break;
                default:
                    /*
                     * consideramos, por enquanto, que a leitura/escrita no disco e escrita no terminal são instantâneos
                     * posteriormente, esse t será influenciado por DiskRequest(), DiskFinish(), PrintRequest() e PrintFinish()
                     */
                    t = 0;
                    break;
            }

            // consideramos que os comandos são atômicos, com exceção de EXEC
            // verifica se o comando é EXEC, e se o t desse comando seria maior que o tempo restante para execução
            if (atual->opcode == EXEC && t > (quantum - tempo_executado))
                t = quantum - tempo_executado;

            // atualiza o relógio com a execução
            relogio += t;
            tempo_executado += t;

            // se o comando atual é EXEC, pode ser que o processo não tenha sido executado por inteiro
            if (atual->opcode == EXEC) {
                atual->parametro -= (int) t; // retiramos o tempo que já foi executado do parâmetro
                if (atual->parametro <= 0) { // se após isso, terminamos a execução, apagamos o comando
                    Comando *aux = atual;
                    atual = atual->prox;
                    freeComando(aux);
                } else {
                    //libera a CPU e volta ao estado de PRONTO
                    executar->estado = PRONTO;
                    sem_post(&sem_CPU);
                    break; // interrompemos a execução
                }
            } else {
                // descarta o comando atual após sua execução
                Comando *aux = atual;
                atual = atual->prox;
                freeComando(aux);
            }

            // atualiza a cabeça da lista de comandos do processo
            executar->comandos->head = atual;

            // verifica se o quantum foi excedido
            if (tempo_executado >= quantum) {
                executar->estado = PRONTO;
                sem_post(&sem_CPU);
                break; // interrompemos a execução
            }
        }

        // verifica se todos os comandos já foram executados
        if (executar->comandos->head == NULL)
            executar->estado = TERMINADO;

        sem_post(&sem_CPU);
    }
    return NULL;
}