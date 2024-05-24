#ifndef SIMULACRO_SO_MEMORIA_H
#define SIMULACRO_SO_MEMORIA_H

#include "processo.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Definições de Macros ------------------------------------------------------------------------------------------------

#define TAMANHO_PAGINA 4096 // 4 KB por página
#define TAMANHO_MEMORIA (1024 * 1024 * 1024) // 1 GB de memória
#define NUMERO_PAGINAS (TAMANHO_MEMORIA / TAMANHO_PAGINA)

// Declaração de Tipos -------------------------------------------------------------------------------------------------

// declaração avançada para impedir dependência circular
typedef struct bcp BCP;

typedef struct pagina {
    //int numero_pagina;
    bool vazia;
    bool segunda_chance; // 1 se a página tiver uma segunda chance, 0 caso contrário
} Pagina;

typedef struct memoria {
    Pagina *memoria;
    int n_paginas_ocupadas;
} Memoria;

// Funções -------------------------------------------------------------------------------------------------------------

// Inicialização da memória e contagem de páginas ocupadas
void inicializarRAM();

// Adiciona um processo à memória
void adicionarProcessoNaMemoria(BCP *processo);

// Encontrar a próxima página a ser substituída usando o algoritmo FIFO com Segunda Chance
int paginaParaSubstituir();

// Carrega páginas necessárias para um processo na memória
void carregarPaginasNecessarias(BCP *processo);

// Verifica se todas as páginas necessárias para um processo estão carregadas na memória
bool verificarPaginasCarregadas(BCP *processo);

#endif //SIMULACRO_SO_MEMORIA_H