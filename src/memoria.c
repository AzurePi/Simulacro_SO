#include "include/memoria.h"

No_Pagina *criaNo() {
    No_Pagina *novoNo = malloc(sizeof(No_Pagina));
    if (!novoNo) return NULL;
    novoNo->uso = 0;
    novoNo->modificado = 0;
    novoNo->prox = NULL;
    return novoNo;
}

Lista_Circ *criaLista() {
    Lista_Circ *lista = malloc(sizeof(Lista_Circ));
    if (!lista) return NULL;
    lista->head = NULL;
    lista->tail = NULL;
    return lista;
}

No_Pagina *inserePagina(No_Pagina *noAdiciona, Lista_Circ *lista) {
    // se a lista estiver vazia
    if (!lista->head) {
        lista->head = lista->tail = noAdiciona; // o nó adicionado é tanto a cabeça quanto a cauda
        noAdiciona->prox = noAdiciona; // o próximo do nó aponta para ele mesmo, pois é o único nó na Lista
    // se a Lista não estiver vazia
    } else {
        lista->tail->prox = noAdiciona; // o próximo do último nó atual aponta para o nó adicionado
        lista->tail = noAdiciona; // o nó adicionado agora é a cauda da Lista
        noAdiciona->prox = lista->head; // o próximo do nó adicionado aponta para a cabeça da Lista, tornando-a circular
    }
    return lista->head; // retorna a cabeça da Lista atualizada TODO: talvez nem precise desse retorno, mas vamos ver
}

// Usa para buscar sem uso, retorno a primeira página que eu encontrar.
No_Pagina *percorreLista(Lista_Circ lista) {
    No_Pagina *aux = lista.head;

    // enquanto eu não chegar na cauda da Lista
    while (aux->prox != lista.head) {
        //Procuro uma página vazia para carregar as informações
        if (aux->ocupado == 0)
            return aux;
        aux = aux->prox;
    } //TODO: em algum momento aqui, temos que mudar as flags das páginas percorridas

    // se não tiver nenhum sem uso

    // enquanto eu não chegar na cauda da Lista
    while (aux->prox != lista.head) {
        // procura uma página sem uso para carregar as informações e sem modificação para não ter que escrever na memória
        if (aux->uso == 0 && aux->modificado == 0)
            return aux;
        aux = aux->prox;
    }
    return NULL;
}