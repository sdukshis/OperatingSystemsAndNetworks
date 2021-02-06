/* count files by running ls and redirecting its output to wc -l */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

/* Wait for child process and report on error.
Returns zero on success and -1 on error */
int wait_report(pid_t pid);

int main() {
    int fd[2];
    if (-1 == pipe(fd)) {
        perror("pipe");
        return EXIT_FAILURE;
    }

    pid_t pid[2];
    pid[0] = fork();
    if (pid[0] == 0) {/* child branch */
        /* close read fd */
        close(fd[0]);
        /* redirect stdout to pipes write fd */
        if (-1 == dup2(fd[1], STDOUT_FILENO)) {
            perror("dup2");
            close(fd[1]);
            exit(EXIT_FAILURE);
        }
        if (-1 == execlp("ls", "ls", NULL)) {
            perror("execve");
            close(fd[1]);
            exit(EXIT_FAILURE);
        }
        close(fd[1]);
        exit(EXIT_SUCCESS);
    } else if (pid[0] == -1) {
        perror("fork");
        return EXIT_FAILURE;
    }

    pid[1] = fork();
    if (pid[1] == 0) {/* child branch */
        /* close write fd */
        close(fd[1]);
        /* redirect stdin from pipes read fd */
        if (-1 == dup2(fd[0], STDIN_FILENO)) {
            perror("dup2");
            close(fd[0]);
            exit(EXIT_FAILURE);
        }
        if (-1 == execlp("wc", "wc", "-l", NULL)) {
            perror("execve");
            close(fd[0]);
            exit(EXIT_FAILURE);
        }
        close(fd[0]);
        exit(EXIT_SUCCESS);
    } else if (pid[1] == -1) {
        perror("fork");
        return EXIT_FAILURE;
    }
    /* close both pipe fd in parent process */
    close(fd[0]);
    close(fd[1]);

    int rv[2];
    rv[0] = wait_report(pid[0]);
    rv[1] = wait_report(pid[1]);
    if (rv[0] || rv[1]) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
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
