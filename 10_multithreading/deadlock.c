/* Deadlock example */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

const int initial_balance = 100;
const int ndeposits = 2;
const int nworkers = 2;
const int niter = 100;

struct deposit {
    int balance;
    pthread_mutex_t mutex;
};

struct deposit deposit_init(int initial_balance);

int sum_balances(struct deposit deps[], int ndeps);

void deposit_destroy(struct deposit *dep);

void exec_transaction(struct deposit *from,
                      struct deposit *to,
                      int amount);

void exit_on_error(int err, char* msg);

void *thread_start(void *deps_);

int main() {
    srand(time(NULL)); /* rnd init */

    struct deposit deposits[ndeposits];
    for (int i = 0; i < ndeposits; ++i) {
        deposits[i] = deposit_init(initial_balance);
    }
    pthread_t workers[nworkers];
    for (int i = 0; i < nworkers; ++i) {
        exit_on_error(pthread_create(&workers[i], NULL,
                                     thread_start, (void*)deposits),
                      "pthread_create"); 
    }

    for (int i = 0; i < nworkers; ++i) {
        exit_on_error(pthread_join(workers[i], NULL), "pthread_join"); 
    }  

    for (int i = 0; i < ndeposits; ++i) {
        deposit_destroy(&deposits[i]);
    }
    return EXIT_SUCCESS;
}

void *thread_start(void *deps_) {
    struct deposit *deps = (struct deposit *)deps_;
    for (int i = 0; i < niter; ++i) {
        usleep(rand() % 100);
        int amount = rand() % 10;
        int from = rand() % ndeposits;
        int to = 1 - from; /* only two deposists */
        exec_transaction(&deps[from], &deps[to], amount);
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

struct deposit deposit_init(int initial_balance) {
    struct deposit dep;
    dep.balance = initial_balance;
    exit_on_error(pthread_mutex_init(&dep.mutex, NULL), "pthread_mutex_init");
    return dep;
}

void deposit_destroy(struct deposit *dep) {
    exit_on_error(pthread_mutex_destroy(&dep->mutex), "pthread_mutex_destroy");
}

int sum_balances(struct deposit deps[], int ndeps) {
    int sum = 0;
    for (int i = 0; i < ndeps; ++i) {
        sum += deps[i].balance;
    }
    return sum;
}


void exec_transaction(struct deposit *from,
                      struct deposit *to,
                      int amount) {
    exit_on_error(pthread_mutex_lock(&from->mutex), "pthread_mutex_lock");
    usleep(rand() % 10);
    exit_on_error(pthread_mutex_lock(&to->mutex), "pthread_mutex_lock");
    
    from->balance -= amount;
    to->balance += amount;
    
    exit_on_error(pthread_mutex_unlock(&to->mutex), "pthread_mutex_unlock");
    exit_on_error(pthread_mutex_unlock(&from->mutex), "pthread_mutex_unlock");
}
