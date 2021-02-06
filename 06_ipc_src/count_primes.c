/* count prime numbers in random array in parallel */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>

const size_t MAX_WORKERS = 8;
const size_t NNUMBERS = 1000000;
const int MAX_RND_NUMBER = 100000;

struct _worker {
    pid_t pid;
    int rfd; /* read pipe descriptor */
};
typedef struct _worker worker;
int run_worker(worker *w, int *begin, int *end);
int get_result(worker *w);

int is_prime(int n);

int *mmap_numbers(size_t num);
int munmap_numbers(int* addr, size_t num);

int wait_report(pid_t pid);

int main(int argc, char *argv[]) {
    srand(time(NULL)); /* initialize random number generator
                            with current timestamp */
    size_t nworkers = 1;
    if (argc == 2) {
        nworkers = atoi(argv[1]);
    }
    if (nworkers > MAX_WORKERS) {
        fprintf(stderr, "max workers is %lu\n", MAX_WORKERS);
        return EXIT_FAILURE;
    }
    int *numbers = mmap_numbers(NNUMBERS);
    if (numbers == MAP_FAILED) {
        perror("mmap_numbers");
        return EXIT_FAILURE;
    }

    worker workers[MAX_WORKERS];
    int num_per_worker = NNUMBERS / nworkers;
    for (size_t i = 0; i < nworkers; ++i) {
        int *begin = numbers + i * num_per_worker;
        int *end = numbers + ((i == nworkers - 1) ? NNUMBERS : (i + 1) * num_per_worker);
        if (-1 == run_worker(&workers[i], begin, end)) {
            fprintf(stderr, "Fatal error!");
            return EXIT_FAILURE;
        }
    }

    size_t total_primes = 0;
    for (size_t i = 0; i < nworkers; ++i) {
        total_primes += get_result(&workers[i]);
    }

    if (-1 == munmap_numbers(numbers, NNUMBERS)) {
        perror("munmap_numbers");
        return EXIT_FAILURE;
    }
    printf("Total primes: %lu\n", total_primes);
    return EXIT_SUCCESS;
}

int run_worker(worker *w, int *begin, int *end) {
    int fd[2];
    if (-1 == pipe(fd)) {
        perror("pipe");
        return -1;
    }

    pid_t pid = fork();
    if (pid == 0) { /* child branch */
        close(fd[0]);
        int nprimes = 0;
        for (; begin < end; ++begin) {
            if (is_prime(*begin)) {
                ++nprimes;
            }
        }
        int wb = write(fd[1], &nprimes, sizeof(nprimes));
        if (wb != sizeof(nprimes)) {
            perror("write");
            close(fd[1]);
            exit(EXIT_FAILURE);
        }
        close(fd[1]);
        exit(EXIT_SUCCESS);
    } else if (pid == -1) {
        perror("fork");
        close(fd[0]);
        close(fd[1]);
        return -1;
    }
    close(fd[1]);
    w->pid = pid;
    w->rfd = fd[0];
    return 0;
}

int get_result(worker *w) {
    int nprimes;
    int retval = 0;
    int rb = read(w->rfd, &nprimes, sizeof(nprimes));
    if (rb != sizeof(nprimes)) {
        perror("read");
        retval = -1;
    } else {
        retval = nprimes;
    }
    if (-1 == wait_report(w->pid)) {
        retval = -1;
    }
    close(w->rfd);
    return retval;
}

int is_prime(int n) {
    if (n < 2) {
        return 0;
    }
    for (int i = 2; i < n; ++i) {
        if (n % i == 0) {
            return 0;
        }
    }
    return 1;
}

int *mmap_numbers(size_t num) {
    int *addr = (int*)mmap(NULL, num * sizeof(int),
                           PROT_READ | PROT_WRITE,
                           MAP_ANONYMOUS | MAP_SHARED,
                           -1, 0);
    if (addr == MAP_FAILED) {
        return MAP_FAILED;
    }

    for (size_t i = 0; i < num; ++i) {
        addr[i] = rand() % MAX_RND_NUMBER;
    }
    return addr;
}

int munmap_numbers(int* addr, size_t num) {
    return munmap(addr, num * sizeof(int));
}

int wait_report(pid_t pid) {
    int status;
    if (-1 == waitpid(pid, &status, 0)) {
        perror("waitpid");
        return -1;
    } else {
        if (WIFEXITED(status)) {
            if (WEXITSTATUS(status) != 0) {
                fprintf(stderr, "Child processed exited with status %d\n", WEXITSTATUS(status));
                return -1;
            }
        } else if (WIFSIGNALED(status)) {
            fprintf(stderr, "Child processed was terminated by signal %d\n", WTERMSIG(status));
            return -1;
        }
    }
    return 0;    
}
