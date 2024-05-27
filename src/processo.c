#include "include/processo.h"

BCP *novoBCP() {
    BCP *new = malloc(sizeof(BCP));
    if (!new) {
        pthread_mutex_lock(&mutex_terminal);
        printf(ERROR "falha na alocação de memória do BCP" CLEAR);
        sleep(2);
        pthread_mutex_unlock(&mutex_terminal);
        return NULL;
    }
    new->semaforos = novaListaSemaforos();
    if (!new->semaforos) return NULL;
    new->comandos = novaFilaComandos();
    if (!new->comandos) return NULL;
    return new;
}

void inserirBCP(BCP *new) {
    if (!head_lista_processos || head_lista_processos->prioridade < new->prioridade) {
        new->prox = head_lista_processos;
        head_lista_processos = new;
        return;
    }

    BCP *atual = head_lista_processos;
    while (atual->prox && atual->prox->prioridade >= new->prioridade)
        atual = atual->prox;

    new->prox = atual->prox;
    atual->prox = novoBCP();
}

void inserirBCPFinal(BCP *processo) {
    // se a lista estiver, inserimos como head
    if (!head_lista_processos)
        head_lista_processos = processo;
    else {
        // vamos do começo até o final
        BCP *aux = head_lista_processos;
        while (aux->prox != NULL)
            aux = aux->prox;
        aux->prox = processo;
    }
}

BCP *buscaBCPExecutar() {
    pthread_mutex_lock(&mutex_lista_processos); // bloqueia o acesso a lista de processos

    BCP *anterior = NULL;
    BCP *aux = head_lista_processos;
    BCP *executar = NULL;
    BCP *anterior_executar = NULL;

    while (aux) {
        if (aux->estado == PRONTO && (executar == NULL || aux->prioridade > executar->prioridade)) {
            executar = aux;
            anterior_executar = anterior;
        }
        anterior = aux;
        aux = aux->prox;
    }

    // remover o processo executar da lista
    if (executar) {
        if (anterior_executar)
            anterior_executar->prox = executar->prox;
        else
            head_lista_processos = executar->prox;
    }

    pthread_mutex_lock(&mutex_lista_processos);

    return executar;
}

void freeBCP(BCP *bcp) {
    if (!bcp) return;
    freeListaSemaforo(bcp->semaforos);
    freeFilaComandos(bcp->comandos);
    free(bcp);
}

void freeListaBCP(BCP *bcp_head) {
    if (!bcp_head) return;
    BCP *temp;
    while (bcp_head != NULL) {
        temp = bcp_head;
        bcp_head = bcp_head->prox;
        freeBCP(temp);
    }
}

bool lerCabecalho(FILE *programa, BCP *bcp) {
    if (fscanf(programa, "%60s\n", bcp->nome) != 1 ||
        fscanf(programa, "%d\n", &bcp->id_seg) != 1 ||
        fscanf(programa, "%d\n", &bcp->prioridade) != 1 ||
        fscanf(programa, "%d\n", &bcp->tamanho_seg) != 1)
        return false; // alguma parte da leitura falhou
    return true; // toda a leitura funcionou
}

void lerSemaforos(FILE *programa, BCP *bcp) {
    char semaforos[11]; //não uma string, mas um vetor de caracteres
    char s;
    char i = 0; // contador de tamanaho pequeno

    //enquanto não chegamos no fim da linha e há caracteres para ler e não ultrapassamos o máximo de 10 semáforos
    while ((s = (char) fgetc(programa)) != '\n' && s != EOF && i < 10) {
        if (s != ' ')
            semaforos[i++] = s; //guardamos o s e passamos para a próxima posição do vetor
    }
    semaforos[i] = '\0'; //finaliza a string

    // para cada semáforo guardado, cria um novo semáforo, e coloca na lista do BCP
    for (char j = 0; j < i; j++) {
        Semaforo *t = novoSemaforo(semaforos[j]);
        inserirSemaforo(t, bcp->semaforos);
    }
}

bool lerComandos(FILE *programa, BCP *bcp) {
    char buffer[11];

    //lê cada comando como uma string e armazena no buffer
    while (fscanf(programa, " %[^\n]", buffer) == 1) {
        char buffer_operacao[6], buffer_parametro[10];
        OPCODE opcode;
        int parametro;

        //se estamos lidando com as operações de semáforo, a string terá formato "operação(parâmetro)"
        if (buffer[0] == 'P' || buffer[0] == 'V') {
            sscanf(buffer, "%[^(](%[^)])", buffer_operacao, buffer_parametro);
            opcode = (buffer_operacao[0] == 'P' ? P : V);
            parametro = (int) buffer_parametro[0];
        } else { //se estamos lidando com as demais operações, a string terá formato "operação parâmetro"
            sscanf(buffer, "%s %s", buffer_operacao, buffer_parametro);

            if (strcmp(buffer_operacao, "exec") == 0) opcode = EXEC;
            else if (strcmp(buffer_operacao, "read") == 0) opcode = READ;
            else if (strcmp(buffer_operacao, "write") == 0) opcode = WRITE;
            else if (strcmp(buffer_operacao, "print") == 0) opcode = PRINT;
            else
                return false;

            parametro = atoi(buffer_parametro);
        }

        Comando *comando = novoComando(opcode, parametro);
        if (!comando) return false;
        inserirComando(comando, bcp->comandos);
    }
    return true;
}

BCP *lerProgramaSintetico(FILE *programa) {
    if (!programa) return NULL; // se não há programa para ler, retorna NULL

    BCP *processo = novoBCP();
    if (!processo) return NULL; // se um novo BCP não pôde ser criado, retorna NULL

    // lê os campos no programa sintético; se houver erro na leitura, informa que há problema no arquivo
    if (!lerCabecalho(programa, processo))
        return mensagemErroBCP("programa sintético contém erro no cabeçalho", processo);

    // se a prioridade for menor do que 1, há um erro
    if (processo->prioridade < 1)
        return mensagemErroBCP("prioridade do programa não pode ser um número menor que 1", processo);

    // calcula o número de páginas da memória que precisam ser utilizadas para esse processo
    processo->n_paginas_usadas = (processo->tamanho_seg + TAMANHO_PAGINA - 1) / TAMANHO_PAGINA;

    // lê a linha que contém os semáforos
    lerSemaforos(programa, processo);

    // ignora uma linha em branco
    while (fgetc(programa) != '\n' && !feof(programa));
    if (!lerComandos(programa, processo))
        return mensagemErroBCP("programa sintético contém erro nos comandos", processo);

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

void freeFilaComandos(Fila_Comandos *comandos) {
    if (!comandos) return;
    Comando *temp;
    while (comandos->head != NULL) {
        temp = comandos->head;
        comandos->head = comandos->head->prox;
        freeComando(temp);
    }
    free(comandos);
}

void inserirSemaforo(Semaforo *semaforo, Lista_Semaforos *lista) {
    if (!lista || !semaforo) return;
    semaforo->prox = lista->head;
    lista->head = semaforo;
}

void process_sleep(BCP *processo) {
    if (!processo) return; // se o processo passado não existe
    processo->estado = BLOQUEADO;

    //freeListaBCP(processo); TODO: isso tem que ser uma remoção do processo do lugar em que ele está na fila
    inserirBCPFinal(processo);
}

void process_wakeup(BCP *processo) {
    if (!processo) return;
    processo->estado = PRONTO;
    inserirBCP(processo);
}
