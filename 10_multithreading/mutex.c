/* demostrate race condition by decrementing integer */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>

const int nthreads = 5;

struct tickets {
    int ntickets;
    pthread_mutex_t mutex;
};

void exit_on_error(int err, char* msg) {
    if (err != 0) {
        errno = err;
        perror(msg);
        exit(EXIT_FAILURE);
    }
}

void *thread_start(void *ticketsp) {
    struct tickets *tickersref = (struct tickets*)ticketsp;
    while (tickersref->ntickets > 0) {
        usleep(100);
        pthread_mutex_lock(&tickersref->mutex);
        if (tickersref->ntickets > 0) {
            --(tickersref->ntickets);
        }
        pthread_mutex_unlock(&tickersref->mutex);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s number\n", argv[0]);
        return EXIT_FAILURE;
    }
    struct tickets tick;
    tick.ntickets = atoi(argv[1]);
    int s = pthread_mutex_init(&tick.mutex, NULL);
    exit_on_error(s, "pthread_mutex_init");
    pthread_t threads[nthreads];
    for (int i = 0; i < nthreads; ++i) {
        s = pthread_create(&threads[i], NULL, thread_start, &tick);
        exit_on_error(s, "pthread_create");
    }

    for (int i = 0; i < nthreads; ++i) {
        int s = pthread_join(threads[i], NULL);
        exit_on_error(s, "pthread_join");
    }
    s = pthread_mutex_destroy(&tick.mutex);
    exit_on_error(s, "pthread_join");
    printf("ntickets = %d\n", tick.ntickets);
    
    return EXIT_SUCCESS;
}
