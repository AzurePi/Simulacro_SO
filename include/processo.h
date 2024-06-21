#ifndef SIMULACRO_SO_PROCESSO_H
#define SIMULACRO_SO_PROCESSO_H

#include "semaforo.h"
#include "memoria.h"
#include "interface.h"

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Definições de Macros ------------------------------------------------------------------------------------------------

#define TAMANHO_PAGINA 8 // 8 unidades de memória (1 um = 1 kbyte)
#define TAMANHO_MEMORIA (1024 * 1024) // 1 GB de memória (1024 * 1024 kbytes = 1024 * 1024 um)
#define NUMERO_PAGINAS (TAMANHO_MEMORIA / TAMANHO_PAGINA)

// Declarações de Tipos ------------------------------------------------------------------------------------------------

// declaração avançada para evitar dependência circular
typedef struct lista_semaforos Lista_Semaforos;
typedef struct pagina Pagina;

// Estado do processo (PRONTO, EXECUTANDO, BLOQUEADO ou TERMINADO)
typedef enum {
    PRONTO, EXECUTANDO, BLOQUEADO, TERMINADO
} ESTADO;

// Operação a ser executada (EXEC, READ, WRITE, P, V ou PRINT)
typedef enum {
    EXEC, READ, WRITE, P, V, PRINT
} OPCODE;

// Estrutura de um comando, com seu opcode e parâmetro
typedef struct comando {
    OPCODE opcode; // EXEC, READ, WRITE, P, V ou PRINT
    int parametro; // parâmetro do comando; tem um tratamento diferente, dependendo do opcode
    struct comando *prox;
} Comando;

// Fila dos comandos presentes num dado programa
typedef struct {
    Comando *head; // começo da fila
    Comando *tail; // fim da fila
} Fila_Comandos;

// Bloco de controle de processo
typedef struct bcp {
    char nome[61]; // nome do programa
    int id_seg; // identificador do segmento
    int prioridade; // prioridade do programa
    int tamanho_seg; // tamanho do segmento (em kbytes)
    int n_paginas_usadas; // número de páginas necessárias na memória
    Pagina *paginas_usadas[NUMERO_PAGINAS]; // Array de ponteiros para páginas utilizadas pelo processo
    ESTADO estado; // PRONTO, EXECUTANDO, BLOQUEADO ou TERMINADO
    Lista_Semaforos *semaforos; // lista de semáforos usados pelo programa
    Fila_Comandos *comandos; // fila dos comandos do programa
    struct bcp *prox; // ponteiro para o próximo processo na lista
} BCP;

// Protótipos de Funções -----------------------------------------------------------------------------------------------

// Cria um novo BCP; inicializa-o com estado PRONTO, lista de semáforos vazia, e lista de comandos vazia.
BCP *novoBCP();

// Insere um BCP ordenadamente (por prioridade) na lista global.
void inserirBCP(BCP *new);

// Insere um BCP ignorando a ordem) no final da lista global.
void inserirBCPFinal(BCP *processo);

// Busca o próximo BCP que pode ser executado na lista.
BCP *buscaBCPExecutar();

// Libera a memória alocada para um BCP.
void freeBCP(BCP *bcp);

// Libera a memória de toda uma lista de BCPs.
void freeListaBCP(BCP *bcp_head);

// Lê o cabeçalho de um programa sintético.
bool lerCabecalho(FILE *programa, BCP *bcp);

// Lê a linha que contém os semáforos de um programa sintético.
void lerSemaforos(FILE *programa, BCP *bcp);

// Lê cada um dos comandos do processo e os armazena nima lista.
bool lerComandos(FILE *programa, Fila_Comandos *fila);

// Cria um BCP com base na leitura de um programa sintético.
BCP *lerProgramaSintetico(FILE *programa);

// Inicializa um novo Comando com base nos parâmetros.
Comando *novoComando(OPCODE opcode, int parametro);

// Libera a memória alocada para um comando.
void freeComando(Comando *comando);

// Inicializa uma Fila_Comandos vazia.
Fila_Comandos *novaFilaComandos();

// Libera a memória alocada para uma fila de comandos.
void freeFilaComandos(Fila_Comandos *fila);

// Insere um novo comando no final de uma fila de comandos.
void inserirComando(Comando *comando, Fila_Comandos *fila);

// Remove o comando no começo de uma fila de comandos.
void removerComando(Fila_Comandos *fila);

// Muda o estado do processo para BLOQUEADO e o manda para o final da fila de escalonamento
void process_sleep(BCP *processo);

// Muda o estado do processo para PRONTO.
void process_wakeup(BCP *processo);

#endif //SIMULACRO_SO_PROCESSO_H
