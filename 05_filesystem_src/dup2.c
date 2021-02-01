/* write output of ls -l into file */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

int redirect_stdout_and_run(const char *path, char *const args[], int outfd);

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Usage: %s <output>\n", argv[0]);
        return EXIT_FAILURE;
    }
    int fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd == -1) {
        perror("open");
        return EXIT_FAILURE;
    }
    char *const ls[] = {"ls", "-l", NULL};

    if (-1 == redirect_stdout_and_run(ls[0], ls, fd)) {
        perror("redirect_stdout_and_run");
        close(fd);
        return EXIT_FAILURE;
    }
    close(fd);
    return EXIT_SUCCESS;
}

int redirect_stdout_and_run(const char *path, char *const args[], int outfd) {
    pid_t pid = fork();
    if (pid == 0) { /* child branch */
        if (-1 == dup2(outfd, STDOUT_FILENO)) {
            return -1;
        }
        if (-1 == execvp(args[0], args)) {
            return -1;
            exit(EXIT_FAILURE);
        }
        /* unreachable code */
        exit(EXIT_SUCCESS); 
    } else if (pid == -1) {
        return -1;
    }
    int status = 0;
    if (-1 == wait(&status)) {
        return -1;
    }
    if (WIFEXITED(status)) {
        if (WEXITSTATUS(status) != 0) {
            return -1;
        }
    } else {
        return -1;
    }
    return 0; 
}

