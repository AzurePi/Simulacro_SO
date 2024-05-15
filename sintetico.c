#include "sintetico.h"

BCP *BCP_From_Sintetico(FILE *programa) {
    //se não há programa para ler, retorna NULL
    if (!programa) {
        printf("ERRO: arquivo do programa sintético não encontrado");
        return NULL;
    }

    BCP *processo = malloc(sizeof(BCP));
    if (!processo) {
        printf("ERRO: falha na alocação de memória do BCP");
        return NULL;
    }

    // lê os campos do BCP no programa sintético; se houver erro na leitura, informa que há problema no arquivo
    if (!fscanf(programa, "%60s", processo->nome) ||
        !fscanf(programa, "%d", &processo->id_seg) ||
        !fscanf(programa, "%d", &processo->prioridade_OG) ||
        !fscanf(programa, "%d", &processo->tamanho_seg)) {
        printf("ERRO: programa sintético contém erro no cabeçalho");
        return NULL;
    }

    processo->estado = 0; //diz que o programa está pronto para ser executado

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

    //TODO: para cada semáforo guardado, cria um novo semáforo, coloca na lista de semáforos geral, e coloca na lista do BCP

    //lê a linha em branco
    fscanf(programa, "%*[^\n]s");

    //lê cada um dos comandos do processo; guarda ele em uma lista em que cada elemento tem um código de operação e um parâmetro
    processo->comandos = novaListaComandos();
    if (!processo->comandos) {
        printf("ERRO: falha na criação de lista de comandos para o programa");
        return NULL;
    }

    //TODO: Heloísa faça daqui pra frente por favor <3
    char buffer[11];

    //lê cada comando como uma string e armazena no buffer
    while (fscanf(programa, "%s", buffer)) {
        char buffer_comando[6], buffer_parametro[6];
        OPCODE opcode;
        int parametro;

        //se estamos lidando com as operações de semáforo, a string terá of formato "operação(parâmetro)"
        if (buffer[0] == 'P' || buffer[0] == 'V') {
            sscanf(buffer, "%s(%s)", buffer_comando, buffer_parametro);

            //TODO: comparar buffer_comando com as strings esperadas para determinar a operação sendo realizada (nesse caso, só P ou V)
            //TODO: converter o parâmetro (um caractere) em um valor inteiro para ser o parâmetro (como o char já é um número, não deve ter nenhum problema)

        } else { //se estamos lidando com as demais operações, a string tera formato "operação parâmetro"
            sscanf(buffer, "%s %s", buffer_comando, buffer_parametro);

            //TODO: comparar buffer_comando com as strings esperadas para determinar a operação sendo realizada (todo o resto)
            //TODO: transformar a string do parâmetro em um valor inteiro
            //TODO: usar novoComando para criar um comando com o opcode e o parâmetro, e usar inserirComando para guardar isso no processo
        }
        //TODO: usar novoComando para criar um comando com o opcode e o parâmetro, e usar inserirComando para guardar isso no processo
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

bool vaziaListaComandos(Lista_Comandos *l) {
    return (l->nElementos == 0);
}

Lista_Comandos *novaListaComandos() {
    Lista_Comandos *l = malloc(sizeof(Lista_Comandos));
    if (!l) return NULL; // se a alocação de memória falhou
    l->nElementos = 0;
    l->head = NULL;
    return l;
}

void inserirComando(Comando *comando, Lista_Comandos *lista) {
    if (!lista || !comando) return; // se os parâmetros são ponteiros nulos

    Comando *aux = lista->head;
    while (aux->prox != NULL)
        aux = aux->prox;
    aux->prox = comando;
    lista->nElementos++;
}

void proc_sleep(BCP *proc) {
    if (!proc) return; // se o processo passado não existe

}

void proc_wakeup(BCP *proc) {
    if (proc) {
        proc->estado = PRONTO;
    }
}
