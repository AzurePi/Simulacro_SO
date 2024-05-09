#include <stdio.h>
#include <malloc.h>
#include "sintetico.h"
#include "lista.h"

lista *lerArquivoProgramas(char *filename) {
    FILE *file;
    lista *listaProgramas = criarLista();
    BCP *programa = malloc(sizeof(BCP));

    file = fopen(filename, "r");
    if (!file) {
        printf("ERRO: arquivo %s não pôde ser aberto", filename);
        return NULL;
    }

    while (lerBCP(file, programa)) {
        inserirLista(listaProgramas, programa);
        programa = malloc(sizeof(BCP));
    }

    fclose(file);
}

int lerBCP(FILE *file, BCP *p) {
    char buffer[68];

    int i = fscanf(file, "%[68]s", buffer);
    if (i == 0)
        return 0;

    sscanf(buffer, "%[^,]s,%d,%d", p->nome, &p->mem_space, &p->cpu_time);
    return 1;
}

