#include "eventos.h"

void processInterrupt() {}

void semaphoreP(semaphore_t* semaph, BCPitem_t* proc){
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
            BCPitem_t* proc = semaph->waiting_list->proc;
            semaph->waiting_list = semaph->waiting_list->next;
            proc_wakeup(proc);
        }
    }
    pthread_mutex_unlock(&semaph->mutex_lock);
}

void DiskRequest() {}

void DiskFinish() {}

void PrintRequest() {}

void PrintFinish() {}

void memLoadReq() {}

void memLoadFinish() {}

void fsRequest() {}

void fsFinish() {}

void processCreate() {}

void processFinish() {}
