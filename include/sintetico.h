#ifndef SIMULACRO_SO_SINTETICO_H
#define SIMULACRO_SO_SINTETICO_H

#include "semaforo.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Declaração de Tipos -------------------------------------------------------------------------------------------------

// declaração avançada de semaforo.h para evitar dependência circular
struct semaforo;
typedef struct semaforo Semaforo;
typedef struct lista_semaforos Lista_Semaforos;

// Estado do processo
typedef enum {
    PRONTO, EXECUTANDO, BLOQUEADO, TERMINADO
} ESTADO;

// Operação a ser executada
typedef enum {
    EXEC, READ, WRITE, P, V, PRINT
} OPCODE;

// Estrutura de um comando, com seu opcode e parâmetro
typedef struct comando {
    OPCODE opcode; // EXEC, READ, WRITE, P, V ou PRINT
    int parametro; // parâmetro do comando; tem um tratamento diferente, dependendo do opcode
    struct comando *prox;
} Comando;

// Lista de comandos presentes num dado programa
typedef struct {
    Comando *head; // começo da fila
    Comando *tail; // fim da fila
} Fila_Comandos;

// Bloco de controle de processos
typedef struct bcp {
    char nome[61]; // nome do programa
    int id_seg; // identificador do segmento
    int prioridade; // prioridade do programa
    int tamanho_seg; // tamanho do segmento (em kbytes)
    ESTADO estado; // PRONTO, EXECUTANDO, BLOQUEADO ou TERMINADO
    Lista_Semaforos *semaforos; // Lista de semáforos usados pelo programa
    Fila_Comandos *comandos; // Lista de comandos do programa
    struct bcp *prox; // ponteiro para o próximo processo na lista
} BCP;

// Funções -------------------------------------------------------------------------------------------------------------

//Cria um novo BCP; inicializa ele com estado PRONTO, lista de semáforos vazia, e lista de ocmandos vazia
BCP *novoBCP();

// Libera a memória alocada para um BCP
void freeBCP(BCP *bcp);

// Libera a memória de toda uma lista de BCPs
void freeListaBCP(BCP *bcp);

// Cria um BCP com base na leitura de um programa sintético
BCP *lerProgramaSintetico(FILE *programa);

// Inicializa um novo Comando com base nos parâmetros
Comando *novoComando(OPCODE opcode, int parametro);

// Libera a meória alocada para um comando
void freeComando(Comando *comando);

// Inicializa uma Fila_Comandos vazia.
Fila_Comandos *novaFilaComandos();

// Libera a memória alocada para uma Fila_Comandos
void freeListaComandos(Fila_Comandos *comandos);

// Insere um novo comando no final de uma Fila_Comandos.
void inserirComando(Comando *comando, Fila_Comandos *fila);

// Remove o último comando de uma Fila_Comandos.
void removerComando(Fila_Comandos *fila);

// Insere um novo semáforo no final de uma Lista_Semaforos.
void inserirSemaforo(Semaforo *semaforo, Lista_Semaforos *lista);

// Muda o estado do processo para BLOQUEADO e o manda pro final da fila de escalonamento
void process_sleep(BCP *proc);

// Muda o estado do processo para PRONTO
void process_wakeup(BCP *proc);

#endif //SIMULACRO_SO_SINTETICO_H