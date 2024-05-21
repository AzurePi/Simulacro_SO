#ifndef SIMULACRO_SO_MEMORIA_H
#define SIMULACRO_SO_MEMORIA_H

#include <stdio.h>
#include <stdlib.h>
#include "sintetico.h"

#define TAMANHO_PAGINA 4096 // 4 KB por página
#define TAMANHO_MEMORIA (1024 * 1024 * 1024) // 1 GB de memória
#define NUMERO_PAGINAS (TAMANHO_MEMORIA / TAMANHO_PAGINA)

typedef struct no {
    char uso; //Um char com valor 0 para sem_terminal uso e 1 para em uso - ver se pode substituir
    char modificado; // 0 para não modificado e 1 para modificado
    char ocupado; //0 para livre e 1 para página preenchida
    struct no *prox;
} No_Pagina;

typedef struct lista {
    No_Pagina *head;
    No_Pagina *tail;
} Lista_Circ;

typedef struct Pagina {
    int numero_pagina;
    int segunda_chance; // 1 se a página tiver uma segunda chance, 0 caso contrário
} Pagina;

/*
typedef struct bcp {
    int id_processo;
    int tamanho_kb;
    Pagina *paginas_usadas[NUMERO_PAGINAS]; // Array de ponteiros para páginas utilizadas pelo processo
} BCP;
*/

typedef struct Memoria {
    Pagina *memoria;
    int paginas_ocupadas;
} Memoria;


No_Pagina *criaNo();

Lista_Circ *criaLista();

No_Pagina *inserePagina(No_Pagina *noAdiciona, Lista_Circ *lista);

//Tomar cuidado pq quando chegar na tail, volta pra cabeça - usa pra buscar sem_terminal uso.
No_Pagina *percorreLista(Lista_Circ lista);

#endif //SIMULACRO_SO_MEMORIA_H
