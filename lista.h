#ifndef SIMULACRO_SO_LISTA_H
#define SIMULACRO_SO_LISTA_H

#include <stdbool.h>

// TAD: Lista seq.
#define MAX 100 //estimativa do tamanho máximo da Lista

// Estruturas e tipos empregados
//-------------------------------------------
//Tipo chave
typedef int Tipo_Chave;

//Tipo registro
typedef struct {
	char nome[30];
	int idade;
	float media_final;
	//... (caso tenha mais campos)
} Tipo_Dado;

//Tipo elemento (registro + chave)
typedef struct {
	Tipo_Chave chave;
	Tipo_Dado info;
} Tipo_Elem;

//Tipo Lista (sequencial encadeada)
typedef struct {
	int nelem; //número de elementos
	Tipo_Elem A[MAX + 1];
} Lista;
//-------------------------------------------

// Operações
//-------------------------------------------
bool Vazia(Lista *L);
bool Cheia(Lista *L);
void Definir(Lista *L);
void Apagar(Lista *L);
bool Inserir_posic(Tipo_Elem x, int p, Lista *L);
bool Busca_bin(Tipo_Chave x, Lista *L, int *p); // Igual a 'Buscar_ord'
void Remover_posic(int *p, Lista *L);
bool Remover_ch(Tipo_Chave x, Lista *L);
void Impr_elem(Tipo_Elem t);
void Imprimir(Lista *L);
int Tamanho(Lista *L);

// Implementadas por Lucas e Pedro:
// -----------------------------------------
bool Inserir_ord_Nome(Tipo_Elem x, Lista *L); //Insere ordenado por nome
bool Buscar_Nome(char *nome, Lista *L, int *p);  //Busca sequencial por nome
bool Buscar_ord_Nome(char *nome, Lista *L, int *p); //Busca ordenada por nome
bool Inserir_ord_ch(Tipo_Elem x, Lista *L);   //Insere ordenado por chave
bool Buscar(Tipo_Chave x, Lista *L, int *p);  //Busca sequencial por chave
bool Buscar_ord(Tipo_Chave x, Lista *L, int *p); //Busca bin�ria por chave
bool Repetido(Tipo_Chave chave, Lista *L);
//-------------------------------------------


#endif //SIMULACRO_SO_LISTA_H
