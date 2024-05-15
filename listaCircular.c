#include <stdio.h>
#include <stdlib.h>
#include "listaCircular.h"

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
    if (!lista->head) { // Se a Lista estiver vazia
        lista->head = lista->tail = noAdiciona; // O nó adicionado é tanto a cabeça quanto a cauda
        noAdiciona->prox = noAdiciona; // O próximo do nó aponta para ele mesmo, pois é o único nó na Lista
    } else { // Se a Lista não estiver vazia
        lista->tail->prox = noAdiciona; // O próximo do último nó atual aponta para o nó adicionado
        lista->tail = noAdiciona; // O nó adicionado agora é a cauda da Lista
        noAdiciona->prox = lista->head; // O próximo do nó adicionado aponta para a cabeça da Lista, tornando-a circular
    }
    return lista->head; // Retorna a cabeça da Lista atualizada
}


No_Pagina *percorreLista(
        Lista_Circ lista) { //Tomar cuidado pq quando chegar na tail, volta pra cabeça - usa pra buscar sem_terminal uso, retorno a primeira página que eu encontrar.
    No_Pagina *aux = malloc(sizeof(No_Pagina));

    while (aux->prox != lista.head) {//Enquanto eu não chegar na cauda da Lista
        if (aux->ocupado == 0) {//Procuro uma página vazia para carregar as informações

            return aux;//VER SE É ISSO
        }
        aux = aux->prox;
    }

    //Se não tiver nenhum sem_terminal uso
    while (aux->prox != lista.head) {//Enquanto eu não chegar na cauda da Lista
        if (aux->uso == 0) {//Procuro uma página sem_terminal uso para carregar as informações
            if (aux->modificado == 0) {//E tbm sem_terminal modificação pra não ter escrever na memória
                return aux;//VER SE É ISSO
            }
        }
        aux = aux->prox;
    }
    return NULL;
}


No_Pagina *inserePrioridade(BCP *noAdiciona, Lista_Circ lista) {
    return NULL;
}
