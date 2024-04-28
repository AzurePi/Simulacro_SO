#include <stdio.h>
#include <malloc.h>
#include "sintetico.h"
#include "lista.h"

Lista *lerArquivoProgramas(char *filename) {
    FILE *file;
    Lista *listaProgramas = criarLista();
    BCP *programa = malloc(sizeof(BCP));

    file = fopen(filename, "r");
    if (!file) {
        printf("ERRO: arquivo %s não pôde ser aberto", filename);
        return NULL;
    }

    while (lerSintetico(file, programa)) {
        inserirLista(listaProgramas, programa);
        programa = malloc(sizeof(BCP));
    }

    fclose(file)
}

int lerSintetico(FILE *file, BCP *p) {
    char buffer[68];

    int i = fscanf(file, "%[68]s", buffer);
    if (i == 0)
        return 0;

    sscanf(buffer, "%[^,]s,%d,%d", p->nome, &p->mem_space, &p->cpu_time);
    p->estado = 1;
    return 1;
}

void escreverSintetico(BCP p, FILE *file) {
    char buffer[68];

    sprintf(buffer, "%s,%d,%d", p.nome, p.mem_space, p.cpu_time);
    fprintf(file, "%s\n", buffer);
}