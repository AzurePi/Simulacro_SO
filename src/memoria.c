#include "include/memoria.h"

void inicializarRAM() {
    if (RAM) return; // se a RAM já foi inicializada, retorna
    RAM = malloc(sizeof(Memoria));
    RAM->memoria = malloc(NUMERO_PAGINAS * sizeof(Pagina));
    for (int i = 0; i < NUMERO_PAGINAS; i++) {
        //RAM->memoria[i].numero_pagina = i;
        RAM->memoria[i].vazia = true; // a página está vazia
        RAM->memoria[i].segunda_chance = 0; // inicializa todas como podendo ser trocadas
    }
    RAM->n_paginas_ocupadas = 0; // Inicializa a contagem de páginas ocupadas
}

void adicionarProcessoNaMemoria(BCP *processo) {
    int n_paginas_necessarias = (processo->tamanho_seg + TAMANHO_PAGINA - 1) / TAMANHO_PAGINA;
    processo->n_paginas_usadas = n_paginas_necessarias;

    for (int i = 0; i < n_paginas_necessarias; i++) {
        int para_substituir = paginaParaSubstituir();
        sem_wait(&sem_RAM);
        RAM->memoria[para_substituir].segunda_chance = 1;

        processo->paginas_usadas[i] = &RAM->memoria[para_substituir]; // Atribui a referência da página ao BCP

        if (RAM->memoria[para_substituir].vazia) {
            RAM->n_paginas_ocupadas++;
            RAM->memoria[para_substituir].vazia = false;
        }
        sem_post(&sem_RAM);
    }
}

void carregarPaginasNecessarias(BCP *processo) {
    if (verificarPaginasCarregadas(processo)) return;

    for (int i = 0; i < processo->n_paginas_usadas; i++) {
        if (processo->paginas_usadas[i] == NULL) {
            int para_substituir = paginaParaSubstituir();
            sem_wait(&sem_RAM);
            RAM->memoria[para_substituir].segunda_chance = true;
            processo->paginas_usadas[i] = &RAM->memoria[para_substituir];

            if (RAM->memoria[para_substituir].vazia) {
                RAM->n_paginas_ocupadas++;
                RAM->memoria[para_substituir].vazia = false;
            }
            sem_post(&sem_RAM);
        }
    }
}

int paginaParaSubstituir() {
    static int ponteiro = 0; // Apontador circular

    for (int i = 0; i < NUMERO_PAGINAS; i++) {
        sem_wait(&sem_RAM);
        if (RAM->memoria[ponteiro].segunda_chance == false) {
            int pagina_para_substituir = ponteiro;
            ponteiro = (ponteiro + 1) % NUMERO_PAGINAS; // Avança o ponteiro
            sem_post(&sem_RAM);
            return pagina_para_substituir;
        } else {
            RAM->memoria[ponteiro].segunda_chance = 0; // Remove a segunda chance
            ponteiro = (ponteiro + 1) % NUMERO_PAGINAS; // Avança o ponteiro
        }
        sem_post(&sem_RAM);
    }
    // Caso todas as páginas tenham segunda chance, substitui a primeira página examinada
    int pagina_para_substituir = ponteiro;
    ponteiro = (ponteiro + 1) % NUMERO_PAGINAS;
    return pagina_para_substituir;
}

bool verificarPaginasCarregadas(BCP *processo) {
    for (int i = 0; i < processo->n_paginas_usadas; i++) {
        if (processo->paginas_usadas[i] == NULL)
            return false; // Página não carregada encontrada
    }
    return true; // Todas as páginas carregadas
}