#include "include/sintetico.h"

BCP *novoBCP() {
    BCP *new = malloc(sizeof(BCP));
    if (!new) {
        printf("ERRO: falha na alocação de memória do BCP");
        return NULL;
    }
    new->estado = PRONTO; //diz que o programa está pronto para ser executado
    new->semaforos = novaListaSemaforos();
    new->comandos = novaListaComandos();
    return new;
}

void freeBCP(BCP *bcp) {
    liberaListaSemaforo(bcp->semaforos);
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

//TODO: debugar isso
BCP *lerProgramaSintetico(FILE *programa) {
    if (!programa) return NULL; // se não há programa para ler, retorna NULL

    BCP *processo = novoBCP();
    if (!processo) return NULL; // se um novo BCP não pôde ser criado, retorna NULL

    // lê os campos no programa sintético; se houver erro na leitura, informa que há problema no arquivo
    if (!fscanf(programa, "%60s", processo->nome) ||
        !fscanf(programa, "%d", &processo->id_seg) ||
        !fscanf(programa, "%d", &processo->prioridade_OG) ||
        !fscanf(programa, "%d", &processo->tamanho_seg)) {
        printf("ERRO: programa sintético contém erro no cabeçalho");
        freeBCP(processo);
        return NULL;
    }

    char semaforos[11]; //não uma string, mas um vetor de caracteres
    char s;
    int i = 0;

    //enquanto não chegamos no fim da linha e há caracteres para ler e não ultrapassamos o máximo de 10 semáforos
    while ((s = fgetc(programa)) != '\n' && s != EOF && i < 10) {
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

    //lê a linha em branco
    fscanf(programa, "%*[^\n]s");

    //lê cada um dos comandos do processo; guarda ele em uma lista em que cada elemento tem um código de operação e um parâmetro
    if (!processo->comandos) {
        printf("ERRO: falha na criação de lista de comandos para o programa");
        freeBCP(processo);
        return NULL;
    }

    char buffer[11];

    //lê cada comando como uma string e armazena no buffer
    while (fscanf(programa, "%s", buffer)) {
        char buffer_comando[6], buffer_parametro[6];
        OPCODE opcode = -1;
        int parametro;

        //se estamos lidando com as operações de semáforo, a string terá of formato "operação(parâmetro)"
        if (buffer[0] == 'P' || buffer[0] == 'V') {
            sscanf(buffer, "%s(%s)", buffer_comando, buffer_parametro);

            if (buffer_comando[0] == 'P')
                opcode = P;
            else if (buffer_comando[0] == 'V')
                opcode = V;

        } else { //se estamos lidando com as demais operações, a string terá formato "operação parâmetro"
            sscanf(buffer, "%s %s", buffer_comando, buffer_parametro);

            if (strcmp(buffer_comando, "exec") == 0)
                opcode = EXEC;
            else if (strcmp(buffer_comando, "read") == 0)
                opcode = READ;
            else if (strcmp(buffer_comando, "write") == 0)
                opcode = WRITE;
            else if (strcmp(buffer_comando, "print") == 0)
                opcode = PRINT;
        }

        if (opcode == -1) {
            printf("ERRO: comando não reconhecido no programa sintético");
            freeBCP(processo);
            return NULL;
        }

        parametro = atoi(buffer_parametro);
        Comando *comando = novoComando(opcode, parametro);
        inserirComando(comando, processo->comandos);
    }
    return processo;
}

Comando *novoComando(OPCODE opcode, int parametro) {
    Comando *c = malloc(sizeof(Comando));
    if (!c) return NULL; // se a alocação de memória falhou
    c->opcode = opcode;
    c->parametro = parametro;
    c->prox = NULL;
    return c;
}

void freeComando(Comando *comando) {
    free(comando);
}

Lista_Comandos *novaListaComandos() {
    Lista_Comandos *new = malloc(sizeof(Lista_Comandos));
    if (!new) return NULL; // se a alocação de memória falhou
    new->nElementos = 0;
    new->head = NULL;
    return new;
}

void freeListaComandos(Lista_Comandos *comandos) {
    Comando *temp;
    while (comandos->head != NULL) {
        temp = comandos->head;
        comandos->head = comandos->head->prox;
        freeComando(temp);
    }
    free(comandos);
}

void inserirComando(Comando *comando, Lista_Comandos *lista) {
    if (!lista || !comando) return; // se os parâmetros são ponteiros nulos

    Comando *aux = lista->head;
    while (aux->prox != NULL)
        aux = aux->prox;
    aux->prox = comando;
    lista->nElementos++;
}

void inserirSemaforo(Semaforo *semaforo, Lista_Semaforos *lista) {
    //TODO: fazer isso
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
