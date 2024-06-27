#include "include/processo.h"

#include <ctype.h>

BCP *novoBCP() {
    BCP *new = malloc(sizeof(BCP));
    if (!new) {
        pthread_mutex_lock(&mutex_IO);
        puts(ERROR "falha na alocação de memória do processo" CLEAR);
        sleep(2);
        pthread_mutex_unlock(&mutex_IO);
        return NULL;
    }
    new->semaforos = novaListaSemaforos();
    new->comandos = novaFilaComandos();
    if (!new->comandos) {
        free(new);
        return NULL;
    }
    return new;
}

void inserirBCP(BCP *new) {
    if (!new)
        return;

    pthread_mutex_lock(&mutex_lista_processos); // bloqueia acesso à lista de processos
    if (!head_lista_processos || head_lista_processos->prioridade < new->prioridade) {
        new->prox = head_lista_processos;
        head_lista_processos = new;
        pthread_mutex_unlock(&mutex_lista_processos); // libera acesso á lista de processos
        return;
    }

    BCP *aux = head_lista_processos;
    while (aux->prox && aux->prox->prioridade >= new->prioridade)
        aux = aux->prox;

    new->prox = aux->prox;
    aux->prox = new;
    pthread_mutex_unlock(&mutex_lista_processos); // libera acesso á lista de processos
}

void inserirBCPFinal(BCP *processo) {
    if (!processo)
        return; // se não há processo para inserir

    pthread_mutex_lock(&mutex_lista_processos); // bloqueia acesso à lista de processos
    if (!head_lista_processos) {
        // se a lista estiver vazia, inserimos como head
        head_lista_processos = processo;
        pthread_mutex_unlock(&mutex_lista_processos); // libera acesso à lista de processos
        return;
    }

    // vamos do começo até o final
    BCP *aux = head_lista_processos;
    while (aux->prox != NULL)
        aux = aux->prox;
    aux->prox = processo;
    pthread_mutex_unlock(&mutex_lista_processos); // libera acesso á lista de processos
}

BCP *buscaBCPExecutar() {
    pthread_mutex_lock(&mutex_lista_processos); // bloqueia o acesso à lista de processos
    if (!head_lista_processos) {
        pthread_mutex_unlock(&mutex_lista_processos); // desbloqueia o acesso à lista de processos
        return NULL;
    }

    BCP *aux = head_lista_processos;
    BCP *executar = NULL;

    while (aux) {
        if (aux->estado == PRONTO && (executar == NULL || aux->prioridade < executar->prioridade))
            executar = aux;
        aux = aux->prox;
    }

    pthread_mutex_unlock(&mutex_lista_processos); // desbloqueia o acesso à lista de processos
    return executar;
}

void freeBCP(BCP *bcp) {
    if (!bcp)
        return;
    freeFilaComandos(bcp->comandos);
    free(bcp);
}

void freeListaBCP(BCP *bcp_head) {
    if (!bcp_head)
        return;
    while (bcp_head) {
        BCP *temp = bcp_head;
        bcp_head = bcp_head->prox;
        freeBCP(temp);
    }
}

bool lerCabecalho(FILE *programa, BCP *bcp) {
    if (!programa || !bcp)
        return false;

    if (fscanf(programa, "%60s\n", bcp->nome) != 1 ||
        fscanf(programa, "%d\n", &bcp->id_seg) != 1 ||
        fscanf(programa, "%d\n", &bcp->prioridade) != 1 ||
        fscanf(programa, "%d\n", &bcp->tamanho_seg) != 1)
        return false; // alguma parte da leitura falhou

    if (bcp->tamanho_seg <= 0 || bcp->prioridade <= 0) // validação de informações
        return false;

    return true; // toda a leitura funcionou
}

void lerSemaforos(FILE *programa, BCP *bcp) {
    if (!programa || !bcp)
        return;

    char semaforos[11]; //não uma string, mas um vetor de caracteres
    char s;
    char i = 0; // contador de tamanho pequeno

    //enquanto não chegamos no fim da linha e há caracteres para ler e não ultrapassamos o máximo de 10 semáforos
    while ((s = (char)fgetc(programa)) != '\n' && s != EOF && i < 10) {
        if (isalpha(s))
            semaforos[i++] = s; //guardamos o s e passamos para a próxima posição do vetor
    }
    semaforos[i] = '\0'; //finaliza a string

    // para cada semáforo guardado, cria um novo semáforo, e coloca na lista do BCP
    for (char j = 0; j < i; j++) {
        Semaforo *sem = novoSemaforo(semaforos[j]);
        inserirSemaforo(bcp->semaforos, sem);
    }
}

bool lerComandos(FILE *programa, Fila_Comandos *fila) {
    if (!programa || !fila)
        return false;

    char buffer[11];

    //lê cada comando como uma string e armazena no buffer
    while (fscanf(programa, " %[^\n]", buffer) == 1) {
        char buffer_operacao[6], buffer_parametro[11];
        OPCODE opcode;
        int parametro;

        //se estamos lidando com as operações de semáforo, a string terá formato "operação(parâmetro)"
        if (buffer[0] == 'P' || buffer[0] == 'V') {
            if (sscanf(buffer, "%5[^(](%10[^)])", buffer_operacao, buffer_parametro) != 2)
                return false; // erro ao analisar a string do comando
            opcode = (buffer_operacao[0] == 'P' ? P : V);
            parametro = (int)buffer_parametro[0];
        }
        else {
            //se estamos lidando com as demais operações, a string terá formato "operação parâmetro"
            if (sscanf(buffer, "%s %s", buffer_operacao, buffer_parametro) != 2)
                return false; // erro ao analisar a string do comando

            if (strcmp(buffer_operacao, "exec") == 0)
                opcode = EXEC;
            else if (strcmp(buffer_operacao, "read") == 0)
                opcode = READ;
            else if (strcmp(buffer_operacao, "write") == 0)
                opcode = WRITE;
            else if (strcmp(buffer_operacao, "print") == 0)
                opcode = PRINT;
            else
                return false;

            parametro = atoi(buffer_parametro);
        }

        Comando *comando = novoComando(opcode, parametro);
        if (!comando) {
            freeFilaComandos(fila);
            return false;
        }
        inserirComando(comando, fila);
    }
    return true;
}

BCP *lerProgramaSintetico(FILE *programa) {
    if (!programa)
        return NULL; // se não há programa para ler, retorna NULL

    BCP *processo = novoBCP();
    if (!processo)
        return NULL; // se um novo BCP não pôde ser criado, retorna NULL

    // lê os campos no programa sintético; se houver erro na leitura, informa que há problema no arquivo
    if (!lerCabecalho(programa, processo))
        return mensagemErroBCP("programa sintético contém erro no cabeçalho", processo);

    // calcula o número de páginas da memória que precisam ser utilizadas para esse processo
    processo->n_paginas_usadas = (int)ceil(processo->tamanho_seg / (double)TAMANHO_PAGINA);

    // lê a linha que contém os semáforos
    lerSemaforos(programa, processo);

    // ignora uma linha em branco
    while (fgetc(programa) != '\n' && !feof(programa));
    if (!lerComandos(programa, processo->comandos))
        return mensagemErroBCP("programa sintético contém erro nos comandos", processo);

    return processo;
}

Comando *novoComando(OPCODE opcode, int parametro) {
    Comando *new = malloc(sizeof(Comando));
    if (!new)
        return NULL; // se a alocação de memória falhou
    new->opcode = opcode;
    new->parametro = parametro;
    new->prox = NULL;
    return new;
}

void freeComando(Comando *comando) {
    if (!comando)
        return;
    free(comando);
}

Fila_Comandos *novaFilaComandos() {
    Fila_Comandos *new = malloc(sizeof(Fila_Comandos));
    if (!new)
        return NULL; // se a alocação de memória falhou
    new->head = NULL;
    new->tail = NULL;
    return new;
}

void inserirComando(Comando *comando, Fila_Comandos *fila) {
    if (!fila || !comando)
        return; // se os parâmetros são ponteiros nulos

    if (!fila->head) {
        fila->head = comando;
        fila->tail = comando;
    }
    else {
        fila->tail->prox = comando;
        fila->tail = comando;
    }
}

void removerComando(Fila_Comandos *fila) {
    if (!fila)
        return; //se não há fila
    if (!fila->head)
        return; // se a fila está vazia

    Comando *aux = fila->head;
    fila->head = fila->head->prox;
    freeComando(aux);
}

void freeFilaComandos(Fila_Comandos *fila) {
    if (!fila)
        return;
    while (fila->head != NULL) {
        Comando *temp = fila->head;
        fila->head = fila->head->prox;
        freeComando(temp);
    }
    free(fila);
}

void process_sleep(BCP *processo) {
    if (!processo) // se o processo passado não existe
        return;

    pthread_mutex_lock(&mutex_lista_processos);
    processo->estado = BLOQUEADO;

    // removemos o processo de sua posição na lista
    if (processo == head_lista_processos) //se o processo está no começo da lista global
        head_lista_processos = processo->prox; // alteramos o começo da lista
    else {
        // procuramos a posição correta
        BCP *aux = head_lista_processos;
        BCP *anterior = NULL;

        while (aux != NULL && aux != processo) {
            anterior = aux;
            aux = aux->prox;
        }

        if (aux == NULL) {
            pthread_mutex_unlock(&mutex_lista_processos);
            return; // o processo não estava na lista
        }


        if (anterior != NULL)
            anterior->prox = aux->prox;
    }
    pthread_mutex_unlock(&mutex_lista_processos);
    inserirBCPFinal(processo); // inserimos o processo no final da lista
}

void process_wakeup(BCP *processo) {
    if (!processo)
        return;
    processo->estado = PRONTO;
}
