#include "include/cpu.h"

void *CPU() {
    while (!encerrar) {
        BCP *executar = buscaBCPExecutar(); // encontra o próximo processo pronto com a maior prioridade

        // se não encontramos um processo para executar, pulamos para a próxima iteração
        if (!executar) continue;

        //se há um processo para executar
        pthread_mutex_lock(&mutex_lista_processos);
        executar->estado = EXECUTANDO;
        executando_agora = executar;
        pthread_mutex_unlock(&mutex_lista_processos);

        sysCall(mem_load_req, executar); // carrega o processo na memória

        processarComandos(executar); // executa os comandos

        // quando terminou o processamento, interrompemos o processo
        InterruptArgs intArgs = {.tipo_interrupcao = FINAL_EXECUCAO, .processo = executar};
        sysCall(process_interrupt, &intArgs);
    }
    return NULL;
}

void processarComandos(BCP *processo) {
    if (!processo) return;

    const long int quantum = QUANTUM / processo->prioridade; // quantum do processo, proporcional à prioridade
    long int tempo_executado = 0; // o tempo que o processo já está executando
    Comando *atual = processo->comandos->head;

    while (atual && tempo_executado < quantum) {
        long int t; // tempo que esse comando leva para ser executado

        /*
         * consideramos, por enquanto, que a leitura/escrita no disco e escrita no terminal levam 100 ut
         * posteriormente, esse tempo será influenciado por DiskRequest(), DiskFinish(), PrintRequest() e PrintFinish()
         *
         * consideramos que todas as operações, exceto EXEC, são atômicas
         */
        switch (atual->opcode) {
        case EXEC: {
            t = atual->parametro;
            if (t > quantum - tempo_executado)
                t = quantum - tempo_executado;

            atual->parametro -= (int)t; // retiramos o tempo que já foi executado do parâmetro
            if (atual->parametro <= 0) {
                // se após isso, terminamos a execução, apagamos o comando
                atual = atual->prox; // passamos para o próximo comando da lista
                removerComando(processo->comandos); // remove o comando da lista de comandos
            }
            break;
        }
        case READ: {
            t = 100;
            atual = atual->prox; // passamos para o próximo comando da lista
            removerComando(processo->comandos); // remove o comando da lista de comandos
            break;
        }
        case WRITE: {
            t = 100;
            atual = atual->prox; // passamos para o próximo comando da lista
            removerComando(processo->comandos); // remove o comando da lista de comandos
            break;
        }
        case P: {
            t = 200;
            Semaforo *sem = retrieveSemaforo(atual->parametro); // encontramos o semáforo sendo chamado

            // criamos uma struct provisória para passar os argumentos para sysCall
            SemaphorePArgs args = {.semaforo = sem, .proc = processo};

            if (sysCall(semaphore_P, &args)) // se o semáforo permite a execução
                atual = atual->prox; // passamos para o próximo comando
            else // se o semáforo bloqueou a execução
                atual = NULL; // interrompemos a execução
            removerComando(processo->comandos); // remove o comando da lista de comandos
            break;
        }
        case V: {
            t = 200;
            Semaforo *sem = retrieveSemaforo(atual->parametro);

            sysCall(semaphore_V, sem);
            atual = atual->prox; // passamos para o próximo comando da lista
            removerComando(processo->comandos); // remove o comando da lista de comandos
            break;
        }
        case PRINT: {
            t = 100;
            atual = atual->prox; // passamos para o próximo comando da lista
            removerComando(processo->comandos);
            break;
        }
        default:
            t = 0;
            break;
        }

        // atualiza o relógio com o tempo do processamento
        relogio += t;
        tempo_executado += t;
        sleep(1);
    }
}
