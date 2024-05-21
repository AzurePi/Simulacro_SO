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

*/

Memoria memoria_global; // Declaração global da estrutura que une a memória e as informações sobre as páginas ocupadas

// Inicialização da memória e contagem de páginas ocupadas
void inicializar_memoria() {
    memoria_global.memoria = (Pagina *)malloc(NUMERO_PAGINAS * sizeof(Pagina));
    for (int i = 0; i < NUMERO_PAGINAS; i++) {
        memoria_global.memoria[i].numero_pagina = i;
        memoria_global.memoria[i].segunda_chance = 0;
    }
    memoria_global.paginas_ocupadas = 0; // Inicializa a contagem de páginas ocupadas
}

// Função para encontrar a próxima página a ser substituída usando o algoritmo FIFO com Segunda Chance
int encontrar_pagina_para_substituir() {
    static int ponteiro = 0; // Apontador circular

    for (int i = 0; i < NUMERO_PAGINAS; i++) {
        if (memoria_global.memoria[ponteiro].segunda_chance == 0) {
            int pagina_para_substituir = ponteiro;
            ponteiro = (ponteiro + 1) % NUMERO_PAGINAS; // Avança o ponteiro
            return pagina_para_substituir;
        } else {
            memoria_global.memoria[ponteiro].segunda_chance = 0; // Remove a segunda chance
            ponteiro = (ponteiro + 1) % NUMERO_PAGINAS; // Avança o ponteiro
        }
    }
    // Caso todas as páginas tenham segunda chance, substitui a primeira página examinada
    int pagina_para_substituir = ponteiro;
    ponteiro = (ponteiro + 1) % NUMERO_PAGINAS;
    return pagina_para_substituir;
}

// Função para adicionar um processo à memória
void adicionar_processo_na_memoria(BCP *processo) {
    int num_paginas_necessarias = (processo->tamanho_kb + TAMANHO_PAGINA - 1) / TAMANHO_PAGINA; // Tamanho em páginas do processo

    for (int i = 0; i < num_paginas_necessarias; i++) {
        int pagina_para_substituir = encontrar_pagina_para_substituir();
        memoria_global.memoria[pagina_para_substituir].segunda_chance = 1;

        processo->paginas_usadas[i] = &memoria_global.memoria[pagina_para_substituir]; // Atribui a referência da página ao BCP

        memoria_global.paginas_ocupadas++; // Incrementa o contador de páginas ocupadas

        printf("Processo %d, Página %d carregada na posição %d da memória\n", processo->id_processo, i, pagina_para_substituir);
    }
}

// Função para calcular e exibir a taxa de ocupação da memória
void exibir_taxa_ocupacao() {
    double taxa_ocupacao = ((double)memoria_global.paginas_ocupadas / NUMERO_PAGINAS) * 100.0;
    printf("Taxa de ocupação da memória: %.2f%%\n", taxa_ocupacao);
}

// Função para carregar páginas necessárias para um processo na memória
void carregar_paginas_necessarias(BCP *processo) {
    for (int i = 0; i < processo->paginas_necessarias; i++) {
        if (processo->paginas_usadas[i] == NULL) {
            int pagina_para_substituir = encontrar_pagina_para_substituir();
            memoria_global.memoria[pagina_para_substituir].segunda_chance = 1;
            processo->paginas_usadas[i] = &memoria_global.memoria[pagina_para_substituir];
            memoria_global.paginas_ocupadas++;
            printf("Processo %d, Página %d carregada na posição %d da memória\n", processo->id_processo, i, pagina_para_substituir);
        }
    }
}

// Função para verificar se todas as páginas necessárias para um processo estão carregadas na memória
int verificar_paginas_carregadas(BCP *processo) {
    for (int i = 0; i < processo->paginas_necessarias; i++) {
        if (processo->paginas_usadas[i] == NULL) {
            return 0; // Página não carregada encontrada
        }
    }
    return 1; // Todas as páginas carregadas
}
