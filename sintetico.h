#ifndef SIMULACRO_SO_SINTETICO_H
#define SIMULACRO_SO_SINTETICO_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

typedef struct comando {
    int opcode; // 0 - exec; 1 - read; 2 - write; 3 - P(); 4 - V(); 5 - print
    int parametro; //parâmetro do comando; tem um tratamento diferente, dependendo do opcode
    struct comando *prox;
} Comando;

typedef struct {
    Comando *head;
    int nElementos;
} Lista_Comandos;

typedef struct {
    char nome[61]; //nome do programa
    int id_seg; //identificador do segmento
    int prioridade_OG; //prioridade original do programa
    int tamanho_seg; //tamanho do segmento (em kbytes)
    int estado; //0 - pronto; 1 - executando; 2 - parado; 3 - terminado
    char *semaforos; //Lista de semáforos usados pelo programa TODO: trocar o tipo para a struct de semáforo
    Lista_Comandos *comandos; //Lista de comandos do programa
} BCP;

//Cria um BCP com base na leitura de um programa sintético
BCP *BCP_From_Sintetico(FILE *programa);

//Inicializa um novo Comando com base nos parâmetros
Comando *novoComando(int opcode, int parametro);

//Retorna true (1) se vazia, false (0) caso contrário.
bool vaziaListaComandos(Lista_Comandos *l);

//Inicializa uma Lista_Comandos vazia.
Lista_Comandos * novaListaComandos();

/*
Insere um novo comando no final de uma Lista_Comandos.
Devolve true se sucesso, false caso contrário.
*/
void inserirComando(Comando *comando, Lista_Comandos *lista);

#endif //SIMULACRO_SO_SINTETICO_H