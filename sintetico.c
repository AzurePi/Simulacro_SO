#include <stdio.h>
#include <malloc.h>
#include "sintetico.h"
#include "lista.h"
#include "listaCircular.h"

Lista *criarLista();

void lerArquivoProgramas(char *filename, Lista_Circ escalonador) {
    FILE *file;
    BCP *programa = malloc(sizeof(BCP));

    file = fopen(filename, "r");
    if (!file) {
        printf("ERRO: arquivo %s não pôde ser aberto", filename);
        return;
    }

    while (lerBCP(file, programa)) {
        inserePrioridade(programa, escalonador);
        programa = malloc(sizeof(BCP));
    }

    fclose(file);
}

Lista *criarLista() {
    return NULL;
}

int lerBCP(FILE *file, BCP *p) {
    char buffer[68];

    int i = fscanf(file, "%[68]s", buffer);
    if (i == 0)
        return 0;

    return 1;
}

