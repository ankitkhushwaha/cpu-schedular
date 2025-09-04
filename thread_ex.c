#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define BUF_SIZE 3

int buffer[BUF_SIZE];
int in = 0, out = 0;

sem_t empty;   // counts empty slots
sem_t full;    // counts full slots
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *producer(void *arg) {
    for (int item = 0; item < 10; item++) {
        sem_wait(&empty);               // wait for empty slot
        pthread_mutex_lock(&mutex);

        buffer[in] = item;
        in = (in + 1) % BUF_SIZE;
        printf("Produced %d\n", item);

        pthread_mutex_unlock(&mutex);
        sem_post(&full);                // signal filled slot
    }
    return NULL;
}

void *consumer(void *arg) {
    for (int i = 0; i < 10; i++) {
        sem_wait(&full);                // wait for full slot
        pthread_mutex_lock(&mutex);

        int item = buffer[out];
        out = (out + 1) % BUF_SIZE;
        printf("Consumed %d\n", item);

        pthread_mutex_unlock(&mutex);
        sem_post(&empty);               // signal empty slot
    }
    return NULL;
}

int main() {
    pthread_t prod, cons;

    sem_init(&empty, 0, BUF_SIZE); // all slots empty initially
    sem_init(&full, 0, 0);         // no slots full initially

    pthread_create(&prod, NULL, producer, NULL);
    pthread_create(&cons, NULL, consumer, NULL);

    pthread_join(prod, NULL);
    pthread_join(cons, NULL);

    sem_destroy(&empty);
    sem_destroy(&full);
    return 0;
}



// #include <pthread.h>
// #include <stdio.h>
// #include <stdlib.h>

// #define BUF_SIZE 3

// int buffer[BUF_SIZE];
// int add = 0;
// int rem = 0;
// int num = 0;
// pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
// pthread_cond_t c_cons = PTHREAD_COND_INITIALIZER;
// pthread_cond_t c_prod = PTHREAD_COND_INITIALIZER;

// void *producer(void *param) {
//     int item;
//     for (item = 0; item < 10; item++) {
//         pthread_mutex_lock(&m);
//         while (num == BUF_SIZE)
//             pthread_cond_wait(&c_prod, &m);
//         buffer[add] = item;
//         add = (add + 1) % BUF_SIZE;
//         num++;
//         printf("Produced %d\n", item);
//         pthread_cond_signal(&c_cons);
//         pthread_mutex_unlock(&m);
//     }
//     return NULL;
// }

// void *consumer(void *param) {
//     int item;
//     while(1) {
//         pthread_mutex_lock(&m);
//         while (num == 0)
//             pthread_cond_wait(&c_cons, &m);
//         item = buffer[rem];
//         rem = (rem + 1) % BUF_SIZE;
//         num--;
//         printf("Consumed %d\n", item);
//         pthread_cond_signal(&c_prod);
//         pthread_mutex_unlock(&m);
//         // exit after consuming all items for demo
//         if (item == 9) break;
//     }
//     return NULL;
// }

// int main() {
//     pthread_t tid1, tid2;
//     pthread_create(&tid1, NULL, producer, NULL);
//     pthread_create(&tid2, NULL, consumer, NULL);
//     pthread_join(tid1, NULL);
//     pthread_join(tid2, NULL);
//     return 0;
// }
