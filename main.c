#include <semaphore.h>
#include <pthread.h>
#include "listaCircular.h"

int main() {
    pthread_attr_t atrib;
    pthread_t t1, t10, t11, t6, t7, t2, t3;

    pthread_attr_init(&atrib);
    pthread_attr_setscope(&atrib, PTHREAD_SCOPE_SYSTEM);

    Lista_Circ circuloPagina;
    circuloPagina.head = NULL;
    circuloPagina.tail = NULL;

    No_Pagina *no = criaNo();
    circuloPagina.head = insereNormal(no, circuloPagina);
    No_Pagina *no2 = criaNo();
    circuloPagina.head = insereNormal(no2, circuloPagina); // Corrigido para inserir o segundo nó
    // Agora você pode percorrer e imprimir os elementos da Lista corretamente
    No_Pagina *temp = circuloPagina.head;
    do {
        printf("No: %d %d\n", temp->uso, temp->modificado);
        temp = temp->prox;
    } while (temp != circuloPagina.head);


    return 0;
}