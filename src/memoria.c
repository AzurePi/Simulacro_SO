#include "include/memoria.h"

No_Pagina *criaNo() {
    No_Pagina *novoNo = malloc(sizeof(No_Pagina));
    novoNo->uso = 0;
    novoNo->modificado = 0;
    novoNo->prox = NULL;
    return novoNo;
}

Lista_Circ *criaLista() {
    Lista_Circ *lista = malloc(sizeof(Lista_Circ));
    lista->head = NULL;
    lista->tail = NULL;
    return lista;
}

No_Pagina *insereNormal(No_Pagina *noAdiciona, Lista_Circ *lista) {
    // Se a Lista estiver vazia
    if (!lista->head) {
        lista->head = lista->tail = noAdiciona; // O nó adicionado é tanto a cabeça quanto a cauda
        noAdiciona->prox = noAdiciona; // O próximo do nó aponta para ele mesmo, pois é o único nó na Lista
    // Se a Lista não estiver vazia
    } else {
        lista->tail->prox = noAdiciona; // O próximo do último nó atual aponta para o nó adicionado
        lista->tail = noAdiciona; // O nó adicionado agora é a cauda da Lista
        noAdiciona->prox = lista->head; // O próximo do nó adicionado aponta para a cabeça da Lista, tornando-a circular
    }
    return lista->head; // Retorna a cabeça da Lista atualizada
}

//usa para buscar sem uso, retorno a primeira página que eu encontrar.
No_Pagina *percorreLista(Lista_Circ lista) {
    No_Pagina *aux = lista.head;

    //Enquanto eu não chegar na cauda da Lista
    while (aux->prox != lista.head) {
        //Procuro uma página vazia para carregar as informações
        if (aux->ocupado == 0)
            return aux;
        aux = aux->prox;
    } //TODO: em algum momento aqui, temos que mudar as flags das páginas percorridas

    //Se não tiver nenhum sem uso

    //Enquanto eu não chegar na cauda da Lista
    while (aux->prox != lista.head) {
        //Procuro uma página sem uso para carregar as informações e tbm sem modificação pra não ter escrever na memória
        if (aux->uso == 0 && aux->modificado == 0)
            return aux;
        aux = aux->prox;
    }
    return NULL;
}