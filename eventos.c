#include <stdio.h>
#include <semaphore.h>
#include "eventos.h"
#include "sintetico.h"
#include "main.h"

void processInterrupt() {}

void insert_semaphore(semaphore_t* item)//insere um semaforo na lista
{
    if(existing_semaphores.head == NULL)
    {
        existing_semaphores.head = item;
        return;
    }
    semaphore_t* aux, *prev = NULL;
    for(aux = existing_semaphores.head; aux; prev = aux, aux = aux->next)
    {
        //semaphore by this name already exists
        if(item->name == aux->name)
        {
            aux->refcount++;
            free(item);
            return;
        }
    }
    item->next = aux;
    if(prev)
        prev->next = item;
    else
        existing_semaphores.head = item;
}

void createSemaphore(char name)//cria um semaforo e adiciona a lista
{
    semaphore_t* new = malloc(sizeof(semaphore_t));
    new->next = NULL;
    new->name = name;
    new->refcount = 1;
    semaphore_init(new, 0);
    insert_semaphore(new);
}

void removeSemaphore(semaphore_t* item)
{
    semaphore_t* aux = existing_semaphores.head, *prev = NULL;
    for(; aux && aux != item; prev = aux, aux = aux->next);
    if(aux)
    {
        if(prev)
            prev->next = aux->next;
        else //aux is the current head
            existing_semaphores.head = existing_semaphores.head->next;
        free(aux);
    }
}

semaphore_t* retrieveSemaphore(char name)
{
    semaphore_t* aux = existing_semaphores.head;
    while(aux)
    {
        if(aux->name == name)
            return aux;
        aux = aux->next;
    }
    printf("algo deu errado.\n");
    sleep(1);
    return NULL;
}

void sem_queue(sem_list_item_t** list, BCP* proc)//Enfileira processos bloqueados por uma chamada falha a semaphoreP()
{
    sem_list_item_t* new = malloc(sizeof(sem_list_item_t));
    new->next = NULL;
    new->proc = proc;

    if(*list == NULL)
    {
        *list = new;
        return;
    }
    sem_list_item_t* aux, *prev = NULL;
    for(aux = *list; aux; prev = aux, aux = aux->next);
    new->next = aux;
    if(prev)
        prev->next = new;
    else
        (*list) = new;


}


void semaphoreP(semaphore_t* semaph, BCP* proc){
    pthread_mutex_lock(&semaph->mutex_lock);
    if(semaph->v < 0)
    {
        sem_queue(&semaph->waiting_list,proc);
        proc_sleep(proc);
    }
    semaph->v--;
    pthread_mutex_unlock(&semaph->mutex_lock);
}

void semaphoreV(semaphore_t* semaph)
{
    pthread_mutex_lock(&semaph->mutex_lock);
    semaph->v++;
    if(semaph->v <= 0)
    {
        if(semaph->waiting_list)
        {
            BCP* proc = semaph->waiting_list->proc;
            semaph->waiting_list = semaph->waiting_list->next;
            proc_wakeup(proc);
        }
    }
    pthread_mutex_unlock(&semaph->mutex_lock);
}

void showSemaphoreList()
{
    sem_wait(&sem);
    CLEAR_SCREEN
    printf("lista de semaforos existentes:\n");
    semaphore_t* aux = existing_semaphores.head;
    if(!aux)
    {
        printf("Nenhum.\n");
        sleep(3);
        sem_post(&sem);
        return;
    }
    while(aux)
    {
        printf("%c | Contador: %d\n",aux->name,aux->refcount);
        aux = aux->next;
    }
    sleep(3);
    sem_post(&sem);
}

void DiskRequest() {}

void DiskFinish() {}

void PrintRequest() {}

void PrintFinish() {}

void memLoadReq() {}

void memLoadFinish() {}

void fsRequest() {}

void fsFinish() {}

void *processCreate() {
    char filename[51];
    sem_wait(&sem_terminal);
    printf("Nome do programa: ");
    scanf(" %108[^\n]", filename);
    sem_post(&sem_terminal);

    FILE *programa = fopen(filename, "r");
    if (programa) {
        BCP *processo = BCP_From_Sintetico(programa);
        if (!processo)
            printf("ERRO: não foi possível criar o processo");
    } else
        printf("ERRO: %s não pode ser aberto\n", filename);
    fclose(programa);
}

void processFinish() {}
