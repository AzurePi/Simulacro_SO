#ifndef SIMULACRO_SO_MEMORIA_H
#define SIMULACRO_SO_MEMORIA_H

#include "sintetico.h"

#include <stdio.h>
#include <stdlib.h>

#define TAMANHO_PAGINA 4096 // 4 KB por página
#define TAMANHO_MEMORIA (1024 * 1024 * 1024) // 1 GB de memória
#define NUMERO_PAGINAS (TAMANHO_MEMORIA / TAMANHO_PAGINA)

/*
typedef struct no {
    char uso; //Um char com valor 0 para sem uso e 1 para em uso - ver se pode substituir
    char modificado; // 0 para não modificado e 1 para modificado
    char ocupado; //0 para livre e 1 para página preenchida
    struct no *prox;
} No_Pagina;


typedef struct lista {
    No_Pagina *head;
    No_Pagina *tail;
} Lista_Circ;
 */

typedef struct pagina {
    int numero_pagina;
    bool segunda_chance; // 1 se a página tiver uma segunda chance, 0 caso contrário
} Pagina;

typedef struct Memoria {
    Pagina *memoria;
    int n_paginas_ocupadas;
} Memoria;

// Inicialização da memória e contagem de páginas ocupadas
void inicializarMemoria();

// Função para encontrar a próxima página a ser substituída usando o algoritmo FIFO com Segunda Chance
int encontrarPaginaParaSubstituir();

// Função para adicionar um processo à memória
void adicionarProcessoNaMemoria(BCP *processo);

// Função para calcular e exibir a taxa de ocupação da memória
void exibirTaxaOcupacao();

// Função para carregar páginas necessárias para um processo na memória
void carregarPaginasNecessarias(BCP *processo);

// Função para verificar se todas as páginas necessárias para um processo estão carregadas na memória
bool verificarPaginasCarregadas(BCP *processo);


/*
No_Pagina *criaNo();

Lista_Circ *criaLista();

No_Pagina *inserePagina(No_Pagina *noAdiciona, Lista_Circ *lista);

//Tomar cuidado pq quando chegar na tail, volta pra cabeça - usa pra buscar sem uso.
No_Pagina *percorreLista(Lista_Circ lista);
 */

#endif //SIMULACRO_SO_MEMORIA_H