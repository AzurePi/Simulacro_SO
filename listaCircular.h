#ifndef SIMULACRO_SO_LISTACIRCULAR_H
#define SIMULACRO_SO_LISTACIRCULAR_H

#include <stdio.h>
#include <stdlib.h>
#include "sintetico.h"

typedef struct no {
    char uso; //Um char com valor 0 para sem uso e 1 para em uso - ver se pode substituir
    char modificado; // 0 para não modificado e 1 para modificado
    char ocupado; //0 para livre e 1 para página preenchida
    struct no *prox;
} No_Pagina;

typedef struct lista {
    No_Pagina *head;
    No_Pagina *tail;
} Lista_Circ;

No_Pagina *criaNo();

Lista_Circ *criaLista();

No_Pagina *insereNormal(No_Pagina *noAdiciona, Lista_Circ *lista);

//Tomar cuidado pq quando chegar na tail, volta pra cabeça - usa pra buscar sem uso.
No_Pagina *percorreLista(Lista_Circ lista);

//Vai ser usado para o round-robin?
No_Pagina *inserePrioridade(BCP *noAdiciona, Lista_Circ lista);

#endif //SIMULACRO_SO_LISTACIRCULAR_H
