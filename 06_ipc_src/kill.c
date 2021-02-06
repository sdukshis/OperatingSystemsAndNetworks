/* Starts child process and sends kill signal to it */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s mode\n", argv[0]);
        return EXIT_FAILURE;
    }
    int mode = atoi(argv[1]);
    pid_t pid = fork();
    if (!pid) { /* child branch */
        if (mode == 1) {
            sleep(5);
        }
        exit(EXIT_SUCCESS);
    } else if (pid == -1) {
        perror("fork");
        return EXIT_FAILURE;
    }
    printf("Child proess with pid %d started\n", pid);
    /* wait 1 second for child process to finish */
    sleep(1);
    int status = 0;
    pid_t rv = waitpid(pid, &status, WNOHANG);
    if (rv == -1) {
        perror("waitpid");
        return EXIT_FAILURE;
    } else if (rv == 0) {
        /* child process is still running */
        printf("Child process hangs. Sending SIGINT\n");
        if (-1 == kill(pid, SIGINT)) {
            perror("kill");
            return EXIT_FAILURE;
        }
        sleep(1); /* wait some time for signal processing */
        
        rv = waitpid(pid, &status, WNOHANG);
        if (rv == -1) {
            perror("waitpid");
            return EXIT_FAILURE;
        } else if (rv == 0) {
            fprintf(stderr, "child process ignored SIGINT\n");
            return EXIT_FAILURE;
        }
    }
    if (WIFEXITED(status)) {
        printf("Child process exited normally with exit code: %d\n", WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        printf("Child process was terminated by signal number: %d\n", WTERMSIG(status));
    } else {
        printf("Child process was finished by unknown cause\n");
    }
    return EXIT_SUCCESS;
}
