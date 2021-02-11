/* Dining philosophers problem solution with semaphore */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>

const int nphilosopher = 5;
const int nforks = 5;

struct philosopher {
    int id;
    pthread_mutex_t *left_fork;
    pthread_mutex_t *right_fork;
    sem_t *sem;
};

void *thread_start(void *arg);

void eat(struct philosopher *);
void take_left_fork(struct philosopher *);
void take_right_fork(struct philosopher *);
void put_left_fork(struct philosopher *);
void put_right_fork(struct philosopher *);

void exit_on_error(int err, char* msg);

int main() {
    sem_t sem;
    if (-1 == sem_init(&sem, 0, nforks - 1)) {
        perror("sem_init");
        return EXIT_FAILURE;
    }

    pthread_mutex_t forks[nforks];
    for (int i = 0; i < nforks; ++i) {
        exit_on_error(pthread_mutex_init(&forks[i], NULL), "pthread_mutex_init");
    }

    struct philosopher phils[nphilosopher];
    for (int i = 0; i < nphilosopher; ++i) {
        phils[i].id = i + 1;
        phils[i].left_fork = &forks[i];
        phils[i].right_fork = &forks[(i + 1) % nforks];
        phils[i].sem = &sem;
    }

    pthread_t workers[nphilosopher];
    for (int i = 0; i < nphilosopher; ++i) {
        exit_on_error(pthread_create(&workers[i] , NULL,
                                     thread_start, (void*)&phils[i]),
                      "pthread_create");
    }

    for (int i = 0; i < nphilosopher; ++i) {
        exit_on_error(pthread_join(workers[i], NULL), "pthread_join");
    }

    for (int i = 0; i < nforks; ++i) {
        exit_on_error(pthread_mutex_destroy(&forks[i]), "pthread_mutex_destroy");
    }

    if (-1 == sem_destroy(&sem)) {
        perror("sem_destroy");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

void eat(struct philosopher *phil) {
    printf("Philosopher %d eating\n", phil->id);
    usleep(10000 + rand() % 90000);
}

void take_left_fork(struct philosopher *phil) {
    printf("Philosopher %d waiting for left fork\n", phil->id);
    exit_on_error(pthread_mutex_lock(phil->left_fork), "pthread_mutex_lock");
    printf("Philosopher %d took left fork\n", phil->id);
}

void take_right_fork(struct philosopher *phil) {
    printf("Philosopher %d waiting for right fork\n", phil->id);
    exit_on_error(pthread_mutex_lock(phil->right_fork), "pthread_mutex_lock");
    printf("Philosopher %d took right fork\n", phil->id);
}

void put_left_fork(struct philosopher *phil) {
    exit_on_error(pthread_mutex_unlock(phil->left_fork), "pthread_mutex_unlock");
    printf("Philosopher %d put left fork\n", phil->id);
}

void put_right_fork(struct philosopher *phil) {
    exit_on_error(pthread_mutex_unlock(phil->right_fork), "pthread_mutex_unlock");
    printf("Philosopher %d put right fork\n", phil->id);
}


void *thread_start(void *arg) {
    struct philosopher *phil = (struct philosopher *)arg;
    for(;;) {
        if (-1 == sem_wait(phil->sem)) {
            perror("sem_wait");
            exit(EXIT_FAILURE);
        }
        take_left_fork(phil);
        take_right_fork(phil);
        eat(phil);
        put_right_fork(phil);
        put_left_fork(phil);
        if (-1 == sem_post(phil->sem)) {
            perror("sem_post");
            exit(EXIT_FAILURE);
        }
    }
    return NULL;
}

void exit_on_error(int err, char* msg) {
    if (err != 0) {
        errno = err;
        perror(msg);
        exit(EXIT_FAILURE);
    }
}
