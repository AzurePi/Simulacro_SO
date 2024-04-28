#ifndef SIMULACRO_SO_LISTA_H
#define SIMULACRO_SO_LISTA_H

#include <stdbool.h>

// TAD: lista seq.
#define MAX 100 //estimativa do tamanho máximo da lista

// Estruturas e tipos empregados
//-------------------------------------------
//Tipo chave
typedef int tipo_chave;

//Tipo registro
typedef struct {
	char nome[30];
	int idade;
	float media_final;
	//... (caso tenha mais campos)
} tipo_dado;

//Tipo elemento (registro + chave)
typedef struct {
	tipo_chave chave;
	tipo_dado info;
} tipo_elem;

//Tipo lista (sequencial encadeada)
typedef struct {
	int nelem; //n�mero de elementos
	tipo_elem A[MAX + 1];
} lista;
//-------------------------------------------

// Operações
//-------------------------------------------
bool Vazia(lista *L);
bool Cheia(lista *L);
void Definir(lista *L);
void Apagar(lista *L);
bool Inserir_posic(tipo_elem x, int p, lista *L);
bool Busca_bin(tipo_chave x, lista *L, int *p); // Igual a 'Buscar_ord'
void Remover_posic(int *p, lista *L);
bool Remover_ch(tipo_chave x, lista *L);
void Impr_elem(tipo_elem t);
void Imprimir(lista *L);
int Tamanho(lista *L);

// Implementadas por Lucas e Pedro:
// -----------------------------------------
bool Inserir_ord_Nome(tipo_elem x, lista *L); //Insere ordenado por nome
bool Buscar_Nome(char *nome, lista *L, int *p);  //Busca sequencial por nome
bool Buscar_ord_Nome(char *nome, lista *L, int *p); //Busca ordenada por nome
bool Inserir_ord_ch(tipo_elem x, lista *L);   //Insere ordenado por chave
bool Buscar(tipo_chave x, lista *L, int *p);  //Busca sequencial por chave
bool Buscar_ord(tipo_chave x, lista *L, int *p); //Busca bin�ria por chave
bool Repetido(tipo_chave chave, lista *L);
//-------------------------------------------


#endif //SIMULACRO_SO_LISTA_H
