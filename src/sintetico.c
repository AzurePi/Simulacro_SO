#include "include/sintetico.h"

BCP *novoBCP() {
    BCP *new = malloc(sizeof(BCP));
    if (!new) {
        sem_wait(&sem_terminal);
        printf(ERROR"falha na alocação de memória do BCP"CLEAR);
        sleep(2);
        sem_post(&sem_terminal);
        return NULL;
    }
    new->estado = PRONTO; //diz que o programa está pronto para ser executado
    new->semaforos = novaListaSemaforos();
    new->comandos = novaFilaComandos();
    new->prox = NULL;
    return new;
}

void freeBCP(BCP *bcp) {
    freeListaSemaforo(bcp->semaforos);
    freeListaComandos(bcp->comandos);
    free(bcp);
}

void freeListaBCP(BCP *bcp) {
    BCP *temp;
    while (bcp != NULL) {
        temp = bcp;
        bcp = bcp->prox;
        freeBCP(temp);
    }
}

BCP *lerProgramaSintetico(FILE *programa) {
    if (!programa) return NULL; // se não há programa para ler, retorna NULL

    BCP *processo = novoBCP();
    if (!processo) return NULL; // se um novo BCP não pôde ser criado, retorna NULL

    // lê os campos no programa sintético; se houver erro na leitura, informa que há problema no arquivo
    if (!fscanf(programa, "%60s\n", processo->nome) ||
        !fscanf(programa, "%d\n", &processo->id_seg) ||
        !fscanf(programa, "%d\n", &processo->prioridade) ||
        !fscanf(programa, "%d\n", &processo->tamanho_seg)) {
        sem_wait(&sem_terminal);
        printf(ERROR"programa sintético contém erro no cabeçalho"CLEAR);
        sleep(2);
        sem_post(&sem_terminal);
        freeBCP(processo);
        return NULL;
    }

    // se a prioridade for menor do que 1, há um erro
    if (processo->prioridade < 1) {
        sem_wait(&sem_terminal);
        printf(ERROR"prioridade do programa não pode ser um número menor que 1"CLEAR);
        sleep(2);
        sem_post(&sem_terminal);
        freeBCP(processo);
        return NULL;
    }

    char semaforos[11]; //não uma string, mas um vetor de caracteres
    char s;
    int i = 0;

    //enquanto não chegamos no fim da linha e há caracteres para ler e não ultrapassamos o máximo de 10 semáforos
    while ((s = (char) fgetc(programa)) != '\n' && s != EOF && i < 10) {
        if (s != ' ') {
            semaforos[i] = s; //guardamos o s
            i++;
        }
    }
    semaforos[i] = '\0'; //finaliza a string

    // para cada semáforo guardado, cria um novo semáforo, e coloca na lista do BCP
    for (int j = 0; j < i; j++) {
        Semaforo *t = novoSemaforo(semaforos[j]);
        inserirSemaforo(t, processo->semaforos);
    }

    //lê uma linha em branco
    while ((s = (char) fgetc(programa)) != '\n' && s != EOF);

    //lê cada um dos comandos do processo; guarda ele em uma lista em que cada elemento tem um código de operação e um parâmetro
    if (!processo->comandos) {
        sem_wait(&sem_terminal);
        printf(ERROR"falha na criação de lista de comandos para o programa"CLEAR);
        sleep(2);
        sem_post(&sem_terminal);
        freeBCP(processo);
        return NULL;
    }

    char buffer[11];

    //lê cada comando como uma string e armazena no buffer
    while (fscanf(programa, " %[^\n]", buffer) == 1) {
        char buffer_operacao[6], buffer_parametro[10];
        OPCODE opcode = -1;
        int parametro;

        //se estamos lidando com as operações de semáforo, a string terá formato "operação(parâmetro)"
        if (buffer[0] == 'P' || buffer[0] == 'V') {
            sscanf(buffer, "%[^(](%[^)])", buffer_operacao, buffer_parametro);

            if (buffer_operacao[0] == 'P')
                opcode = P;
            else if (buffer_operacao[0] == 'V')
                opcode = V;

            parametro = (int) buffer_parametro[0];
        } else { //se estamos lidando com as demais operações, a string terá formato "operação parâmetro"
            sscanf(buffer, "%s %s", buffer_operacao, buffer_parametro);

            if (strcmp(buffer_operacao, "exec") == 0)
                opcode = EXEC;
            else if (strcmp(buffer_operacao, "read") == 0)
                opcode = READ;
            else if (strcmp(buffer_operacao, "write") == 0)
                opcode = WRITE;
            else if (strcmp(buffer_operacao, "print") == 0)
                opcode = PRINT;

            parametro = atoi(buffer_parametro);
        }

        if (opcode == -1) {
            sem_wait(&sem_terminal);
            printf(ERROR"comando não reconhecido no programa sintético");
            sleep(2);
            sem_post(&sem_terminal);
            freeBCP(processo);
            return NULL;
        }

        Comando *comando = novoComando(opcode, parametro);
        inserirComando(comando, processo->comandos);
    }
    return processo;
}

Comando *novoComando(OPCODE opcode, int parametro) {
    Comando *new = malloc(sizeof(Comando));
    if (!new) return NULL; // se a alocação de memória falhou
    new->opcode = opcode;
    new->parametro = parametro;
    new->prox = NULL;
    return new;
}

void freeComando(Comando *comando) {
    free(comando);
}

Fila_Comandos *novaFilaComandos() {
    Fila_Comandos *new = malloc(sizeof(Fila_Comandos));
    if (!new) return NULL; // se a alocação de memória falhou
    new->head = NULL;
    new->tail = NULL;
    return new;
}

void freeListaComandos(Fila_Comandos *comandos) {
    Comando *temp;
    while (comandos->head != NULL) {
        temp = comandos->head;
        comandos->head = comandos->head->prox;
        freeComando(temp);
    }
    free(comandos);
}

void inserirComando(Comando *comando, Fila_Comandos *fila) {
    if (!fila || !comando) return; // se os parâmetros são ponteiros nulos

    if (fila->head == NULL) {
        fila->head = comando;
        fila->tail = comando;
    } else {
        fila->tail->prox = comando;
        fila->tail = comando;
    }
}

void removerComando(Fila_Comandos *fila) {
    if (!fila) return; //se não há fila
    if (!fila->head) return; // se a fila está vazia

    Comando *aux = fila->head;
    fila->head = fila->head->prox;
    freeComando(aux);
}

void inserirSemaforo(Semaforo *semaforo, Lista_Semaforos *lista) {
    if (!lista || !semaforo) return;
    semaforo->prox = lista->head;
    lista->head = semaforo;
}

void process_sleep(BCP *proc) {
    if (!proc) return; // se o processo passado não existe
    proc->estado = BLOQUEADO;

    //TODO: mandar o processo para o final da lista
}

void process_wakeup(BCP *proc) {
    if (!proc) return;
    proc->estado = PRONTO;
}
