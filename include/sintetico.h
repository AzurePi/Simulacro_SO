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
    Comando *head; // começo da lista
    int nElementos; // número de elementos da lista
} Lista_Comandos;

// Bloco de controle de processos
typedef struct bcp {
    char nome[61]; // nome do programa
    int id_seg; // identificador do segmento
    int prioridade_OG; // prioridade original do programa
    int tamanho_seg; // tamanho do segmento (em kbytes)
    ESTADO estado; // PRONTO, EXECUTANDO, BLOQUEADO ou TERMINADO
    Lista_Semaforos *semaforos; // Lista de semáforos usados pelo programa
    Lista_Comandos *comandos; // Lista de comandos do programa
    struct bcp *prox; // ponteiro para o próximo processo na lista
} BCP;

// Funções -------------------------------------------------------------------------------------------------------------

// Cria um BCP com base na leitura de um programa sintético
BCP *lerProgramaSintetico(FILE *programa);

// Inicializa um novo Comando com base nos parâmetros
Comando *novoComando(OPCODE opcode, int parametro);

// Retorna true (1) se vazia, false (0) caso contrário.

// Inicializa uma Lista_Comandos vazia.
Lista_Comandos *novaListaComandos();

// Insere um novo comando no final de uma Lista_Comandos.
void inserirComando(Comando *comando, Lista_Comandos *lista);

// Insere um novo semáforo no final de uma Lista_Semaforos.
void inserirSemaforo(Semaforo *semaforo, Lista_Semaforos *lista);

// Muda o estado do processo para BLOQUEADO e o manda pro final da fila de escalonamento
void process_sleep(BCP *proc);

// Muda o estado do processo para PRONTO
void process_wakeup(BCP *proc);

#endif //SIMULACRO_SO_SINTETICO_H