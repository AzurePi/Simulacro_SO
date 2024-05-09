#include <semaphore.h>
#include <pthread.h>

int main() {
    pthread_attr_t atrib;
    pthread_t t1, t10, t11, t6, t7, t2, t3;

    pthread_attr_init(&atrib);
    pthread_attr_setscope(&atrib, PTHREAD_SCOPE_SYSTEM);


    return 0;
}
