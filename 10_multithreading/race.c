/* demostrate race condition by decrementing integer */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>

const int nthreads = 5;

void *thread_start(void *ntickets) {
    int *nticketsref = (int*)ntickets;
    while (*nticketsref > 0) {
        usleep(100);
        --(*nticketsref);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s number\n", argv[0]);
        return EXIT_FAILURE;
    }
    int ntickets = atoi(argv[1]);

    pthread_t threads[nthreads];
    for (int i = 0; i < nthreads; ++i) {
        int s = pthread_create(&threads[i], NULL, thread_start, &ntickets);
        if (s != 0) {
            errno = s;
            perror("pthread_create");
            return EXIT_FAILURE;
        }
    }

    for (int i = 0; i < nthreads; ++i) {
        int s = pthread_join(threads[i], NULL);
        if (s != 0) {
            errno = s;
            perror("pthread_join");
            return EXIT_FAILURE;
        }
    }

    printf("ntickets = %d\n", ntickets);
    
    return EXIT_SUCCESS;
}
