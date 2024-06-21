#ifndef SIMULACRO_SO_MEMORIA_H
#define SIMULACRO_SO_MEMORIA_H

#include "processo.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Definições de Macros ------------------------------------------------------------------------------------------------

/*
 * Nossa menor unidade é um programa com segmento de tamanho 1 KB, que podemos definir como 1 unidade de memória (um).
 * Nossas páginas têm 8 KB, ou seja, 8 um.
 * A memória total tem 1 GB, ou seja, 1024 * 1024 * 1024 bytes => 1024 * 1024 kbytes, ou seja, 1024*1024 um.
 */

#define TAMANHO_PAGINA 8 // 8 unidades de memória (1 um = 1 kbyte)
#define TAMANHO_MEMORIA (1024 * 1024) // 1 GB de memória (1024 * 1024 kbytes = 1024 * 1024 um)
#define NUMERO_PAGINAS (TAMANHO_MEMORIA / TAMANHO_PAGINA)

// Declaração de Tipos -------------------------------------------------------------------------------------------------

// declaração avançada para impedir dependência circular
typedef struct bcp BCP;

typedef struct pagina {
    bool segunda_chance; // se a página tem ou não uma segunda chance no algoritmo de swapping de páginas
    BCP *conteudo; // referência ao processo cujo conteúdo está na página
} Pagina;

typedef struct memoria {
    Pagina *memoria;
    int n_paginas_ocupadas;
} Memoria;

// Funções -------------------------------------------------------------------------------------------------------------

// Inicializa a variável global da memória do simulador.
void inicializarRAM();

// Libera a variável global da memória do simulador.
void freeRAM();

// Encontra a próxima página a ser substituída usando o algoritmo FIFO com Segunda Chance.
int paginaParaSubstituir();

// Carrega páginas necessárias para um processo na memória.
void carregarPaginasNecessarias(BCP *processo);

// Descarrega todas as páginas de um processo.
void descarregarPaginas(BCP *processo);

// Verifica se todas as páginas necessárias para um processo estão carregadas na memória.
bool verificarPaginasCarregadas(const BCP *processo);

#endif //SIMULACRO_SO_MEMORIA_H
