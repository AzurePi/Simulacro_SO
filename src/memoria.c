#include "include/memoria.h"

void inicializarRAM() {
    if (RAM) return; // se a RAM já foi inicializada, retorna
    RAM = malloc(sizeof(Memoria));
    RAM->memoria = malloc(NUMERO_PAGINAS * sizeof(Pagina));
    for (int i = 0; i < NUMERO_PAGINAS; i++) {
        RAM->memoria[i].segunda_chance = false; // inicializa todas como podendo ser trocadas
        RAM->memoria[i].conteudo = NULL; // essa página não aponta para ninguém
    }
    RAM->n_paginas_ocupadas = 0; // Inicializa a contagem de páginas ocupadas
}

void freeRAM() {
    if (!RAM) return;
    free(RAM->memoria);
    free(RAM);
}

void carregarPaginasNecessarias(BCP *processo) {
    if (verificarPaginasCarregadas(processo)) return;

    for (int i = 0; i < processo->n_paginas_usadas; i++) {
        if (processo->paginas_usadas[i] == NULL) {
            const int para_substituir = paginaParaSubstituir();

            pthread_mutex_lock(&mutex_RAM);
            Pagina *pagina = &RAM->memoria[para_substituir];
            pagina->segunda_chance = true;

            if (pagina->conteudo == NULL) //se a página estava vaia
                RAM->n_paginas_ocupadas++; // aumentamos o número de páginas ocupadas na memória
            else { // se não, remove a referência do processo que utilizava a página sendo substituída
                BCP *conteudo_velho = pagina->conteudo;

                for (int j = 0; j < conteudo_velho->n_paginas_usadas; j++) {
                    if (conteudo_velho->paginas_usadas[j] == pagina) {
                        conteudo_velho->paginas_usadas[j] = NULL;
                        break;
                    }
                }
            }

            processo->paginas_usadas[i] = pagina;
            pagina->conteudo = processo;

            pthread_mutex_unlock(&mutex_RAM);
        }
    }
}

int paginaParaSubstituir() {
    static int ponteiro = 0; // Apontador circular

    pthread_mutex_lock(&mutex_RAM);
    for (int i = 0; i < NUMERO_PAGINAS; i++) {
        if (RAM->memoria[ponteiro].segunda_chance == false) {
            const int pagina_para_substituir = ponteiro;
            ponteiro = (ponteiro + 1) % NUMERO_PAGINAS; // Avança o ponteiro
            pthread_mutex_unlock(&mutex_RAM);
            return pagina_para_substituir;
        }

        RAM->memoria[ponteiro].segunda_chance = false; // Remove a segunda chance
        ponteiro = (ponteiro + 1) % NUMERO_PAGINAS; // Avança o ponteiro
    }
    pthread_mutex_unlock(&mutex_RAM);

    // Caso todas as páginas tenham segunda chance, substitui a primeira página examinada
    const int pagina_para_substituir = ponteiro;
    ponteiro = (ponteiro + 1) % NUMERO_PAGINAS;
    return pagina_para_substituir;
}

void descarregarPaginas(BCP *processo) {
    pthread_mutex_lock(&mutex_RAM); // bloqueia acesso à memória
    for (int i = 0; i < processo->n_paginas_usadas; i++) {
        Pagina *pag = processo->paginas_usadas[i];

        if (pag) { // se a página está carregada
            pag->conteudo = NULL;
            processo->paginas_usadas[i] = NULL;
            RAM->n_paginas_ocupadas--;
        }
    }
    pthread_mutex_unlock(&mutex_RAM); // libera acesso à memória
}

bool verificarPaginasCarregadas(const BCP *processo) {
    for (int i = 0; i < processo->n_paginas_usadas; i++) {
        if (processo->paginas_usadas[i] == NULL)
            return false; // Página não carregada encontrada
    }
    return true; // Todas as páginas carregadas
}