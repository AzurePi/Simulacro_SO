#include "include/memoria.h"

/*No_Pagina *criaNo() {
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
    return lista->head; // retorna a cabeça da Lista atualizada
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
    }

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

*/


void inicializarMemoria() {
    RAM->memoria = malloc(NUMERO_PAGINAS * sizeof(Pagina));
    for (int i = 0; i < NUMERO_PAGINAS; i++) {
        RAM->memoria[i]->numero_pagina = i;
        RAM->memoria[i]->segunda_chance = 0;
    }
    RAM->n_paginas_ocupadas = 0; // Inicializa a contagem de páginas ocupadas
}

int encontrarPaginaParaSubstituir() {
    static int ponteiro = 0; // Apontador circular

    for (int i = 0; i < NUMERO_PAGINAS; i++) {
        if (RAM->memoria[ponteiro]->segunda_chance == 0) {
            int pagina_para_substituir = ponteiro;
            ponteiro = (ponteiro + 1) % NUMERO_PAGINAS; // Avança o ponteiro
            return pagina_para_substituir;
        } else {
            RAM->memoria[ponteiro]->segunda_chance = 0; // Remove a segunda chance
            ponteiro = (ponteiro + 1) % NUMERO_PAGINAS; // Avança o ponteiro
        }
    }
    // Caso todas as páginas tenham segunda chance, substitui a primeira página examinada
    int pagina_para_substituir = ponteiro;
    ponteiro = (ponteiro + 1) % NUMERO_PAGINAS;
    return pagina_para_substituir;
}

void adicionarProcessoNaMemoria(BCP *processo) {
    int n_paginas_necessarias = (processo->tamanho_seg + TAMANHO_PAGINA - 1) / TAMANHO_PAGINA;

    for (int i = 0; i < n_paginas_necessarias; i++) {
        int para_substituir = encontrarPaginaParaSubstituir();
        RAM->memoria[para_substituir].segunda_chance = 1;

        processo->paginas_usadas[i] = &RAM->memoria[para_substituir]; // Atribui a referência da página ao BCP

        RAM->n_paginas_ocupadas++; // Incrementa o contador de páginas ocupadas

        //printf("Processo %d, Página %d carregada na posição %d da memória\n", processo->id_seg, i, para_substituir);
    }
}

void exibirTaxaOcupacao() {
    double taxa_ocupacao = (RAM->n_paginas_ocupadas / (double) NUMERO_PAGINAS) * 100.0;
    printf("Taxa de ocupação da memória: %.2f%%\n", taxa_ocupacao);
}

void carregarPaginasNecessarias(BCP *processo) {
    for (int i = 0; i < processo->paginas_usadas; i++) {
        if (processo->paginas_usadas[i] == NULL) {
            int pagina_para_substituir = encontrarPaginaParaSubstituir();
            RAM.memoria[pagina_para_substituir].segunda_chance = 1;
            processo->paginas_usadas[i] = &RAM.memoria[pagina_para_substituir];
            RAM.paginas_ocupadas++;
            printf("Processo %d, Página %d carregada na posição %d da memória\n", processo->id_seg, i,
                   pagina_para_substituir);
        }
    }
}

bool verificarPaginasCarregadas(BCP *processo) {
    for (int i = 0; i < processo->paginas_usadas; i++) {
        if (processo->paginas_usadas[i] == NULL)
            return false; // Página não carregada encontrada
    }
    return true; // Todas as páginas carregadas
}