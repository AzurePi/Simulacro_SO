#include "include/cpu.h"

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

        sem_post(&sem_lista_processos);

        // se não encontramos um processo para executar, é porque todos os processos terminaram
        if (!executar) {
            sem_post(&sem_CPU);
            break;
        }

        executar->estado = EXECUTANDO;
        rodando_agora = executar;

        // TODO: requisição da memória para o processo a ser executado

        // determinamos o quantum desse processo, proporcional a sua prioridade
        long double quantum = QUANTUM / executar->prioridade;
        long double tempo_executado = 0; // tempo que o processo está sendo executado (não pode exceder o quantum time)

        // processamento dos comandos do processo sendo executado
        Comando *atual = executar->comandos->head;
        bool interromper = false;

        while (atual && tempo_executado < quantum && !interromper) {
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

            // atualiza o relógio com o tempo do processamento
            relogio += t;
            tempo_executado += t;

            // processa o comando
            switch (atual->opcode) {
                case EXEC: // pode ser que o processo não seja executado por inteiro
                    atual->parametro -= (int) t; // retiramos o tempo que já foi executado do parâmetro
                    if (atual->parametro <= 0) { // se após isso, terminamos a execução, apagamos o comando
                        Comando *aux = atual;
                        atual = atual->prox;
                        freeComando(aux);
                    }
                    break;
                case READ: //por enquanto, nada além de liberar o comando
                {
                    Comando *aux = atual;
                    atual = atual->prox;
                    freeComando(aux);
                }
                    break;
                case WRITE: //por enquanto, nada além de liberar o comando
                {
                    Comando *aux = atual;
                    atual = atual->prox;
                    freeComando(aux);
                }
                    break;
                case P: {
                    Semaforo *sem = retrieveSemaphore((char) atual->parametro);
                    semaphoreP(sem, executar);
                    Comando *aux = atual;
                    atual = atual->prox;
                    freeComando(aux);
                }
                    break;
                case V: {
                    Semaforo *sem;
                    sem = retrieveSemaphore((char) atual->parametro);
                    semaphoreV(sem);
                    Comando *aux = atual;
                    atual = atual->prox;
                    freeComando(aux);
                }
                    break;
                case PRINT: //por enquanto, nada além de liberar o comando
                {
                    Comando *aux = atual;
                    atual = atual->prox;
                    freeComando(aux);
                }
                    break;
            }

            // atualiza a cabeça da lista de comandos do processo
            executar->comandos->head = atual;

            // verifica se o quantum foi excedido
            if (tempo_executado >= quantum) {
                executar->estado = PRONTO;
                interromper = true; // interrompe o processamento dos comandos
                sem_post(&sem_CPU); // libera a CPU
            }
        }

        // verifica se todos os comandos já foram executados
        if (executar->comandos->head == NULL)
            executar->estado = TERMINADO;

        sem_post(&sem_CPU); // libera a CPU
    }
    return NULL;
}