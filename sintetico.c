#include "sintetico.h"

BCP *BCP_From_Sintetico(FILE *programa) {
    //se não há programa para ler, retorna NULL
    if (!programa) {
        printf("ERRO: arquivo não encontrado");
        return NULL;
    }

    BCP *processo = malloc(sizeof(BCP));

    // lê os campos do BCP no programa sintético; se houver erro na leitura, informa que há problema no arquivo
    if (!fscanf(programa, "%[60]s", processo->nome) ||
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

    //enquanto não chegamos no fim da linha e há caracteres para ler
    while ((caractere = fgetc(programa)) != '\n' && caractere != EOF && i < 50) {
        semaforos[i] = caractere; //guardamos o caractere
        i++;
    }
    semaforos[i] = '\0'; //finaliza a string

    //para cada semáforo guardado, cria uma struct semáforo e coloca no BCP

    //lê a linha em branco
    fscanf(programa, "%*[^\n]s");

    //lê cada um dos comandos do processo; guarda ele em uma lista, em que cada elemento tem um código de operação e um parâmetro
    processo->comandos = malloc(sizeof(Lista_Comandos));
    Definir(processo->comandos);

    char buffer_comando[6], buffer_parametro[6];

    while (fscanf(programa, "%s %s", buffer_comando, buffer_parametro)) {
        //TODO: criar um novo Comando com base nos bufferes
            //comparar a string lida com os possíveis comandos esperados, e com base nisso determinar o opcode
        //TODO: adicionar o comando na Lista_Comandos usando Inserir()
    }

    return processo;
}

bool Vazia(Lista_Comandos *l) {
    return (l->nElementos == 0);
}

bool Cheia(Lista_Comandos *l) {
    return (l->nElementos == MAX);
}

void Definir(Lista_Comandos *l) {
    l->nElementos = 0;
    l->head = NULL;
}

bool Inserir(Comando *comando, Lista_Comandos *lista) {
    // Lista cheia
    if (Cheia(lista))
        return false; //Inserção falhou

    Comando *aux = lista->head;
    while (aux->prox != NULL) {
        aux = aux->prox;
    }
    aux->prox = comando;
    lista->nElementos++;
    return true; //Inserção feita com sucesso
}