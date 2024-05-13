#include "sintetico.h"

BCP *BCP_From_Sintetico(FILE *programa) {
    //se não há programa para ler, retorna NULL
    if (!programa) {
        printf("ERRO: arquivo não encontrado");
        return NULL;
    }

    BCP *processo = malloc(sizeof(BCP));

    // lê os campos do BCP no programa sintético; se houver erro na leitura, informa que há problema no arquivo
    if (!fscanf(programa, "%60s", processo->nome) ||
        !fscanf(programa, "%d", &processo->id_seg) ||
        !fscanf(programa, "%d", &processo->prioridade_OG) ||
        !fscanf(programa, "%d", &processo->tamanho_seg)
            ) {
        printf("ERRO: programa sintético contém erro no BCP");
        return NULL;
    }

    processo->estado = 0; //diz que o programa está pronto para ser executado

    char semaforos[51];
    char caractere;
    int i = 0;

    //enquanto não chegamos no fim da linha e há caracteres para ler e não ultrapassamos o máximo de 10 semáforos
    while ((caractere = fgetc(programa)) != '\n' && caractere != EOF && i < 10) {
        if (caractere != ' ') {
            semaforos[i] = caractere; //guardamos o caractere
            i++;
        }
    }
    semaforos[i] = '\0'; //finaliza a string

    //TODO: para cada semáforo guardado, cria uma struct semáforo e coloca no BCP

    //lê a linha em branco
    fscanf(programa, "%*[^\n]s");

    //lê cada um dos comandos do processo; guarda ele em uma lista, em que cada elemento tem um código de operação e um parâmetro
    processo->comandos = novaListaComandos();

    //TODO: Heloísa faça daqui pra frente
    char buffer_comando[6], buffer_parametro[6];

    //lê cada comando, e guarda as strings nos buffers
    while (fscanf(programa, "%s %s", buffer_comando, buffer_parametro)) {
        //comparar a string lida com os possíveis comandos esperados, e com base nisso determinar o opcode
        //TODO: criar um novoComando com o opcode e parâmetro
        //TODO: adicionar o comando na Lista_Comandos usando inserirComando()
    }

    return processo;
}

Comando *novoComando(int opcode, int parametro) {
    Comando *c = malloc(sizeof(Comando));
    if (!c) return NULL;
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
    if (!l) return NULL;
    l->nElementos = 0;
    l->head = NULL;
    return l;
}

void inserirComando(Comando *comando, Lista_Comandos *lista) {
    Comando *aux = lista->head;
    while (aux->prox != NULL)
        aux = aux->prox;
    aux->prox = comando;
    lista->nElementos++;
}