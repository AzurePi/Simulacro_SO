#include "include/cpu.h"

// TODO: fazer o roundRobin funcionar em paralelo ao interface ( while(1),

_Noreturn void *roundRobin() {
    while (1) {
        pthread_mutex_lock(&mutex_CPU); // bloqueamos o uso da CPU

        // encontrar o próximo processo pronto com a maior prioridade
        while (!head_lista_processos); // espera até haver um BCP na lista de processos
        BCP *executar = buscaBCPExecutar();

        // se não encontramos um processo para executar, é porque não há processos prontos
        if (!executar)
            pthread_mutex_unlock(&mutex_CPU); // libera a CPU
        else { //se tem um processo para executar
            executar->estado = EXECUTANDO;
            executando_agora = executar;

            sysCall(mem_load_req, executar); // carrega o processo na memória

            processarComandos(executar); // executa os comandos

            sysCall(process_interrupt, NULL);

            sysCall(mem_load_finish, executar); // descarrega o processo da memória

            // se todos os comandos já foram executados
            if (executar->comandos->head == NULL)
                sysCall(process_finish, executar); // finaliza o processo

            pthread_mutex_unlock(&mutex_CPU); // libera a CPU
        }
    }
}

void processarComandos(BCP *processo) {
    long double quantum = QUANTUM / processo->prioridade; // quantum do processo, proporcional à prioridade
    long double tempo_executado = 0; // o tempo que o processo já está executando
    Comando *atual = processo->comandos->head;

    while (atual && tempo_executado < quantum) {
        long double t; // tempo que esse comando leva para ser executado

        // dependendo da operação, o t é contado de uma forma diferente
        /* consideramos, por enquanto, que a leitura/escrita no disco e escrita no terminal são instantâneos
         * posteriormente, esse t será influenciado por DiskRequest(), DiskFinish(), PrintRequest() e PrintFinish()
         */
        switch (atual->opcode) {
            case EXEC:
                t = atual->parametro;
                break;
            case READ:
                t = 0;
                break;
            case WRITE:
                t = 0;
                break;
            case P:
            case V:
                t = 200;
                break;
            default:
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
                    atual = atual->prox; // passamos para o próximo comando da lista
                    removerComando(processo->comandos); // remove o comando da lista de comandos
                }
                break;
            case READ:  //por enquanto, nada além de liberar o comando
                atual = atual->prox; // passamos para o próximo comando da lista
                removerComando(processo->comandos); // remove o comando da lista de comandos
                break;
            case WRITE: //por enquanto, nada além de liberar o comando
                atual = atual->prox; // passamos para o próximo comando da lista
                removerComando(processo->comandos); // remove o comando da lista de comandos
                break;
            case P: {
                Semaforo *sem = retrieveSemaforo((char) atual->parametro);

                // criamos uma struct provisória para passar os argumentos para sysCall
                SemaphorePArgs *args = malloc(sizeof (SemaphorePArgs));
                args->semaforo = sem;
                args->proc = processo;

                if (sysCall(semaphore_P, args)) // se o semáforo permite a execução
                    atual = atual->prox; // passamos para o próximo comando
                else // se o semáforo bloqueou a execução
                    atual = NULL; // interrompemos a execução
                removerComando(processo->comandos); // remove o comando da lista de comandos
                free(args); // libera a estrutura provisória
            }
                break;
            case V: {
                Semaforo *sem = retrieveSemaforo((char) atual->parametro);
                sysCall(semaphore_V, sem);
                atual = atual->prox; // passamos para o próximo comando da lista
                removerComando(processo->comandos); // remove o comando da lista de comandos
            }
                break;
            case PRINT: //por enquanto, nada além de liberar o comando
                atual = atual->prox; // passamos para o próximo comando da lista
                removerComando(processo->comandos);
                break;
        }
    }
}

