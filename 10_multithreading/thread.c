/* create thread and join */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>

void *thread_start(void *state) {
    *(int*)state = 42;

    return NULL;
}

int main() {
    pthread_t thread;
    volatile int state = 0; /* volatile for suppress compiler optimizations */

    int s = pthread_create(&thread, NULL, thread_start, (void*)&state);
    if (s != 0) {
        errno = s;
        perror("pthread_create");
        return EXIT_FAILURE;
    }

    s = pthread_join(thread, NULL);
    if (s != 0) {
        errno = s;
        perror("pthread_join");
        return EXIT_FAILURE;
    }
    printf("state = %d\n", state);
    return EXIT_SUCCESS;
}

